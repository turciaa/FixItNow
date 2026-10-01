#include "../include/Service.h"
#include "../include/ApplianceFactory.h"
#include "../include/Utils.h"
#include <algorithm>
#include <cmath>
#include <climits>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

Service::Service() {}

Service::~Service() {}

Service *Service::getInstance() {
  // Created on first use and destroyed automatically at program exit
  static Service instance;
  return &instance;
}

void Service::ensureUniqueCNP(const string &cnp) const {
  if (findEmployeeByCNP(cnp) != nullptr) {
    throw invalid_argument("An employee with this CNP already exists");
  }
}

// Employee management
// The ID is only consumed after the constructor's validation succeeds
void Service::addReceptionist(const string &lastName, const string &firstName, const string &cnp, const string &city, const string &hireDate) {
  ensureUniqueCNP(cnp);
  employees.push_back(make_unique<Receptionist>(nextEmployeeId, lastName, firstName, cnp, city, hireDate));
  nextEmployeeId++;
}

void Service::addTechnician(const string &lastName, const string &firstName, const string &cnp, const string &city, const string &hireDate, const vector<pair<string, string>> &skills) {
  ensureUniqueCNP(cnp);
  for (const auto &skill : skills) {
    if (!ApplianceFactory::isKnownApplianceType(skill.first)) {
      throw invalid_argument("Unknown appliance type in skill: " + skill.first);
    }
  }

  auto technician = make_unique<Technician>(nextEmployeeId, lastName, firstName, cnp, city, hireDate);

  // Add skills
  for (const auto &skill : skills) {
    technician->addSkill(skill.first, skill.second);
  }

  employees.push_back(std::move(technician));
  nextEmployeeId++;
}

void Service::addSupervisor(const string &lastName, const string &firstName, const string &cnp, const string &city, const string &hireDate) {
  ensureUniqueCNP(cnp);
  employees.push_back(make_unique<Supervisor>(nextEmployeeId, lastName, firstName, cnp, city, hireDate));
  nextEmployeeId++;
}

// Employee operations
bool Service::changeEmployeeName(const string &cnp, const string &newLastName, const string &newFirstName) {
  for (auto &emp : employees) {
    if (emp->get_CNP() == cnp) {
      string oldLastName = emp->get_LastName();
      try {
        emp->set_LastName(newLastName);
        try {
          emp->set_FirstName(newFirstName);
        } catch (const exception &) {
          emp->set_LastName(oldLastName);  // Change both names or neither
          throw;
        }
        return true;
      } catch (const exception &e) {
        cerr << "Error while changing: " << e.what() << endl;
        return false;
      }
    }
  }
  return false; // CNP not found
}

bool Service::removeEmployee(const string &cnp) {
  for (auto it = employees.begin(); it != employees.end(); ++it) {
    if ((*it)->get_CNP() == cnp) {
      int employeeId = (*it)->get_ID();
      bool isTechnician = dynamic_cast<Technician *>(it->get()) != nullptr;
      employees.erase(it);

      // A leaving technician's unfinished requests go back to the waiting queue
      if (isTechnician) {
        vector<RepairRequest *> waiting;
        while (!pendingRequests.empty()) {
          waiting.push_back(pendingRequests.front());
          pendingRequests.pop();
        }
        for (auto &request : requests) {
          string status = request->get_Status();
          if (request->get_TechnicianId() == employeeId &&
              (status == "assigned" || status == "in_progress")) {
            request->unassign();
            waiting.push_back(request.get());
          }
        }

        // Keep the queue in timestamp order: these requests are older than some of the waiting ones
        sort(waiting.begin(), waiting.end(), [](const RepairRequest *a, const RepairRequest *b) {
          return a->get_Id() < b->get_Id();
        });
        for (RepairRequest *request : waiting) {
          pendingRequests.push(request);
        }
      }
      return true;
    }
  }
  return false; // CNP not found
}

Employee* Service::findEmployeeByCNP(const string &cnp) const {
  for (const auto &emp : employees) {
    if (emp->get_CNP() == cnp) {
      return emp.get();
    }
  }
  return nullptr; // CNP not found
}

