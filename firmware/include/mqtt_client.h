#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "types.h"

class MQTTClient {
public:
    MQTTClient();
    ~MQTTClient();

    bool init(const char* ssid, const char* password, const char* broker, uint16_t port);
    void disconnect();
    bool isConnected();

    bool publishCANData(const CanFrame& frame);
    bool publishVehicleState(const VehicleState& state);
    bool publishAnomaly(const Anomaly& anomaly);
    bool publishGPSData(const GpsData& gps);

    void update();

private:
    WiFiClient _wifi_client;
    PubSubClient _mqtt_client;

    bool _connected;
    uint32_t _last_reconnect;

    const uint32_t RECONNECT_INTERVAL = 5000;  // 5 seconds

    void onMessageReceived(char* topic, byte* payload, unsigned int length);
    void reconnect();
};

#endif // MQTT_CLIENT_H
