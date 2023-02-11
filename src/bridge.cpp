/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-esp-now-wi-fi-web-server/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <esp_now.h>
#include <WiFi.h>

#include <ArduinoLog.h>
#include <SerialTransfer.h>

#include "credentials.h"
#include "data_structure.h"
#include "network.h"

// globale Variablen
static uint32_t statusPublishPreviousMillis = 0;
//static SensorBMP280 sensor;
static NetCom netCom((char *)SSID, (char *)PASSWORD, (char *)BROKER_IP, (char *)DEVICE_NAME, 1883);
SerialTransfer myTransfer;

// function Prototypes
void onConnectionEstablished();
static void error_hander(const uint8_t errorCode);
static void bootMessage();
static void asyncHandler(const uint16_t interval);
static void OnDataRecv(struct_message &incomingReadings);
static void serialHandler();









#define RXD2 16
#define TXD2 17

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ;
  Log.begin(LOG_LEVEL_NOTICE, &Serial);

  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  while (!Serial2)
    ;
  myTransfer.begin(Serial2);

  bootMessage();
  netCom.init();
}

void loop()
{
  netCom.loop();
  asyncHandler(PUBLISH_INTERVAL);
  serialHandler();
}


static void bootMessage()
{
  Log.noticeln("");
  Log.noticeln("---- MQTTBridge v1 ----");
}

void onConnectionEstablished()
{
  netCom.sendStatus();
}

static void asyncHandler(const uint16_t interval)
{
  const uint32_t currentMillis = millis();
  if (currentMillis - statusPublishPreviousMillis >= interval)
  {
    statusPublishPreviousMillis = currentMillis;

    //netCom.sendStatus();
    //sensorHandler();
  }
}



static void OnDataRecv(struct_message &incomingReadings)
{
  char clientStr[9];
  snprintf(clientStr, sizeof(clientStr), "%08x", incomingReadings.clientID);

  Log.noticeln("");
  Log.noticeln("---- Print Received Data ----");
  Log.noticeln("Client ID: %s, Message ID: %i", clientStr, incomingReadings.messageID);
  Log.noticeln("Status: %i", incomingReadings.status);
  Log.noticeln("Temperature: %F °C", incomingReadings.temperature);
  Log.noticeln("Humidity: %F", incomingReadings.humidity);
  Log.noticeln("Pressure: %F mbar", incomingReadings.pressure);
  Log.noticeln("Reduced pressure: %F mbar", incomingReadings.r_pressure);
  Log.noticeln("PM2,5 conentration: %i ug/m3", incomingReadings.pm25);
  Log.noticeln("Battery Voltage %F V", incomingReadings.battery);

  if (incomingReadings.status)
  {
    if (incomingReadings.status == 1)
    {
      error_hander(incomingReadings.status);
    }
    else
    {
      error_hander(incomingReadings.status);
      return;
    }
  }

  netCom.sendData(incomingReadings);
}

static void serialHandler()
{
  static struct_message incommingData;

  if (myTransfer.available())
  {
    // use this variable to keep track of how many
    // bytes we've processed from the receive buffer
    uint16_t recSize = 0;
    recSize = myTransfer.rxObj(incommingData, recSize);

    OnDataRecv(incommingData);
  }
}

static void error_hander(const uint8_t errorCode)
{
  Log.errorln("");
  Log.errorln("---- Error Occured ----");

  switch (errorCode)
  {
  case TRANSMISSION_ERROR:
    Log.noticeln("ESP-Now transmission error");
    break;

  case SENSOR_ERROR:
    Log.noticeln("Sensor Error");
    break;

  case WIFI_ERROR:
    Log.noticeln("WiFi error");
    break;

  case MQTT_ERROR:
    Log.noticeln("MQTT transmission error");
    break;
  default:
    Log.errorln("Unkonwn error code");
  }

  netCom.sendSensorError(errorCode);
}