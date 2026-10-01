#ifndef REPAIRREQUEST_H
#define REPAIRREQUEST_H

#include "Appliance.h"
#include <memory>
#include <string>
#include <chrono>
#include <ctime>

using namespace std;

class RepairRequest {
    private:
        int id;
        static int idCounter;
        unique_ptr<Appliance> appliance;
        string timestamp;
        int complexity;
        int estimatedDuration;
        int remainingDuration;
        double repairPrice;
        string status;
        int technicianId;
        int receptionistId;

    public:
        RepairRequest(unique_ptr<Appliance>&&, int);  // No timestamp - generated automatically

        // Getters
        int get_Id() const;
        const Appliance* get_Appliance() const;
        string get_Timestamp() const;
        int get_Complexity() const;
        int get_EstimatedDuration() const;
        int get_RemainingDuration() const;
        double get_RepairPrice() const;
        string get_Status() const;
        int get_TechnicianId() const;
        int get_ReceptionistId() const;

        // Setters for management
        void set_Status(const string&);
        void set_ReceptionistId(int);
        void assignTechnician(int);
        void unassign();  // Back to pending, e.g. when the technician leaves
        void reduceDuration(int units = 1);
        bool isCompleted() const;

        void display() const;
};

#endif // REPAIRREQUEST_H
