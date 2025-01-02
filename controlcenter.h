// The control center class is the main core for the concurrency problem in this project.
// This class gives to the vehicles the task to move in specified areas of the field and get datas from the soil.
// After that, the vehicles will send datas to the control center, including the position data, and the control center will elaborate them.
// In this way, the control center will help  users to take future decisions for their field, for example giving alerts for areas with low moisture soil level.
// The control center will analyze periodically all the datas stored in his buffer and sent by the vehicles.
// Lastly, vehicle will always communicate with the control center his actual position.
// It is important to consider this unit and every vehicle unit as a different thread, and the control center as the main thread.
// It is important to consider where to put the shared buffer, and how to manage the access to it.
//It is important to consider where to put the comunication channels between the vehicles and the control center.
// Lastly, it is important to consider which condition variables to use, and where to put them, and the mutexes to use.

#ifndef CONTROLCENTER_H
#define CONTROLCENTER_H
#include "vehicle.h"
#include "field.h"
#include "sensor.h"
#include <vector>
#include <map>
#include <mutex>
#include <condition_variable>
using std::condition_variable;
using std::mutex;
#include <queue>


typedef struct SoilData {
    int x;
    int y;
    Sensor::SensorType type;
    double data; 
};

class ControlCenter {
    public:
        void sendMovementCommandToVehicle(Vehicle& vehicle, int x, int y);
        std::pair<int, int> getVehiclePosition(int vehicleId);
        void ControlCenter::commandDataRead(Vehicle& vehicle);
        void appendData(const std::vector<SoilData>& dataBatch);
        void analyzeData();

    private:
        std::map<int, std::pair<int, int>> vehiclepositions_;
        std::queue<vector<SoilData>> databuffer_;
        std::mutex mtx_;
        std::condition_variable cvnotdata_;
        Field field_;
        bool isanalyzing_ = false;
        std::string evaluateData(const std::string& soilType, Sensor::SensorType sensorType, double value);
        std::string evaluateSoilMoisture(double value, const std::string& soilType);
        std::string evaluateSoilTemperature(double value, const std::string& soilType);
        std::string evaluateAirTemperature(double value, const std::string& soilType);
        std::string evaluateAirHumidity(double value, const std::string& soilType);



};

#endif