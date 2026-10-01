#ifndef SERVICE_H
#define SERVICE_H

#include "Employee.h"
#include "Receptionist.h"
#include "Technician.h"
#include "Supervisor.h"
#include "RepairRequest.h"
#include <vector>
#include <memory>
#include <string>
#include <queue>
#include <map>
#include <set>
#include <tuple>

using namespace std;

class Service {
    private:

        vector<unique_ptr<Employee>> employees;
        int nextEmployeeId = 1;  // IDs are never reused, even after an employee leaves
        vector<unique_ptr<RepairRequest>> requests;
        queue<RepairRequest*> pendingRequests;  // Queue for unassigned requests

        // Catalog of repairable appliances: type -> set of (brand, model) pairs
        map<string, map<string, set<string>>> repairableCatalog;  // type -> brand -> set<models>

        // Appliances that cannot be repaired (from invalid requests): (type, brand, model) -> number of occurrences
        map<tuple<string, string, string>, int> unrepairableAppliances;

        // Private constructor for Singleton
        Service();

        // Disable copy
        Service(const Service&) = delete;
        Service& operator=(const Service&) = delete;

        // Internal helpers
        void ensureUniqueCNP(const string& cnp) const;  // Throws if the CNP is already used
        string technicianName(int technicianId) const;
        void finishRequest(RepairRequest* request);  // Frees the technician's slot and adds the bonus value
        void assignReceptionist(RepairRequest* request);  // Credits the least-loaded receptionist

    public:
        static Service* getInstance();
        ~Service();

        // Employee management
        void addReceptionist(const string& lastName, const string& firstName, const string& cnp,
                             const string& city, const string& hireDate);
        void addTechnician(const string& lastName, const string& firstName, const string& cnp,
                           const string& city, const string& hireDate,
                           const vector<pair<string, string>>& skills);
        void addSupervisor(const string& lastName, const string& firstName, const string& cnp,
                           const string& city, const string& hireDate);

        // Employee operations
        bool changeEmployeeName(const string& cnp, const string& newLastName, const string& newFirstName);
        bool removeEmployee(const string& cnp);
        Employee* findEmployeeByCNP(const string& cnp) const;

        // Appliance catalog management
        void addBrandModel(const string& type, const string& brand, const string& model);
        bool removeBrandModel(const string& type, const string& brand, const string& model);
        bool canBeRepaired(const string& type, const string& brand, const string& model) const;
        void displayCatalog() const;
        void displayUnrepairableAppliances() const;

        // Request management
        const RepairRequest* addRequest(unique_ptr<Appliance>&& appliance, int complexity);  // Returns the created request
        void autoAssignTechnician(RepairRequest* request);
        vector<RepairRequest*> processPendingRequests();  // Tries to assign requests from the queue, returns the assigned ones

        // Simulation
        void simulateRealTimeTick(int currentTick);  // Full simulation with status messages

        // Reporting
        void displayEmployees() const;
        void displayRequests() const;

        // Reading from files
        void readEmployeesFromCSV(const string& fileName);
        void readRequestsFromCSV(const string& fileName);
        void exportCSV_Employees(const string& filename = "reports/employees.csv") const;
        void exportCSV_Requests(const string& filename = "reports/requests.csv") const;

        // Special reports (requirement)
        void reportTop3Salaries(const string& filename = "reports/top_3_salaries.csv") const;
        void reportTechnicianMaxDuration(const string& filename = "reports/technician_max_duration.csv") const;
        void reportPendingRequests(const string& filename = "reports/pending_requests.csv") const;

        // Getters
        int getEmployeeCount() const;
        int getRequestCount() const;
        int getPendingRequestCount() const;
        bool checkMinimumEmployees() const;  // Checks for at least 3 technicians, 1 receptionist, 1 supervisor
};

#endif // SERVICE_H
