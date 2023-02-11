#pragma once

#define OK 0
#define TRANSMISSION_ERROR 1
#define SENSOR_ERROR 2
#define WIFI_ERROR 3
#define MQTT_ERROR 4

#define CLIENT_ID_T     0  // Client ID
#define MESSAGE_ID_T    1  // Message ID
#define STATUS_T        2  // Status
#define TEMP_T          3  // Temperature
#define TEMP2_T         4  // Temperature #2
#define TEMP3_T         5  // Temperature #3
#define HUMIDITY_T      6  // Relative Humidity
#define PRESSURE_T      7  // Atmospheric Pressure
#define R_PRESSURE_T    8  // Reduced Pressure
#define SOIL_T          9  // Soil Moisture
#define SOIL2_T         10 // Soil Moisture #2
#define BAT_VOLTAGE_T   11 // Battery Voltage

struct dataReading // Structure for saveing measurements
{
    float measurement;
    uint8_t property;
};

struct state // Structure to save state over deep sleep
{
    uint32_t messageID;
    uint8_t lastError;
    uint8_t errorCout;
};

