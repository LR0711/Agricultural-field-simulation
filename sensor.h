// La classe "sensor" rappresenta i soli strumenti di lettura dei dati del terreno e dell'aria a disposizione dell'utente.
// L'utente infatti può settare tali dati ma non può leggerli direttamente senza l'ausilio di un sensore.
// I sensori possono essere di diversi tipi: sensori di umidità del terreno, sensori di temperatura del terreno, sensori di umidità dell'aria, sensori di temperatura dell'aria.
// Si era pensato di implementare anche un sensore della salute delle piante come da consegna ma, data la necessità di introdurre una funzione apposita per valutare lo stato di salute della pianta, non è stata messa.
// La classe ha un costruttore di default e un costruttore con parametro di input che prende come parametro un enumeratore SensorType.
// La descrizione delle funzioni è presente nel file "sensor.cpp".
#ifndef SENSOR_H
#define SENSOR_H
#include <string>

class Soil;


class Sensor {
    public:
        enum class SensorType {MoistureSensor, SoilTemperatureSensor, HumiditySensor, AirTemperatureSensor};
        Sensor();
        Sensor(SensorType type);
        float readTemperature(const Soil& soil) const;
        double readMoisture(const Soil& soil) const;
        double readHumidity(const Soil& soil) const;
        SensorType getType() const { return sensortype_; }
        static std::string sensorTypeToString(SensorType type);
        
        
    private:
        SensorType sensortype_;
        
};

#endif