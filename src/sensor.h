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