// Request management
const RepairRequest *Service::addRequest(unique_ptr<Appliance> &&appliance,
                                         int complexity) {
  string type = appliance->get_Type();
  string brand = appliance->get_Brand();
  string model = appliance->get_Model();

  // Check whether it can be repaired
  if (!canBeRepaired(type, brand, model)) {
    // Add to the list of unrepairable appliances
    auto key = make_tuple(type, brand, model);
    unrepairableAppliances[key]++;

    // Create a request with complexity 0 (rejected)
    auto request = make_unique<RepairRequest>(std::move(appliance), 0);
    assignReceptionist(request.get());
    requests.push_back(std::move(request));
    return requests.back().get();
  }

  // Valid request - create it with the given complexity
  auto request =
      make_unique<RepairRequest>(std::move(appliance), complexity);
  assignReceptionist(request.get());

  // Try automatic assignment if complexity > 0 (the request may have clamped an invalid value to 0)
  if (request->get_Complexity() > 0) {
    autoAssignTechnician(request.get());

    // If it was not assigned, put it in the waiting queue
    if (request->get_Status() == "pending") {
      pendingRequests.push(request.get());
    }
  }

  requests.push_back(std::move(request));
  return requests.back().get();
}

void Service::assignReceptionist(RepairRequest *request) {
  Receptionist *chosen = nullptr;
  for (auto &emp : employees) {
    Receptionist *rec = dynamic_cast<Receptionist *>(emp.get());
    if (rec != nullptr &&
        (chosen == nullptr || rec->registeredRequestCount() < chosen->registeredRequestCount())) {
      chosen = rec;
    }
  }

  if (chosen != nullptr) {
    chosen->addRequest(request->get_Id());
    request->set_ReceptionistId(chosen->get_ID());
  }
}

void Service::autoAssignTechnician(RepairRequest *request) {
  const Appliance *app = request->get_Appliance();
  string type = app->get_Type();
  string brand = app->get_Brand();

  Technician *chosenTechnician = nullptr;
  long long minDuration = LLONG_MAX;

  // Look for a technician with the matching skill and minimum load
  for (auto &emp : employees) {
    Technician *tech = dynamic_cast<Technician *>(emp.get());
    if (tech != nullptr) {
      // Check whether they have the skill and can take the request
      if (tech->hasSkill(type, brand) && tech->canTakeRequest()) {
        // Load balancing: pick the technician with the minimum total duration
        if (tech->getTotalWorkDuration() < minDuration) {
          minDuration = tech->getTotalWorkDuration();
          chosenTechnician = tech;
        }
      }
    }
  }

  // Assign the request if a technician was found
  if (chosenTechnician != nullptr) {
    chosenTechnician->assignRequest(request->get_Id(), request->get_EstimatedDuration());
    request->assignTechnician(chosenTechnician->get_ID());
  }
}

// Simulation helpers
string Service::technicianName(int technicianId) const {
  for (const auto &emp : employees) {
    if (emp->get_ID() == technicianId) {
      return emp->get_LastName() + " " + emp->get_FirstName();
    }
  }
  return "Unknown";
}

void Service::finishRequest(RepairRequest *request) {
  for (auto &emp : employees) {
    if (emp->get_ID() == request->get_TechnicianId()) {
      Technician *tech = dynamic_cast<Technician *>(emp.get());
      if (tech != nullptr) {
        tech->completeRequest(request->get_Id(), request->get_RepairPrice());
      }
      return;
    }
  }
}

// Reporting
void Service::displayEmployees() const {
  cout << "\n=== EMPLOYEE LIST ===" << endl;
  for (const auto &emp : employees) {
    emp->display();
  }
}

void Service::displayRequests() const {
  cout << "\n=== REQUEST LIST ===" << endl;
  for (const auto &request : requests) {
    request->display();
  }
}

void Service::exportCSV_Employees(const string &filename) const {
  ensureParentDirectory(filename);
  ofstream file(filename);
  if (!file.is_open()) {
    cerr << "Error opening file: " << filename << " (does the folder exist?)" << endl;
    return;
  }

  file << "ID,LastName,FirstName,CNP,City,HireDate,Salary\n";
  for (const auto &emp : employees) {
    file << emp->get_ID() << "," << emp->get_LastName() << "," << emp->get_FirstName() << "," << emp->get_CNP() << "," << emp->get_City() << "," << emp->get_HireDate() << "," << emp->calculateSalary() << "\n";
  }

  file.close();
  cout << "Employees export: " << filename << " - SUCCESS" << endl;
}

