#include "vehicle.h"
#include <iostream>
#include <thread>
#include <chrono>


Vehicle::Vehicle()
    :name_{"???"},
    type_{VehicleType::FieldVehicle},
    x_{0},
    y_{0},
    speed_{1.0},
    battery_{100.0},
    sensors_{},
    field_{Field()}
    {}

Vehicle::Vehicle(string name, VehicleType type, int x, int y, float speed, float battery, std::vector<Sensor> sensors, const Field& field)
    :name_{name},
    type_{type},
    x_{x},
    y_{y},
    speed_{speed},
    battery_{battery},
    sensors_{sensors},
    field_{field}
    {}

void Vehicle::setPosition(int x, int y) {
    if (x < 0 || x >= field_.getLength() || y < 0 || y >= field_.getWidth()) {
        std::cerr << "Invalid coordinates: vehicle is out of field." << std::endl;
        exit(EXIT_FAILURE);
    }
    x_ = x;
    y_ = y;
}

void Vehicle::moveToTarget(int targetx, int targety) {
    if (targetx < 0 || targetx >= field_.getLength() || targety < 0 || targety >= field_.getWidth()) {
        std::cerr << "Invalid coordinates: target is out of field." << std::endl;
        exit(EXIT_FAILURE);
    }
    
    float steptime {1.0 / speed_};
    while (x_ != targetx || y_ != targety) {
        if (x_ < targetx) {
            x_++;
        } else if (x_ > targetx) {
            x_--;
        }

        if (y_ < targety) {
            y_++;
        } else if (y_ > targety) {
            y_--;
        }

        std::this_thread::sleep_for(std::chrono::duration<float>(steptime));
        
    
    }
}

void Vehicle::readDataFromCurrentCell() const {
    Soil soil;
    if (!field_.getSoil(x_, y_, soil)) {
        std::cerr << "Error: Unable to read soil data at position (" << x_ << ", " << y_ << ")" << std::endl;
        return;
    }

    for (const auto& sensor : sensors_) {
        switch (sensor.getType()) {
            case Sensor::SensorType::SoilTemperatureSensor:
                std::cout << "Soil Temperature Sensor Data: " << soil.PassTemperatureToSensor(Sensor::SensorType::SoilTemperatureSensor) << std::endl;
                break;
            case Sensor::SensorType::AirTemperatureSensor:
                std::cout << "Air Temperature Sensor Data: " << soil.PassTemperatureToSensor(Sensor::SensorType::AirTemperatureSensor) << std::endl;
                break;
            case Sensor::SensorType::MoistureSensor:
                std::cout << "Moisture Sensor Data: " << soil.PassSoilMoistureToSensor(Sensor::SensorType::MoistureSensor) << std::endl;
                break;
            case Sensor::SensorType::HumiditySensor:
                std::cout << "Humidity Sensor Data: " << soil.PassAirHumidityToSensor(Sensor::SensorType::HumiditySensor) << std::endl;
            default:
                std::cerr << "Unknown sensor type" << std::endl;
        }
    }   
}