#pragma once

#define OK 0
#define TRANSMISSION_ERROR 1
#define SENSOR_ERROR 2
#define WIFI_ERROR 3
#define MQTT_ERROR 4

struct struct_message
{
    uint32_t clientID;
    uint32_t messageID;

    float temperature;
    float humidity;
    float pressure;
    float r_pressure;
    uint32_t pm25;
    float battery;

    uint8_t status;
    uint8_t availableSensor; // Indicates the available Sensors: Form LSB --> temperature, humitity, pressure, r_pressure, pm25, battery
};

struct struct_data
{
    float temperature;
    float humidity;
    float pressure;
    float r_pressure;
    uint32_t pm25;
};
