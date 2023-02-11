#pragma once

#define OK 0
#define TRANSMISSION_ERROR 1
#define SENSOR_ERROR 2
#define WIFI_ERROR 3
#define MQTT_ERROR 4

#define CLIENT_ID_T     0  // Client ID
#define MESSAGE_ID_T    1  // Message ID
#define STATUS_T        2  // Status
#define LENGTH_T        3  // Array Size
#define TEMP_T          4  // Temperature
#define TEMP2_T         5  // Temperature #2
#define TEMP3_T         6  // Temperature #3
#define HUMIDITY_T      7  // Relative Humidity
#define PRESSURE_T      8  // Atmospheric Pressure
#define R_PRESSURE_T    9  // Reduced Pressure
#define SOIL_T          10  // Soil Moisture
#define SOIL2_T         11 // Soil Moisture #2
#define BAT_VOLTAGE_T   12 // Battery Voltage

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

/* Set the MAC Address of the receiver
    the Client and the receiver have to be compliled with the same MAC Address
    if you have more than one receiver comple the client with the MAC Address of the nearest receiver
*/
//uint8_t receiverMACAddress[] = {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};  // test receiver
uint8_t receiverMACAddress[] = {0x6e, 0x5f, 0x5b, 0x7d, 0x69, 0xae};