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
#include "sensor.h"

struct state
{
    uint32_t messageID;
    uint8_t errorCout;
};

state rtc_state;

//MAC Address of the receiver
//uint8_t broadcastAddress[] = {0x50, 0x02, 0x91, 0xFD, 0x37, 0x00};
uint8_t broadcastAddress[] = {0x30, 0x83, 0x98, 0xB1, 0x05, 0xE4};

SensorSi7021 sensor;

unsigned long timeOut = 500; // timeout after bord gets shut down after no acnolagement was received


// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus)
{
    if (sendStatus == 0)
    {
        Log.noticeln("Last Packet Send Status: Delivery success");
        rtc_state.errorCout = 0;
    }
    else
    {
        Log.errorln("Last Packet Send Status: Delivery failed");
        rtc_state.errorCout++;
    }

    // Go into deep sleep
    ESP.rtcUserMemoryWrite(0, (uint32_t *)&rtc_state, sizeof(rtc_state));

    Log.noticeln("Going into deep sleep");
    if (rtc_state.errorCout < 5)
        {
            ESP.deepSleep(SLEEP_INTERVAL * 1e6);
        }
        else
        {
            ESP.deepSleep(SLEEP_INTERVAL * 5 * 1e6);
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
    if (esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0) != 0)
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

void updateData(struct_message &out_myData)
{
    rtc_state.messageID++;
    out_myData.messageID = rtc_state.messageID;

    if (rtc_state.errorCout)
    {
        out_myData.status = TRANSMISSION_ERROR;
    }

    struct_data sensorData;
    sensor.update();
    sensor.print();
    sensorData = sensor.get();
    out_myData.temperature = sensorData.temperature;
    out_myData.humidity = sensorData.humidity;
    out_myData.battery = batteryVoltage();
}

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
    struct_message myData{CLIENT_ID, 0, 0, 0, 0, 0, 0, 0, 0, 0b00100011};
    updateData(myData);

    // Send message via ESP-NOW
    esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));
}

void loop()
{
    if (millis() > timeOut)
    {
        // Go into deep sleep
        Log.errorln("Connection timeout");
        rtc_state.errorCout++;
        ESP.rtcUserMemoryWrite(0, (uint32_t *)&rtc_state, sizeof(rtc_state));

        Log.noticeln("Going into deep sleep");

        if (rtc_state.errorCout < 5)
        {
            ESP.deepSleep(SLEEP_INTERVAL * 1e6);
        }
        else
        {
            ESP.deepSleep(SLEEP_INTERVAL * 5 * 1e6);
        }
    }
}