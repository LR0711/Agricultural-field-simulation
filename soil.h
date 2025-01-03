// This class here is the "soil" class, that represents the base unit for the simulation of the agricultural field.
// It's like a square portion of the bigger field, and it represents the idea that we can have different properties of the soil in the same field.
// In the first version of this project, we consider that we can have different conditions in different areas of the field but this properties are static and hardcoded.
// This class takes as input parameters an enum class representing 4 different type of possible soils (clay, sand, loam, silt), a bool saying if there are plants or not
// an int representing the percentage of soil moisture and two int representing air temperature and air humidity in percentage.
// There's another parameter, the soil temeperature, that is calculated by the class itself, based on the air temperature, the type of soil and the soil moisture.
// The class gives method to get presence of plants and type of soil and to set all the parameters, but user can't get temeratures, moisture and humidity without a proper sensor class.

#ifndef SOIL_H
#define SOIL_H
#include <string>

#include "sensor.h"
using SensorType = Sensor::SensorType;


class Soil {
    public:
        enum class SoilType {clay, sand, loam, silt};
        Soil();
        Soil(SoilType soilType, bool plants, double soilMoisture, float airTemperature, double airHumidity);
        void setSoilType(SoilType soilType);
        void setPlants(bool plants);
        void setSoilMoisture(double soilMoisture);
        void setAirTemperature(float airTemperature);
        void setAirHumidity(double airHumidity);
        SoilType getSoilType() const {return soiltype_;}
        bool getPlants() const {return plants_;}
        static std::string soilTypeToString(SoilType soilType);
        float PassTemperatureToSensor(SensorType sensorType) const;
        double PassSoilMoistureToSensor(SensorType sensorType) const;
        double PassAirHumidityToSensor(SensorType sensorType) const;
    

    private:
        SoilType soiltype_;
        bool plants_;
        double soilmoisture_;
        float airtemperature_;
        double airhumidity_;
        float soiltemperature_;
        float calculateSoilTemperature();
        bool ValidHumidity () const;
        float getAirTemperature() const {return airtemperature_;}
        float getSoilTemperature() const {return soiltemperature_;}
        double getAirHumidity() const {return airhumidity_;}
        double getSoilMoisture() const {return soilmoisture_;}
    };

#endif