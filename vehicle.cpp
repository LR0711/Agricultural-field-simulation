#include "vehicle.h"
#include "controlcenter.h"
#include <iostream>
#include <thread>
#include <chrono>

// Inizializzazione del contatore statico per gli id dei veicoli.
int Vehicle::nextId_ = 10000;
// Costruttore di default
Vehicle::Vehicle()
    :name_{"???"},
    type_{VehicleType::FieldVehicle},
    x_{0},
    y_{0},
    speed_{1.0},
    battery_{100.0},
    sensors_{},
    field_{Field()},
    isBusy_{false},
    id_{nextId_++}
    

    {}

// Costruttore con parametri
Vehicle::Vehicle(std::string name, int id, VehicleType type, int x, int y, double speed, float battery, std::vector<Sensor> sensors, const Field& field)
    : name_{name},
      type_{type},
      x_{x},
      y_{y},
      speed_{speed},
      battery_{battery},
      sensors_{sensors},
      field_{field},
      isBusy_{false},
      id_{nextId_++}
    
    
    {
        // Quando viene creato, il veicolo deve trovarsi nel campo: se non è così, il programma termina.
        if (x < 0 || x >= field_.getLength() || y < 0 || y >= field_.getWidth()) {
            std::cerr << "Invalid coordinates: vehicle is out of field." << std::endl;
            exit(EXIT_FAILURE);
        }
        // La velocità è intesa come massima velocità del veicolo, è in celle al secondo e va riportata in valore assoluto.
        if (speed <= 0) {
            std::cerr << "Invalid speed: speed must be greater than 0." << std::endl;
            exit(EXIT_FAILURE);
        }
    }

// Funzione per settare la posizione del veicolo: può essere usata anche nell'ipotesi irrealistica di spostamenti istantanei.
void Vehicle::setPosition(int x, int y) {
    // Check per controllare di non avere impostato posizioni fuori dal campo.
    if (x < 0 || x >= field_.getLength() || y < 0 || y >= field_.getWidth()) {
        std::cerr << "Invalid coordinates: vehicle is out of field." << std::endl;
        exit(EXIT_FAILURE);
    }
    x_ = x;
    y_ = y;
   

    std::cout << "Vehicle " << name_ << " moved to position (" << x_ << ", " << y_ << ")" << std::endl;
}

// Funzione per spostare il veicolo in una posizione target: il tempo di spostamento è proporzionale alla distanza tra la posizione attuale e la posizione target e alla velocità del veicolo.
void Vehicle::moveToTarget(int targetx, int targety) {
    // Lock per evitare che il veicolo vada verso un target se già in movimento o in fase di lettura.
    std::unique_lock<std::mutex> lock(mtx_);
    while (isBusy_) {
        cvnotbusy_.wait(lock);
    }
    isBusy_ = true;

    if (targetx < 0 || targetx >= field_.getLength() || targety < 0 || targety >= field_.getWidth()) {
        std::cerr << "Invalid coordinates: target is out of field." << std::endl;
        exit(EXIT_FAILURE);
        isBusy_ = false;
        cvnotbusy_.notify_one();
    }
    
    double steptime {1.0 / speed_};
    // Il veicolo può spostarsi in diagonale, dato che si sposta in contemporanea in x e y.
    while (x_ != targetx || y_ != targety) {
        if (x_ < targetx) {
            x_++;
        } else if (x_ > targetx) {
            x_--;
        }

        if (y_ < targety) {
            y_++;
        } else if (y_ > targety) {
            y_--;
        }

        std::cout << "Vehicle " << name_ << " moved to position (" << x_ << ", " << y_ << ")" << std::endl;
        std::this_thread::sleep_for(std::chrono::duration<float>(steptime)); // La thread si ferma per un tempo pari a steptime, simulando uno spostamento in tempo discreto del veicolo.
        
    
    }
    std::cout << "Vehicle " << name_ << " reached target at position (" << x_ << ", " << y_ << ")" << std::endl;
    
    

    isBusy_ = false;
    cvnotbusy_.notify_one();
}