void Service::exportCSV_Requests(const string &filename) const {
  ensureParentDirectory(filename);
  ofstream file(filename);
  if (!file.is_open()) {
    cerr << "Error opening file: " << filename << " (does the folder exist?)" << endl;
    return;
  }

  file << "ID,Timestamp,Type,Brand,Model,Complexity,EstimatedDuration,RemainingDuration,Price,Status,TechnicianId,ReceptionistId\n";
  for (const auto &request : requests) {
    const Appliance *app = request->get_Appliance();
    file << request->get_Id() << "," << request->get_Timestamp() << "," << app->get_Type() << "," << app->get_Brand() << "," << app->get_Model() << "," << request->get_Complexity() << "," << request->get_EstimatedDuration() << "," << request->get_RemainingDuration() << "," << request->get_RepairPrice() << "," << request->get_Status() << "," << request->get_TechnicianId() << "," << request->get_ReceptionistId() << "\n";
  }

  file.close();
  cout << "Requests export: " << filename << " - SUCCESS" << endl;
}

int Service::getEmployeeCount() const { return employees.size(); }

int Service::getRequestCount() const { return requests.size(); }

int Service::getPendingRequestCount() const { return pendingRequests.size(); }

// Appliance catalog management
void Service::addBrandModel(const string &type, const string &brand, const string &model) {
  repairableCatalog[type][brand].insert(model);
}

bool Service::removeBrandModel(const string &type, const string &brand, const string &model) {
  if (!canBeRepaired(type, brand, model))
    return false;

  repairableCatalog[type][brand].erase(model);

  // If there are no more models for this brand, remove the brand
  if (repairableCatalog[type][brand].empty()) {
    repairableCatalog[type].erase(brand);
  }

  // If there are no more brands for this type, remove the type
  if (repairableCatalog[type].empty()) {
    repairableCatalog.erase(type);
  }
  return true;
}

bool Service::canBeRepaired(const string &type, const string &brand, const string &model) const {
  auto itType = repairableCatalog.find(type);
  if (itType == repairableCatalog.end())
    return false;

  auto itBrand = itType->second.find(brand);
  if (itBrand == itType->second.end())
    return false;

  return itBrand->second.count(model) > 0;
}

void Service::displayCatalog() const {
  cout << "\n=== REPAIRABLE APPLIANCES CATALOG ===" << endl;
  if (repairableCatalog.empty()) {
    cout << "The catalog is empty!" << endl;
    return;
  }

  for (const auto &[type, brands] : repairableCatalog) {
    cout << "\n" << type << ":" << endl;
    for (const auto &[brand, models] : brands) {
      cout << "  " << brand << ": ";
      bool first = true;
      for (const auto &model : models) {
        if (!first)
          cout << ", ";
        cout << model;
        first = false;
      }
      cout << endl;
    }
  }
}

void Service::displayUnrepairableAppliances() const {
  cout << "\n=== APPLIANCES THAT CANNOT BE REPAIRED ===" << endl;
  if (unrepairableAppliances.empty()) {
    cout << "There are no registered unrepairable appliances." << endl;
    return;
  }

  // Sort in descending order by number of occurrences
  vector<pair<tuple<string, string, string>, int>> sorted(unrepairableAppliances.begin(), unrepairableAppliances.end());

  sort(sorted.begin(), sorted.end(), [](const auto &a, const auto &b) { return a.second > b.second; });

  for (const auto &[app, count] : sorted) {
    cout << get<0>(app) << " " << get<1>(app) << " " << get<2>(app) << " - " << count << " occurrences" << endl;
  }
}

// Processing pending requests
vector<RepairRequest *> Service::processPendingRequests() {
  vector<RepairRequest *> assigned;
  int initialSize = pendingRequests.size();

  for (int i = 0; i < initialSize; i++) {
    RepairRequest *request = pendingRequests.front();
    pendingRequests.pop();

    // Try to assign
    autoAssignTechnician(request);

    // If it still was not assigned, put it back in the queue
    if (request->get_Status() == "pending") {
      pendingRequests.push(request);
    } else {
      assigned.push_back(request);
    }
  }
  return assigned;
}

