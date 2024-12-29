#include <iostream>
#include <string>
using std::cerr;
using std::endl;
using std::string;
#include "soil.h"
#include "sensor.h"


Soil::Soil() 
    :soiltype_{SoilType::loam},
    plants_{false},
    soilmoisture_{50},
    airtemperature_{20.0},
    airhumidity_{50},
    soiltemperature_{calculateSoilTemperature()}
    {}

Soil::Soil(SoilType soilType, bool plants, int soilMoisture, float airTemperature, int airHumidity)
    :soiltype_{soilType},
    plants_{plants},
    soilmoisture_{soilMoisture},
    airtemperature_{airTemperature},
    airhumidity_{airHumidity},
    soiltemperature_{calculateSoilTemperature()}
    {
        if (!ValidHumidity()) 
        {
            std::cerr << "Invalid humidity or moisture value. Humidity and moisture must be between 0 and 100." << std::endl;
            exit(EXIT_FAILURE);

        }
    }

    float Soil::calculateSoilTemperature()
    {
        switch(soiltype_)
        {
            case SoilType::clay:
                return 0.6 * airtemperature_ + 0.006 * soilmoisture_ + 0.001 * airhumidity_ + 2.0;
            case SoilType::sand:
                return 0.8 * airtemperature_ + 0.0015 * soilmoisture_ + 0.0005 * airhumidity_ + 3.0;
            case SoilType::loam:
                return 0.7 * airtemperature_ + 0.002 * soilmoisture_ + 0.001 * airhumidity_ + 2.5;
            case SoilType::silt:
                return 0.65 * airtemperature_ + 0.0025 * soilmoisture_ + 0.001 * airhumidity_ + 2.0;
            default:
                cerr << "Invalid soil type." << endl;
                exit(EXIT_FAILURE);
        }
    }
    bool Soil::ValidHumidity() const
    {
        if (airhumidity_ < 0 || airhumidity_ > 100 || soilmoisture_ < 0 || soilmoisture_ > 100) 
        {
            return false;
        }
        return true;
    }

    void Soil::setSoilType(SoilType soilType)
    {
        soiltype_ = soilType;
        soiltemperature_ = calculateSoilTemperature();
    }   

    void Soil::setPlants(bool plants)
    {
        plants_ = plants;
    }
    
    void Soil::setSoilMoisture(int soilMoisture)
    {
        soilmoisture_ = soilMoisture;
        if (!ValidHumidity()) 
            {
                std::cerr << "Invalid humidity or moisture value. Humidity and moisture must be between 0 and 100." << std::endl;
                exit(EXIT_FAILURE);
            }
        soiltemperature_ = calculateSoilTemperature();
    }

    void Soil::setAirTemperature(float airTemperature)
    {
        airtemperature_ = airTemperature;
        soiltemperature_ = calculateSoilTemperature();
    }

    void Soil::setAirHumidity(int airHumidity)
    {
        airhumidity_ = airHumidity;
        if (!ValidHumidity()) 
            {
                std::cerr << "Invalid humidity or moisture value. Humidity and moisture must be between 0 and 100." << std::endl;
                exit(EXIT_FAILURE);

            }
        soiltemperature_ = calculateSoilTemperature();
    }

    float Soil::PassTemperatureToSensor(SensorType sensorType) const {
        if (sensorType == SensorType::SoilTemperatureSensor) {
            return getSoilTemperature();
        } else if (sensorType == SensorType::AirTemperatureSensor) {
            return getAirTemperature();
        } else {
            cerr << "Invalid sensor type." << endl;
            exit(EXIT_FAILURE);
        }
    }

    int Soil::PassSoilMoistureToSensor(SensorType sensorType) const {
        if (sensorType == SensorType::MoistureSensor) {
            return getSoilMoisture();
        } else {
            cerr << "Invalid sensor type." << endl;
            exit(EXIT_FAILURE);
        }
    }

    int Soil::PassAirHumidityToSensor(SensorType sensorType) const {
        if (sensorType == SensorType::HumiditySensor) {
            return getAirHumidity();
        } else {
            cerr << "Invalid sensor type." << endl;
            exit(EXIT_FAILURE);
        }
    }



    std::string Soil::soilTypeToString(SoilType soilType)
    {
        switch(soilType)
        {
            case SoilType::clay:
                return "Clay";
            case SoilType::sand:
                return "Sand";
            case SoilType::loam:
                return "Loam";
            case SoilType::silt:
                return "Silt";
            default:
                cerr << "Invalid soil type." << endl;
                exit(EXIT_FAILURE);
        }
    }