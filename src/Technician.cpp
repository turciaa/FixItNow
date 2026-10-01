#include "../include/Technician.h"
#include <algorithm>

Technician::Technician(int id, const string& ln, const string& fn, const string& cnp, const string& c, const string& date)
    : Employee(id, ln, fn, cnp, c, date), repairsValue(0.0), totalWorkDuration(0) {}

double Technician::calculateSalary() const {
    const double BASE_SALARY = 4000.0;
    const double TRANSPORT_ALLOWANCE = 400.0;
    const double LOYALTY_BONUS_PERCENT = 0.05;
    const double REPAIRS_BONUS_PERCENT = 0.02;

    // Loyalty bonus: +5% of the base salary for every 3 years
    double salary = BASE_SALARY;
    int years_of_service = get_YearsOfService();
    int three_year_periods = years_of_service / 3;
    double loyalty_bonus = three_year_periods * LOYALTY_BONUS_PERCENT * BASE_SALARY;
    salary += loyalty_bonus;

    // Repairs bonus: 2% of the total value
    double repairs_bonus = repairsValue * REPAIRS_BONUS_PERCENT;
    salary += repairs_bonus;

    // Transport allowance for those outside Bucharest
    if (get_City() != "Bucuresti")
        salary += TRANSPORT_ALLOWANCE;

    return salary;
}

void Technician::addSkill(const string& type, const string& brand) {
    skills.push_back({type, brand});
}

bool Technician::hasSkill(const string& type, const string& brand) const {
    for (const auto& skill : skills) {
        if (skill.type == type && skill.brand == brand) {
            return true;
        }
    }
    return false;
}

bool Technician::canTakeRequest() const {
    return activeRequests.size() < 3;
}

void Technician::assignRequest(int requestId, int duration) {
    if (canTakeRequest()) {
        activeRequests.push_back(requestId);
        totalWorkDuration += duration;  // For load balancing
    }
}

void Technician::completeRequest(int requestId, double value) {
    // Remove the request from the active list
    auto it = find(activeRequests.begin(), activeRequests.end(), requestId);
    if (it != activeRequests.end()) {
        activeRequests.erase(it);
    }

    // Add the value for the bonus
    repairsValue += value;
}

int Technician::getActiveRequestCount() const {
    return activeRequests.size();
}

long long Technician::getTotalWorkDuration() const {
    return totalWorkDuration;
}

void Technician::display() const {
    cout << "\n=== TECHNICIAN ===" << endl;
    Employee::display();
    cout << "Current salary: " << calculateSalary() << " RON" << endl;
    cout << "Repairs value: " << repairsValue << " RON" << endl;

    // Display skills
    cout << "Skills: ";
    if (skills.empty()) {
        cout << "No skills" << endl;
    } else {
        cout << endl;
        for (const auto& skill : skills) {
            cout << "  - " << skill.type << " " << skill.brand << endl;
        }
    }

    // Display active requests
    cout << "Active requests: " << getActiveRequestCount() << endl;
    if (!activeRequests.empty()) {
        cout << " Request IDs: ";
        for (int i = 0; i < getActiveRequestCount(); i++) {
            cout << activeRequests[i];
            if (i < getActiveRequestCount() - 1)
                cout << ", ";
        }
        cout << endl;
    }
    cout << "Total work duration: " << totalWorkDuration << " units" << endl;
}

