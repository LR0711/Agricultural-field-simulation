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



void vehicleTask(Vehicle& vehicle, ControlCenter& controlCenter, const std::vector<std::pair<int, int>>& plantPositions) {
    for (const auto& pos : plantPositions) {
        std::cout << "Debug: Plant position (" << pos.first << ", " << pos.second << ")" << std::endl;
        controlCenter.sendMovementCommandToVehicle(vehicle, pos.first, pos.second);
        controlCenter.commandDataRead(vehicle);
    }
    // Segnala che la raccolta dati è completata
    controlCenter.setDataCollectionComplete(true);
    controlCenter.notifyDataCollectionComplete();
}

void controlCenterTask(ControlCenter& controlCenter) {
    while (true) {
        {
            if (controlCenter.isDataCollectionComplete() && controlCenter.isBufferEmpty() && !controlCenter.isAnalyzing()) {
                std::cout << "Debug: Exiting analysis loop - all data processed" << std::endl;
                break;
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(1)); // Riduci il tempo di attesa
        controlCenter.analyzeData();
    }
    controlCenter.setAnalysisComplete(true);
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
    
    // Creazione di due veicoli
    Vehicle vehicle("Veicolo1", 10000, Vehicle::VehicleType::FieldVehicle, 2, 2, 1.0, 100.0, sensors, field);
    Vehicle vehicle2("Veicolo2", 10001, Vehicle::VehicleType::FieldVehicle, 3, 3, 1.2, 100.0, sensors, field);

    // Creazione del centro di controllo, a cui viene passato il campo e impostato il numero di veicoli attivi
    ControlCenter controlCenter(field);
    controlCenter.setActiveVehicles(2);

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
    // Creazione di un suolo con tipo di suolo, presenza di piante, umidità del suolo, temperatura dell'aria e umidità dell'aria
    Soil soil(Soil::SoilType::silt, true, 13.0, 12.0, 61.0);
    // Modifica del suolo in una specifica posizione del campo
    field.changeDimensions(10, 10);
    field.setSoil(soil, 6, 8, 6, 9);
    // Stampa a video dei tipi di suolo presenti nel campo
    field.printPlantPresence();




    // Acquisizione delle posizioni delle piante
    std::vector<std::pair<int, int>> plantPositions;
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            if (field.getPlants(i, i, j, j)[0][0]) {
                plantPositions.emplace_back(i, j);
            }
        }
    }

// Suddivisione delle posizioni delle piante tra i due veicoli
std::vector<std::pair<int, int>> plantPositions1(plantPositions.begin(), plantPositions.begin() + plantPositions.size() / 2);
std::vector<std::pair<int, int>> plantPositions2(plantPositions.begin() + plantPositions.size() / 2, plantPositions.end());


    // Creazione dei thread
    std::thread vehicle1Thread(vehicleTask, std::ref(vehicle), std::ref(controlCenter), plantPositions1);
    std::thread vehicle2Thread(vehicleTask, std::ref(vehicle2), std::ref(controlCenter), plantPositions2);

    std::thread controlCenterThread(controlCenterTask, std::ref(controlCenter));

    // Attesa del completamento del thread del veicolo
    vehicle1Thread.join();
    vehicle2Thread.join();

    // Aspetta che il centro di controllo termini l'analisi
    controlCenterThread.join(); 

    // Stampa dei risultati dell'analisi in un file di testo consultabile dall'utente
    std::ofstream outFile("analysis_results.txt");
    for (const auto& result : controlCenter.getAnalysisResults()) {
        outFile << result << std::endl;
    }
    outFile.close();
    // Stampa a video il messaggio di completamento
    std::cout << "Exiting from Main Thread" << std::endl;
    return 0;
}


