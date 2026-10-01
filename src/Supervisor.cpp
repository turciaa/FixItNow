#include "Supervisor.h"

Supervisor::Supervisor(int id, const string& ln, const string& fn, const string& cnp, const string& c, const string& date)
    : Employee(id, ln, fn, cnp, c, date) {}

double Supervisor::calculateSalary() const {
    const double BASE_SALARY = 4000.0;
    const double TRANSPORT_ALLOWANCE = 400.0;
    const double LOYALTY_BONUS_PERCENT = 0.05;
    const double MANAGEMENT_BONUS_PERCENT = 0.20;

    // Loyalty bonus: +5% of the base salary for every 3 years
    double salary = BASE_SALARY;
    int years_of_service = get_YearsOfService();
    int three_year_periods = years_of_service / 3;
    double loyalty_bonus = three_year_periods * LOYALTY_BONUS_PERCENT * BASE_SALARY;
    salary += loyalty_bonus;

    // Management bonus: +20% of the base salary (= 800 RON)
    double management_bonus = MANAGEMENT_BONUS_PERCENT * BASE_SALARY;
    salary += management_bonus;

    // Transport allowance for those outside Bucharest
    if (get_City() != "Bucuresti")
        salary += TRANSPORT_ALLOWANCE;

    return salary;
}

void Supervisor::display() const {
    cout << "\n=== SUPERVISOR ===" << endl;
    Employee::display();
    cout << "Current salary: " << calculateSalary() << " RON" << endl;
}
