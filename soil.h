// Questa classe è la classe "Soil", che rappresenta l'unità base per la simulazione del campo agricolo.
// È come una porzione quadrata del campo più grande e rappresenta l'idea che si possano avere diverse proprietà del suolo nello stesso campo.
// Nella prima versione di questo progetto si considera di poter avere condizioni diverse in aree diverse del campo, ma queste proprietà sono statiche e hardcoded.
// Questa classe prende come parametri di input i seguenti parametri: 
// 1) Un enumeratore SoilType che rappresenta il tipo di suolo (argilla, sabbia, limo, limo). In base al tipo di suolo ho una diversa temperatura del terreno a parità di altre condizioni.
// 2) Un booleano che rappresenta la presenza di piante, ad indicare le zone del campo di maggiore interesse per l'analisi (si procederà con un analisi dettagliata solo per le zone con le colture)
// 3) Un double che rappresenta l'umidità del suolo, che è un valore compreso tra 0 e 100.
// 4) Un float che rappresenta la temperatura dell'aria, che è un valore in gradi Celsius.
// 5) Un double che rappresenta l'umidità dell'aria, che è un valore compreso tra 0 e 100.
// In Base a questi parametri, la classe calcola la temperatura del suolo, che è un valore in gradi Celsius variabile in base al tipo di suolo (per simulare l'idea di differente conducibilità termica in base al tipo di terreno e a quanto sia bagnato).
// Sono presenti varie funzioni la cui utilità è spiegata nel file soil.cpp.

#ifndef SOIL_H
#define SOIL_H
#include <string>

#include "sensor.h"
using SensorType = Sensor::SensorType; 


class Soil {
    public:
        enum class SoilType {clay, sand, loam, silt};
        Soil();
        Soil(SoilType soilType, bool plants, double soilMoisture, float airTemperature, double airHumidity);
        void setSoilType(SoilType soilType);
        void setPlants(bool plants);
        void setSoilMoisture(double soilMoisture);
        void setAirTemperature(float airTemperature);
        void setAirHumidity(double airHumidity);
        SoilType getSoilType() const {return soiltype_;}
        bool getPlants() const {return plants_;}
        static std::string soilTypeToString(SoilType soilType);
        float PassTemperatureToSensor(SensorType sensorType) const;
        double PassSoilMoistureToSensor(SensorType sensorType) const;
        double PassAirHumidityToSensor(SensorType sensorType) const;
    

    private:
        SoilType soiltype_;
        bool plants_;
        double soilmoisture_;
        float airtemperature_;
        double airhumidity_;
        float soiltemperature_;
        float calculateSoilTemperature();
        bool ValidHumidity () const;
        float getAirTemperature() const {return airtemperature_;}
        float getSoilTemperature() const {return soiltemperature_;}
        double getAirHumidity() const {return airhumidity_;}
        double getSoilMoisture() const {return soilmoisture_;}
    };

#endif