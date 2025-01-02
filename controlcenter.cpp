#include "controlcenter.h"
#include "vehicle.h"
#include "field.h"
#include <mutex>
#include <thread>
#include <chrono>
#include <iostream>

// Funzione per ottenere la posizione di un veicolo in ogni istante
std::pair<int, int> ControlCenter::getVehiclePosition(int vehicleId)  {
    std::unique_lock<std::mutex> lock(mtx_); // Protegge l'accesso alla mappa
    auto it = vehiclepositions_.find(vehicleId);
    if (it != vehiclepositions_.end()) {
        return (*it).second;
    }
    return {0, 0}; // Valore di default se il veicolo non è trovato
}

// Funzione per inviare un comando di movimento a un veicolo
void ControlCenter::sendMovementCommandToVehicle(Vehicle& vehicle, int x, int y) {
    vehicle.moveToTarget(x, y);
    std::unique_lock<std::mutex> lock(mtx_); // Protegge l'aggiornamento della mappa
    vehiclepositions_[vehicle.getId()] = {vehicle.getX(), vehicle.getY()};
}

// Funzione per inviare un comando di lettura dei dati a un veicolo
void ControlCenter::commandDataRead(Vehicle& vehicle) {
    vehicle.readAndSendData(*this); // Il veicolo legge i dati e li invia al control center
}

// Funzione per aggiungere i dati al buffer del control center
void ControlCenter::appendData(const std::vector<SoilData>& dataBatch) {
    std::unique_lock<std::mutex> lock(mtx_); // Protegge l'accesso al buffer: posso scrivere solo se nessun altro veicolo sta scrivendo
    databuffer_.push(dataBatch);
    cvnotdata_.notify_one(); // Notifica il control center che ci sono nuovi dati nel buffer
}

// Funzione per la lettura e l'analisi dei dati del buffer
void ControlCenter::analyzeData() {
    std::unique_lock<std::mutex> lock(mtx_);
    while (isanalyzing_ || databuffer_.empty()) {
        cvnotdata_.wait(lock);
    }
    isanalyzing_ = true;

    auto dataBatch = databuffer_.front();
    databuffer_.pop();
    lock.unlock();

    // Simula il tempo di analisi
    std::this_thread::sleep_for(std::chrono::seconds(10));

    // Analisi del dato
    std::map<Sensor::SensorType, double> dataMap;
    for (const auto& data : dataBatch) {
        dataMap[data.type] += data.data;
    }

    // Analisi del dato
    std::map<Sensor::SensorType, double> dataMap;
    for (const auto& data : dataBatch) {
        dataMap[data.type] = data.data;
    }

    int x {dataBatch[0].x};
    int y {dataBatch[0].y};
    Soil AnalyzedSoil;
    field_.getSoil(x, y, AnalyzedSoil);

    bool hasPlants = AnalyzedSoil.getPlants();
    std::string soilType = AnalyzedSoil.soilTypeToString(AnalyzedSoil.getSoilType());
    std::cout << "Analyzing data for cell (" << x << ", " << y << ") with sensor values in appropriate units:" << std::endl;
    // Buffer per salvare i risultati dell'analisi
    std::vector<std::string> analysisResults;

    for (const auto& sensorData : dataMap) {
        std::string result = evaluateData(soilType, sensorData.first, sensorData.second);
        std::cout << "  " << Sensor::sensorTypeToString(sensorData.first) << ": " << sensorData.second << " (" << result << ")" << std::endl;
        analysisResults.push_back(result);

}
    lock.lock();
    isanalyzing_ = false;
    cvnotdata_.notify_all();
}

// Funzione per valutare i dati in base al tipo di sensore e al tipo di suolo
std::string ControlCenter::evaluateData(const std::string& soilType, Sensor::SensorType sensorType, double value) {
        switch (sensorType) {
        case Sensor::SensorType::MoistureSensor:
            return evaluateSoilMoisture(value, soilType);
        case Sensor::SensorType::SoilTemperatureSensor:
            return evaluateSoilTemperature(value, soilType);
        case Sensor::SensorType::HumiditySensor:
            return evaluateAirHumidity(value, soilType);
        case Sensor::SensorType::AirTemperatureSensor:
            return evaluateAirTemperature(value, soilType);
        default:
            return "Unrecognized sensor type";
    }
}

// Funzione per valutare l'umidità del suolo in base al tipo di suolo: suoli diversi hanno valori di umidità ottimali diversi
std::string ControlCenter::evaluateSoilMoisture(double value, const std::string& soilType) {
    if (soilType == "Sand") {
        if (value >= 10 && value <= 30) return "Optimal soil moisture"; // Suoli sabbiosi è bene che siano poco umidi
        if ((value >= 5 && value < 10) || (value > 30 && value <= 40)) return "Discrete soil moisture";
        return "Critical: " + std::string(value < 5 ? "Too dry" : "Too wet");
    }
    else if (soilType == "Clay") {
        if (value >= 40 && value <= 60) return "Optimal soil moisture"; // Suoli argillosi è bene che siano molto umidi
        if ((value >= 30 && value < 40) || (value > 60 && value <= 70)) return "Discrete soil moisture";
        return "Critical: " + std::string(value < 30 ? "Too dry" : "Too wet");
    }
    else if (soilType == "Silt") {
        if (value >= 30 && value <= 50) return "Optimal soil moisture"; // Suoli limosi è bene che siano umidi
        if ((value >= 20 && value < 30) || (value > 50 && value <= 60)) return "Discrete soil moisture";
        return "Critical: " + std::string(value < 20 ? "Too dry" : "Too wet");
    }
    else if (soilType == "Loam") {
        if (value >= 25 && value <= 50) return "Optimal soil moisture"; // Suoli limosi è bene che siano umidi
        if ((value >= 20 && value < 25) || (value > 50 && value <= 60)) return "Discrete soil moisture";
        return "Critical: " + std::string(value < 20 ? "Too dry" : "Too wet");
    }
    else {
        return "Unrecognized soil type";
    }
}

