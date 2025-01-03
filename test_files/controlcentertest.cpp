// Test per il corretto funzionamento del control center. 

#include "controlcenter.h"
#include "vehicle.h"
#include "field.h"
#include "sensor.h"
#include "soil.h"

void testControlCenter() {
    // Creazione di un campo di 10x10 celle.
    Field field("fattoriaRoss", 10, 10);
    std::cout << "Field address in test: " << &field << std::endl;
    // Creazione di un veicolo con 4 sensori.
    std::vector<Sensor> sensors = {Sensor(Sensor::SensorType::SoilTemperatureSensor),
                                   Sensor(Sensor::SensorType::AirTemperatureSensor),
                                   Sensor(Sensor::SensorType::MoistureSensor),
                                   Sensor(Sensor::SensorType::HumiditySensor)};
    Vehicle vehicle("Ford", 10000, Vehicle::VehicleType::FieldVehicle, 0, 0, 1.0, 100.0, sensors, field);
    // Creazione di un control center.
    ControlCenter controlCenter(field);
    // Invio di un comando di movimento al veicolo.
    controlCenter.sendMovementCommandToVehicle(vehicle, 5, 5);
    // invio di comando di lettura dei dati al veicolo.
    controlCenter.commandDataRead(vehicle);
    // Analisi dei dati.
    controlCenter.analyzeData();
    std::cout << vehicle.getX();
    std::cout << vehicle.getY();
    // Aggiungi piante in una zona del campo
    field.modifySoilProperty(0, 4, 0, 4, [](Soil& soil) { soil.setPlants(true); });
    Soil soil2;
field.getSoil(2, 2, soil2);
std::cout << "After modification: hasPlants = " << (soil2.getPlants() ? "Yes" : "No") << std::endl;


    // stampa le piante presenti nel campo
    field.printPlantPresence();
    
    // manda il veicolo a leggere i dati in quella zona del campo con le piante
    controlCenter.sendMovementCommandToVehicle(vehicle, 2, 2);
    controlCenter.commandDataRead(vehicle);
    controlCenter.analyzeData();

    // Modifico dati in una cella attigua
    field.modifySoilProperty(3, 4, 3, 4, [](Soil& soil) { soil.setSoilMoisture(95); });
    // Mando il veicolo a leggere i dati in quella zona
    controlCenter.sendMovementCommandToVehicle(vehicle, 3, 3);
    controlCenter.commandDataRead(vehicle);
    controlCenter.analyzeData();
}

int main() {
    testControlCenter();
    // stampa i risultati del test
    return 0;
}


