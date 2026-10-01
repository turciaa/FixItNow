#include "../include/ApplianceFactory.h"
#include "../include/Utils.h"
#include <iostream>

using namespace std;

bool ApplianceFactory::isKnownApplianceType(const string &type) {
  return type == "Fridge" || type == "Television" || type == "WashingMachine";
}

unique_ptr<Appliance>
ApplianceFactory::create(const string &type, const string &brand, const string &model, int manufactureYear, double listPrice, const map<string, string> &specificParams) {
  if (manufactureYear < 1950 || manufactureYear > currentYear()) {
    throw invalid_argument("Manufacture year must be between 1950 and " + to_string(currentYear()));
  }
  if (listPrice <= 0) {
    throw invalid_argument("List price must be positive");
  }

  if (type == "Fridge") {
    // Specific parameter: hasFreezer (true/false)
    bool hasFreezer = false;
    if (specificParams.count("hasFreezer")) {
      string value = specificParams.at("hasFreezer");
      if (value == "true" || value == "1" || value == "yes") {
        hasFreezer = true;
      } else if (value != "false" && value != "0" && value != "no") {
        throw invalid_argument("Freezer must be yes/no (or 1/0, true/false), got: " + value);
      }
    }

    return make_unique<Fridge>(brand, model, manufactureYear, listPrice, hasFreezer);

  } else if (type == "Television") {
    // Specific parameter: diagonal (in cm or inches)
    double diagonal = 0.0;
    if (specificParams.count("diagonal") &&
        (!parseDouble(specificParams.at("diagonal"), diagonal) || diagonal <= 0)) {
      throw invalid_argument("Diagonal must be a positive number, got: " + specificParams.at("diagonal"));
    }

    return make_unique<Television>(brand, model, manufactureYear, listPrice, diagonal);

  } else if (type == "WashingMachine") {
    // Specific parameter: capacity (in kg)
    int capacity = 0;
    if (specificParams.count("capacity") &&
        (!parseInt(specificParams.at("capacity"), capacity) || capacity <= 0)) {
      throw invalid_argument("Capacity must be a positive whole number, got: " + specificParams.at("capacity"));
    }

    return make_unique<WashingMachine>(brand, model, manufactureYear, listPrice, capacity);

  } else {
    throw invalid_argument("Unknown appliance type: " + type);
  }
}
