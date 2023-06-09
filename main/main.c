
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
/* Enable this to show verbose logging for this file only. */
// #define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "esp_log.h"

#include "OwnN2K.h"

static const char *TAG = "main";

// Main routine. 
void app_main(void)
{

	OwnN2KInit();

	while (1)
	{
		vTaskDelay(1000UL / portTICK_PERIOD_MS);
		//ESP_LOGD(TAG, "free stack: %d", uxTaskGetStackHighWaterMark(NULL));
	}
}
