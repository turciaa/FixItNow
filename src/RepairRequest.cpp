#include "../include/RepairRequest.h"
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

int RepairRequest::idCounter = 1;

RepairRequest::RepairRequest(unique_ptr<Appliance>&& app, int complex)
    : appliance(move(app)), complexity(complex), technicianId(-1), receptionistId(-1)
{
    id = idCounter++;

    // Automatic timestamp generation using chrono
    // Timestamps have a one-second resolution, so requests registered in the same second
    // (e.g. read from a file) get the following seconds: no two requests share a timestamp
    static time_t lastTime = 0;
    time_t now_time = chrono::system_clock::to_time_t(chrono::system_clock::now());
    if (now_time <= lastTime) {
        now_time = lastTime + 1;
    }
    lastTime = now_time;
    tm* local_tm = localtime(&now_time);

    // Format: "YYYY-MM-DD HH:MM:SS"
    ostringstream oss;
    oss << put_time(local_tm, "%Y-%m-%d %H:%M:%S");
    timestamp = oss.str();

    if (complexity < 0 || complexity > 5) {
        complexity = 0;
    }

    if (complexity == 0) {
        estimatedDuration = 0;
        status = "rejected";
    } else {
        // An appliance from the current year has age 0, but its repair still takes time
        int age = max(1, appliance->getAge());
        estimatedDuration = age * complexity;
        status = "pending";
    }

    repairPrice = appliance->get_ListPrice() * estimatedDuration;
    remainingDuration = estimatedDuration;
}

int RepairRequest::get_Id() const {
    return id;
}

const Appliance* RepairRequest::get_Appliance() const {
    return appliance.get();
}

string RepairRequest::get_Timestamp() const {
    return timestamp;
}

int RepairRequest::get_Complexity() const {
    return complexity;
}

int RepairRequest::get_EstimatedDuration() const {
    return estimatedDuration;
}

int RepairRequest::get_RemainingDuration() const {
    return remainingDuration;
}

double RepairRequest::get_RepairPrice() const {
    return repairPrice;
}

string RepairRequest::get_Status() const {
    return status;
}

int RepairRequest::get_TechnicianId() const {
    return technicianId;
}

int RepairRequest::get_ReceptionistId() const {
    return receptionistId;
}

void RepairRequest::set_Status(const string& s) {
    status = s;
}

void RepairRequest::set_ReceptionistId(int recId) {
    receptionistId = recId;
}

void RepairRequest::assignTechnician(int techId) {
    technicianId = techId;
    status = "assigned";
}

void RepairRequest::unassign() {
    technicianId = -1;
    status = "pending";
}

void RepairRequest::reduceDuration(int units) {
    remainingDuration -= units;
    if (remainingDuration <= 0) {
        remainingDuration = 0;
        status = "completed";
    } else {
        status = "in_progress";
    }
}

bool RepairRequest::isCompleted() const {
    return remainingDuration == 0;
}

void RepairRequest::display() const {
    cout << "\n=== REPAIR REQUEST #" << id << " ===" << endl;
    cout << "Timestamp: " << timestamp << endl;
    appliance->display();
    cout << "Complexity: " << complexity << endl;
    cout << "Estimated duration: " << estimatedDuration << " units" << endl;
    cout << "Remaining duration: " << remainingDuration << " units" << endl;
    cout << "Repair price: " << repairPrice << " RON" << endl;
    cout << "Status: " << status << endl;
    if (receptionistId != -1) {
        cout << "Registered by receptionist: ID " << receptionistId << endl;
    }
    if (technicianId != -1) {
        cout << "Assigned technician: ID " << technicianId << endl;
    }
}
