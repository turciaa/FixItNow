#include "./include/ApplianceFactory.h"
#include "./include/Menu.h"
#include "./include/Service.h"
#include <iostream>

using namespace std;

int main() {
  cout << "=== FIXITNOW - SERVICE MANAGEMENT SYSTEM ===" << endl;

  Service *service = Service::getInstance();

  // Initialize the catalog of repairable appliances
  cout << "\nInitializing catalog..." << endl;
  service->addBrandModel("Fridge", "Samsung", "Frost200");
  service->addBrandModel("Fridge", "Samsung", "CoolMax");
  service->addBrandModel("Fridge", "LG", "SmartFridge");
  service->addBrandModel("Fridge", "Arctic", "FreshPro");

  service->addBrandModel("Television", "LG", "OLED55");
  service->addBrandModel("Television", "LG", "NanoCell");
  service->addBrandModel("Television", "Samsung", "QLED65");
  service->addBrandModel("Television", "Sony", "Bravia");

  service->addBrandModel("WashingMachine", "Arctic", "WashPro");
  service->addBrandModel("WashingMachine", "Whirlpool", "CleanMax");
  service->addBrandModel("WashingMachine", "Bosch", "Serie6");


  // ========================================
  // READING DATA FROM TEST FILES
  // ========================================
  // To switch the test files, change the paths below:
  // - employees_valid.csv    -> correct data
  // - employees_invalid.csv  -> validation tests
  // - requests_valid.csv     -> requests for known appliances
  // - requests_invalid.csv   -> requests for unknown appliances

  cout << "\n========================================" << endl;
  cout << "  READING EMPLOYEES FROM CSV FILE" << endl;
  cout << "========================================" << endl;

  // CHANGE HERE the file for different tests:
  service->readEmployeesFromCSV("tests/employees_valid.csv");
  //service->readEmployeesFromCSV("tests/employees_invalid.csv");  // Validation tests

  cout << "\n========================================" << endl;
  cout << "  READING REQUESTS FROM CSV FILE" << endl;
  cout << "========================================" << endl;

  // CHANGE HERE the file for different tests:
  service->readRequestsFromCSV("tests/requests_valid.csv");
  //service->readRequestsFromCSV("tests/requests_invalid.csv");  // Validation tests

  // Check whether the service can operate
  if (!service->checkMinimumEmployees()) {
    cout << "\n!!! WARNING: The service does not have enough employees to "
            "operate !!!"
         << endl;
    cout << "Minimum required: 3 technicians, 1 receptionist, 1 supervisor"
         << endl;
    return 1;
  }

  // Start the interactive menu
  Menu menu(service);
  menu.run();

  return 0;
}
