#include <cstdio>
#include "ESP32N2kStream.h"
#include "esp_log.h"

static const char *TAG = "ESP32N2kStream";

int ESP32N2kStream::read() {
    return 0;
}

int ESP32N2kStream::peek() {
    return 0;
}

size_t ESP32N2kStream::write(const uint8_t *data, size_t size) {
    for( int i =0 ; i < size; i++){
        if(data[i] == '\n' || len == (sizeof(buff) -1)) {
            buff[len] = '\0';
            printf("%s\n\r", buff);
            len = 0;
        }else{
            buff[len++] = (char)data[i];
        }
    }
    return size;
}

void ESP32N2kStream::println() {
    print("\n");
}

void ESP32N2kStream::println(long l) {
    print((int)l, 10);
}

void ESP32N2kStream::println(char *s) {
    print(s);
}

void ESP32N2kStream::println(const char *format, ...) {
    char buffer[256];
    va_list args;
    va_start (args, format);
    vsnprintf (buffer, 255, format, args);

    print(buffer);
    println();
    //do something with the error

    va_end (args);
}