// Real-time simulation with messages
void Service::simulateRealTimeTick(int currentTick) {
  cout << "\n[Time " << currentTick << "]";

  bool firstLine = true;

  // 1. Process active requests and display status
  for (auto &request : requests) {
    string status = request->get_Status();

    if (status == "assigned") {
      // First time the work starts
      if (!firstLine)
        cout << "         ";
      else {
        cout << " ";
        firstLine = false;
      }

      string techName = technicianName(request->get_TechnicianId());

      cout << "Technician " << techName << " receives the request with id "
           << request->get_Id() << endl;

      // Reduce the duration
      request->reduceDuration(1);

      // Requests with a duration of 0 or 1 are already done after the first tick
      if (request->isCompleted()) {
        cout << "         Technician " << techName << " completes the request "
             << request->get_Id() << endl;
        finishRequest(request.get());
      }

    } else if (status == "in_progress") {
      // Reduce the duration
      request->reduceDuration(1);

      if (!firstLine)
        cout << "         ";
      else {
        cout << " ";
        firstLine = false;
      }

      string techName = technicianName(request->get_TechnicianId());

      if (request->isCompleted()) {
        cout << "Technician " << techName << " completes the request "
             << request->get_Id() << endl;

        // Update the technician
        finishRequest(request.get());
      } else {
        cout << "Technician " << techName << " is processing the request with id "
             << request->get_Id() << " (" << request->get_RemainingDuration()
             << " time units remaining)" << endl;
      }
    }
  }

  // 2. Try to assign pending requests, announcing them in this tick
  for (RepairRequest *request : processPendingRequests()) {
    if (!firstLine)
      cout << "         ";
    else {
      cout << " ";
      firstLine = false;
    }

    cout << "Technician " << technicianName(request->get_TechnicianId())
         << " receives the request with id " << request->get_Id() << endl;

    // Already announced: the work starts on the next tick
    request->set_Status("in_progress");
  }

  // 3. Display pending requests
  if (!pendingRequests.empty()) {
    if (!firstLine)
      cout << "         ";
    else {
      cout << " ";
      firstLine = false;
    }

    cout << "Pending requests: ";
    queue<RepairRequest *> temp = pendingRequests;
    bool first = true;
    while (!temp.empty()) {
      if (!first)
        cout << ", ";
      cout << temp.front()->get_Id();
      temp.pop();
      first = false;
    }
    cout << "." << endl;
  }

  if (firstLine) {
    cout << " (No activity)" << endl;
  }
}

// Minimum employees check
bool Service::checkMinimumEmployees() const {
  int numTechnicians = 0, numReceptionists = 0, numSupervisors = 0;

  for (const auto &emp : employees) {
    if (dynamic_cast<Technician *>(emp.get()) != nullptr)
      numTechnicians++;
    else if (dynamic_cast<Receptionist *>(emp.get()) != nullptr)
      numReceptionists++;
    else if (dynamic_cast<Supervisor *>(emp.get()) != nullptr)
      numSupervisors++;
  }

  return (numTechnicians >= 3 && numReceptionists >= 1 && numSupervisors >= 1);
}

// Special reports
void Service::reportTop3Salaries(const string &filename) const {
  // Build a vector of employees and salaries
  vector<pair<double, const Employee *>> salaries;
  for (const auto &emp : employees) {
    salaries.push_back({emp->calculateSalary(), emp.get()});
  }

  // Sort: descending by salary, then alphabetically by last name + first name
  sort(salaries.begin(), salaries.end(), [](const auto &a, const auto &b) {
    if (abs(a.first - b.first) > 0.01) { // Significant difference
      return a.first > b.first;          // Descending by salary
    }
    // On a tie: alphabetically by name
    string nameA = a.second->get_LastName() + " " + a.second->get_FirstName();
    string nameB = b.second->get_LastName() + " " + b.second->get_FirstName();
    return nameA < nameB;
  });

  // CSV export
  ensureParentDirectory(filename);
  ofstream file(filename);
  if (!file.is_open()) {
    cerr << "Error opening file: " << filename << " (does the folder exist?)" << endl;
    return;
  }

  file << "Position,LastName,FirstName,Salary\n";
  int count = min(3, (int)salaries.size());
  for (int i = 0; i < count; i++) {
    file << (i + 1) << "," << salaries[i].second->get_LastName() << ","
         << salaries[i].second->get_FirstName() << "," << salaries[i].first << "\n";
  }

  file.close();
  cout << "Top 3 salaries report generated: " << filename << endl;
}

