// The following class is the "vehicle" class, that represents the vehicles that can be used in the simulation of the agricultural field.
// The class has a constructor that takes as input parameters the name of the vehicle, the type of the vehicle (field or aerial), the initial coordinates in the field, the vehicle speed and the battery level.
// This vehicles will have on them also sensor objects, that will be used to get the temperature, humidity and moisture of the soil.
// This class has the ability to move in the field class and collect datas from the soil celles.
// In order to simulate a real vehicle, the vehicle does a navigation towards the target cell taking a time proportinal the distance between the current cell and the target cell and the speed of the vehicle.
// The vehicle consumes battery in the movement, and the battery level is updated after the movement. If the battery level is too low, the vehicle can't move anymore and it has to wait for a recharge.
// The vehicle can also recharge itself, but it can't move during the recharge.
// Lastly, the vehicle has to communicate to a central unit his position and, when reached the target and get datas back from the sensor, the datas collected.
// Sensors are built in another class.


#ifndef VEHICLE_H
#define VEHICLE_H
#include <string>
using std::string;
#include <vector>
using std::vector;
#include "sensor.h"
#include "field.h"
#include <iostream>
using std::ostream;



class Vehicle {
    public:
        enum class VehicleType {FieldVehicle, AerialVehicle};
        Vehicle();
        Vehicle(string name, VehicleType type, int x, int y, double speed, float battery, std::vector<Sensor> sensors, const Field& field);
        void setPosition(int x, int y);
        void moveToTarget(int targetx, int targety);
        void readDataFromCurrentCell() const;
        std::string getName() const {return name_;}
        VehicleType getType() const {return type_;}
        int getX() const {return x_;}
        int getY() const {return y_;}
        double getSpeed() const {return speed_;}
        std::string vehicleTypeToString(VehicleType type) const;



    private:
        string name_;
        VehicleType type_;
        int x_;
        int y_;
        double speed_;
        float battery_;
        std::vector<Sensor> sensors_;
        const Field& field_;
        
        
};

std::ostream& operator<<(std::ostream& os, const Vehicle& vehicle);





#endif
