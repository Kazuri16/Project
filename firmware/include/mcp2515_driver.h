#ifndef MCP2515_DRIVER_H
#define MCP2515_DRIVER_H

#include <Arduino.h>
#include <SPI.h>
#include "types.h"

class MCP2515Driver {
public:
    enum ErrorCode {
        ERROR_OK = 0,
        ERROR_FAIL = 1,
        ERROR_ALLTXBUSY = 2,
        ERROR_ALLOCMEM = 3,
        ERROR_NOMSG = 4
    };

    MCP2515Driver(uint8_t cs_pin, uint32_t bitrate = 500000);
    ~MCP2515Driver();

    // Initialization
    ErrorCode init();
    ErrorCode setBitrate(uint32_t bitrate);
    ErrorCode setListenOnly(bool enable);

    // Frame operations
    ErrorCode readFrame(CanFrame& frame);
    ErrorCode writeFrame(const CanFrame& frame);

    // Status
    uint8_t getStatus();
    void enableInterrupt();
    void disableInterrupt();

private:
    uint8_t _cs_pin;
    uint32_t _bitrate;
    SPISettings _spi_settings;

    // Register operations
    uint8_t readRegister(uint8_t address);
    void writeRegister(uint8_t address, uint8_t value);
    void modifyRegister(uint8_t address, uint8_t mask, uint8_t data);

    // Bitrate setup
    ErrorCode setBitrateCfg(uint32_t bitrate);

    // SPI commands
    uint8_t sendCommand(uint8_t cmd);
    uint8_t readStatus();
    void readFrame(uint8_t buffer_addr, CanFrame& frame);
    void writeFrame(uint8_t buffer_addr, const CanFrame& frame);
};

#endif // MCP2515_DRIVER_H
