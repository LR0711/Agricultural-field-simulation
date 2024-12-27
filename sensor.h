// The class "sensor" represens the main class that will be used to get the datas from the field.
// In this simulation, there will be 4 different kind of sensors meant for the 4 different kind of datas that we want to get from the field.
// The datas are the soil and air temperature, the soil moisture, the air humidity and plant health.
// The sensors will be placed in the vehicles to get the datas from the field.
// When the veichle reaches the target cell, the sensors will be activated and will get the datas from the field.
// The sensors will be activated by the vehicle class, that will call the method "getDatas" of the sensor class.
// The sensor class will have a method for each kind of data that we want to get from the field.
// After getting the data, sensor will send it to the vehicle class, that will send it to the central unit.
// The sensor class will have a method to get the data from the field, and a method to send the data to the vehicle class.
// Just remember that sensor is the only way to read those data from the field, and the vehicle class is the only way to get the data from the sensor class.

#ifndef SENSOR_H
#define SENSOR_H
#include "soil.h"



class Sensor {
    public:
        enum class SensorType {MoistureSensor, TemperatureSensor, HumiditySensor, PlantHealthSensor};
        Sensor();
        Sensor(SensorType type);
        float readTemperature(const Soil& soil) const;
        int readMoisture(const Soil& soil) const;
        int readHumidity(const Soil& soil) const;
        void sendDatas();
        
    private:
        SensorType sensortype_;
        
};

#endif