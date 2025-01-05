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
    id_{nextId_++},
    type_{VehicleType::FieldVehicle},
    x_{0},
    y_{0},
    speed_{1.0},
    battery_{100.0},
    sensors_{},
    field_{Field()},
    isBusy_{false} // All'inizio il veicolo non è impegnato
    {}

// Costruttore con parametri
Vehicle::Vehicle(std::string name, int id, VehicleType type, int x, int y, double speed, float battery, std::vector<Sensor> sensors, const Field& field)
    : name_{name},
      id_{nextId_++},
      type_{type},
      x_{x},
      y_{y},
      speed_{speed},
      battery_{battery},
      sensors_{sensors},
      field_{field},
      isBusy_{false}
      
    
    
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

        // La batteria è espressa in percentuale e deve essere compresa tra 0 e 100.
        if (battery <= 0 || battery > 100) {
            std::cerr << "Invalid battery: battery must be between 0 and 100%." << std::endl;
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
    std::unique_lock<std::mutex> lock(vehiclemutex_); // Lock per proteggere l'accesso alla variabile isBusy_: solo un thread alla volta può muovere il veicolo
    while (isBusy_) {
        cvnotbusy_.wait(lock); // Attendi finché il veicolo è impegnato
    }
    isBusy_ = true;

    // Controllo per verificare che la posizione target sia all'interno del campo
    if (targetx < 0 || targetx >= field_.getLength() || targety < 0 || targety >= field_.getWidth()) {
        std::cerr << "Invalid coordinates: target is out of field." << std::endl;
        isBusy_ = false;
        cvnotbusy_.notify_one();
        return;
    }

    double steptime = 1.0 / speed_; // Simula il tempo di movimento in base alla velocità

    std::cout << "Debug: Vehicle starting movement to (" << targetx << ", " << targety << ") from (" << x_ << ", " << y_ << ")" << std::endl;

    while (x_ != targetx || y_ != targety) {
        // Controllo della batteria
        if (battery_ <= 10.0) {
            std::cout << "Low battery. Returning to base for recharge..." << std::endl;

            // Ritorno alla base (0, 0)
            while (x_ != 0 || y_ != 0) {
                if (x_ > 0) {
                    --x_;}
                else if (x_ < 0) {
                ++x_;}

                if (y_ > 0) {
                    --y_;}
                else if (y_ < 0) {
                    ++y_;
                }

                std::cout << "Returning to base, current position: (" << x_ << ", " << y_ << ")" << std::endl;
                std::this_thread::sleep_for(std::chrono::duration<float>(steptime));
            }

            lock.unlock(); // Rilascia il lock durante la ricarica, in modo da consentire altre operazioni
            rechargeBattery(); // Ricarica la batteria
            lock.lock(); // Riacquisisce il lock dopo la ricarica

            std::cout << "Resuming movement to target (" << targetx << ", " << targety << ")" << std::endl;
        }

        // Movimento verso il target
        drainBattery(5); // Consuma 5% di batteria per ogni spostamento
        if (x_ < targetx) {
        ++x_;}
        else if (x_ > targetx) {
            --x_;}

        if (y_ < targety) {
            ++y_;}
        else if (y_ > targety) {
            --y_;}

        std::cout << "Vehicle " << name_ << " moved to position (" << x_ << ", " << y_ << ")" << std::endl;
        std::cout << "Battery level: " << battery_ << "%" << std::endl;

        std::this_thread::sleep_for(std::chrono::duration<float>(steptime)); // Simula il tempo di movimento
    }

    std::cout << "Vehicle " << name_ << " reached target at position (" << x_ << ", " << y_ << ")" << std::endl;

    isBusy_ = false;
    cvnotbusy_.notify_one(); // Notifica che il veicolo non è più impegnato e può essere utilizzato da altri thread
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
    std::unique_lock<std::mutex> lock(vehiclemutex_);
    while (isBusy_) {
        cvnotbusy_.wait(lock);
    }
    isBusy_ = true;

    int xToBeRead {x_};
    int yToBeRead {y_};

    while (battery_ <= 10.0) {
        std::cout << "Low battery. Returning to base for recharge..." << std::endl;

        // Ritorno alla base (0, 0)
        while (x_ != 0 || y_ != 0) {
            if (x_ > 0) --x_;
            else if (x_ < 0) ++x_;

            if (y_ > 0) --y_;
            else if (y_ < 0) ++y_;

            std::cout << "Returning to base, current position: (" << x_ << ", " << y_ << ")" << std::endl;
            std::this_thread::sleep_for(std::chrono::duration<float>(1.0 / speed_));
        }

        lock.unlock(); // Rilascia il lock durante la ricarica 
        rechargeBattery(); // Ricarica la batteria
        lock.lock(); // Riacquisisce il lock dopo la ricarica

        std::cout << "Resuming data collection at (" << xToBeRead << ", " << yToBeRead << ")" << std::endl;
    }
    // Consuma il 15% della batteria per la lettura dei dati
    drainBattery(15.0);

    Soil soil;
    if (!field_.getSoil(xToBeRead, yToBeRead, soil)) {
        std::cerr << "Error: Unable to read soil data at position (" << xToBeRead << ", " << yToBeRead << ")" << std::endl;
        isBusy_ = false;
        cvnotbusy_.notify_one();
        return;
    }

    // Lettura dei dati dai sensori
    std::vector<SoilData> dataBatch;
    for (const auto& sensor : sensors_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simula il tempo di lettura dei dati
        SoilData data;
        switch (sensor.getType()) {
            case Sensor::SensorType::SoilTemperatureSensor:
                data = {xToBeRead, yToBeRead, Sensor::SensorType::SoilTemperatureSensor, soil.PassTemperatureToSensor(Sensor::SensorType::SoilTemperatureSensor)};
                break;
            case Sensor::SensorType::AirTemperatureSensor:
                data = {xToBeRead, yToBeRead, Sensor::SensorType::AirTemperatureSensor, soil.PassTemperatureToSensor(Sensor::SensorType::AirTemperatureSensor)};
                break;
            case Sensor::SensorType::MoistureSensor:
                data = {xToBeRead, yToBeRead, Sensor::SensorType::MoistureSensor, soil.PassSoilMoistureToSensor(Sensor::SensorType::MoistureSensor)};
                break;
            case Sensor::SensorType::HumiditySensor:
                data = {xToBeRead, yToBeRead, Sensor::SensorType::HumiditySensor, soil.PassAirHumidityToSensor(Sensor::SensorType::HumiditySensor)};
                break;
            default:
                std::cerr << "Unknown sensor type" << std::endl;
        }
        dataBatch.push_back(data);
        std::cout << "Debug: Data read at position (" << xToBeRead << ", " << yToBeRead << ") for sensor " 
                  << Sensor::sensorTypeToString(sensor.getType()) << ": " << data.data << std::endl;
    }

    // Invio dei dati al centro di controllo
    controlCenter.appendData(dataBatch);
    std::cout << "Debug: Data sent for position (" << xToBeRead << ", " << yToBeRead << ")" << std::endl;

    isBusy_ = false;
    cvnotbusy_.notify_one();
}
// Funzione per la scarica della batteria del veicolo.
void Vehicle::drainBattery(float amount) {
    battery_ -= amount;
    if (battery_ < 0) {
        battery_ = 0;
        std::cout << "Battery empty. Vehicle stopped." << std::endl;
    }
    
}

// Funzione per la ricarica della batteria del veicolo.
void Vehicle::rechargeBattery() {
    std::cout << "Battery low. Recharging..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(15)); // Simula il tempo di ricarica
    battery_ = 100.0;
    std::cout << "Battery fully recharged." << std::endl;
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
