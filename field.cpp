#include <vector>
using std::vector;
#include <algorithm>
using std::fill;
using std::for_each;
#include "field.h"

// Costruttore di default
Field::Field()
    :fieldname_{"???"},
    length_{1},
    width_{1},
    field_{vector<vector<Soil>>(1, vector<Soil>(1))}
    {}
// Costruttore con parametri: crea una matrice length*witdh riempendola del tipo di suolo Soil()
Field::Field(std::string fieldname, int length, int width)
    :fieldname_{fieldname},
    length_{length},
    width_{width},
    field_{vector<vector<Soil>>(length, vector<Soil>(width))}
    {
        for (auto row = field_.begin(); row != field_.end(); ++row)
        {
            std::fill((*row).begin(), (*row).end(), Soil());
        }
    }

// Setta il tipo di suolo in un'area specifica della matrice
void Field::setSoil(const Soil& soil, int startlength, int endlength, int startwidth, int endwidth)
    {
         std::lock_guard<std::mutex> lock(mtx_); // Necessario per proteggere l'accesso alla matrice
        // Controllo se l'area selezionata è all'interno dei limiti della matrice con la funzione CheckBoundaries
        if (!CheckBoundaries(startlength, endlength, startwidth, endwidth)) {
                std::cerr << "Selected range is out of boundaries." << std::endl;
                exit(EXIT_FAILURE);
        }

        for (auto row = field_.begin() + startlength; row != field_.begin() + endlength + 1; ++row) {
                std::fill((*row).begin() + startwidth, (*row).begin() + endwidth + 1, soil);

        }
    }

// Tale funzione si usa nel momento in cui si voglia cambiare una specifica proprietà del suolo in un'area specifica del campo
void Field::modifySoilProperty(int startlength, int endlength, int startwidth, int endwidth, std::function<void(Soil&)> modifyFunc)
    {
         std::lock_guard<std::mutex> lock(mtx_);
        if (!CheckBoundaries(startlength, endlength, startwidth, endwidth)) {
                std::cerr << "Selected range is out of boundaries." << std::endl;
                exit(EXIT_FAILURE);
        }

        for (auto row = field_.begin() + startlength; row != field_.begin() + endlength + 1; ++row) {
                std::for_each((*row).begin() + startwidth, (*row).begin() + endwidth + 1, modifyFunc);
        }
    }

// Funzione che cambia il nome del campo
void Field::changeFieldname(std::string fieldname)
    {
        fieldname_ = fieldname;
    }

// Funzione che cambia le dimensioni del campo
void Field::changeDimensions(int lengthChange, int widthChange)
{
    int newLength, newWidth; 
    calculateNewDimensions(lengthChange, widthChange, newLength, newWidth); // Calcola le nuove dimensioni del campo con la funzione calculateNewDimensions
    int oldLength {length_};
    int oldWidth  {width_};
    resizeField(newLength, newWidth); // Ridimensiona il campo 
    length_ = newLength; // Aggiorna la lunghezza del campo
    width_ = newWidth;  // Aggiorna la larghezza del campo
    // Se le nuove dimensioni sono maggiori delle vecchie, allora si riempie la parte aggiunta con il tipo di suolo Soil()
    if (newLength > oldLength) {
        setSoil(Soil(), oldLength, newLength - 1, 0, newWidth - 1);
    }
    if (newWidth > oldWidth) {
        setSoil(Soil(), 0, newLength - 1, oldWidth, newWidth - 1);
    }
    
}

// Funzione privata usata dai setter del campo per controllare se l'area selezionata è all'interno dei limiti della matrice
bool Field::CheckBoundaries(int startlength, int endlength, int startwidth, int endwidth) const
    {
        std::cout << "Checking boundaries: startlength=" << startlength << ", endlength=" << endlength
              << ", startwidth=" << startwidth << ", endwidth=" << endwidth << std::endl;
        std::cout << "Field dimensions: length=" << length_ << ", width=" << width_ << std::endl;
        if (startlength < 0 || startlength >= length_ || endlength < 0 || endlength >= length_ || startwidth < 0 || startwidth >= width_ || endwidth < 0 || endwidth >= width_) {
            return false;
        }
        return true;
    }   

