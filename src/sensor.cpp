#include "sensor.h"

SensorSi7021::SensorSi7021()
    : m_si7021()
{
}

bool SensorSi7021::init()
{
    Log.noticeln("---- Setup Sensor ----");

    if (!m_si7021.begin())
    {
        Log.errorln("Did not find Si7021 sensor!");
        return false;
    }

    // Print sensor details.
    switch (m_si7021.getModel())
    {
    case SI_Engineering_Samples:
        Log.noticeln("Found model SI engineering samples");
        break;
    case SI_7013:
        Log.noticeln("Found model Si7013");
        break;
    case SI_7020:
        Log.noticeln("Found model Si7020");
        break;
    case SI_7021:
        Log.noticeln("Found model Si7021");
        break;
    case SI_UNKNOWN:
    default:
        Log.noticeln("Found model Unknown");
    }
    Log.noticeln(" Rev(%i) Serial #%x%x", m_si7021.getRevision(), m_si7021.sernum_a, m_si7021.sernum_b);

    return true;
}

bool SensorSi7021::update()
{

    Log.noticeln("---- Get Sensor Data ----");
    bool status = true;

    m_temperature = m_si7021.readTemperature();
    m_humidity = m_si7021.readHumidity();

    return status;
}

void const SensorSi7021::print()
{
    Log.noticeln("---- Print Sensor Data ----");

    Log.noticeln("Temperature: %F °C", m_temperature);
    Log.noticeln("Humidity: %F", m_humidity);

}

float const SensorSi7021::temperature()
{
    return m_temperature;
}

float const SensorSi7021::humidity()
{
    return m_humidity;
}