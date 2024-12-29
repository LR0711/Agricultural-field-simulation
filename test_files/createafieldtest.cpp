//This test file has the purpose of testing the field class creating a field object and checking if the object is created correctly.

#include "soil.h"
#include "field.h"
#include "sensor.h"
#include <iostream>
using std::cout;
using std::endl;

void testFieldCreation()
    {
        Field field1;
        Soil soil1(Soil::SoilType::clay, true, 50, 25.2, 70);
        Soil soil2(Soil::SoilType::sand, false, 30, 30.1, 60);
        Field field2("FattoriaRoss", 10, 10);
        cout << field1 << endl;
        field1.printSoilTypes();
        cout << field2 << endl;
        field2.setSoil(soil1, 0, 3, 1, 4);
        field2.setSoil(soil2, 5, 8, 5, 9);
        cout << field2 << endl;
        field2.printSoilTypes();
        field2.printPlantPresence();
        field2.modifySoilProperty(0, 3, 1, 4, [](Soil& soil) {soil.setPlants(true);});
        field1.changeFieldname("FattoriaVerde");
        cout << field1 << endl;
        cout << field2 << endl;
        field2.printPlantPresence();
        field2.modifySoilProperty(5, 8, 5, 9, [](Soil& soil) {soil.setPlants(true);});
        field2.printPlantPresence();
        field2.changeDimensions(5, 5);
        cout << field2 << endl;
        field2.printSoilTypes();
        Field field3("FattoriaGialla", 15, 15);
        cout << field3 << endl;
        field3.printSoilTypes();
        field3.changeDimensions(-5, -4);
        cout << field3 << endl;
        field3.printSoilTypes();
        field3.modifySoilProperty(0, 3, 1, 4, [](Soil& soil) {soil.setPlants(true);});
        field3.printPlantPresence();
        //test for error checking: remove comment to see the error message
        //field3.changeDimensions(0, -20);
        //field3.modifySoilProperty(0, 3, 1, 4, [](Soil& soil) {soil.setAirHumidity(101);});
        field3.setSoil(soil1, 0, -3, 1, 4);

    }

int main()
    {
        testFieldCreation();
        return 0;
    }
        
