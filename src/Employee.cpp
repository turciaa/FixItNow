#include "../include/Employee.h"
#include "../include/Utils.h"

Employee::Employee(int id, const string& ln, const string& fn, const string& cnp, const string& c, const string& date)
    : Unique_ID(id), lastName(ln), firstName(fn), CNP(cnp), city(c), Hire_Date(date)
{
    // Last name validation
    if (lastName.length() < 3 || lastName.length() > 30) {
        throw invalid_argument("Last name must be between 3 and 30 characters");
    }

    // First name validation
    if (firstName.length() < 3 || firstName.length() > 30) {
        throw invalid_argument("First name must be between 3 and 30 characters");
    }

    // CNP validation
    if (!validateCNP(cnp)) {
        throw invalid_argument("The entered CNP is not valid");
    }

    // Hire date validation
    if (!validateDate(date)) {
        throw invalid_argument("The hire date is not valid (expected a real DD-MM-YYYY date)");
    }

    // Minimum age validation (16 full years at the hire date)
    if (ageAt(date) < 16) {
        throw invalid_argument("The employee must be at least 16 years old at the hire date");
    }
}

void Employee::display() const
{
    cout << "ID: " << Unique_ID << endl;
    cout << "Last name: " << lastName << endl;
    cout << "First name: " << firstName << endl;
    cout << "CNP: " << CNP << endl;
    cout << "City: " << city << endl;
    cout << "Hire date: " << Hire_Date << endl;
    cout << "Years worked: " << get_YearsOfService() << endl;
}

void Employee::set_LastName(const string& ln) {
    if (ln.length() < 3 || ln.length() > 30) {
        throw invalid_argument("Last name must be between 3 and 30 characters");
    }
    lastName = ln;
}

void Employee::set_FirstName(const string& fn) {
    if (fn.length() < 3 || fn.length() > 30) {
        throw invalid_argument("First name must be between 3 and 30 characters");
    }
    firstName = fn;
}

void Employee::set_CNP(const string& cnp) {
    if (!validateCNP(cnp)) {
        throw invalid_argument("The entered CNP is not valid");
    }
    CNP = cnp;
}

void Employee::set_City(const string& c) {
    city = c;
}

void Employee::set_HireDate(const string& date) {
    if (!validateDate(date)) {
        throw invalid_argument("The hire date is not valid (expected a real DD-MM-YYYY date)");
    }
    if (ageAt(date) < 16) {
        throw invalid_argument("The employee must be at least 16 years old at the hire date");
    }
    Hire_Date = date;
}


int Employee::get_ID() const {
    return Unique_ID;
}

string Employee::get_LastName() const {
    return lastName;
}

string Employee::get_FirstName() const {
    return firstName;
}

string Employee::get_CNP() const {
    return CNP;
}

string Employee::get_City() const {
    return city;
}

string Employee::get_HireDate() const {
    return Hire_Date;
}


int Employee::ageAt(const string& date) const {
    Date birth, at;
    extractBirthDate(CNP, birth);
    parseDate(date, at);
    return completedYears(birth, at);
}

int Employee::get_YearsOfService() const {
    Date hire;
    parseDate(Hire_Date, hire);
    return completedYears(hire, today());
}

