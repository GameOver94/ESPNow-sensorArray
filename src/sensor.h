#pragma once

#include <Adafruit_Si7021.h>
#include <ArduinoLog.h>


class SensorSi7021
{
private:
    Adafruit_Si7021 m_si7021;
    float m_temperature, m_humidity;

public:
    SensorSi7021();
    bool init();
    bool update();
    void const print();
    float const temperature();
    float const humidity();
};



#include <DHT.h>
#include <DHT_U.h>

class SensorDHT22
{
private:
    DHT_Unified m_dht;
    float m_temperature, m_humidity;

public:
    SensorDHT22(int pin, int type);
    bool init();
    bool update();
    void const print();
    float const temperature();
    float const humidity();
};


#include <Adafruit_BMP085.h>

class SensorBMP180
{
private:
    Adafruit_BMP085 m_bmp;
    float m_temperature, m_pressure, m_reducedPressure;

    /* barometrische Höhenformel */
    const float m_g = 9.80665;
    const float m_R = 287.05;
    const float m_alpha = 0.0065;
    const float m_C_h = 0.12;
    const float m_h = 465; // Change to your height above seelevel

    /* Antonie Parameter */
    const float m_A = 5.20389;
    const float m_B = 1733.926;
    const float m_C = 39.485;

    const float m_pSealevel = 1013.25;

    float compensateAltitude(float &temp, float &hum, float &pres);

public:
    SensorBMP180();
    bool init();
    bool update();
    bool update(float hum);
    void const print();
    float const temperature();
    float const pressure();
    float const reducedPressure();
};


#include <Adafruit_BMP280.h>

class SensorBMP280
{
private:
    Adafruit_BMP280 m_bmp;
    float m_temperature, m_pressure, m_reducedPressure;

    /* barometrische Höhenformel */
    const float m_g = 9.80665;
    const float m_R = 287.05;
    const float m_alpha = 0.0065;
    const float m_C_h = 0.12;
    const float m_h = 465; // Change to your height above seelevel

    /* Antonie Parameter */
    const float m_A = 5.20389;
    const float m_B = 1733.926;
    const float m_C = 39.485;

    const float m_pSealevel = 1013.25;

    float compensateAltitude(float &temp, float &hum, float &pres);

public:
    SensorBMP280();
    bool init();
    bool update();
    bool update(float hum);
    void const print();
    float const temperature();
    float const pressure();
    float const reducedPressure();
};


#include <Adafruit_BME280.h>
class SensorBME280
{
private:
    Adafruit_BME280 m_bme;
    float m_temperature, m_humidity, m_pressure, m_reducedPressure;

    /* barometrische Höhenformel */
    const float m_g = 9.80665;
    const float m_R = 287.05;
    const float m_alpha = 0.0065;
    const float m_C_h = 0.12;
    const float m_h = 465; // Change to your height above seelevel

    /* Antonie Parameter */
    const float m_A = 5.20389;
    const float m_B = 1733.926;
    const float m_C = 39.485;

    const float m_pSealevel = 1013.25;

    float compensateAltitude(float &temp, float &hum, float &pres);

public:
    SensorBME280();
    bool init();
    bool update();
    void const print();
    float const temperature();
    float const humidity();
    float const pressure();
    float const reducedPressure();
};