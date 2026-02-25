#include "vehicle_state_manager.h"
#include "logger.h"

VehicleStateManager::VehicleStateManager() {
    memset(&_current_state, 0, sizeof(VehicleState));
    _last_update = 0;
}

VehicleStateManager::~VehicleStateManager() {}

void VehicleStateManager::init() {
    LOG_I("VSTATE", "Vehicle State Manager initialized");
}

void VehicleStateManager::update(const CanFrame& frame) {
    _current_state.timestamp = frame.timestamp;

    // Common OBD-II CAN IDs (vehicle-specific mapping needed)
    switch (frame.id) {
        case 0x100:  // Speed (example)
            decodeSpeed(frame);
            break;
        case 0x101:  // RPM (example)
            decodeRPM(frame);
            break;
        case 0x102:  // Throttle (example)
            decodeThrottle(frame);
            break;
        case 0x103:  // Engine Status (example)
            decodeEngineStatus(frame);
            break;
        case 0x10A:  // Fault Status (example)
            decodeFaultStatus(frame);
            break;
        default:
            break;
    }

    _last_update = millis();
}

void VehicleStateManager::decodeSpeed(const CanFrame& frame) {
    if (frame.dlc < 2) return;
    // Example: Speed in bytes 0-1, LSB first, 0.1 km/h per unit
    uint16_t raw = (frame.data[1] << 8) | frame.data[0];
    _current_state.speed = raw / 10;
}

void VehicleStateManager::decodeRPM(const CanFrame& frame) {
    if (frame.dlc < 2) return;
    // Example: RPM in bytes 0-1, LSB first, 0.25 RPM per unit
    uint16_t raw = (frame.data[1] << 8) | frame.data[0];
    _current_state.rpm = raw / 4;
}

void VehicleStateManager::decodeThrottle(const CanFrame& frame) {
    if (frame.dlc < 1) return;
    // Example: Throttle position in byte 0, 0-255 -> 0-100%
    _current_state.throttle = (frame.data[0] * 100) / 255;
}

void VehicleStateManager::decodeEngineStatus(const CanFrame& frame) {
    if (frame.dlc < 1) return;
    // Example: Engine status in bit 0
    _current_state.engine_status = (frame.data[0] & 0x01) ? 1 : 0;
    // Gear in bits 1-3
    _current_state.gear = (frame.data[0] >> 1) & 0x07;
}

void VehicleStateManager::decodeFaultStatus(const CanFrame& frame) {
    if (frame.dlc < 1) return;
    // Example: Fault flag in bit 0
    _current_state.fault_status = (frame.data[0] & 0x01) ? 1 : 0;
}

VehicleState VehicleStateManager::getState() {
    return _current_state;
}

void VehicleStateManager::reset() {
    memset(&_current_state, 0, sizeof(VehicleState));
    _last_update = 0;
}

uint16_t VehicleStateManager::extractBits(const uint8_t* data, uint8_t start_bit, uint8_t length) {
    uint16_t result = 0;
    for (uint8_t i = 0; i < length; i++) {
        uint8_t bit_pos = start_bit + i;
        uint8_t byte_index = bit_pos / 8;
        uint8_t bit_index = bit_pos % 8;
        uint8_t bit = (data[byte_index] >> bit_index) & 0x01;
        result |= (bit << i);
    }
    return result;
}

int16_t VehicleStateManager::extractSignedBits(const uint8_t* data, uint8_t start_bit, uint8_t length) {
    int16_t result = extractBits(data, start_bit, length);
    if (result & (1 << (length - 1))) {
        result |= ~((1 << length) - 1);
    }
    return result;
}
