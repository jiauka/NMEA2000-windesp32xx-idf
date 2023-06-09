| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- |

# NMEA2000 Simple Wind monitor using esp idf


Inspired by  [NMEA2000_switchbank_example_esp-idf]( https://github.com/phatpaul/NMEA2000_switchbank_example_esp-idf.git) library in an ESP-IDF project.  
Example of using [NMEA2000](https://github.com/ttlappalainen/NMEA2000) library in an ESP-IDF project.  

The NMEA2000 library is typically used in the Arduino environment, but it can be used without Arduino.  Documentation was lacking on how to do this, so I created this example to demonstrate how it can be done in the ESP-IDF (4.4+) environment.


## How to use this example

Select the instructions depending on Espressif chip installed on your development board:

- [ESP32 Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/stable/get-started/index.html)

Clone this repo and make sure to get the submodules.
`git clone https://github.com/jiauka/NMEA2000-windesp32xx-idf.git --recursive`

(If you forgot the --recusive clone, then you can get the submodules later with `git submodule update --init --recursive`)

Launch the ESP-IDF build environment (varies with OS).  Then `idf build`

## Example project structure

The [NMEA2000](https://github.com/ttlappalainen/NMEA2000) and [NMEA2000_esp32xx](https://github.com/ttlappalainen/NMEA2000_esp32xx) repos are located in components/ dir of this project as git submodules.

ESP-IDF app code is typically written in C.  So to call C++ library functions, I created a wrapper called `OwnN2K` as another component.
```
my-esp-idf-project/
  - components/
    - NMEA2000/
    - NMEA2000_esp32c3/
    - OwnN2K/
      - CMakeLists.txt
      - OwnN2K.cpp
      - OwnN2K.h
  - main/
    - CMakeLists.txt
    - main.c
  - CMakeLists.txt 
```
## Debug NMEA2000

If you wanna to use the native serial logging port at esp32xx at the NMEA2000 library, a quick and dirty hack is @NMEA2000.cpp

```
...
#include <stdlib.h>

#include "../../ESP32N2kStream/ESP32N2kStream.h"

extern ESP32N2kStream Serial;

#define DebugStream Serial

#define NMEA2000_FRAME_ERROR_DEBUG
#define NMEA2000_FRAME_IN_DEBUG
#define NMEA2000_FRAME_OUT_DEBUG
#define NMEA2000_MSG_DEBUG
#define NMEA2000_BUF_DEBUG
#define NMEA2000_DEBUG
...

```
