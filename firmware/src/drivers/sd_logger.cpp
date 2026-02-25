#include "sd_logger.h"
#include "logger.h"

SDLogger::SDLogger(uint8_t cs_pin) : _cs_pin(cs_pin) {}

SDLogger::~SDLogger() {
    if (_can_log_file) _can_log_file.close();
    if (_anomaly_log_file) _anomaly_log_file.close();
    if (_gps_log_file) _gps_log_file.close();
}

bool SDLogger::init() {
    if (!SD.begin(_cs_pin)) {
        LOG_E("SDLOG", "SD card initialization failed");
        return false;
    }

    LOG_I("SDLOG", "SD card initialized");

    // Create CSV headers if files don't exist
    if (!createFileIfNotExists(CAN_LOG_FILE)) {
        _can_log_file = SD.open(CAN_LOG_FILE, FILE_WRITE);
        if (_can_log_file) {
            _can_log_file.println("timestamp,can_id,dlc,data0,data1,data2,data3,data4,data5,data6,data7");
            _can_log_file.close();
        }
    }

    if (!createFileIfNotExists(ANOMALY_LOG_FILE)) {
        _anomaly_log_file = SD.open(ANOMALY_LOG_FILE, FILE_WRITE);
        if (_anomaly_log_file) {
            _anomaly_log_file.println("timestamp,type,severity,can_id,description");
            _anomaly_log_file.close();
        }
    }

    if (!createFileIfNotExists(GPS_LOG_FILE)) {
        _gps_log_file = SD.open(GPS_LOG_FILE, FILE_WRITE);
        if (_gps_log_file) {
            _gps_log_file.println("timestamp,latitude,longitude,altitude,speed,fix_quality,satellites");
            _gps_log_file.close();
        }
    }

    return true;
}

bool SDLogger::logCANFrame(const CanFrame& frame) {
    _can_log_file = SD.open(CAN_LOG_FILE, FILE_APPEND);
    if (!_can_log_file) {
        LOG_E("SDLOG", "Failed to open CAN log file");
        return false;
    }

    _can_log_file.printf("%lu,%03X,%d", frame.timestamp, frame.id, frame.dlc);
    for (int i = 0; i < 8; i++) {
        _can_log_file.printf(",%02X", i < frame.dlc ? frame.data[i] : 0x00);
    }
    _can_log_file.println();

    _can_log_file.close();
    return true;
}

bool SDLogger::logVehicleState(const VehicleState& state) {
    File state_file = SD.open("/vehicle_state.csv", FILE_APPEND);
    if (!state_file) return false;

    state_file.printf("%lu,%u,%u,%u,%u,%u,%u\n",
        state.timestamp, state.speed, state.rpm, state.throttle,
        state.gear, state.engine_status, state.fault_status);

    state_file.close();
    return true;
}

bool SDLogger::logAnomaly(const Anomaly& anomaly) {
    _anomaly_log_file = SD.open(ANOMALY_LOG_FILE, FILE_APPEND);
    if (!_anomaly_log_file) {
        LOG_E("SDLOG", "Failed to open anomaly log file");
        return false;
    }

    _anomaly_log_file.printf("%lu,%u,%u,%03X,%s\n",
        anomaly.timestamp, anomaly.type, anomaly.severity,
        anomaly.can_id, anomaly.description);

    _anomaly_log_file.close();
    return true;
}

bool SDLogger::logGPSData(const GpsData& gps) {
    _gps_log_file = SD.open(GPS_LOG_FILE, FILE_APPEND);
    if (!_gps_log_file) {
        LOG_E("SDLOG", "Failed to open GPS log file");
        return false;
    }

    _gps_log_file.printf("%lu,%.6f,%.6f,%.2f,%.2f,%u,%u\n",
        gps.timestamp, gps.latitude, gps.longitude,
        gps.altitude, gps.speed, gps.fix_quality, gps.satellites);

    _gps_log_file.close();
    return true;
}

bool SDLogger::createFileIfNotExists(const char* filename) {
    return SD.exists(filename);
}

void SDLogger::flush() {
    if (_can_log_file) _can_log_file.flush();
    if (_anomaly_log_file) _anomaly_log_file.flush();
    if (_gps_log_file) _gps_log_file.flush();
}

uint32_t SDLogger::getFileSize() {
    if (SD.exists(CAN_LOG_FILE)) {
        File f = SD.open(CAN_LOG_FILE);
        uint32_t size = f.size();
        f.close();
        return size;
    }
    return 0;
}
