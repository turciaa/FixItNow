#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <string>
using namespace std;

struct Date {
    int day;
    int month;
    int year;
};

// Today's date, taken from the system clock
Date today();
int currentYear();

// CNP validation
bool validateCNP(const string& cnp);

// Extracts the birth date from a valid CNP (false if the CNP is too short)
bool extractBirthDate(const string& cnp, Date& birth);

// Parses a real "DD-MM-YYYY" date
bool parseDate(const string& text, Date& date);

// Validates a "DD-MM-YYYY" hire date (real date, not in the future)
bool validateDate(const string& date);

// Number of full years between two dates (e.g. age, years of service)
int completedYears(const Date& from, const Date& to);

// Whole-string number parsing: "2020abc" or "" are rejected
bool parseInt(const string& text, int& value);
bool parseDouble(const string& text, double& value);

// Creates the folder of a file path (e.g. "reports/" for "reports/x.csv") if it is missing
void ensureParentDirectory(const string& filename);

#endif // UTILS_H
