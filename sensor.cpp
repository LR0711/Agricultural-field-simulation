#include "sensor.h"




Sensor::Sensor()
    :sensortype_{SensorType::AirTemperatureSensor}
    {}

Sensor::Sensor(SensorType sensortype)
    :sensortype_{sensortype}
    {}

float Sensor::readTemperature(const Soil& soil) const {
    return soil.PassTemperatureToSensor(sensortype_);
}

int Sensor::readMoisture(const Soil& soil) const {
    return soil.PassSoilMoistureToSensor(sensortype_);
}

int Sensor::readHumidity(const Soil& soil) const {
    return soil.PassAirHumidityToSensor(sensortype_);
}
  