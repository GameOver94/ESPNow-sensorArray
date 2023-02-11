#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

extern "C"
{
#include "user_interface.h"

    extern struct rst_info resetInfo;
}

#include <ArduinoLog.h>

#include "data_structure.h"
#include "macAddress.h"
#include "sensor.h"

SensorSi7021 sensor;            // Initiate sensor class / select connected Sensor
state rtc_state;                // Structure to save state over deep sleep
const uint8_t arraySize = 7;    // 4 for Status Messages + NR. of Sensor Values
dataReading message[arraySize]; // Structure to save state sensor measurements
unsigned long timeOut = 500;    // timeout after bord gets shut down after no acnolagement was received
uint8_t retryCounter = 0;       // counter for transmision retries

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus)
{
    if (sendStatus == 0)
    {
        Log.noticeln("Last Packet Send Status: Delivery success");
        rtc_state.errorCout = 0;
        rtc_state.lastError = OK;
    }
    else
    {
        Log.errorln("Last Packet Send Status: Delivery failed");
        rtc_state.errorCout++;
        rtc_state.lastError = TRANSMISSION_ERROR;
    }

    if (retryCounter == 0 && rtc_state.lastError)
    {
        Log.noticeln("Retry Transmision");
        retryCounter = 1;
        delay(100);
        esp_now_send(receiverMACAddress, (uint8_t *)&message, sizeof(message));
    }
    else
    {
        // Go into deep sleep
        ESP.rtcUserMemoryWrite(0, (uint32_t *)&rtc_state, sizeof(rtc_state));

        Log.noticeln("Going into deep sleep");
        if (rtc_state.errorCout = 0){
            ESP.deepSleep(SLEEP_INTERVAL * 1e6);
        }
        else if (rtc_state.errorCout < 5)
        {
            ESP.deepSleep(60 * 1e6);
        }
        else
        {
            ESP.deepSleep(60 * 30 * 1e6);
        }
    }
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

    // Once ESPNow is successfully Init, we will register for Send CB to
    // get the status of Trasnmitted packet
    esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
    esp_now_register_send_cb(OnDataSent);

    // Register peer
    if (esp_now_add_peer(receiverMACAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0) != 0)
    {
        Log.errorln("Pairing failed");
    };
}

float batteryVoltage()
{
    uint32_t adcCount = analogRead(A0);
    delay(20);
    adcCount += analogRead(A0);
    delay(20);
    adcCount += analogRead(A0);

    adcCount /= 3;
    float batteryVoltage = (adcCount / 1023.0) * 1.0 * 1 * 3.636; // Vref * attenuation * voltage divider
    Log.noticeln("ADC count: %i", adcCount);
    Log.noticeln("Battery Voltage: %F", batteryVoltage);

    return batteryVoltage;
}

void readSensor()
{
    sensor.update();
    sensor.print();

    message[4].property = TEMP_T;
    message[4].measurement = sensor.temperature();

    message[5].property = HUMIDITY_T;
    message[5].measurement = sensor.humidity();

    message[6].property = BAT_VOLTAGE_T;
    message[6].measurement = batteryVoltage();
}

void initMessage()
{
    Log.noticeln("---- Prepair Message Header ----");
    message[0].property = CLIENT_ID_T;
    message[0].measurement = CLIENT_ID;
    Log.noticeln("ClientID: %i", (int)message[0].measurement);

    rtc_state.messageID++;
    message[1].property = MESSAGE_ID_T;
    message[1].measurement = rtc_state.messageID;
    Log.noticeln("MessageID: %i", (int)message[1].measurement);

    if (rtc_state.errorCout >= 3)
    {
        message[2].property = STATUS_T;
        message[2].measurement = TRANSMISSION_ERROR;
    }
    else
    {
        message[2].property = STATUS_T;
        message[2].measurement = OK;
    }
    Log.noticeln("Status: %i", (int)message[2].measurement);

    message[3].property = LENGTH_T;
    message[3].measurement = arraySize;
    Log.noticeln("Array Size: %i", (int)message[3].measurement);
}

// -------------------------------------------------------------------------------------------

void setup()
{
    // Init Serial Monitor
    Serial.begin(115200);
    while (!Serial)
        ;
    Log.begin(LOG_LEVEL_SILENT, &Serial);
    Log.noticeln("\n\nWake up: %s\n", ESP.getResetReason().c_str());

    setupESPnow();
    sensor.init();

    if (resetInfo.reason == REASON_DEEP_SLEEP_AWAKE)
    {
        // this was a deep sleep
        // get state from rtc memory
        ESP.rtcUserMemoryRead(0, (uint32_t *)&rtc_state, sizeof(rtc_state));
    }

    // Set values to send
    initMessage();
    readSensor();

    // Send message via ESP-NOW
    esp_now_send(receiverMACAddress, (uint8_t *)&message, sizeof(message));
}

void loop()
{
    if (millis() > timeOut)
    {
        // Go into deep sleep
        Log.errorln("Connection timeout");
        rtc_state.errorCout++;
        rtc_state.lastError = TRANSMISSION_ERROR;
        ESP.rtcUserMemoryWrite(0, (uint32_t *)&rtc_state, sizeof(rtc_state));

        Log.noticeln("Going into deep sleep");
        if (rtc_state.errorCout = 0){
            ESP.deepSleep(SLEEP_INTERVAL * 1e6);
        }
        else if (rtc_state.errorCout < 5)
        {
            ESP.deepSleep(60 * 1e6);
        }
        else
        {
            ESP.deepSleep(60 * 30 * 1e6);
        }
    }
}