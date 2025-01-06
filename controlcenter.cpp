#include "controlcenter.h"
#include "vehicle.h"
#include "field.h"
#include <mutex>
#include <thread>
#include <chrono>
#include <iostream>

// Costruttore del control center: viene passato il campo come parametro per poter accedere ai dati del terreno
ControlCenter::ControlCenter(const Field& field)
    : field_(field)
{}

// Funzione per inviare un comando di movimento a un veicolo
void ControlCenter::sendMovementCommandToVehicle(Vehicle& vehicle, int x, int y) {
    std::unique_lock<std::mutex> lock(vehiclepositionmutex_); // Lock per proteggere l'accesso alla mappa delle posizioni dei veicoli
    cellfreecv_.wait(lock, [this, x, y] { 
        for (const auto& pos : vehiclepositions_) {
            if (pos.second.first == x && pos.second.second == y) {
                return false;
            }
        }
        return true;
    }); 
    vehiclepositions_[vehicle.getId()] = {x, y}; 
    lock.unlock(); 

    vehicle.moveToTarget(x, y);

    lock.lock(); 
    vehiclepositions_[vehicle.getId()] = {vehicle.getX(), vehicle.getY()}; // modified
    cellfreecv_.notify_all(); 
}


// Funzione per inviare un comando di lettura dei dati a un veicolo
void ControlCenter::commandDataRead(Vehicle& vehicle) {
    vehicle.readAndSendData(*this); // Il veicolo legge i dati e li invia al control center
}

// Funzione per aggiungere i dati al buffer del control center
void ControlCenter::appendData(const std::vector<SoilData>& dataBatch) {
    std::unique_lock<std::mutex> lock(bufferMutex_); // Protegge l'accesso al buffer: posso scrivere solo se nessun altro veicolo sta scrivendo
    databuffer_.push(dataBatch);
    std::cout << "Debug: Data appended to buffer" << std::endl;
    cvnotdata_.notify_one(); // Notifica il control center che ci sono nuovi dati nel buffer
}

// Funzione per la lettura e l'analisi dei dati del buffer
void ControlCenter::analyzeData() {
    std::cout << "Debug: Buffer size before analysis: " << databuffer_.size() << std::endl;
    std::unique_lock<std::mutex> lock(bufferMutex_); // Protegge l'accesso al buffer su cui agisce sia il control center che i veicoli
    // Debug iniziale
    std::cout << "Debug (analyzeData): Entrando in wait, activevehicles_ = " 
              << activevehicles_ 
              << ", dataCollectionComplete_ = " << dataCollectionComplete_ 
              << ", databuffer_.empty() = " << databuffer_.empty() 
              << std::endl;
    // Finché ci sono dati nel buffer o la raccolta dati non è completata, il control center analizza i dati
    while (true) { 
        cvnotdata_.wait(lock, [this] {
        // Debug dentro la lambda
        std::cout << "Debug (wait lambda): activevehicles_ = " 
                  << activevehicles_
                  << ", dataCollectionComplete_ = " << dataCollectionComplete_
                  << ", databuffer_.empty() = " << databuffer_.empty() 
                  << std::endl;
             return !databuffer_.empty() || dataCollectionComplete_; }); // Attendi finché non ci sono dati nel buffer o la raccolta dati non è completata
        std::cout << "Debug (analyzeData): Uscito da wait, databuffer_.empty() = " 
              << databuffer_.empty() 
              << std::endl;
        if (databuffer_.empty() && dataCollectionComplete_) { 
            break; 
        }
        std::cout << "Debug: Analyzing data..." << std::endl;
        isanalyzing_ = true; // Tale booleano indica che il control center sta analizzando i dati

        auto dataBatch = databuffer_.front();
        databuffer_.pop();
        lock.unlock();

        // Simula il tempo di analisi
        std::this_thread::sleep_for(std::chrono::seconds(5));

        // Analisi del dato
        std::map<Sensor::SensorType, double> dataMap;
        for (const auto& data : dataBatch) {
            dataMap[data.type] += data.data;
        }

        int x{dataBatch[0].x};
        int y{dataBatch[0].y};
        std::cout << "Debug: Analyzing data for cell (" << x << ", " << y << ")" << std::endl;
        // Dalle coordinate del veicolo, si ottiene il tipo di suolo e si verifica la presenza di piante
        Soil AnalyzedSoil;
        field_.getSoil(x, y, AnalyzedSoil);
        bool hasPlants{AnalyzedSoil.getPlants()};
        std::string soilType = AnalyzedSoil.soilTypeToString(AnalyzedSoil.getSoilType());

        std::cout << "Debug: Soil has plants: " << (hasPlants ? "Yes" : "No") << std::endl;

        std::vector<std::string> analysisResults;
        // Se ci sono piante, si procede con l'analisi, altrimenti questa non è necessaria
        if (hasPlants) {
            std::cout << "Plants detected, proceeding with analysis." << std::endl;
            for (const auto& sensorData : dataMap) {
                std::string result = evaluateData(soilType, sensorData.first, sensorData.second, x, y);
                std::cout << "  " << Sensor::sensorTypeToString(sensorData.first) << ": " << sensorData.second << " (" << result << ")" << std::endl;
                analysisResults.push_back(result);
            }
        } else {
            std::cout << "No plants in this area. No need for analysis." << std::endl;
        }

        lock.lock(); // Riacquisisce il lock per scrivere
        analysisResults_.insert(analysisResults_.end(), analysisResults.begin(), analysisResults.end());
    }

    isanalyzing_ = false;
    cvnotdata_.notify_all(); // Notifica che l'analisi è completata e che il buffer è vuoto
    std::cout << "Debug: Analysis complete for current buffer." << std::endl;
}


