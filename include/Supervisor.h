#ifndef SUPERVISOR_H
#define SUPERVISOR_H

#include "Employee.h"

class Supervisor : public Employee {
    public:
        Supervisor(int, const string&, const string&, const string&, const string&, const string&);
        ~Supervisor() override = default;

        double calculateSalary() const override;
        void display() const override;
};

#endif // SUPERVISOR_H

