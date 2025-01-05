// La classe "Vehicle" rappresenta l'unità autonoma che si muove all'interno del campo per raccogliere i dati.
// Ogni veicolo ha un nome, un id univoco, una posizione (x, y), una velocità, una batteria, un insieme di sensori e un riferimento al campo di operazione.
// I veicoli possono essere di due tipi: veicoli terrestri e veicoli aerei. Nella versione attuale del progetto la scelta tra l'uno e l'altro tipo non comporta alcuna distinzione.
// I veicoli possono muoversi all'interno del campo, leggere i dati dalla cella corrente e inviarli al control center.
// Al fine di aggiungere aspetti di realtà alla simulazione, i veicoli scaricano la loro batteria eseguendo azioni, e al raggiungimento di una soglia critica sono "costretti" ad interrompere per del tempo le operazioni.
// Ciò viene simulato con apposite funzioni spiegate nel file "vehicle.cpp".
// Nel file sono inoltre inseriti descrizioni sintetiche delle funzioni.


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
#include <mutex>
using std::mutex;
#include <condition_variable>
using std::condition_variable;
#include <utility>
using std::pair;


class ControlCenter;


class Vehicle {
    public:
        enum class VehicleType {FieldVehicle, AerialVehicle};
        Vehicle();
        Vehicle(string name, int id, VehicleType type, int x, int y, double speed, float battery, std::vector<Sensor> sensors, const Field& field);
        void setPosition(int x, int y);
        void moveToTarget(int targetx, int targety);
        void readDataFromCurrentCell() const;
        void readAndSendData(ControlCenter& controlCenter);
        std::string getName() const {return name_;}
        VehicleType getType() const {return type_;}
        int getId() const { return id_; }
        int getX() const {return x_;}
        int getY() const {return y_;}
        double getSpeed() const {return speed_;}
        std::string vehicleTypeToString(VehicleType type) const;
        void drainBattery(float amount);
        void rechargeBattery();


    private:
        string name_;
        int id_;
        static int nextId_; // Tale contatore statico serve per assegnare un id univoco ad ogni veicolo.
        VehicleType type_;
        int x_;
        int y_;
        double speed_;
        float battery_;
        std::vector<Sensor> sensors_;
        const Field& field_;
        bool isBusy_;
        std::mutex vehiclemutex_;
        std::condition_variable cvnotbusy_;

        
        
};

std::ostream& operator<<(std::ostream& os, const Vehicle& vehicle);


#endif
