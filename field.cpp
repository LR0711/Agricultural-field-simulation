#include <vector>
using std::vector;
#include <algorithm>
using std::fill;
using std::for_each;
#include "field.h"

Field::Field()
    :fieldname_{"???"},
    length_{1},
    width_{1},
    field_{vector<vector<Soil>>(1, vector<Soil>(1))}
    {}

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

void Field::setSoil(const Soil& soil, int startlength, int endlength, int startwidth, int endwidth)
    {
        
        if (!CheckBoundaries(startlength, endlength, startwidth, endwidth)) {
                std::cerr << "Selected range is out of boundaries." << std::endl;
                exit(EXIT_FAILURE);
        }

        for (auto row = field_.begin() + startlength; row != field_.begin() + endlength + 1; ++row) {
                std::fill((*row).begin() + startwidth, (*row).begin() + endwidth + 1, soil);

        }
    }

void Field::modifySoilProperty(int startlength, int endlength, int startwidth, int endwidth, std::function<void(Soil&)> modifyFunc)
    {
        if (!CheckBoundaries(startlength, endlength, startwidth, endwidth)) {
                std::cerr << "Selected range is out of boundaries." << std::endl;
                exit(EXIT_FAILURE);
        }

        for (auto row = field_.begin() + startlength; row != field_.begin() + endlength + 1; ++row) {
                std::for_each((*row).begin() + startwidth, (*row).begin() + endwidth + 1, modifyFunc);
        }
    }

void Field::changeFieldname(std::string fieldname)
    {
        fieldname_ = fieldname;
    }

void Field::changeDimensions(int lengthChange, int widthChange)
{
    int newLength, newWidth;
    calculateNewDimensions(lengthChange, widthChange, newLength, newWidth);
    int oldLength {length_};
    int oldWidth  {width_};
    resizeField(newLength, newWidth);
    length_ = newLength;
    width_ = newWidth;
    if (newLength > oldLength) {
        setSoil(Soil(), oldLength, newLength - 1, 0, newWidth - 1);
    }
    if (newWidth > oldWidth) {
        setSoil(Soil(), 0, newLength - 1, oldWidth, newWidth - 1);
    }
    
}

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

void Field::calculateNewDimensions(int lengthChange, int widthChange, int& newLength, int& newWidth) const
{
    newLength = length_ + lengthChange;
    newWidth = width_ + widthChange;
    if (newLength <= 0 || newWidth <= 0) {
        std::cerr << "Invalid new dimensions." << std::endl;
        exit(EXIT_FAILURE);
    }
}

void Field::resizeField(int newlength, int newwidth)
{
    field_.resize(newlength);
    for (auto& row : field_) {
        row.resize(newwidth);
    }
}

vector<vector<bool>> Field::getPlants(int startlength, int endlength, int startwidth, int endwidth) const
{
    if (!CheckBoundaries(startlength, endlength, startwidth, endwidth)) {
            std::cerr << "Selected range is out of boundaries." << std::endl;
            exit(EXIT_FAILURE);
        }

    vector<vector<bool>> plants(endlength - startlength + 1, vector<bool>(endwidth - startwidth + 1));
    auto row = field_.begin() + startlength;
    for (int i = 0; row != field_.begin() + endlength + 1; ++row, ++i) {
        transform((*row).begin() + startwidth, (*row).begin() + endwidth + 1, plants[i].begin(), [](const Soil& soil) {
            return soil.getPlants();
        });
    }
    return plants;
}

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

bool Field::getSoil(int x, int y, Soil& soil) const
{
    if (x < 0 || x >= length_ || y < 0 || y >= width_) {
        return false;
    }
    auto row = field_.begin() + x;
    auto col = (*row).begin() + y;
    soil = *col;
    return true;
    
}

void Field::printSoilTypes() const
{
    for (auto row = field_.begin(); row != field_.end(); ++row) {
        for (auto col = (*row).begin(); col != (*row).end(); ++col) {
            std::cout << Soil::soilTypeToString((*col).getSoilType()) << " ";
        }
        std::cout << std::endl;
    }
}

void Field::printPlantPresence() const
{
    for (auto row = field_.begin(); row != field_.end(); ++row) {
        for (auto col = (*row).begin(); col != (*row).end(); ++col) {
            std::cout << ((*col).getPlants() ? "P " : "x ");
        }
        std::cout << std::endl;
    }
}

std::ostream& operator<<(std::ostream& os, const Field& field)
{
    os << "Field name: " << field.getFieldname() << std::endl
       << "Field dimensions: " << field.getLength() << "x" << field.getWidth() << std::endl;
    return os;
}
