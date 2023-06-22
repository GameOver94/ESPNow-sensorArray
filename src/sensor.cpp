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



//--------------------------------------------------------------------------------------------------------
SensorDHT22::SensorDHT22(int pin, int type)
    : m_dht(pin, type)
{
}

bool SensorDHT22::init()
{
    Log.noticeln("---- Setup Sensor ----");
    m_dht.begin();

    sensor_t sensor;
    // Print temperature sensor details.
    m_dht.temperature().getSensor(&sensor);
    Log.noticeln("Temperatrue Sensor");
    Log.noticeln("Unique ID:   %d", sensor.sensor_id);

    // Print humidity sensor details.
    m_dht.humidity().getSensor(&sensor);
    Log.noticeln("Humidity Sensor");
    Log.noticeln("Unique ID:   %s", sensor.sensor_id);

    return true;
}

bool SensorDHT22::update()
{
    Serial.println();
    Log.noticeln("---- Get Sensor Data ----");

    sensors_event_t event;
    bool status = true;

    m_dht.temperature().getEvent(&event);
    if (isnan(event.temperature))
    {
        status = false;
        Log.errorln("Error reading temperature!");
    }
    else
    {
        m_temperature = event.temperature;
    }
    // Get humidity event and print its value.
    m_dht.humidity().getEvent(&event);
    if (isnan(event.relative_humidity))
    {
        status = false;
        Log.errorln("Error reading humidity!");
    }
    else
    {
        m_humidity = event.relative_humidity;
    }

    return status;
}

void const SensorDHT22::print()
{
    Log.noticeln("---- Print Sensor Data ----");

    Log.noticeln("Temperature: %F °C", m_temperature);
    Log.noticeln("Humidity: %F", m_humidity);
}

float const SensorDHT22::temperature()
{
    return m_temperature;
}

float const SensorDHT22::humidity()
{
    return m_humidity;
}



//--------------------------------------------------------------------------------------------------------
SensorBMP280::SensorBMP280()
    : m_bmp()
{
}

bool SensorBMP280::init()
{
    Log.noticeln("---- Setup Sensor ----");

    bool status;
    status = m_bmp.begin(0x76);

    if (!status)
    {
        Log.errorln("Could not find a valid BMP280 sensor, check wiring, address, sensor ID!");
        return false;
    }
    else
    {
        m_bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                          Adafruit_BMP280::SAMPLING_X2, // temperature
                          Adafruit_BMP280::SAMPLING_X16, // pressure
                          Adafruit_BMP280::FILTER_X16,
                          Adafruit_BMP280::STANDBY_MS_4000);
        return true;
    }
    return false;
}

bool SensorBMP280::update()
{
    Log.noticeln("");
    Log.noticeln("---- Get Sensor Data ----");

    uint8_t status = m_bmp.getStatus();

    if (status == 0)
    {
        float temp = m_bmp.readTemperature();
        float hum = 50.0F;
        float pres = m_bmp.readPressure();

        m_temperature = temp;
        m_pressure = pres/100;
        m_reducedPressure = compensateAltitude(temp, hum, pres);
        return true;
    }
    else
    {
        Log.errorln("Error reading sensor data");
        return false;
    }

    return status;
}

float SensorBMP280::compensateAltitude(float &temp, float &hum, float &pres)
{
    float p_s(NAN), E(NAN), p_r(NAN);
    p_s = pow(10, m_A - m_B / (m_C + temp));
    E = hum / 100 * p_s;

    p_r = pres / 100 * exp(m_g * m_h / (m_R * (temp + 273.15 + m_C_h * E + m_alpha * m_h / 2)));
    return p_r;
}

void const SensorBMP280::print()
{
    Log.noticeln("");
    Log.noticeln("---- Print Sensor Data ----");

    Log.noticeln("Temperature: %F °C", m_temperature);
    Log.noticeln("Pressure: %F mbar", m_pressure);
    Log.noticeln("Reduced pressure: %F mbar", m_reducedPressure);
}

float const SensorBMP280::temperature()
{
    return m_temperature;
}

float const SensorBMP280::pressure()
{
    return m_pressure;
}

float const SensorBMP280::reducedPressure()
{
    return m_reducedPressure;
}