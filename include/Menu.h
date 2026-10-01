#ifndef MENU_H
#define MENU_H

#include "Service.h"
#include <string>

using namespace std;

class Menu {
private:
  Service *service;

  // Input helpers: re-prompt on invalid input, exit cleanly when input ends
  string readLine(const string &prompt);
  int readInt(const string &prompt);
  int readIntInRange(const string &prompt, int min, int max);
  double readDouble(const string &prompt);

  // Submenus
  void employeesMenu();
  void appliancesMenu();
  void requestsMenu();
  void reportsMenu();

  // Helper functions
  void addEmployee();
  void changeEmployeeName();
  void removeEmployee();
  void findEmployeeByCNP();

  void addRepairableBrandModel();
  void removeRepairableBrandModel();

  void registerRequest();
  void realTimeSimulation();

public:
  Menu(Service *srv);
  void displayMainMenu();
  void run();
};

#endif // MENU_H
