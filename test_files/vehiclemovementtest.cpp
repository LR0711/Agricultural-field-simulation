#include "field.h"
#include "vehicle.h"
#include "sensor.h"

void testVehicleMovement()
{
    Field field("TestField", 10, 10);
    Vehicle robot("TestVehicle", 10000, Vehicle::VehicleType::FieldVehicle, 0, 0, 1.0, 100.0, {Sensor::SensorType::SoilTemperatureSensor, Sensor::SensorType::MoistureSensor, Sensor::SensorType::AirTemperatureSensor }, field);
    robot.setPosition(5, 5);
    robot.moveToTarget(9, 7);
    Vehicle drone("TestDrone", 10001, Vehicle::VehicleType::AerialVehicle, 0, 0, 2.0, 100.0, {}, field);
    drone.setPosition(2, 9);
    drone. moveToTarget(9, 2);
    robot.readDataFromCurrentCell();
    //Some test for errors checking: remove comment to see the error message
    //Vehicle robot2("TestVehicle2", Vehicle::VehicleType::FieldVehicle, 0, 0, -1.0, 100.0, {}, field);
    //Vehicle robot3("TestVehicle3", Vehicle::VehicleType::FieldVehicle, -10, 0, 1.0, 100.0, {}, field);
    //Vehicle robot4("TestVehicle4", Vehicle::VehicleType::FieldVehicle, 10, 10, 1.0, 100.0, {}, field);
}


int main()
{
    testVehicleMovement();


    return 0;
    
}
