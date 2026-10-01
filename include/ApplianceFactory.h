#ifndef APPLIANCEFACTORY_H
#define APPLIANCEFACTORY_H

#include "Appliance.h"
#include "Fridge.h"
#include "WashingMachine.h"
#include "Television.h"
#include <map>
#include <memory>
#include <stdexcept>
#include <string>


using namespace std;

class ApplianceFactory {
public:
  // Factory Method for creating appliances
  static unique_ptr<Appliance>
  create(const string &type, const string &brand, const string &model,
         int manufactureYear, double listPrice,
         const map<string, string> &specificParams);

  // Checks whether the type is one the factory can create
  static bool isKnownApplianceType(const string &type);
};

#endif // APPLIANCEFACTORY_H