// Funzione per leggere i dati dalla cella corrente: i dati sono passati ai sensori e stampati a video.
void Vehicle::readDataFromCurrentCell() const {
    Soil soil;
    // Viene chiamata la funzione getSoil per ottenere i dati del suolo nella posizione attuale del veicolo: se sono fuori dal campo o se avessi una cella "vuota", il programma termina.
    if (!field_.getSoil(x_, y_, soil)) {
        std::cerr << "Error: Unable to read soil data at position (" << x_ << ", " << y_ << ")" << std::endl;
        return;
    }
    // Per ogni sensore, viene chiamata la funzione corrispondente per ottenere i dati e stamparli a video.
    for (const auto& sensor : sensors_) {
        switch (sensor.getType()) {
            case Sensor::SensorType::SoilTemperatureSensor:
                std::cout << "Soil Temperature Sensor Data: " << soil.PassTemperatureToSensor(Sensor::SensorType::SoilTemperatureSensor) << std::endl;
                break;
            case Sensor::SensorType::AirTemperatureSensor:
                std::cout << "Air Temperature Sensor Data: " << soil.PassTemperatureToSensor(Sensor::SensorType::AirTemperatureSensor) << std::endl;
                break;
            case Sensor::SensorType::MoistureSensor:
                std::cout << "Moisture Sensor Data: " << soil.PassSoilMoistureToSensor(Sensor::SensorType::MoistureSensor) << std::endl;
                break;
            case Sensor::SensorType::HumiditySensor:
                std::cout << "Humidity Sensor Data: " << soil.PassAirHumidityToSensor(Sensor::SensorType::HumiditySensor) << std::endl;
            default:
                std::cerr << "Unknown sensor type" << std::endl;
        }
    }   
}

// Funzione per la lettura reale dei dati sul campo e l'invio al control center per la futura analisi.
void Vehicle::readAndSendData(ControlCenter& controlCenter) {
    std::unique_lock<std::mutex> lock(mtx_);
    while (isBusy_) {
        cvnotbusy_.wait(lock);
    }
    isBusy_ = true;
    Soil soil;
    if (!field_.getSoil(x_, y_, soil)) {
        std::cerr << "Error: Unable to read soil data at position (" << x_ << ", " << y_ << ")" << std::endl;
        isBusy_ = false;
        cvnotbusy_.notify_one();
        return;
    }

    std::vector<SoilData> dataBatch;
    for (const auto& sensor : sensors_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulazione di un tempo di lettura dei dati.
        SoilData data;
        switch (sensor.getType()) {
            case Sensor::SensorType::SoilTemperatureSensor:
                data = {x_, y_, Sensor::SensorType::SoilTemperatureSensor, soil.PassTemperatureToSensor(Sensor::SensorType::SoilTemperatureSensor)};
                break;
            case Sensor::SensorType::AirTemperatureSensor:
                data = {x_, y_, Sensor::SensorType::AirTemperatureSensor, soil.PassTemperatureToSensor(Sensor::SensorType::AirTemperatureSensor)};
                break;
            case Sensor::SensorType::MoistureSensor:
                data = {x_, y_, Sensor::SensorType::MoistureSensor, soil.PassSoilMoistureToSensor(Sensor::SensorType::MoistureSensor)};
                break;
            case Sensor::SensorType::HumiditySensor:
                data = {x_, y_, Sensor::SensorType::HumiditySensor, soil.PassAirHumidityToSensor(Sensor::SensorType::HumiditySensor)};
                break;
            default:
                std::cerr << "Unknown sensor type" << std::endl;
        }
        dataBatch.push_back(data);
    }

    controlCenter.appendData(dataBatch); // Invia il batch di dati al centro di controllo

    isBusy_ = false;
    cvnotbusy_.notify_one();
}

// Funzione per convertire il tipo di veicolo in una stringa
std::string Vehicle::vehicleTypeToString(VehicleType type) const {
    switch (type) {
        case VehicleType::FieldVehicle:
            return "Field Vehicle ";
        case VehicleType::AerialVehicle:
            return "Aerial Vehicle ";
        default:
            return "Unknown Vehicle Type ";
    }
}

// Overload dell'operatore di output per stampare i dati del veicolo.
std::ostream& operator<<(std::ostream& os, const Vehicle& vehicle) {
    os << vehicle.getName() << "currently at position (" << vehicle.getX() << ", " << vehicle.getY() << ")" << std::endl;
    return os;
        
}
