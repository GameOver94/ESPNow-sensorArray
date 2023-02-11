#pragma once

#include <Adafruit_Si7021.h>
#include <ArduinoLog.h>

#include "data_structure.h"

class SensorSi7021
{
private:
    Adafruit_Si7021 m_si7021;
    struct_data m_data;

public:
    SensorSi7021();
    bool init();
    bool update();
    void const print();
    struct_data const get();
};