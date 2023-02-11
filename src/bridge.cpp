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
static uint32_t statusPublishPreviousMillis = 0;  //satus variave for asyncronus execution
static NetCom netCom((char *)SSID, (char *)PASSWORD, (char *)BROKER_IP, (char *)DEVICE_NAME, 1883);  //handls all the networking
SerialTransfer myTransfer;

// function Prototypes
void onConnectionEstablished();
static void error_hander(const uint8_t errorCode);
static void bootMessage();
static void asyncHandler(const uint16_t interval);
static void OnDataRecv(dataReading (&incomingReadings)[32]);
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



static void OnDataRecv(dataReading (&incomingReadings)[32])
{
  char clientStr[9];
  snprintf(clientStr, sizeof(clientStr), "%08x", (int)incomingReadings[0].measurement);

  Log.noticeln("");
  Log.noticeln("---- Print Received Data ----");
  Log.noticeln("Client ID: %s, Message ID: %i", clientStr, (int)incomingReadings[1].measurement);
  Log.noticeln("Status: %i", (int)incomingReadings[2].measurement);

  int arraySize = (int)incomingReadings[3].measurement;
  for (int i = 4; i < arraySize; i++)
  {
      Log.noticeln("Proerty ID: %i, Measurement: %F", incomingReadings[i].property, incomingReadings[i].measurement);
  }
  
  Log.noticeln("");


  if ((int)incomingReadings[2].measurement)
  {
    if ((int)incomingReadings[2].measurement == TRANSMISSION_ERROR)  // A previous transmission from the sensor had an error | execute error handler and then send the data
    {
      error_hander((int)incomingReadings[2].measurement);
    }
    else
    {
      error_hander((int)incomingReadings[2].measurement);  // A error other than a transmision error occured | execute the error handler and do NOT send the data
      return;
    }
  }

  netCom.sendData(incomingReadings);
}

static void serialHandler()
{
  static dataReading incommingData[32];  // Buffer for incoming Data / max 32 measurements

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

  netCom.sendError(errorCode);
}