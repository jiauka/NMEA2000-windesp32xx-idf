set(srcs
    "ESP32N2kStream.cpp"
)

set(reqs
    "NMEA2000"
)

idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS "."
    REQUIRES ${reqs}
)