// Funzione per valutare la temperatura del suolo in base al tipo di suolo: suoli diversi hanno valori di temperatura ottimali diversi
std::string ControlCenter::evaluateSoilTemperature(double value, const std::string& soilType) {
    if (soilType == "Sand") {
        if (value >= 18 && value <= 25) return "Optimal soil temperature"; // Suoli sabbiosi è bene che siano più caldi
        if ((value >= 10 && value < 18) || (value > 25 && value <= 30)) return "Discrete soil temperature";
        return "Critical: " + std::string(value < 10 ? "Too cold soil" : "Too hot soil");
    }
    else if (soilType == "Clay") {
        if (value >= 15 && value <= 22) return "Optimal soil temperature"; // Suoli argillosi è bene che siano freddi
        if ((value >= 10 && value < 15) || (value > 22 && value <= 28)) return "Discrete soil temperature";
        return "Critical: " + std::string(value < 10 ? "Too cold soil" : "Too hot soil");
    }
    else if (soilType == "Silt") {
        if (value >= 16 && value <= 24) return "Optimal soil temperature"; // Suoli limosi è bene che siano caldi
        if ((value >= 10 && value < 16) || (value > 24 && value <= 30)) return "Discrete soil temperature";
        return "Critical: " + std::string(value < 10 ? "Too cold soil" : "Too hot soil");
    }
    else if (soilType == "Loam") {
        if (value >= 18 && value <= 25) return "Optimal soil temperature"; // Suoli limosi è bene che siano caldi
        if ((value >= 15 && value < 18) || (value > 25 && value <= 30)) return "Discrete soil temperature";
        return "Critical: " + std::string(value < 15 ? "Too cold soil" : "Too hot soil");
    }
    else {
        return "Unrecognized soil type";
    }
}

// Funzione per valutare l'umidità dell'aria in base al tipo di suolo: suoli diversi hanno valori di umidità ottimali diversi
std::string ControlCenter::evaluateAirHumidity(double value, const std::string& soilType) {
    if (soilType == "Sand") {
        if (value >= 40 && value <= 60) return "Optimal air humidity"; // Suoli sabbiosi è bene che siano poco umidi
        if ((value >= 30 && value < 40) || (value > 60 && value <= 70)) return "Discrete air humidity";
        return "Critical: " + std::string(value < 30 ? "Too dry air" : "Too humid");
    }
    else if (soilType == "Clay") {
        if (value >= 50 && value <= 70) return "Optimal air humidity"; // Suoli argillosi è bene che siano molto umidi
        if ((value >= 40 && value < 50) || (value > 70 && value <= 80)) return "Discrete air humidity";
        return "Critical: " + std::string(value < 40 ? "Too dry air" : "Too humid");
    }
    else if (soilType == "Silt") {
        if (value >= 40 && value <= 65) return "Optimal air humidity"; // Suoli limosi è bene che siano poco umidi
        if ((value >= 30 && value < 40) || (value > 65 && value <= 75)) return "Discrete air humidity";
        return "Critical: " + std::string(value < 30 ? "Too dry air" : "Too humid");
    }
    else if (soilType == "Loam") {
        if (value >= 50 && value <= 70) return "Optimal air humidity"; // Suoli limosi è bene che siano tanto umidi
        if ((value >= 40 && value < 50) || (value > 70 && value <= 80)) return "Discrete air humidity";
        return "Critical: " + std::string(value < 40 ? "Too dry air" : "Too humid");
    }
    else {
        return "Unrecognized soil type";
    }
}

// Funzione per valutare la temperatura dell'aria in base al tipo di suolo: suoli diversi hanno valori di temperatura ottimali diversi
std::string ControlCenter::evaluateAirTemperature(double value, const std::string& soilType) {
    if (soilType == "Sand") {
        if (value >= 20 && value <= 30) return "Optimal air temperature"; // Suoli sabbiosi è bene che siano in ambienti caldi
        if ((value >= 15 && value < 20) || (value > 30 && value <= 35)) return "Discrete air temperature";
        return "Critical: " + std::string(value < 15 ? "Air too cold" : "Air too hot");
    }
    else if (soilType == "Clay") {
        if (value >= 18 && value <= 25) return "Optimal air temperature"; // Suoli argillosi è bene che siano in ambienti areati
        if ((value >= 15 && value < 18) || (value > 25 && value <= 30)) return "Discrete air temperature";
        return "Critical: " + std::string(value < 15 ? "Air too cold" : "Air too hot");
    }
    else if (soilType == "Silt") {
        if (value >= 18 && value <= 26) return "Optimal air temperature"; // Suoli limosi è bene che siano in ambienti areati
        if ((value >= 15 && value < 18) || (value > 26 && value <= 30)) return "Discrete air temperature";
        return "Critical: " + std::string(value < 15 ? "Air too cold" : "Air too hot");
    }
    else if (soilType == "Loam") {
        if (value >= 20 && value <= 28) return "Optimal air temperature"; // Suoli limosi è bene che siano in ambienti caldi
        if ((value >= 15 && value < 20) || (value > 28 && value <= 32)) return "Discrete air temperature";
        return "Critical: " + std::string(value < 15 ? "Air too cold" : "Air too hot");
    }
    else {
        return "Unrecognized soil type";
    }
}
