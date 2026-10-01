#include "../include/Menu.h"
#include "../include/ApplianceFactory.h"
#include <cstdlib>
#include <iostream>

using namespace std;

Menu::Menu(Service *srv) : service(srv) {}

// Input helpers
string Menu::readLine(const string &prompt) {
  cout << prompt;
  string line;
  if (!getline(cin, line)) {
    cout << "\nInput closed. Exiting." << endl;
    exit(0);
  }
  if (!line.empty() && line.back() == '\r') {
    line.pop_back();
  }
  return line;
}

int Menu::readInt(const string &prompt) {
  while (true) {
    string line = readLine(prompt);
    try {
      size_t pos;
      int value = stoi(line, &pos);
      if (line.find_first_not_of(" \t", pos) == string::npos) {
        return value;
      }
    } catch (const exception &) {
    }
    cout << "Invalid number! Try again." << endl;
  }
}

int Menu::readIntInRange(const string &prompt, int min, int max) {
  while (true) {
    int value = readInt(prompt);
    if (value >= min && value <= max) {
      return value;
    }
    cout << "Value must be between " << min << " and " << max << "!" << endl;
  }
}

double Menu::readDouble(const string &prompt) {
  while (true) {
    string line = readLine(prompt);
    try {
      size_t pos;
      double value = stod(line, &pos);
      if (line.find_first_not_of(" \t", pos) == string::npos) {
        return value;
      }
    } catch (const exception &) {
    }
    cout << "Invalid number! Try again." << endl;
  }
}

void Menu::displayMainMenu() {
  cout << "\n========================================" << endl;
  cout << "      FIXITNOW - SERVICE MANAGEMENT     " << endl;
  cout << "========================================" << endl;
  cout << "1. Employee Management" << endl;
  cout << "2. Appliance Management" << endl;
  cout << "3. Request Processing" << endl;
  cout << "4. Reports" << endl;
  cout << "5. Exit" << endl;
  cout << "========================================" << endl;
}

void Menu::run() {
  // Check whether the service can operate
  if (!service->checkMinimumEmployees()) {
    cout << "\n!!! ERROR: The service cannot operate !!!" << endl;
    cout << "Minimum required:" << endl;
    cout << "  - 3 technicians" << endl;
    cout << "  - 1 receptionist" << endl;
    cout << "  - 1 supervisor" << endl;
    cout << "\nAdd employees before using the system!" << endl;
    return;
  }

  int option;
  do {
    displayMainMenu();
    option = readInt("Choose an option: ");

    switch (option) {
    case 1:
      employeesMenu();
      break;
    case 2:
      appliancesMenu();
      break;
    case 3:
      requestsMenu();
      break;
    case 4:
      reportsMenu();
      break;
    case 5:
      cout << "\nGoodbye!" << endl;
      break;
    default:
      cout << "Invalid option! Choose 1-5." << endl;
    }
  } while (option != 5);
}

void Menu::employeesMenu() {
  int option;
  do {
    cout << "\n--- EMPLOYEE MANAGEMENT ---" << endl;
    cout << "1. Add employee" << endl;
    cout << "2. Change employee name" << endl;
    cout << "3. Remove employee (resignation)" << endl;
    cout << "4. Find and display employee by CNP" << endl;
    cout << "5. Display all employees" << endl;
    cout << "6. Back to main menu" << endl;
    option = readInt("Choose an option: ");

    switch (option) {
    case 1:
      addEmployee();
      break;
    case 2:
      changeEmployeeName();
      break;
    case 3:
      removeEmployee();
      break;
    case 4:
      findEmployeeByCNP();
      break;
    case 5:
      service->displayEmployees();
      break;
    case 6:
      break;
    default:
      cout << "Invalid option!" << endl;
    }
  } while (option != 6);
}

