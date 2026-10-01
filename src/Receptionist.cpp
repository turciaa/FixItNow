#include "Receptionist.h"

Receptionist::Receptionist(int id, const string& ln, const string& fn, const string& cnp, const string& c, const string& date) : Employee(id, ln, fn, cnp, c, date) {};

void Receptionist::addRequest(int id) {
    registered_requests.push_back(id);
}

int Receptionist::registeredRequestCount() const {
    return registered_requests.size();
}

vector<int> Receptionist::get_RegisteredRequests() const {
    return registered_requests;
}

double Receptionist::calculateSalary() const {
    const double BASE_SALARY = 4000.0;
    const double TRANSPORT_ALLOWANCE = 400.0;
    const double LOYALTY_BONUS_PERCENT = 0.05;

    // Loyalty bonus: +5% of the base salary for every 3 years
    double salary = BASE_SALARY;
    int years_of_service = get_YearsOfService();
    int three_year_periods = years_of_service / 3;
    double loyalty_bonus = three_year_periods * LOYALTY_BONUS_PERCENT * BASE_SALARY;
    salary += loyalty_bonus;

    // Transport allowance for those outside Bucharest
    if (get_City() != "Bucuresti")
        salary += TRANSPORT_ALLOWANCE;

    return salary;
}

void Receptionist::display() const {
    cout << "\n=== RECEPTIONIST ===" << endl;
    Employee::display();
    cout << "Current salary: " << calculateSalary() << " RON" << endl;
    cout << "Registered requests: " << registeredRequestCount() << endl;
    if (!registered_requests.empty()) {
        cout << " Request IDs: ";
        for (int i = 0; i < registeredRequestCount(); i++){
            cout << registered_requests[i];
            if (i < registeredRequestCount() - 1)
                cout << ", ";
        }
        cout << endl;
    }
}