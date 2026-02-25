#include <Arduino.h>
#include "config.h"
#include "logger.h"
#include "types.h"
#include "mcp2515_driver.h"
#include "gps_module.h"
#include "sd_logger.h"
#include "vehicle_state_manager.h"
#include "anomaly_detector.h"
#include "mqtt_client.h"

// Global objects
MCP2515Driver can_driver(CAN_CS_PIN, CAN_BITRATE);
GPSModule gps_module(GPS_RX_PIN, GPS_TX_PIN, GPS_BAUDRATE);
SDLogger sd_logger(SD_CS_PIN);
VehicleStateManager vehicle_state;
AnomalyDetector anomaly_detector;
MQTTClient mqtt_client;

// Timing variables
uint32_t last_log_time = 0;
uint32_t last_mqtt_time = 0;
uint32_t last_gps_time = 0;

void setup() {
    // Initialize logger first
    Logger::init();
    LOG_I("MAIN", "=== CAN Bus Data Logger Starting ===");

    // Initialize CAN bus
    LOG_I("MAIN", "Initializing CAN bus...");
    if (can_driver.init() != MCP2515Driver::ERROR_OK) {
        LOG_E("MAIN", "CAN initialization failed!");
        while (1) { delay(1000); }
    }
    can_driver.setListenOnly(true);
    can_driver.enableInterrupt();

    // Initialize GPS
    LOG_I("MAIN", "Initializing GPS...");
    gps_module.init();

    // Initialize SD card
    LOG_I("MAIN", "Initializing SD card...");
    if (!sd_logger.init()) {
        LOG_W("MAIN", "SD card initialization failed!");
    }

    // Initialize vehicle state manager
    vehicle_state.init();

    // Initialize anomaly detector
    anomaly_detector.init();

    // Initialize MQTT
    LOG_I("MAIN", "Initializing MQTT...");
    if (!mqtt_client.init(WIFI_SSID, WIFI_PASSWORD, MQTT_BROKER, MQTT_PORT)) {
        LOG_W("MAIN", "MQTT initialization failed, will retry in loop");
    }

    LOG_I("MAIN", "Setup complete!");
}

void loop() {
    uint32_t current_time = millis();

    // ===== CAN BUS HANDLING =====
    CanFrame frame;
    if (can_driver.readFrame(frame) == MCP2515Driver::ERROR_OK) {
        LOG_D("MAIN", "CAN RX: ID=0x%03X DLC=%d", frame.id, frame.dlc);

        // Update vehicle state
        vehicle_state.update(frame);
        VehicleState state = vehicle_state.getState();

        // Anomaly detection
        anomaly_detector.update(frame, state);

        // Log to SD card
        if (current_time - last_log_time > SD_LOG_INTERVAL) {
            sd_logger.logCANFrame(frame);
            sd_logger.logVehicleState(state);

            if (gps_module.hasValidFix()) {
                GpsData gps = gps_module.getLatestData();
                sd_logger.logGPSData(gps);
            }

            last_log_time = current_time;
        }

        // Check for anomalies
        if (anomaly_detector.hasAnomaly()) {
            Anomaly anom = anomaly_detector.getLatestAnomaly();
            LOG_W("MAIN", "ANOMALY DETECTED: %s", anom.description);
            sd_logger.logAnomaly(anom);

            // Publish anomaly via MQTT
            if (mqtt_client.isConnected()) {
                mqtt_client.publishAnomaly(anom);
            }
        }
    }

    // ===== GPS HANDLING =====
    if (current_time - last_gps_time > 1000) {
        gps_module.update();
        last_gps_time = current_time;
    }

    // ===== MQTT PUBLISHING =====
    mqtt_client.update();

    if (mqtt_client.isConnected() && current_time - last_mqtt_time > MQTT_PUBLISH_INTERVAL) {
        VehicleState state = vehicle_state.getState();
        mqtt_client.publishVehicleState(state);

        if (gps_module.hasValidFix()) {
            GpsData gps = gps_module.getLatestData();
            mqtt_client.publishGPSData(gps);
        }

        last_mqtt_time = current_time;
    }

    // Brief delay to prevent watchdog triggers
    delay(10);
}
