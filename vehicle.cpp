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

Vehicle::Vehicle(string name, VehicleType type, int x, int y, double speed, float battery, std::vector<Sensor> sensors, const Field& field)
    :name_{name},
    type_{type},
    x_{x},
    y_{y},
    speed_{speed},
    battery_{battery},
    sensors_{sensors},
    field_{field}
    {
        if (x < 0 || x >= field_.getLength() || y < 0 || y >= field_.getWidth()) {
            std::cerr << "Invalid coordinates: vehicle is out of field." << std::endl;
            exit(EXIT_FAILURE);
        }

        if (speed <= 0) {
            std::cerr << "Invalid speed: speed must be greater than 0." << std::endl;
            exit(EXIT_FAILURE);
        }
    }

void Vehicle::setPosition(int x, int y) {
    if (x < 0 || x >= field_.getLength() || y < 0 || y >= field_.getWidth()) {
        std::cerr << "Invalid coordinates: vehicle is out of field." << std::endl;
        exit(EXIT_FAILURE);
    }
    x_ = x;
    y_ = y;

    std::cout << "Vehicle " << name_ << " moved to position (" << x_ << ", " << y_ << ")" << std::endl;
}

void Vehicle::moveToTarget(int targetx, int targety) {
    if (targetx < 0 || targetx >= field_.getLength() || targety < 0 || targety >= field_.getWidth()) {
        std::cerr << "Invalid coordinates: target is out of field." << std::endl;
        exit(EXIT_FAILURE);
    }
    
    double steptime {1.0 / speed_};
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

        std::cout << "Vehicle " << name_ << " moved to position (" << x_ << ", " << y_ << ")" << std::endl;
        std::this_thread::sleep_for(std::chrono::duration<float>(steptime));
        
    
    }
    std::cout << "Vehicle " << name_ << " reached target at position (" << x_ << ", " << y_ << ")" << std::endl;
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

std::string Vehicle::vehicleTypeToString(VehicleType type) const {
    switch (type) {
        case VehicleType::FieldVehicle:
            return "Field Vehicle ";
        case VehicleType::AerialVehicle:
            return "Aerial Vehicle ";
        default:
            return "Unknown Vehicle Type ";
    }
}

std::ostream& operator<<(std::ostream& os, const Vehicle& vehicle) {
    os << vehicle.getName() << "currently at position (" << vehicle.getX() << ", " << vehicle.getY() << ")" << std::endl;
    return os;
        
}