void Service::reportTechnicianMaxDuration(const string &filename) const {
  const Technician *maxTechnician = nullptr;
  int maxDuration = 0;

  // Find the request with the maximum duration
  // Only requests that have a technician count, so the duration always belongs to the reported technician
  for (const auto &request : requests) {
    int techId = request->get_TechnicianId();
    if (techId == -1 || request->get_EstimatedDuration() <= maxDuration)
      continue;

    for (const auto &emp : employees) {
      if (emp->get_ID() == techId) {
        maxTechnician = dynamic_cast<const Technician *>(emp.get());
        maxDuration = request->get_EstimatedDuration();
        break;
      }
    }
  }

  // CSV export
  ensureParentDirectory(filename);
  ofstream file(filename);
  if (!file.is_open()) {
    cerr << "Error opening file: " << filename << " (does the folder exist?)" << endl;
    return;
  }

  file << "ID,LastName,FirstName,CNP,City,HireDate,Salary,MaxRepairDuration\n";
  if (maxTechnician != nullptr) {
    file << maxTechnician->get_ID() << "," << maxTechnician->get_LastName() << "," << maxTechnician->get_FirstName() << "," << maxTechnician->get_CNP() << "," << maxTechnician->get_City() << "," << maxTechnician->get_HireDate() << "," << maxTechnician->calculateSalary() << "," << maxDuration << "\n";
  }

  file.close();
  cout << "Technician max duration report generated: " << filename << endl;
}

void Service::reportPendingRequests(const string &filename) const {
  // Group by type -> brand -> model
  map<string, map<string, map<string, int>>> grouped;

  queue<RepairRequest *> temp = pendingRequests;
  while (!temp.empty()) {
    RepairRequest *request = temp.front();
    temp.pop();

    const Appliance *app = request->get_Appliance();
    grouped[app->get_Type()][app->get_Brand()][app->get_Model()]++;
  }

  // CSV export
  ensureParentDirectory(filename);
  ofstream file(filename);
  if (!file.is_open()) {
    cerr << "Error opening file: " << filename << " (does the folder exist?)" << endl;
    return;
  }

  file << "Type,Brand,Model,RequestCount\n";
  for (const auto &[type, brands] : grouped) {
    for (const auto &[brand, models] : brands) {
      for (const auto &[model, count] : models) {
        file << type << "," << brand << "," << model << "," << count << "\n";
      }
    }
  }

  file.close();
  cout << "Pending requests report generated: " << filename << endl;
}