void Menu::addEmployee() {
  cout << "\n--- ADD EMPLOYEE ---" << endl;
  int type = readInt("Employee type (1=Receptionist, 2=Technician, 3=Supervisor): ");
  if (type < 1 || type > 3) {
    cout << "Invalid type!" << endl;
    return;
  }

  string lastName = readLine("Last name: ");
  string firstName = readLine("First name: ");
  string cnp = readLine("CNP: ");
  string city = readLine("City: ");
  string hireDate = readLine("Hire date (DD-MM-YYYY): ");

  try {
    if (type == 1) {
      service->addReceptionist(lastName, firstName, cnp, city, hireDate);
      cout << "Receptionist added successfully!" << endl;

    } else if (type == 2) {
      int skillCount = readIntInRange("Number of skills: ", 0, 20);

      vector<pair<string, string>> skills;
      for (int i = 0; i < skillCount; i++) {
        string prefix = "  Skill " + to_string(i + 1);
        string applianceType = readLine(prefix + " - Type (Fridge/Television/WashingMachine): ");
        string brand = readLine(prefix + " - Brand: ");
        skills.push_back({applianceType, brand});
      }

      service->addTechnician(lastName, firstName, cnp, city, hireDate, skills);
      cout << "Technician added successfully!" << endl;

    } else {
      service->addSupervisor(lastName, firstName, cnp, city, hireDate);
      cout << "Supervisor added successfully!" << endl;
    }
  } catch (const exception &e) {
    cout << "ERROR: " << e.what() << endl;
  }
}

void Menu::changeEmployeeName() {
  cout << "\n--- CHANGE EMPLOYEE NAME ---" << endl;
  string cnp = readLine("Employee CNP: ");

  // Check whether it exists
  Employee* emp = service->findEmployeeByCNP(cnp);
  if (emp == nullptr) {
    cout << "ERROR: There is no employee with CNP " << cnp << endl;
    return;
  }

  cout << "Employee found: " << emp->get_LastName() << " " << emp->get_FirstName() << endl;
  string newLastName = readLine("New last name: ");
  string newFirstName = readLine("New first name: ");

  if (service->changeEmployeeName(cnp, newLastName, newFirstName)) {
    cout << "Name changed successfully!" << endl;
  } else {
    cout << "ERROR: Could not change the name!" << endl;
  }
}

void Menu::removeEmployee() {
  cout << "\n--- REMOVE EMPLOYEE (RESIGNATION) ---" << endl;
  string cnp = readLine("Employee CNP: ");

  // Check whether it exists
  Employee* emp = service->findEmployeeByCNP(cnp);
  if (emp == nullptr) {
    cout << "ERROR: There is no employee with CNP " << cnp << endl;
    return;
  }

  cout << "Employee found: " << emp->get_LastName() << " " << emp->get_FirstName() << endl;
  string confirmation = readLine("Confirm removal? (yes/no): ");

  if (confirmation == "yes" || confirmation == "Yes" || confirmation == "YES") {
    if (service->removeEmployee(cnp)) {
      cout << "Employee removed successfully!" << endl;
      if (!service->checkMinimumEmployees()) {
        cout << "WARNING: The service is now below the minimum staff "
                "(3 technicians, 1 receptionist, 1 supervisor)!" << endl;
      }
    } else {
      cout << "ERROR: Could not remove the employee!" << endl;
    }
  } else {
    cout << "Removal cancelled." << endl;
  }
}

void Menu::findEmployeeByCNP() {
  cout << "\n--- FIND EMPLOYEE BY CNP ---" << endl;
  string cnp = readLine("Employee CNP: ");

  Employee* emp = service->findEmployeeByCNP(cnp);
  if (emp == nullptr) {
    cout << "ERROR: There is no employee with CNP " << cnp << endl;
  } else {
    cout << "\nEmployee found:" << endl;
    emp->display();  // display() already includes the current salary
  }
}

void Menu::appliancesMenu() {
  int option;
  do {
    cout << "\n--- APPLIANCE MANAGEMENT ---" << endl;
    cout << "1. Add repairable brand/model" << endl;
    cout << "2. Remove brand/model" << endl;
    cout << "3. Display full catalog" << endl;
    cout << "4. Display unrepairable appliances" << endl;
    cout << "5. Back to main menu" << endl;
    option = readInt("Choose an option: ");

    switch (option) {
    case 1:
      addRepairableBrandModel();
      break;
    case 2:
      removeRepairableBrandModel();
      break;
    case 3:
      service->displayCatalog();
      break;
    case 4:
      service->displayUnrepairableAppliances();
      break;
    case 5:
      break;
    default:
      cout << "Invalid option!" << endl;
    }
  } while (option != 5);
}

void Menu::addRepairableBrandModel() {
  cout << "\n--- ADD REPAIRABLE BRAND/MODEL ---" << endl;
  string type = readLine("Type (Fridge/Television/WashingMachine): ");
  if (!ApplianceFactory::isKnownApplianceType(type)) {
    cout << "ERROR: Unknown appliance type: " << type << endl;
    return;
  }
  string brand = readLine("Brand: ");
  string model = readLine("Model: ");

  service->addBrandModel(type, brand, model);
  cout << "Brand/model added to the catalog successfully!" << endl;
}

