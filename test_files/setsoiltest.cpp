//The following code is a snippet from soiltest.cpp, which tests the soil class.
// In order to test the soil class, we need to create a soil object, set its properties and get them.
// We test also methods for setting the properties of the soil, and for getting the available soil properties.
// The test will not cover the method for calculating the soil temperature, because it's a private method, and it's called by the set methods.
// This thing means that the test will not check soil and air temperatures and humidity, that will be checked later on with the implementation of sensor.
// We remind that this properties cannot be read directly by the user.

#include "soil.h"
#include <iostream>


void testSoilProperties() {
    Soil soil1;
    std::cout << "Initial Soil 1 type: " << Soil::soilTypeToString(soil1.getSoilType()) << std::endl;
    std::cout << "Initial Soil 1 has plants: " << (soil1.getPlants() ? "true" : "false") << std::endl;

    // Set properties for the soil object built with the default constructor
    soil1.setSoilType(Soil::SoilType::sand);
    soil1.setPlants(true);
    soil1.setSoilMoisture(99);
    soil1.setAirTemperature(22.2);
    soil1.setAirHumidity(73);

    // Verify properties have been set correctly
    std::cout << "Updated Soil 1 type: " << Soil::soilTypeToString(soil1.getSoilType()) << std::endl;
    std::cout << "Updated Soil 1 has plants: " << (soil1.getPlants() ? "true" : "false") << std::endl;

    // Create another Soil object with specific properties
    Soil soil2(Soil::SoilType::clay, true, 70, 25.0, 60);
    std::cout << "Soil 2 type: " << Soil::soilTypeToString(soil2.getSoilType()) << std::endl;
    std::cout << "Soil 2 has plants: " << (soil2.getPlants() ? "true" : "false") << std::endl;
}

int main() {
    testSoilProperties();
    // Test wrong values
    // Soil soilwrong(Soil::SoilType::clay, true, 70, 25.0, 101);
    // Soil soilwrong2(Soil::SoilType::clay, true, 70, 25.0, -1);
    // Soil soilwrong3(Soil::SoilType::clay, true, 101, 25.0, 60);
    Soil soilwrong4(Soil::SoilType::clay, true, -101, 25.0, 60);
    return 0;
}
