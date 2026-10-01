#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <iostream>
#include <string>
#include <stdexcept>
using namespace std;

class Employee
{
    protected:
        const int Unique_ID;
        string lastName, firstName, CNP, city;
        string Hire_Date;  // Format: "DD-MM-YYYY"

        int ageAt(const string& date) const;  // Full years between birth (from the CNP) and date

    public:
        Employee(int, const string&, const string&, const string&, const string&, const string&);
        virtual ~Employee() = default;
        virtual double calculateSalary() const = 0;
        virtual void display() const;
        int get_YearsOfService() const;

        // Setters
        void set_LastName(const string&);
        void set_FirstName(const string&);
        void set_CNP(const string&);
        void set_City(const string&);
        void set_HireDate(const string&);
        // Getters
        int get_ID() const;
        string get_LastName() const;
        string get_FirstName() const;
        string get_CNP() const;
        string get_City() const;
        string get_HireDate() const;

};

#endif // EMPLOYEE_H
