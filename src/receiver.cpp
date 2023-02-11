#include <ESP8266WiFi.h>
#include <espnow.h>

#include <ArduinoLog.h>
#include <SerialTransfer.h>

#include "data_structure.h"

// globale Variablen
struct_message incomingReadings;
SerialTransfer myTransfer;


// callback function that will be executed when data is received
void OnDataRecv(uint8_t *mac_addr, uint8_t *incomingData, uint8_t len)
{
  // Copies the sender mac address to a string
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Log.noticeln("Packet received from: %s", macStr);

  
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));

  char clientStr[9];
  snprintf(clientStr, sizeof(clientStr), "%08x", incomingReadings.clientID);

  Log.noticeln("Client ID: %s, Message ID: %i", clientStr, incomingReadings.messageID);
  Log.noticeln("Status: %i", incomingReadings.status);
  Log.noticeln("Temperature: %F °C", incomingReadings.temperature);
  Log.noticeln("Humidity: %F", incomingReadings.humidity);
  Log.noticeln("Pressure: %F mbar", incomingReadings.pressure);
  Log.noticeln("Reduced pressure: %F mbar", incomingReadings.r_pressure);
  Log.noticeln("PM2,5 conentration: %i ug/m3", incomingReadings.pm25);
  Log.noticeln("Battery voltage: %F v", incomingReadings.battery);
  Log.noticeln("");

  uint16_t sendSize = 0;
  sendSize = myTransfer.txObj(incomingReadings, sendSize);
  myTransfer.sendData(sendSize);
}

void setupESPnow()
{
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
    if (esp_now_init() != 0)
    {
        Log.errorln("Error initializing ESP-NOW");
        return;
    }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
}
 
void setup() {
  // Init Serial Monitor
    Serial.begin(115200);
    while (!Serial);
    Log.begin(LOG_LEVEL_NOTICE, &Serial);
    Log.noticeln("MAC-Address: %s", WiFi.macAddress().c_str());

    myTransfer.begin(Serial);

    setupESPnow();

}

void loop() {

}
