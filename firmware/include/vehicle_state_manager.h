#ifndef VEHICLE_STATE_MANAGER_H
#define VEHICLE_STATE_MANAGER_H

#include <Arduino.h>
#include "types.h"

class VehicleStateManager {
public:
    VehicleStateManager();
    ~VehicleStateManager();

    void init();
    void update(const CanFrame& frame);
    VehicleState getState();
    void reset();

private:
    VehicleState _current_state;
    uint32_t _last_update;

    // Decoding functions for common CAN message IDs
    void decodeSpeed(const CanFrame& frame);
    void decodeRPM(const CanFrame& frame);
    void decodeThrottle(const CanFrame& frame);
    void decodeEngineStatus(const CanFrame& frame);
    void decodeFaultStatus(const CanFrame& frame);

    // Helper functions
    uint16_t extractBits(const uint8_t* data, uint8_t start_bit, uint8_t length);
    int16_t extractSignedBits(const uint8_t* data, uint8_t start_bit, uint8_t length);
};

#endif // VEHICLE_STATE_MANAGER_H
