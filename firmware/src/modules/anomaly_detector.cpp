#include "anomaly_detector.h"
#include "logger.h"
#include <cmath>
#include <cstring>

AnomalyDetector::AnomalyDetector()
    : _anomaly_flagged(false), _history_index(0), _last_check_time(0), _tracked_messages(0) {
    memset(&_latest_anomaly, 0, sizeof(Anomaly));
    memset(&_prev_state, 0, sizeof(VehicleState));
    memset(_history, 0, sizeof(_history));
    memset(_message_track, 0, sizeof(_message_track));
}

AnomalyDetector::~AnomalyDetector() {}

void AnomalyDetector::init() {
    LOG_I("ANOMALY", "Anomaly Detector initialized");
}

void AnomalyDetector::update(const CanFrame& frame, const VehicleState& state) {
    _anomaly_flagged = false;

    // Run detections
    if (checkRPMSpike(state)) {
        _anomaly_flagged = true;
        return;
    }

    if (checkSpeedThreshold(state)) {
        _anomaly_flagged = true;
        return;
    }

    if (checkCANFrequency(frame)) {
        _anomaly_flagged = true;
        return;
    }

    if (checkEngineInconsistency(state)) {
        _anomaly_flagged = true;
        return;
    }

    if (checkStatisticalDeviation(state)) {
        _anomaly_flagged = true;
        return;
    }

    recordHistory(state);
    _prev_state = state;
}

bool AnomalyDetector::checkRPMSpike(const VehicleState& state) {
    if (_prev_state.rpm == 0) {
        _prev_state.rpm = state.rpm;
        return false;
    }

    uint16_t rpm_delta = abs((int32_t)state.rpm - (int32_t)_prev_state.rpm);

    if (rpm_delta > RPM_SPIKE_THRESHOLD) {
        char desc[128];
        snprintf(desc, sizeof(desc), "RPM spike detected: %u RPM -> %u RPM (delta: %u)",
                 _prev_state.rpm, state.rpm, rpm_delta);
        createAnomaly(ANOMALY_RPM_SPIKE, desc, 2, 0x101);
        return true;
    }

    return false;
}

bool AnomalyDetector::checkSpeedThreshold(const VehicleState& state) {
    if (state.speed > SPEED_MAX_THRESHOLD) {
        char desc[128];
        snprintf(desc, sizeof(desc), "Speed threshold exceeded: %u km/h (max: %u)",
                 state.speed, SPEED_MAX_THRESHOLD);
        createAnomaly(ANOMALY_SPEED_EXCEED, desc, 2, 0x100);
        return true;
    }

    return false;
}

bool AnomalyDetector::checkCANFrequency(const CanFrame& frame) {
    uint32_t current_time = millis();

    for (uint8_t i = 0; i < _tracked_messages; i++) {
        if (_message_track[i].can_id == frame.id) {
            uint32_t time_delta = current_time - _message_track[i].last_seen;

            if (time_delta > CAN_MSG_TIMEOUT && _message_track[i].last_seen > 0) {
                char desc[128];
                snprintf(desc, sizeof(desc), "CAN message frequency anomaly: ID=0x%03X, last seen %lu ms ago",
                         frame.id, time_delta);
                createAnomaly(ANOMALY_CAN_FREQUENCY, desc, 1, frame.id);
                return true;
            }

            _message_track[i].last_seen = current_time;
            _message_track[i].frequency++;
            return false;
        }
    }

    // Add new message ID
    if (_tracked_messages < 16) {
        _message_track[_tracked_messages].can_id = frame.id;
        _message_track[_tracked_messages].last_seen = current_time;
        _message_track[_tracked_messages].frequency = 1;
        _tracked_messages++;
    }

    return false;
}

bool AnomalyDetector::checkEngineInconsistency(const VehicleState& state) {
    // Engine OFF but RPM > 0
    if (state.engine_status == 0 && state.rpm > 0) {
        char desc[128];
        snprintf(desc, sizeof(desc), "Engine inconsistency: Engine OFF but RPM = %u", state.rpm);
        createAnomaly(ANOMALY_ENGINE_INCONSISTENCY, desc, 3, 0x103);
        return true;
    }

    return false;
}

bool AnomalyDetector::checkStatisticalDeviation(const VehicleState& state) {
    if (_history_index < 10) return false;  // Need enough history

    // Calculate statistics for RPM
    uint16_t rpm_values[MAX_HISTORY];
    for (uint8_t i = 0; i < _history_index; i++) {
        rpm_values[i] = _history[i].rpm;
    }

    double mean = calculateMean(rpm_values, _history_index);
    double variance = calculateVariance(rpm_values, _history_index, mean);
    double std_dev = sqrt(variance);

    // Check 3-sigma rule
    double deviation = abs((double)state.rpm - mean);
    if (deviation > 3 * std_dev && std_dev > 0) {
        char desc[128];
        snprintf(desc, sizeof(desc), "Statistical deviation detected: RPM=%u (mean=%.0f, 3σ=%.0f)",
                 state.rpm, mean, 3 * std_dev);
        createAnomaly(ANOMALY_STATISTICAL_DEVIATION, desc, 1, 0x101);
        return true;
    }

    return false;
}

double AnomalyDetector::calculateMean(uint16_t* values, uint8_t count) {
    if (count == 0) return 0;
    uint32_t sum = 0;
    for (uint8_t i = 0; i < count; i++) {
        sum += values[i];
    }
    return (double)sum / count;
}

double AnomalyDetector::calculateVariance(uint16_t* values, uint8_t count, double mean) {
    if (count == 0) return 0;
    double sum = 0;
    for (uint8_t i = 0; i < count; i++) {
        double diff = values[i] - mean;
        sum += diff * diff;
    }
    return sum / count;
}

void AnomalyDetector::recordHistory(const VehicleState& state) {
    _history[_history_index % MAX_HISTORY].rpm = state.rpm;
    _history[_history_index % MAX_HISTORY].speed = state.speed;
    _history[_history_index % MAX_HISTORY].timestamp = state.timestamp;
    _history_index++;
}

void AnomalyDetector::createAnomaly(uint8_t type, const char* description, uint8_t severity, uint16_t can_id) {
    _latest_anomaly.type = type;
    _latest_anomaly.severity = severity;
    _latest_anomaly.can_id = can_id;
    _latest_anomaly.timestamp = millis();
    strncpy(_latest_anomaly.description, description, sizeof(_latest_anomaly.description) - 1);
    _latest_anomaly.description[sizeof(_latest_anomaly.description) - 1] = '\0';

    LOG_W("ANOMALY", "[%u] %s", type, description);
}

bool AnomalyDetector::hasAnomaly() {
    return _anomaly_flagged;
}

Anomaly AnomalyDetector::getLatestAnomaly() {
    return _latest_anomaly;
}
