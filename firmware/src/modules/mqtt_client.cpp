#include "mqtt_client.h"
#include "logger.h"
#include <ArduinoJson.h>

MQTTClient::MQTTClient() : _mqtt_client(_wifi_client), _connected(false), _last_reconnect(0) {}

MQTTClient::~MQTTClient() {
    disconnect();
}

bool MQTTClient::init(const char* ssid, const char* password, const char* broker, uint16_t port) {
    // Connect to WiFi
    LOG_I("MQTT", "Connecting to WiFi: %s", ssid);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    uint32_t start_time = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start_time < WIFI_CONNECT_TIMEOUT) {
        delay(500);
    }

    if (WiFi.status() != WL_CONNECTED) {
        LOG_E("MQTT", "WiFi connection failed");
        return false;
    }

    LOG_I("MQTT", "WiFi connected. IP: %s", WiFi.localIP().toString().c_str());

    // Connect to MQTT broker
    _mqtt_client.setServer(broker, port);
    reconnect();

    return _connected;
}

void MQTTClient::disconnect() {
    if (_mqtt_client.connected()) {
        _mqtt_client.disconnect();
    }
    WiFi.disconnect();
    _connected = false;
}

bool MQTTClient::isConnected() {
    return _mqtt_client.connected();
}

bool MQTTClient::publishCANData(const CanFrame& frame) {
    if (!isConnected()) return false;

    StaticJsonDocument<256> doc;
    doc["timestamp"] = frame.timestamp;
    doc["can_id"] = "0x" + String(frame.id, HEX);
    doc["dlc"] = frame.dlc;

    JsonArray data = doc.createNestedArray("data");
    for (int i = 0; i < frame.dlc; i++) {
        data.add("0x" + String(frame.data[i], HEX));
    }

    char buffer[512];
    serializeJson(doc, buffer);

    return _mqtt_client.publish("vehicle/data", buffer);
}

bool MQTTClient::publishVehicleState(const VehicleState& state) {
    if (!isConnected()) return false;

    StaticJsonDocument<256> doc;
    doc["timestamp"] = state.timestamp;
    doc["speed"] = state.speed;
    doc["rpm"] = state.rpm;
    doc["throttle"] = state.throttle;
    doc["gear"] = state.gear;
    doc["engine_status"] = state.engine_status ? "ON" : "OFF";
    doc["fault"] = state.fault_status ? true : false;

    char buffer[512];
    serializeJson(doc, buffer);

    return _mqtt_client.publish("vehicle/state", buffer);
}

bool MQTTClient::publishAnomaly(const Anomaly& anomaly) {
    if (!isConnected()) return false;

    StaticJsonDocument<256> doc;
    doc["type"] = anomaly.type;
    doc["description"] = anomaly.description;
    doc["timestamp"] = anomaly.timestamp;
    doc["severity"] = anomaly.severity;
    doc["can_id"] = "0x" + String(anomaly.can_id, HEX);

    char buffer[512];
    serializeJson(doc, buffer);

    return _mqtt_client.publish("vehicle/anomaly", buffer);
}

bool MQTTClient::publishGPSData(const GpsData& gps) {
    if (!isConnected()) return false;

    StaticJsonDocument<256> doc;
    doc["timestamp"] = gps.timestamp;
    doc["latitude"] = serialized(String(gps.latitude, 6));
    doc["longitude"] = serialized(String(gps.longitude, 6));
    doc["altitude"] = gps.altitude;
    doc["speed"] = gps.speed;
    doc["fix_quality"] = gps.fix_quality;
    doc["satellites"] = gps.satellites;

    char buffer[512];
    serializeJson(doc, buffer);

    return _mqtt_client.publish("vehicle/location", buffer);
}

void MQTTClient::update() {
    if (!_mqtt_client.connected()) {
        if (millis() - _last_reconnect > RECONNECT_INTERVAL) {
            reconnect();
            _last_reconnect = millis();
        }
    } else {
        _mqtt_client.loop();
    }
}

void MQTTClient::reconnect() {
    if (_mqtt_client.connect("ESP32-CANLogger", MQTT_USER, MQTT_PASS)) {
        LOG_I("MQTT", "Connected to MQTT broker");
        _connected = true;
        _mqtt_client.subscribe("vehicle/commands");
    } else {
        LOG_W("MQTT", "MQTT connection failed, rc=%d", _mqtt_client.state());
        _connected = false;
    }
}