// Reading employees from CSV
void Service::readEmployeesFromCSV(const string &fileName) {
  ifstream file(fileName);
  if (!file.is_open()) {
    cerr << "ERROR: Could not open file: " << fileName << endl;
    return;
  }

  string line;
  int lineNumber = 0;
  int employeesAdded = 0;
  int errors = 0;

  // Skip header
  getline(file, line);
  lineNumber++;

  while (getline(file, line)) {
    lineNumber++;

    // Skip empty lines
    if (line.empty() || line == "\r") continue;

    stringstream ss(line);
    string type, lastName, firstName, cnp, city, hireDate, skillsStr;

    try {
      // Parse CSV: Type,LastName,FirstName,CNP,City,HireDate,Skills
      getline(ss, type, ',');
      getline(ss, lastName, ',');
      getline(ss, firstName, ',');
      getline(ss, cnp, ',');
      getline(ss, city, ',');
      getline(ss, hireDate, ',');
      getline(ss, skillsStr);

      // Remove \r if present
      if (!skillsStr.empty() && skillsStr.back() == '\r') {
        skillsStr.pop_back();
      }

      if (type == "Receptionist") {
        addReceptionist(lastName, firstName, cnp, city, hireDate);
        employeesAdded++;

      } else if (type == "Technician") {
        // Parse skills: "Fridge-Samsung;Television-LG"
        vector<pair<string, string>> skills;
        if (!skillsStr.empty()) {
          stringstream skillSS(skillsStr);
          string skill;
          while (getline(skillSS, skill, ';')) {
            size_t pos = skill.find('-');
            if (pos != string::npos) {
              string applianceType = skill.substr(0, pos);
              string brand = skill.substr(pos + 1);
              skills.push_back({applianceType, brand});
            }
          }
        }
        addTechnician(lastName, firstName, cnp, city, hireDate, skills);
        employeesAdded++;

      } else if (type == "Supervisor") {
        addSupervisor(lastName, firstName, cnp, city, hireDate);
        employeesAdded++;

      } else {
        cerr << "Read error: Invalid employee type on line " << lineNumber
             << ", type: " << type << endl;
        errors++;
      }

    } catch (const exception &e) {
      cerr << "Read error: Invalid employee on line " << lineNumber
           << ", cause: " << e.what() << endl;
      errors++;
    }
  }

  file.close();
  cout << "\n=== EMPLOYEES READ RESULT ===" << endl;
  cout << "File: " << fileName << endl;
  cout << "Employees added successfully: " << employeesAdded << endl;
  cout << "Errors detected: " << errors << endl;
}

// Reading requests from CSV
void Service::readRequestsFromCSV(const string &fileName) {
  ifstream file(fileName);
  if (!file.is_open()) {
    cerr << "ERROR: Could not open file: " << fileName << endl;
    return;
  }

  string line;
  int lineNumber = 0;
  int requestsAdded = 0;
  int errors = 0;

  // Skip header
  getline(file, line);
  lineNumber++;

  while (getline(file, line)) {
    lineNumber++;

    // Skip empty lines
    if (line.empty() || line == "\r") continue;

    stringstream ss(line);
    string type, brand, model, manufactureYearStr, listPriceStr, specificAttribute,
        complexityStr;

    try {
      // Parse CSV: type,brand,model,manufacture_year,list_price,[specific_attribute],complexity
      getline(ss, type, ',');
      getline(ss, brand, ',');
      getline(ss, model, ',');
      getline(ss, manufactureYearStr, ',');
      getline(ss, listPriceStr, ',');
      getline(ss, specificAttribute, ',');
      getline(ss, complexityStr);

      // Remove \r if present
      if (!complexityStr.empty() && complexityStr.back() == '\r') {
        complexityStr.pop_back();
      }

      int manufactureYear, complexity;
      double listPrice;
      if (!parseInt(manufactureYearStr, manufactureYear)) {
        throw invalid_argument("Manufacture year is not a number: " + manufactureYearStr);
      }
      if (!parseDouble(listPriceStr, listPrice)) {
        throw invalid_argument("List price is not a number: " + listPriceStr);
      }
      if (!parseInt(complexityStr, complexity)) {
        throw invalid_argument("Complexity is not a number: " + complexityStr);
      }
      if (complexity < 0 || complexity > 5) {
        throw invalid_argument("Complexity must be between 0 and 5");
      }

      // Specific parameters
      map<string, string> params;
      if (type == "Fridge") {
        params["hasFreezer"] = specificAttribute;
      } else if (type == "Television") {
        params["diagonal"] = specificAttribute;
      } else if (type == "WashingMachine") {
        params["capacity"] = specificAttribute;
      }

      // Create the appliance and add the request
      auto appliance = ApplianceFactory::create(
          type, brand, model, manufactureYear, listPrice, params);
      addRequest(move(appliance), complexity);
      requestsAdded++;

    } catch (const exception &e) {
      cerr << "Read error: Invalid request on line " << lineNumber
           << ", cause: " << e.what() << endl;
      errors++;
    }
  }

  file.close();
  cout << "\n=== REQUESTS READ RESULT ===" << endl;
  cout << "File: " << fileName << endl;
  cout << "Requests added successfully: " << requestsAdded << endl;
  cout << "Errors detected: " << errors << endl;
}
