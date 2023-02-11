#pragma once

#include <EspMQTTClient.h>
#include "data_structure.h"

class NetCom
{
private:
    const char* device_name = DEVICE_NAME;
    const String m_statTopic = "/devices/" + String(DEVICE_NAME) + "/status";
    const String m_errorTopic = "/devices/" + String(DEVICE_NAME) + "/errors";
    const String m_measurementTopic = "/logger/" + String(DEVICE_NAME) + "/measurement";

    EspMQTTClient m_MQTTclient;

public:
    NetCom(char* ssid, char* password, char* brokerIP, char* deviceName, short port);
    void init();
    void loop();
    void sendStatus();
    void sendData(struct_message &measurement);
    void sendSensorError(const uint8_t errorCode);
};