#include "sensor.h"




Sensor::Sensor()
    :sensortype_{SensorType::TemperatureSensor}
    {}

Sensor::Sensor(SensorType sensortype)
    :sensortype_{sensortype}
    {}

