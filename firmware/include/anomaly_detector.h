#ifndef ANOMALY_DETECTOR_H
#define ANOMALY_DETECTOR_H

#include <Arduino.h>
#include "types.h"

class AnomalyDetector {
public:
    AnomalyDetector();
    ~AnomalyDetector();

    void init();
    void update(const CanFrame& frame, const VehicleState& state);
    bool hasAnomaly();
    Anomaly getLatestAnomaly();

private:
    static const uint8_t MAX_HISTORY = 32;

    Anomaly _latest_anomaly;
    bool _anomaly_flagged;

    // Rule-based detection
    bool checkRPMSpike(const VehicleState& state);
    bool checkSpeedThreshold(const VehicleState& state);
    bool checkCANFrequency(const CanFrame& frame);
    bool checkEngineInconsistency(const VehicleState& state);

    // Statistical detection
    bool checkStatisticalDeviation(const VehicleState& state);

    // State history for statistical analysis
    struct History {
        uint16_t rpm;
        uint16_t speed;
        uint32_t timestamp;
    } _history[MAX_HISTORY];
    uint8_t _history_index;

    // Previous state for spike detection
    VehicleState _prev_state;
    uint32_t _last_check_time;

    // Message frequency tracking
    struct MessageTrack {
        uint32_t can_id;
        uint32_t last_seen;
        uint32_t frequency;
    } _message_track[16];
    uint8_t _tracked_messages;

    // Helper functions
    double calculateMean(uint16_t* values, uint8_t count);
    double calculateVariance(uint16_t* values, uint8_t count, double mean);
    void recordHistory(const VehicleState& state);
    void createAnomaly(uint8_t type, const char* description, uint8_t severity, uint16_t can_id);
};

#endif // ANOMALY_DETECTOR_H
