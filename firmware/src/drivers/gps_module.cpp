#include "gps_module.h"
#include "logger.h"
#include <cstring>
#include <cstdlib>

GPSModule::GPSModule(uint8_t rx_pin, uint8_t tx_pin, uint32_t baudrate)
    : _rx_pin(rx_pin), _tx_pin(tx_pin), _baudrate(baudrate), _buffer_index(0) {
    memset(&_latest_data, 0, sizeof(GpsData));
}

GPSModule::~GPSModule() {}

void GPSModule::init() {
    Serial2.begin(_baudrate, SERIAL_8N1, _rx_pin, _tx_pin);
    LOG_I("GPS", "GPS module initialized at %lu baud", _baudrate);
}

void GPSModule::update() {
    while (Serial2.available()) {
        char ch = Serial2.read();

        if (ch == '\n') {
            _nmea_buffer[_buffer_index] = '\0';
            parseNMEA(_nmea_buffer);
            _buffer_index = 0;
        } else if (ch != '\r' && _buffer_index < NMEA_BUFFER_SIZE - 1) {
            _nmea_buffer[_buffer_index++] = ch;
        }
    }
}

bool GPSModule::parseNMEA(const char* sentence) {
    if (sentence[0] != '$') return false;

    char fields[20][32];
    int field_count = 0;
    size_t i = 1;

    while (i < strlen(sentence) && field_count < 20) {
        size_t start = i;
        while (i < strlen(sentence) && sentence[i] != ',') i++;

        size_t len = i - start;
        if (len > 31) len = 31;
        strncpy(fields[field_count], sentence + start, len);
        fields[field_count][len] = '\0';

        field_count++;
        i++;
    }

    if (strncmp(sentence, "$GPGGA", 6) == 0) {
        return parseGPGGA(fields);
    } else if (strncmp(sentence, "$GPRMC", 6) == 0) {
        return parseGPRMC(fields);
    }

    return false;
}

bool GPSModule::parseGPGGA(const char* fields[]) {
    _latest_data.fix_quality = atoi(fields[6]);
    _latest_data.satellites = atoi(fields[7]);

    if (_latest_data.fix_quality > 0) {
        _latest_data.latitude = parseCoordinate(fields[2], fields[3]);
        _latest_data.longitude = parseCoordinate(fields[4], fields[5]);
        _latest_data.altitude = atof(fields[9]);
        _latest_data.timestamp = millis();
        return true;
    }

    return false;
}

bool GPSModule::parseGPRMC(const char* fields[]) {
    if (fields[2][0] == 'A') {  // Active (valid fix)
        _latest_data.speed = atof(fields[7]) * 1.852;  // Convert knots to km/h
        _latest_data.timestamp = millis();
        return true;
    }
    return false;
}

double GPSModule::parseCoordinate(const char* coord, const char* dir) {
    double value = atof(coord);
    int degrees = (int)(value / 100.0);
    double minutes = value - (degrees * 100.0);
    double decimal = degrees + minutes / 60.0;

    if (dir[0] == 'S' || dir[0] == 'W') {
        decimal = -decimal;
    }

    return decimal;
}

bool GPSModule::hasValidFix() {
    return _latest_data.fix_quality > 0 && _latest_data.satellites >= 4;
}

GpsData GPSModule::getLatestData() {
    return _latest_data;
}