// Funzione privata usata per calcolare le nuove dimensioni del campo
void Field::calculateNewDimensions(int lengthChange, int widthChange, int& newLength, int& newWidth) const
{
    newLength = length_ + lengthChange; 
    newWidth = width_ + widthChange;
    // Se le nuove dimensioni sono minori o uguali a 0, allora si stampa un messaggio di errore e si esce dal programma
    if (newLength <= 0 || newWidth <= 0) {
        std::cerr << "Invalid new dimensions." << std::endl;
        exit(EXIT_FAILURE);
    }
}

// Funzione privata usata per ridimensionare il campo
void Field::resizeField(int newlength, int newwidth)
{
    std::lock_guard<std::mutex> lock(mtx_);
    field_.resize(newlength); 
    for (auto& row : field_) {
        row.resize(newwidth);
    }
}

// Funzione che restituisce la presenza di piante in un'area specifica del campo
vector<vector<bool>> Field::getPlants(int startlength, int endlength, int startwidth, int endwidth) const
{
    if (!CheckBoundaries(startlength, endlength, startwidth, endwidth)) {
            std::cerr << "Selected range is out of boundaries." << std::endl;
            exit(EXIT_FAILURE);
        }

    vector<vector<bool>> plants(endlength - startlength + 1, vector<bool>(endwidth - startwidth + 1)); // 
    auto row = field_.begin() + startlength;
    for (int i = 0; row != field_.begin() + endlength + 1; ++row, ++i) {
        transform((*row).begin() + startwidth, (*row).begin() + endwidth + 1, plants[i].begin(), [](const Soil& soil) {
            return soil.getPlants();
        });
    }
    return plants;
}

// Funzione che restituisce il tipo di suolo in un'area specifica del campo
vector<vector<Soil::SoilType>> Field::getSoilTypes(int startlength, int endlength, int startwidth, int endwidth) const
{
    if (!CheckBoundaries(startlength, endlength, startwidth, endwidth)) {
            std::cerr << "Selected range is out of boundaries." << std::endl;
            exit(EXIT_FAILURE);
        }

    vector<vector<Soil::SoilType>> soilTypes(endlength - startlength + 1, vector<Soil::SoilType>(endwidth - startwidth + 1));
    auto row = field_.begin() + startlength;
    for (int i = 0; row != field_.begin() + endlength + 1; ++row, ++i) {
        transform((*row).begin() + startwidth, (*row).begin() + endwidth + 1, soilTypes[i].begin(), [](const Soil& soil) {
            return soil.getSoilType();
        });
    }
    return soilTypes;
}

// funzione per la restituzione del tipo di suolo in una specifica posizione del campo
bool Field::getSoil(int x, int y, Soil& soil) const
{
    //controllo se la posizione è all'interno dei limiti della matrice
    if (x < 0 || x >= length_ || y < 0 || y >= width_) {
        return false;
    }
    auto row = field_.begin() + x;
    auto col = (*row).begin() + y;
    soil = *col; // Assegna il tipo di suolo della posizione alla variabile soil
    return true;
    
}

// Funzione che stampa i tipi di suolo presenti nel campo
void Field::printSoilTypes() const
{
    for (auto row = field_.begin(); row != field_.end(); ++row) {
        for (auto col = (*row).begin(); col != (*row).end(); ++col) {
            std::cout << Soil::soilTypeToString((*col).getSoilType()) << " ";
        }
        std::cout << std::endl;
    }
}

// Funzione che stampa la presenza di piante nel campo
void Field::printPlantPresence() const
{
    for (auto row = field_.begin(); row != field_.end(); ++row) {
        for (auto col = (*row).begin(); col != (*row).end(); ++col) {
            std::cout << ((*col).getPlants() ? "P " : "x ");
        }
        std::cout << std::endl;
    }
}

// Overloading dell'operatore di output per la stampa dei dati di base del campo
std::ostream& operator<<(std::ostream& os, const Field& field)
{
    os << "Field name: " << field.getFieldname() << std::endl
       << "Field dimensions: " << field.getLength() << "x" << field.getWidth() << std::endl;
    return os;
}
