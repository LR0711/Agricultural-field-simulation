#include "sensor.h"
#include "soil.h"
#include <string>


// Costruttore di default
Sensor::Sensor()
    :sensortype_{SensorType::AirTemperatureSensor}
    {}

// Costruttore con parametri
Sensor::Sensor(SensorType sensortype)
    :sensortype_{sensortype}
    {}

// Funzioni di lettura dei dati dal suolo
float Sensor::readTemperature(const Soil& soil) const {
    return soil.PassTemperatureToSensor(sensortype_);
}

int Sensor::readMoisture(const Soil& soil) const {
    return soil.PassSoilMoistureToSensor(sensortype_);
}

int Sensor::readHumidity(const Soil& soil) const {
    return soil.PassAirHumidityToSensor(sensortype_);
}

// Fuzione per conversione dell'enumerazione SensorType in stringa
std::string Sensor::sensorTypeToString(SensorType type) {
        switch (type) {
            case SensorType::SoilTemperatureSensor: return "Soil Temperature";
            case SensorType::AirTemperatureSensor: return "Air Temperature";
            case SensorType::MoistureSensor: return "Moisture";
            case SensorType::HumiditySensor: return "Humidity";
           
            default: return "Unknown";
        }
 }
