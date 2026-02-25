#ifndef CONFIG_H
#define CONFIG_H

// ===== CAN BUS CONFIGURATION =====
#define CAN_BITRATE 500000  // 500 kbps
#define CAN_CS_PIN 5        // GPIO5 for MCP2515 chip select
#define CAN_INT_PIN 4       // GPIO4 for interrupt

// ===== GPS CONFIGURATION =====
#define GPS_RX_PIN 16       // GPIO16 UART2 RX
#define GPS_TX_PIN 17       // GPIO17 UART2 TX
#define GPS_BAUDRATE 9600

// ===== SD CARD CONFIGURATION =====
#define SD_CS_PIN 2         // GPIO2 for SD card chip select
#define SD_LOG_INTERVAL 5000 // Log interval in ms

// ===== WIFI CONFIGURATION =====
#define WIFI_SSID "your-ssid"
#define WIFI_PASSWORD "your-password"
#define WIFI_CONNECT_TIMEOUT 10000

// ===== MQTT CONFIGURATION =====
#define MQTT_BROKER "your-broker-ip"
#define MQTT_PORT 1883
#define MQTT_USER "mqtt-user"
#define MQTT_PASS "mqtt-password"
#define MQTT_PUBLISH_INTERVAL 10000

// ===== ANOMALY DETECTION =====
#define RPM_SPIKE_THRESHOLD 500       // RPM change threshold
#define SPEED_MAX_THRESHOLD 200       // Max speed km/h
#define CAN_MSG_TIMEOUT 5000          // ms

// ===== POWER MANAGEMENT =====
#define DEEP_SLEEP_ENABLED 0
#define DEEP_SLEEP_DURATION 60000000  // 60 seconds in microseconds

// ===== LOGGING =====
#define LOG_BUFFER_SIZE 256
#define LOG_ENABLE 1
#define LOG_LEVEL 2  // 0: ERROR, 1: WARN, 2: INFO, 3: DEBUG

// ===== DATA STRUCTURES =====
#define MAX_CAN_ID 0x7FF
#define MAX_DLC 8
#define BUFFER_SIZE 512

#endif // CONFIG_H
