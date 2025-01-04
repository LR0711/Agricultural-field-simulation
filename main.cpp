// Il seguente file main è stato creato per testare il funzionamento del sistema:
// In tale file vengono creati un campo in condizioni statiche ed harcoded ma varibili nelle sue aree, un veicolo e un centro di controllo.
// Di seguito vengono eseguite le seguenti operazioni:
// -Il sistema acquisisce tutte le posizioni delle piante presenti nel campo
// -Il centro di controllo chiede al veicolo di spostarsi in tutte queste posizioni
// - All'arrivo in ogni posizione, il veicolo legge i dati del suolo e li invia al centro di controllo
// - Nel mentre, il centro di controllo periodicamente preleva i dati dal buffer e li analizza
// Il centro di controllo, al termine dell'operazione di movimento del veicolo e svuotato il buffer, stampa il vettore di risultati in un apposito file .txt

#include "controlcenter.h"
#include "vehicle.h"
#include "field.h"
#include "sensor.h"
#include "soil.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>


// Variabili globali per il buffer dei dati e lo stato del sistema
std::vector<SoilData> globalDataBuffer;
std::mutex bufferMutex; // 
bool dataCollectionComplete = false; // Indica che la raccolta dati è completata
bool analysisComplete = false; // Indica che l'analisi è terminata

void vehicleTask(Vehicle& vehicle, ControlCenter& controlCenter, const std::vector<std::pair<int, int>>& plantPositions) {
    for (const auto& pos : plantPositions) {
        std::cout << "Debug: Plant position (" << pos.first << ", " << pos.second << ")" << std::endl;
        controlCenter.sendMovementCommandToVehicle(vehicle, pos.first, pos.second);
        controlCenter.commandDataRead(vehicle);
    }
    // Segnala che la raccolta dati è completata
    std::unique_lock<std::mutex> lock(bufferMutex);
    dataCollectionComplete = true;
    controlCenter.notifyDataCollectionComplete(); // Notifica il centro di controllo
}

void controlCenterTask(ControlCenter& controlCenter) {
    while (true) {
        {
            std::unique_lock<std::mutex> lock(bufferMutex);
            if (dataCollectionComplete && controlCenter.isBufferEmpty() && !controlCenter.isAnalyzing()) {
                std::cout << "Debug: Exiting analysis loop - all data processed" << std::endl;
                break;
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(1)); // Riduci il tempo di attesa
        controlCenter.analyzeData();
    }
    analysisComplete = true;
}

int main() {
    // Creazione del campo con dimensioni 10x10 e nome "Fattoria"
    Field field("Fattoria", 5, 4);

    // Creazione di un veicolo con 4 sensori
    std::vector<Sensor> sensors = {
        Sensor(Sensor::SensorType::SoilTemperatureSensor),
        Sensor(Sensor::SensorType::AirTemperatureSensor),
        Sensor(Sensor::SensorType::MoistureSensor),
        Sensor(Sensor::SensorType::HumiditySensor)
    };
    Vehicle vehicle("Veicolo1", 10000, Vehicle::VehicleType::FieldVehicle, 2, 3, 1.0, 100.0, sensors, field);

    // Creazione del centro di controllo
    ControlCenter controlCenter(field);

    // Aggiunta di piante in alcune aree del campo
    field.modifySoilProperty(0, 2, 1, 3, [](Soil& soil) { soil.setPlants(true); });
    field.modifySoilProperty(3, 3, 3, 3, [](Soil& soil) { soil.setPlants(true); });
    field.modifySoilProperty(1, 2, 0, 1, [](Soil& soil) { soil.setSoilMoisture(95); });
    field.modifySoilProperty(3, 4, 0, 1, [](Soil& soil) { soil.setSoilMoisture(5); });
    field.modifySoilProperty(0, 1, 2, 3, [](Soil& soil) { soil.setAirTemperature(30.0); });
    field.modifySoilProperty(3, 4, 2, 3, [](Soil& soil) { soil.setAirTemperature(10.0); });
    field.modifySoilProperty(0, 1, 0, 1, [](Soil& soil) { soil.setAirHumidity(80.0); });
    field.modifySoilProperty(3, 4, 0, 1, [](Soil& soil) { soil.setSoilType(Soil::SoilType::clay); });
    field.modifySoilProperty(0, 1, 2, 3, [](Soil& soil) { soil.setSoilType(Soil::SoilType::sand); });
    field.modifySoilProperty(3, 4, 2, 3, [](Soil& soil) { soil.setSoilType(Soil::SoilType::loam); });


    // Acquisizione delle posizioni delle piante
    std::vector<std::pair<int, int>> plantPositions;
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (field.getPlants(i, i, j, j)[0][0]) {
                plantPositions.emplace_back(i, j);
            }
        }
    }

        // Creazione dei thread
    std::thread vehicleThread(vehicleTask, std::ref(vehicle), std::ref(controlCenter), plantPositions);
    std::thread controlCenterThread(controlCenterTask, std::ref(controlCenter));

    // Attesa del completamento del thread del veicolo
    vehicleThread.join();

    // Aspetta che il centro di controllo termini l'analisi
    controlCenterThread.join(); // Ora non servono più cicli di attesa

    // Stampa dei risultati dell'analisi
    std::ofstream outFile("analysis_results.txt");
    for (const auto& result : controlCenter.getAnalysisResults()) {
        outFile << result << std::endl;
    }
    outFile.close();

    std::cout << "Exiting from Main Thread" << std::endl;
    return 0;
}


