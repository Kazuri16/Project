#ifndef SD_LOGGER_H
#define SD_LOGGER_H

#include <Arduino.h>
#include <SD.h>
#include "types.h"

class SDLogger {
public:
    SDLogger(uint8_t cs_pin);
    ~SDLogger();

    bool init();
    bool logCANFrame(const CanFrame& frame);
    bool logVehicleState(const VehicleState& state);
    bool logAnomaly(const Anomaly& anomaly);
    bool logGPSData(const GpsData& gps);

    void flush();
    uint32_t getFileSize();

private:
    uint8_t _cs_pin;
    File _can_log_file;
    File _anomaly_log_file;
    File _gps_log_file;

    const char* CAN_LOG_FILE = "/can_log.csv";
    const char* ANOMALY_LOG_FILE = "/anomaly_log.csv";
    const char* GPS_LOG_FILE = "/gps_log.csv";

    bool createFileIfNotExists(const char* filename);
    char* frameToCSV(const CanFrame& frame);
};

#endif // SD_LOGGER_H
