#include <iostream>
#include <string>
using std::cerr;
using std::endl;
using std::string;
#include "soil.h"
#include "sensor.h"

// Costruttore di default
Soil::Soil() 
    :soiltype_{SoilType::loam},
    plants_{false},
    soilmoisture_{50},
    airtemperature_{20.0},
    airhumidity_{50},
    soiltemperature_{calculateSoilTemperature()}
    {}

// Costruttore con parametri
Soil::Soil(SoilType soilType, bool plants, double soilMoisture, float airTemperature, double airHumidity)
    :soiltype_{soilType},
    plants_{plants},
    soilmoisture_{soilMoisture},
    airtemperature_{airTemperature},
    airhumidity_{airHumidity},
    soiltemperature_{calculateSoilTemperature()} // La temperatura del suolo si suppone dipenda dal tipo di suolo e dalle condizioni atmosferiche e di umidità.
    {
        // I valori di umidità sono espressi in percentuale, quindi devono essere compresi tra 0 e 100
        if (!ValidHumidity()) 
        {
            std::cerr << "Invalid humidity or moisture value. Humidity and moisture must be between 0 and 100." << std::endl;
            exit(EXIT_FAILURE);

        }
    }
    // Funzione che calcola la temperatura del suolo in base al tipo di suolo e alle condizioni atmosferiche e di umidità
    float Soil::calculateSoilTemperature()
    {
        switch(soiltype_)
        {
            // Valori in gradi celsius. I pesi tengono conto di come ogni tipo di terreno sia più o meno isolante e quanto sia più o meno raffreddato se bagnato.
            case SoilType::clay: 
                return 0.6 * airtemperature_ - 0.005 * soilmoisture_ + 0.001 * airhumidity_ + 2.0; 
            case SoilType::sand:
                return 0.8 * airtemperature_ - 0.002 * soilmoisture_ + 0.0005 * airhumidity_ + 3.0;
            case SoilType::loam:
                return 0.7 * airtemperature_ - 0.003 * soilmoisture_ + 0.0015 * airhumidity_ + 2.5;
            case SoilType::silt:
                return 0.65 * airtemperature_ - 0.0025 * soilmoisture_ + 0.0015 * airhumidity_ + 2.0;
            default:
                cerr << "Invalid soil type." << endl;
                exit(EXIT_FAILURE);
        }
    }

    // Funzione privata per controllare che i valori di umidità siano compresi tra 0 e 100
    bool Soil::ValidHumidity() const
    {
        if (airhumidity_ < 0 || airhumidity_ > 100 || soilmoisture_ < 0 || soilmoisture_ > 100) 
        {
            return false;
        }
        return true;
    }
    // Funzione per cambiare il tipo di terreno del suolo.
    void Soil::setSoilType(SoilType soilType)
    {
        soiltype_ = soilType;
        soiltemperature_ = calculateSoilTemperature(); // Cambiando il tipo di terreno, cambia anche la temperatura del suolo.
    }   

    // Funzione per aggiungere o rimuovere colture sul terreno
    void Soil::setPlants(bool plants)
    {
        plants_ = plants;
    }
    
    // Funzione per cambiare il livello di umidità del suolo
    void Soil::setSoilMoisture(double soilMoisture)
    {
        soilmoisture_ = soilMoisture;
        if (!ValidHumidity()) // Se il livello di umidità non è valido, si stampa un messaggio di errore e si esce dal programma
            {
                std::cerr << "Invalid humidity or moisture value. Humidity and moisture must be between 0 and 100." << std::endl;
                exit(EXIT_FAILURE);
            }
        soiltemperature_ = calculateSoilTemperature(); // Cambiando il livello di umidità, cambia anche la temperatura del suolo.
    }

    // Funzione per cambiare la temperatura dell'aria
    void Soil::setAirTemperature(float airTemperature)
    {
        airtemperature_ = airTemperature;
        soiltemperature_ = calculateSoilTemperature(); // Cambiando la temperatura dell'aria, cambia anche la temperatura del suolo.
    }

    // Funzione per cambiare l'umidità dell'aria
    void Soil::setAirHumidity(double airHumidity)
    {
        airhumidity_ = airHumidity;
        if (!ValidHumidity()) 
            {
                std::cerr << "Invalid humidity or moisture value. Humidity and moisture must be between 0 and 100." << std::endl;
                exit(EXIT_FAILURE);

            }
        soiltemperature_ = calculateSoilTemperature(); // Cambiando l'umidità dell'aria, cambia anche la temperatura del suolo.
    }

    // Funzioni Pass per passare i dati misurabili solo dai sensori al veicolo: è necessario avere un sensore apposito per misurare dati fisici.
    float Soil::PassTemperatureToSensor(SensorType sensorType) const {
        if (sensorType == SensorType::SoilTemperatureSensor) {
            return getSoilTemperature(); // I dati letti vengono stampati a video se si usa tale funzione.
        } else if (sensorType == SensorType::AirTemperatureSensor) {
            return getAirTemperature();
        } else {
            cerr << "Invalid sensor type." << endl;
            exit(EXIT_FAILURE);
        }
    }

    double Soil::PassSoilMoistureToSensor(SensorType sensorType) const {
        if (sensorType == SensorType::MoistureSensor) {
            return getSoilMoisture();
        } else {
            cerr << "Invalid sensor type." << endl;
            exit(EXIT_FAILURE);
        }
    }

    double Soil::PassAirHumidityToSensor(SensorType sensorType) const {
        if (sensorType == SensorType::HumiditySensor) {
            return getAirHumidity();
        } else {
            cerr << "Invalid sensor type." << endl;
            exit(EXIT_FAILURE);
        }
    }


    // Funzione per convertire l'enumerazione SoilType in una stringa ai fini di stampa a video.
    std::string Soil::soilTypeToString(SoilType soilType)
    {
        switch(soilType)
        {
            case SoilType::clay:
                return "Clay";
            case SoilType::sand:
                return "Sand";
            case SoilType::loam:
                return "Loam";
            case SoilType::silt:
                return "Silt";
            default:
                cerr << "Invalid soil type." << endl;
                exit(EXIT_FAILURE);
        }
    }