void Menu::removeRepairableBrandModel() {
  cout << "\n--- REMOVE BRAND/MODEL ---" << endl;
  string type = readLine("Type: ");
  string brand = readLine("Brand: ");
  string model = readLine("Model: ");

  if (service->removeBrandModel(type, brand, model)) {
    cout << "Brand/model removed from the catalog!" << endl;
  } else {
    cout << "ERROR: This brand/model is not in the catalog." << endl;
  }
}

void Menu::requestsMenu() {
  int option;
  do {
    cout << "\n--- REQUEST PROCESSING ---" << endl;
    cout << "1. Register new request" << endl;
    cout << "2. Real-time simulation" << endl;
    cout << "3. Display all requests" << endl;
    cout << "4. Back to main menu" << endl;
    option = readInt("Choose an option: ");

    switch (option) {
    case 1:
      registerRequest();
      break;
    case 2:
      realTimeSimulation();
      break;
    case 3:
      service->displayRequests();
      break;
    case 4:
      break;
    default:
      cout << "Invalid option!" << endl;
    }
  } while (option != 4);
}

void Menu::registerRequest() {
  cout << "\n--- REGISTER NEW REQUEST ---" << endl;
  string type = readLine("Appliance type (Fridge/Television/WashingMachine): ");
  if (!ApplianceFactory::isKnownApplianceType(type)) {
    cout << "ERROR: Unknown appliance type: " << type << endl;
    return;
  }
  string brand = readLine("Brand: ");
  string model = readLine("Model: ");
  int manufactureYear = readInt("Manufacture year: ");
  double listPrice = readDouble("List price: ");
  int complexity = readIntInRange("Complexity (1-5, 0=cannot be repaired): ", 0, 5);

  // Specific parameters
  map<string, string> params;

  if (type == "Fridge") {
    params["hasFreezer"] = readLine("Has freezer? (yes/no): ");
  } else if (type == "Television") {
    params["diagonal"] = to_string(readDouble("Diagonal (cm): "));
  } else {
    params["capacity"] = to_string(readInt("Capacity (kg): "));
  }

  try {
    // Use the Factory to create the appliance
    auto appliance = ApplianceFactory::create(type, brand, model, manufactureYear, listPrice, params);
    bool repairable = service->canBeRepaired(type, brand, model);
    const RepairRequest *request = service->addRequest(move(appliance), complexity);
    string status = request->get_Status();

    cout << "Request #" << request->get_Id() << " registered. ";
    if (!repairable) {
      cout << "Rejected: " << type << " " << brand << " " << model
           << " is not in the catalog of repairable appliances." << endl;
    } else if (status == "rejected") {
      cout << "Rejected: complexity 0 means the appliance cannot be repaired." << endl;
    } else if (status == "pending") {
      cout << "No technician is available for " << type << " " << brand
           << " right now, so it is waiting in the queue." << endl;
    } else {
      cout << "Assigned to technician ID " << request->get_TechnicianId() << "." << endl;
    }

  } catch (const exception &e) {
    cout << "ERROR: " << e.what() << endl;
  }
}

void Menu::realTimeSimulation() {
  cout << "\n--- REAL-TIME SIMULATION ---" << endl;
  int units = readIntInRange("Number of time units to simulate: ", 1, 1000);

  for (int i = 1; i <= units; i++) {
    service->simulateRealTimeTick(i);
  }

  cout << "\nSimulation completed!" << endl;
}

void Menu::reportsMenu() {
  int option;
  do {
    cout << "\n--- REPORTS ---" << endl;
    cout << "1. Top 3 employees with the highest salary" << endl;
    cout << "2. Technician with the longest repair" << endl;
    cout << "3. Pending requests (grouped)" << endl;
    cout << "4. Export all employees (CSV)" << endl;
    cout << "5. Export all requests (CSV)" << endl;
    cout << "6. Back to main menu" << endl;
    option = readInt("Choose an option: ");

    switch (option) {
    case 1:
      service->reportTop3Salaries();
      break;
    case 2:
      service->reportTechnicianMaxDuration();
      break;
    case 3:
      service->reportPendingRequests();
      break;
    case 4:
      service->exportCSV_Employees();
      break;
    case 5:
      service->exportCSV_Requests();
      break;
    case 6:
      break;
    default:
      cout << "Invalid option!" << endl;
    }
  } while (option != 6);
}