// Funzione per valutare i dati in base al tipo di sensore e al tipo di suolo
std::string ControlCenter::evaluateData(const std::string& soilType, Sensor::SensorType sensorType, double value, int x, int y) {
        std::string result;
        switch (sensorType) {
        case Sensor::SensorType::MoistureSensor:
            result = evaluateSoilMoisture(value, soilType);
            break;
        case Sensor::SensorType::SoilTemperatureSensor:
            result = evaluateSoilTemperature(value, soilType);
            break;
        case Sensor::SensorType::HumiditySensor:
            result = evaluateAirHumidity(value, soilType);
            break;
        case Sensor::SensorType::AirTemperatureSensor:
            result = evaluateAirTemperature(value, soilType);
            break;
        default:
            return "Unrecognized sensor type";
            break;
    }
// Aggiungi la posizione alla stringa result quando stampi i risultati
    result += " at position (" + std::to_string(x) + ", " + std::to_string(y) + ")";
    return result;
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

// Funzione per ottenere i risultati dell'analisi
std::vector<std::string> ControlCenter::getAnalysisResults() const {
    return analysisResults_;
}

// Funzione per verificare se il buffer è vuoto
bool ControlCenter::isBufferEmpty() {
    std::lock_guard<std::mutex> lock(bufferMutex_);
    return dataBuffer_.empty();
}

// Funzione per notificare che la raccolta dati è completata
void ControlCenter::notifyDataCollectionComplete() {
    std::unique_lock<std::mutex> lock(bufferMutex_);
    cvnotdata_.notify_all();
}

// Funzione per verificare se il control center sta analizzando i dati
bool ControlCenter::isAnalyzing() {
    std::unique_lock<std::mutex> lock(bufferMutex_);
    return isanalyzing_;
}

// Funzione per impostare la flag di completamento della raccolta dati
void ControlCenter::setDataCollectionComplete(bool status) {
    std::unique_lock<std::mutex> lock(bufferMutex_);
    if (status) {
        if (activevehicles_ > 0) { // Verifica che activevehicles_ non diventi negativo
            --activevehicles_;
        }
    } else {
        ++activevehicles_;
    }
    dataCollectionComplete_ = (activevehicles_ == 0);
    // Messaggio di debug
    std::cout << "Debug (setDataCollectionComplete): activevehicles_ = " 
              << activevehicles_ 
              << ", dataCollectionComplete_ = " << dataCollectionComplete_ 
              << std::endl;
    // Notifica tutti i thread in attesa che qualcosa è cambiato
    cvnotdata_.notify_all();
}

// Funzione per impostare la flag di completamento dell'analisi
void ControlCenter::setAnalysisComplete(bool status) {
    analysisComplete_ = status;
}

void ControlCenter::setActiveVehicles(int activevehicles) {
    activevehicles_ = activevehicles;
}