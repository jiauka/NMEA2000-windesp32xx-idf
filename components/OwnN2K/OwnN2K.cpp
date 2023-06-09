
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE /* Enable this to show verbose logging for this file only. */
#include "esp_log.h"
#include <N2kMsg.h>
#include <NMEA2000.h> // https://github.com/ttlappalainen/NMEA2000
#include "sdkconfig.h"
#define ESP32_CAN_TX_PIN (gpio_num_t) GPIO_NUM_5 
#define ESP32_CAN_RX_PIN (gpio_num_t) GPIO_NUM_4
#include <NMEA2000_esp32xx.h> // https://github.com/ttlappalainen/NMEA2000_esp32
#include "N2kMessages.h"
#include "ESP32N2kStream.h"

static const char *TAG = "my_N2K_lib";
tNMEA2000_esp32xx NMEA2000;
//tActisenseReader ActisenseReader;


static TaskHandle_t N2K_task_handle = NULL;
ESP32N2kStream Serial;

// List here messages your device will transmit.
const unsigned long TransmitMessages[] PROGMEM={130306L,0};

// Define schedulers for messages. Define schedulers here disabled. Schedulers will be enabled
// on OnN2kOpen so they will be synchronized with system.
// We use own scheduler for each message so that each can have different offset and period.
// Setup periods according PGN definition (see comments on IsDefaultSingleFrameMessage and
// IsDefaultFastPacketMessage) and message first start offsets. Use a bit different offset for
// each message so they will not be sent at same time.
tN2kSyncScheduler WindScheduler(false,100,500);

// *****************************************************************************
// Call back for NMEA2000 open. This will be called, when library starts bus communication.
// See NMEA2000.SetOnOpen(OnN2kOpen); on setup()
void OnN2kOpen() {
  // Start schedulers now.
  WindScheduler.UpdateNextTime();
}


void HandleStreamN2kMsg(const tN2kMsg &N2kMsg) {
  // N2kMsg.Print(&Serial);
  NMEA2000.SendMsg(N2kMsg);
}

// NMEA 2000 message handler
void HandleNMEA2000Msg(const tN2kMsg &N2kMsg)
{
  //printf("HandleNMEA2000Msg PGN: %ld\n\r",N2kMsg.PGN);
}

// *****************************************************************************
double ReadWindAngle() {
  return DegToRad(50); // Read here the measured wind angle e.g. from analog input
}

// *****************************************************************************
double ReadWindSpeed() {
  return 10.3; // Read here the wind speed e.g. from analog input
}

// *****************************************************************************
void SendN2kWind() {
  tN2kMsg N2kMsg;

  if ( WindScheduler.IsTime() ) {
    WindScheduler.UpdateNextTime();
    SetN2kWindSpeed(N2kMsg, 1, ReadWindSpeed(), ReadWindAngle(),N2kWind_Apprent);
    NMEA2000.SendMsg(N2kMsg);
  }
}

// This is a FreeRTOS task
void N2K_task(void *pvParameters)
{
//  Serial=new ESP32N2kStream();
    ESP_LOGI(TAG, "Starting task");

  NMEA2000.SetProductInformation("00000002", // Manufacturer's Model serial code
                                 100, // Manufacturer's product code
                                 "jiauka wind mon",  // Manufacturer's Model ID
                                 "1.2.0.24 (2023-06-09)",  // Manufacturer's Software version code
                                 "1.2.0.0 (2023-06-09)" // Manufacturer's Model version
                                 );
  // Set device information
  NMEA2000.SetDeviceInformation(1, // Unique number. Use e.g. Serial number.
                                130, // Device function=Atmospheric. See codes on https://web.archive.org/web/20190531120557/https://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                85, // Device class=External Environment. See codes on https://web.archive.org/web/20190531120557/https://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                2046 // Just choosen free from code list on https://web.archive.org/web/20190529161431/http://www.nmea.org/Assets/20121020%20nmea%202000%20registration%20list.pdf
                               );
  // Uncomment 2 rows below to see, what device will send to bus. Use e.g. OpenSkipper or Actisense NMEA Reader                           
  //Serial.begin(115200);
  //NMEA2000.SetForwardStream(&Serial);
  // If you want to use simple ascii monitor like Arduino Serial Monitor, uncomment next line
  //NMEA2000.SetForwardType(tNMEA2000::fwdt_Text); // Show in clear text. Leave uncommented for default Actisense format.

  // If you also want to see all traffic on the bus use N2km_ListenAndNode instead of N2km_NodeOnly below
  NMEA2000.SetMode(tNMEA2000::N2km_ListenAndNode,23);
  // NMEA2000.SetDebugMode(tNMEA2000::dm_Actisense); // Uncomment this, so you can test code without CAN bus chips on Arduino Mega
  NMEA2000.SetForwardStream(&Serial); // PC output on native port
  NMEA2000.SetForwardType(tNMEA2000::fwdt_Text);   // Show in clear text
  NMEA2000.EnableForward(true);
  // Here we tell library, which PGNs we transmit
  NMEA2000.ExtendTransmitMessages(TransmitMessages);
  // Define OnOpen call back. This will be called, when CAN is open and system starts address claiming.
  NMEA2000.SetOnOpen(OnN2kOpen);
  NMEA2000.Open();
  // I originally had problem to use same Serial stream for reading and sending.
  // It worked for a while, but then stopped. Later it started to work.

    for (;;)
    {
        // put your main code here, to run repeatedly:
//        TaskN2kBinStatus();
  SendN2kWind();
        NMEA2000.ParseMessages();
        vTaskDelay(10UL / portTICK_PERIOD_MS);

    }
    vTaskDelete(NULL); // should never get here...
}

// was setup() in Arduino example:
extern "C" int OwnN2KInit(void)
{
    esp_err_t result = ESP_OK;
    ESP_LOGV(TAG, "create task");
    xTaskCreate(
        &N2K_task,            // Pointer to the task entry function.
        "N2K_task",           // A descriptive name for the task for debugging.
        3072,                 // size of the task stack in bytes.
        NULL,                 // Optional pointer to pvParameters
        tskIDLE_PRIORITY + 3, // priority at which the task should run
        &N2K_task_handle      // Optional pass back task handle
    );
    if (N2K_task_handle == NULL)
    {
        ESP_LOGE(TAG, "Unable to create task.");
        result = ESP_ERR_NO_MEM;
        goto err_out;
    }

err_out:
    if (result != ESP_OK)
    {
        if (N2K_task_handle != NULL)
        {
            vTaskDelete(N2K_task_handle);
            N2K_task_handle = NULL;
        }
    }

    return result;
}
