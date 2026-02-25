#ifndef GPS_MODULE_H
#define GPS_MODULE_H

#include <Arduino.h>
#include "types.h"

class GPSModule {
public:
    GPSModule(uint8_t rx_pin, uint8_t tx_pin, uint32_t baudrate = 9600);
    ~GPSModule();

    void init();
    void update();
    bool hasValidFix();
    GpsData getLatestData();

private:
    uint8_t _rx_pin;
    uint8_t _tx_pin;
    uint32_t _baudrate;
    GpsData _latest_data;

    bool parseNMEA(const char* sentence);
    bool parseGPGGA(const char* fields[]);
    bool parseGPRMC(const char* fields[]);
    double parseCoordinate(const char* coord, const char* dir);

    static const size_t NMEA_BUFFER_SIZE = 128;
    char _nmea_buffer[NMEA_BUFFER_SIZE];
    uint16_t _buffer_index;
};

#endif // GPS_MODULE_H
