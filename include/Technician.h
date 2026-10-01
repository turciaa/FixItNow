#ifndef TECHNICIAN_H
#define TECHNICIAN_H

#include "Employee.h"
#include <vector>
#include <string>

using namespace std;

// Struct for skills (type + brand)
struct Skill {
    string type;   // e.g. "Fridge", "Television"
    string brand;  // e.g. "Samsung", "LG"
};

class Technician : public Employee {
    private:
        vector<Skill> skills;
        vector<int> activeRequests;    // IDs of the requests in progress
        double repairsValue;           // Total repairs value (for the 2% bonus)
        long long totalWorkDuration;   // For load balancing

    public:
        Technician(int, const string&, const string&, const string&, const string&, const string&);
        ~Technician() override = default;
        double calculateSalary() const override;
        void display() const override;

        // Skill management
        void addSkill(const string& type, const string& brand);
        bool hasSkill(const string& type, const string& brand) const;

        // Request management
        bool canTakeRequest() const;  // Checks whether it has < 3 active requests
        void assignRequest(int requestId, int duration);
        void completeRequest(int requestId, double value);

        // Getters for simulation
        int getActiveRequestCount() const;
        long long getTotalWorkDuration() const;
};

#endif // TECHNICIAN_H

