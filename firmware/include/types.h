#ifndef TYPES_H
#define TYPES_H

#include <cstdint>
#include <cstring>

// ===== CAN FRAME =====
typedef struct {
    uint32_t id;
    uint8_t dlc;
    uint8_t data[8];
    uint32_t timestamp;
} CanFrame;

// ===== GPS DATA =====
typedef struct {
    double latitude;
    double longitude;
    double speed;
    double altitude;
    uint32_t timestamp;
    uint8_t fix_quality;
    uint8_t satellites;
} GpsData;

// ===== VEHICLE STATE =====
typedef struct {
    uint16_t speed;
    uint16_t rpm;
    uint8_t throttle;
    uint8_t gear;
    uint8_t engine_status;
    uint8_t fault_status;
    uint32_t timestamp;
} VehicleState;

// ===== ANOMALY DETECTION =====
typedef struct {
    uint8_t type;
    char description[128];
    uint32_t timestamp;
    uint16_t can_id;
    uint8_t severity; // 1: LOW, 2: MEDIUM, 3: HIGH
} Anomaly;

// ===== SENSOR READING =====
typedef struct {
    uint32_t timestamp;
    uint32_t can_id;
    uint8_t dlc;
    uint8_t data[8];
    double latitude;
    double longitude;
    uint16_t speed;
    uint16_t rpm;
} SensorReading;

// ===== ANOMALY TYPES =====
enum AnomalyType {
    ANOMALY_RPM_SPIKE = 1,
    ANOMALY_SPEED_EXCEED = 2,
    ANOMALY_CAN_FREQUENCY = 3,
    ANOMALY_MISSING_MESSAGE = 4,
    ANOMALY_ENGINE_INCONSISTENCY = 5,
    ANOMALY_STATISTICAL_DEVIATION = 6
};

#endif // TYPES_H
