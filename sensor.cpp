#include "sensor.h"
#include "soil.h"
#include <string>
#include <random>

// Costruttore di default
Sensor::Sensor()
    :sensortype_{SensorType::AirTemperatureSensor}
    {}

// Costruttore con parametri
Sensor::Sensor(SensorType sensortype)
    :sensortype_{sensortype}
    {}

float Sensor::readTemperature(const Soil& soil) const {
    float value = soil.PassTemperatureToSensor(sensortype_);
    // Aggiunta di rumore casuale
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(-0.5, 0.5); // Rumore nell'intervallo [-0.5, 0.5]
    return value + dis(gen);
}

double Sensor::readMoisture(const Soil& soil) const {
    double value = soil.PassSoilMoistureToSensor(sensortype_);
    // Aggiunta di rumore casuale
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(-2, 2); // Rumore nell'intervallo [-2, 2]
    double noisyValue {value + dis(gen)};
    if (noisyValue < 0) 
    {
        return 0;
    }
    else if (noisyValue > 100) 
    {
        return 100;
    }
    else 
    {
        return noisyValue;
    }
    
}

double Sensor::readHumidity(const Soil& soil) const {
    double value = soil.PassAirHumidityToSensor(sensortype_);
    // Aggiunta di rumore casuale
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(-3, 3); // Rumore nell'intervallo [-3, 3]
    double noisyValue {value + dis(gen)};
    if (noisyValue < 0) 
    {
        return 0;
    }
    else if (noisyValue > 100) 
    {
        return 100;
    }
    else 
    {
        return noisyValue;
    }
    
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
