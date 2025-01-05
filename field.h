// La classe "Field" rappresenta il terreno di base dell'algoritmo di simulazione dell'ambiente agricolo ed è una raccolta, sotto forma di matrice, di diversi oggetti Soil.
// La classe ha un costruttore che prende come parametri di input il nome del campo, il numero di righe e colonne del campo, creando un vettore di vettori di oggetti Soil costruiti col costruttore di default.
// Ciò per simulare l'idea di "costruire da zero" il proprio campo: in questo modo all'inizio della simulazione il campo sarà riempito con lo stesso tipo di suolo e le stesse condizioni atmosferiche.
// Per mettere condizioni diverse in aree diverse del campo, la classe ha un metodo che prende come input una sotto-parte del campo e un oggetto Soil, e imposta l'oggetto Soil nella sotto-parte.
// Sono presenti diversi metodi la cui funzione è sintetizzata nel file "field.cpp".
// Allo stato attuale del progetto, il campo è statico e hardcoded, ma l'idea è di poter avere un campo con condizioni diverse in aree diverse con apposite future implementazioni. 

#ifndef FIELD_H
#define FIELD_H
#include <vector>
using std::vector;
#include <string>
using std::string;
#include "soil.h"
#include <iostream>
using std::ostream;
#include <functional>
using std::function;
#include <mutex>

class Field {
    public:
        Field();
        Field(std::string fieldname, int length, int width);
        void setSoil(const Soil& soil, int startlength, int endlength, int startwidth, int endwidth);
        void modifySoilProperty(int startlength, int endlength, int startwidth, int endwidth, std::function<void(Soil&)> modifyFunc);
        void changeFieldname(std::string fieldname);
        void changeDimensions(int lengthchange, int widthchange);
        std::string getFieldname() const {return fieldname_;}
        int getLength() const {return length_;}
        int getWidth() const {return width_;}
        bool getSoil(int x, int y, Soil& soil) const;
        vector<vector<Soil::SoilType>> getSoilTypes(int startlength, int endlength, int startwidth, int endwidth) const;
        vector<vector<bool>> getPlants(int startlength, int endlength, int startwidth, int endwidth) const;
        void printSoilTypes() const;
        void printPlantPresence() const;


    private:
        std::string fieldname_;
        int length_;
        int width_;
        std::vector<std::vector<Soil>> field_;
        bool CheckBoundaries(int startlength, int endlength, int startwidth, int endwidth) const;
        void calculateNewDimensions(int lengthChange, int widthChange, int& newLength, int& newWidth) const;
        void resizeField(int newlength, int newwidth);
        std::mutex mtx_;

};
std::ostream& operator<<(std::ostream& os, const Field& field);




#endif
