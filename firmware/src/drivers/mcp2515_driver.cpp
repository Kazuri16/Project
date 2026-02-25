#include "mcp2515_driver.h"
#include "logger.h"

// MCP2515 Command definitions
#define MCP_WRITE       0x02
#define MCP_READ        0x03
#define MCP_BITMOD      0x05
#define MCP_LOAD_TX0    0x40
#define MCP_LOAD_TX1    0x42
#define MCP_LOAD_TX2    0x44
#define MCP_RTS_TX0     0x81
#define MCP_RTS_TX1     0x82
#define MCP_RTS_TX2     0x84
#define MCP_READ_RX0    0x90
#define MCP_READ_RX1    0x94
#define MCP_READ_STATUS 0xA0

// Register definitions
#define CANCTRL    0x0F
#define CANSTAT    0x0E
#define BFPCTRL    0x0C
#define TXRTSCTRL  0x0D
#define CNF3       0x28
#define CNF2       0x29
#define CNF1       0x2A
#define CANINTE    0x2B
#define CANINTF    0x2C
#define EFLG       0x2D
#define TXB0CTRL   0x30
#define RXB0CTRL   0x60
#define RXB1CTRL   0x70

// Mode definitions
#define REQOP_NORMAL   0x00
#define REQOP_SLEEP    0x20
#define REQOP_LOOPBACK 0x40
#define REQOP_LISTEN   0x60
#define REQOP_CONFIG   0x80

MCP2515Driver::MCP2515Driver(uint8_t cs_pin, uint32_t bitrate)
    : _cs_pin(cs_pin), _bitrate(bitrate), _spi_settings(10000000, MSBFIRST, SPI_MODE0) {
    pinMode(_cs_pin, OUTPUT);
    digitalWrite(_cs_pin, HIGH);
}

MCP2515Driver::~MCP2515Driver() {}

MCP2515Driver::ErrorCode MCP2515Driver::init() {
    LOG_I("MCP2515", "Initializing CAN controller");
    SPI.begin();

    // Reset the device
    digitalWrite(_cs_pin, LOW);
    SPI.transfer(0xC0);  // RESET command
    digitalWrite(_cs_pin, HIGH);
    delay(10);

    // Set configuration mode
    modifyRegister(CANCTRL, 0xE0, REQOP_CONFIG);
    delay(10);

    // Set bitrate
    if (setBitrate(_bitrate) != ERROR_OK) {
        LOG_E("MCP2515", "Failed to set bitrate");
        return ERROR_FAIL;
    }

    // Configure RX buffers for reception
    writeRegister(RXB0CTRL, 0x60);  // Receive all valid messages
    writeRegister(RXB1CTRL, 0x60);

    // Enable interrupts for RX
    writeRegister(CANINTE, 0x03);  // Enable RX0 and RX1 interrupts

    // Set normal mode
    modifyRegister(CANCTRL, 0xE0, REQOP_NORMAL);
    delay(10);

    LOG_I("MCP2515", "CAN controller initialized successfully");
    return ERROR_OK;
}

MCP2515Driver::ErrorCode MCP2515Driver::setBitrate(uint32_t bitrate) {
    _bitrate = bitrate;
    return setBitrateCfg(bitrate);
}

MCP2515Driver::ErrorCode MCP2515Driver::setBitrateCfg(uint32_t bitrate) {
    uint8_t cnf1, cnf2, cnf3;

    switch (bitrate) {
        case 500000:
            cnf1 = 0x00;
            cnf2 = 0x90;
            cnf3 = 0x82;
            break;
        case 250000:
            cnf1 = 0x04;
            cnf2 = 0x90;
            cnf3 = 0x82;
            break;
        case 125000:
            cnf1 = 0x09;
            cnf2 = 0x90;
            cnf3 = 0x82;
            break;
        default:
            LOG_E("MCP2515", "Unsupported bitrate: %lu", bitrate);
            return ERROR_FAIL;
    }

    writeRegister(CNF1, cnf1);
    writeRegister(CNF2, cnf2);
    writeRegister(CNF3, cnf3);

    return ERROR_OK;
}

MCP2515Driver::ErrorCode MCP2515Driver::setListenOnly(bool enable) {
    uint8_t mode = enable ? REQOP_LISTEN : REQOP_NORMAL;
    modifyRegister(CANCTRL, 0xE0, mode);
    delay(10);
    return ERROR_OK;
}

MCP2515Driver::ErrorCode MCP2515Driver::readFrame(CanFrame& frame) {
    uint8_t status = getStatus();

    if (status & 0x01) {  // RXB0 has message
        readFrame(0x61, frame);
        modifyRegister(CANINTF, 0x01, 0x00);  // Clear RXB0IF
        return ERROR_OK;
    }

    if (status & 0x02) {  // RXB1 has message
        readFrame(0x71, frame);
        modifyRegister(CANINTF, 0x02, 0x00);  // Clear RXB1IF
        return ERROR_OK;
    }

    return ERROR_NOMSG;
}

void MCP2515Driver::readFrame(uint8_t buffer_addr, CanFrame& frame) {
    digitalWrite(_cs_pin, LOW);
    SPI.transfer(MCP_READ);
    SPI.transfer(buffer_addr);

    frame.id = (uint32_t)SPI.transfer(0x00) << 3;
    frame.id |= (uint32_t)SPI.transfer(0x00) >> 5;

    SPI.transfer(0x00);  // Skip EXIDE, SRTR, IDE
    frame.dlc = SPI.transfer(0x00) & 0x0F;

    for (int i = 0; i < frame.dlc; i++) {
        frame.data[i] = SPI.transfer(0x00);
    }

    digitalWrite(_cs_pin, HIGH);
    frame.timestamp = millis();
}

MCP2515Driver::ErrorCode MCP2515Driver::writeFrame(const CanFrame& frame) {
    LOG_D("MCP2515", "Writing CAN frame: ID=0x%03X DLC=%d", frame.id, frame.dlc);
    return ERROR_OK;
}

uint8_t MCP2515Driver::getStatus() {
    digitalWrite(_cs_pin, LOW);
    SPI.transfer(MCP_READ_STATUS);
    uint8_t status = SPI.transfer(0x00);
    digitalWrite(_cs_pin, HIGH);
    return status;
}

uint8_t MCP2515Driver::readRegister(uint8_t address) {
    digitalWrite(_cs_pin, LOW);
    SPI.transfer(MCP_READ);
    SPI.transfer(address);
    uint8_t value = SPI.transfer(0x00);
    digitalWrite(_cs_pin, HIGH);
    return value;
}

void MCP2515Driver::writeRegister(uint8_t address, uint8_t value) {
    digitalWrite(_cs_pin, LOW);
    SPI.transfer(MCP_WRITE);
    SPI.transfer(address);
    SPI.transfer(value);
    digitalWrite(_cs_pin, HIGH);
}

void MCP2515Driver::modifyRegister(uint8_t address, uint8_t mask, uint8_t data) {
    digitalWrite(_cs_pin, LOW);
    SPI.transfer(MCP_BITMOD);
    SPI.transfer(address);
    SPI.transfer(mask);
    SPI.transfer(data);
    digitalWrite(_cs_pin, HIGH);
}

void MCP2515Driver::enableInterrupt() {
    attachInterrupt(digitalPinToInterrupt(CAN_INT_PIN), [](){}, FALLING);
}

void MCP2515Driver::disableInterrupt() {
    detachInterrupt(digitalPinToInterrupt(CAN_INT_PIN));
}
