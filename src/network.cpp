#include "network.h"

#include <WiFi.h>

#include <EspMQTTClient.h>
#include <ArduinoJson.h>
#include <ArduinoLog.h>

NetCom::NetCom(char *ssid, char *password, char *brokerIP, char *deviceName, short port)
    : m_MQTTclient(ssid, password, brokerIP, deviceName, port)
{
}

void NetCom::init()
{
#ifdef NETWORK_DEBUGGING
    m_MQTTclient.enableDebuggingMessages();
#endif
    m_MQTTclient.setKeepAlive(30);
    m_MQTTclient.enableMQTTPersistence();
    m_MQTTclient.enableLastWillMessage(m_statTopic.c_str(),"disconnected", true);
    m_MQTTclient.setMaxPacketSize(1024);
}

void NetCom::loop()
{
    m_MQTTclient.loop();
}

//-------------------------------------------------------------------------------------------------------------------
void NetCom::sendStatus()
{
    Log.noticeln("");
    Log.noticeln("---- Publish state ----");

    bool MQTTstatus = m_MQTTclient.publish(m_statTopic, "connected", true);

    if (MQTTstatus)
    {
        Log.noticeln("Publishing status sucessfull.");
    }
    else
    {
        Log.noticeln("Publishing status failed.");
        sendError(MQTT_ERROR);
    }
}

//-------------------------------------------------------------------------------------------------------------------
void NetCom::sendData(dataReading (&measurement)[32])
{
    StaticJsonDocument<256> JSONMeasurement;
    char buffer[256];

    Log.noticeln("---- Publish message ----");

    // build Json Object
    char clientStr[9];
    snprintf(clientStr, sizeof(clientStr), "%08x", (int)measurement[0].measurement);
    JSONMeasurement["clientID"] = clientStr;
    JSONMeasurement["messageID"] = (int)measurement[1].measurement;
    JSONMeasurement["status"] = (int)measurement[2].measurement;


    int arraySize = (int)measurement[3].measurement;
    for (int i = 4; i < arraySize; i++)
    {
        JSONMeasurement[sensorproperty[measurement[i].property]] = measurement[i].measurement;
    }


    serializeJson(JSONMeasurement, buffer);
    bool MQTTstatus = m_MQTTclient.publish(m_measurementTopic, buffer);

    if (MQTTstatus)
    {
        Log.noticeln("Message send sucessful after %Fs \n", millis() / 1000.0);
    }
    else
    {
        Log.noticeln("Faild to send message.");
        sendError(MQTT_ERROR);
    }
}

//-------------------------------------------------------------------------------------------------------------------
void NetCom::sendError(const uint8_t errorCode)
{
    bool MQTTstatus = false;

    switch (errorCode)
    {
    case TRANSMISSION_ERROR:
        MQTTstatus = m_MQTTclient.publish(m_errorTopic, "ESP-Now transmission error");
        break;

    case SENSOR_ERROR:
        MQTTstatus = m_MQTTclient.publish(m_errorTopic, "Sensor Error");
        break;

    case WIFI_ERROR:
        MQTTstatus = m_MQTTclient.publish(m_errorTopic, "WiFi error");
        break;

    case MQTT_ERROR:
        MQTTstatus = m_MQTTclient.publish(m_errorTopic, "MQTT transmission error");
        break;

    default:
        MQTTstatus = m_MQTTclient.publish(m_errorTopic, "Unkonwn Error");
    }

    if (MQTTstatus)
    {
        Log.noticeln("Error message send sucessful after %Fs \n", millis() / 1000.0);
    }
    else
    {
        Log.noticeln("Faild to send message.");
    }
}