#ifndef RECEPTIONIST_H
#define RECEPTIONIST_H

#include "Employee.h"
#include <vector>

class Receptionist : public Employee
{
    vector <int> registered_requests;

    public:
        Receptionist(int, const string&, const string&, const string&, const string&, const string&);
        ~Receptionist() override = default;

        double calculateSalary() const override;
        void display() const override;

        vector<int> get_RegisteredRequests() const;
        void addRequest(int);
        int registeredRequestCount() const;
};

#endif // RECEPTIONIST_H
