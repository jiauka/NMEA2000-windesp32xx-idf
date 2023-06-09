set(srcs
    "OwnN2K.cpp"
)

set(reqs
    "NMEA2000"
    "NMEA2000_esp32xx"
    "ESP32N2kStream"
)

idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS "."
    REQUIRES ${reqs}
)
