// La classe "ControlCenter" rappresenta il centro di controllo del sistema di monitoraggio agricolo.
// Per tale ragione, essa viene definita passando per const reference un oggetto di tipo "Field" che rappresenta il campo agricolo da monitorare.
// Sfruttando i concetti basilari della programmazione concorrente, la classe "ControlCenter" comanda i veicoli sul campo e riceve i dati da essi.
// All'interno della classe ho infatti un buffer di dati raccolti tramite sensori, oltre che una mappa che tiene traccia delle posizioni dei veicoli.
// La classe ha anche un mutex per proteggere l'accesso a tali strutture dati.
// Sono presenti vari metodi legati all'invio di comandi ai veicoli, alla raccolta dei dati, all'analisi dei dati e alla restituzione dei risultati.
// La classe è inoltre dotata di vari metodi per la gestione del buffer e delle variabili di stato.
// Tutti i metodi sono sinteticamente spiegati nel file "controlcenter.cpp".

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


struct SoilData {
    int x;
    int y;
    Sensor::SensorType type;
    double data; 
};

class ControlCenter {
    public:
        ControlCenter(const Field& field);
        void sendMovementCommandToVehicle(Vehicle& vehicle, int x, int y);
        void commandDataRead(Vehicle& vehicle);
        void appendData(const std::vector<SoilData>& dataBatch);
        void analyzeData();
        std::vector<std::string> getAnalysisResults() const;
        bool isBufferEmpty();
        void notifyDataCollectionComplete();
        bool isAnalyzing();
        bool isDataCollectionComplete() const { return dataCollectionComplete_; }
        void setDataCollectionComplete(bool status);
        bool isAnalysisComplete() const { return analysisComplete_; }
        void setAnalysisComplete(bool status);
        void setActiveVehicles(int activevehicles);

    private:
        const Field& field_;
        std::map<int, std::pair<int, int>> vehiclepositions_;
        std::queue<vector<SoilData>> databuffer_;
        std::mutex bufferMutex_;
        std::mutex vehiclepositionmutex_;
        std::condition_variable cvnotdata_;
        std::condition_variable cellfreecv_;
        std::vector<std::string> dataBuffer_;
        std::vector<std::string> analysisResults_;
        std::string evaluateData(const std::string& soilType, Sensor::SensorType sensorType, double value, int x, int y);
        std::string evaluateSoilMoisture(double value, const std::string& soilType);
        std::string evaluateSoilTemperature(double value, const std::string& soilType);
        std::string evaluateAirTemperature(double value, const std::string& soilType);
        std::string evaluateAirHumidity(double value, const std::string& soilType);
        int activevehicles_;
        bool isanalyzing_ = false;
        bool dataCollectionComplete_ = false;
        bool analysisComplete_ = false;



};

#endif