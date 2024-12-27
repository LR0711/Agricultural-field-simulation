#include "vehicle.h"

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

void Vehicle::move(int x, int y)
