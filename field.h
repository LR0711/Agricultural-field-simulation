//This class here, the "field" class, represents the real simulated agricultural environment, and it is a collection, in the form of a matrix, of different soil objects.
// The class has a constructor that takes as input parameters the name of the field, the number of rows and columns of the field, creating a vector of vectors of soil objects.
// Simulating the idea of "building from "scratches" your field, at the start of the simulation the field will be filled with all the same type of soil, and the same conditions.
// In order to put different conditions in different areas of the field, the class has a method that takes as input a subpart of the field, and a soil object, and sets the soil object in the subpart.
// There are also methods for changing just some parameters of the soil in a subpart of the field, and for getting the type of soil in a subpart of the field.
// In a future version, there could be a method for filling the field with different random types of soil, keeping for all the elements same air temperature.
// There is also a method for changing the dimensions and the name of the field.


#ifndef FIELD_H
#define FIELD_H
#include <vector>
using std::vector;
#include <string>
using std::string;
#include "soil.h"
#include <iostream>
using std::ostream;

class Field {
    public:
        Field();
        Field(std::string fieldname, int length, int width);
        void setSoil(const Soil& soil, int startlength, int endlength, int startwidth, int endwidth);
        void setSoilType(Soil::SoilType soilType, int startlength, int endlength, int startwidth, int endwidth);
        void setPlants(bool plants, int startlength, int endlength, int startwidth, int endwidth);
        void setSoilMoisture(int soilMoisture, int startlength, int endlength, int startwidth, int endwidth);
        void setAirTemperature(float airTemperature, int startlength, int endlength, int startwidth, int endwidth);
        void setAirHumidity(int airHumidity, int startlength, int endlength, int startwidth, int endwidth);
        void changeFieldname(std::string fieldname);
        void changeDimensions(int length, int width);
        std::string getFieldname() const {return fieldname_;}
        vector<vector<Soil::SoilType>> getSoilTypes(int startlength, int endlength, int startwidth, int endwidth) const;




    private:
        std::string fieldname_;
        int length_;
        int width_;
        std::vector<std::vector<Soil>> field_;

};
std::ostream& operator<<(std::ostream& os, const Field& field);




#endif
