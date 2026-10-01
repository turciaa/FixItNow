#include "../include/Utils.h"
#include <chrono>
#include <ctime>
#include <sys/stat.h>
#ifdef _WIN32
#include <direct.h>
#endif

// Century from the first CNP digit: 1/2 -> 1900, 3/4 -> 1800, 5/6 -> 2000, 7/8 (residents) -> 1900
static int cnpCentury(char s)
{
    if (s == '3' || s == '4')
        return 1800;
    if (s == '5' || s == '6')
        return 2000;
    return 1900;
}

static bool isLeapYear(int year)
{
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

static int daysInMonth(int month, int year)
{
    if (month == 2)
        return isLeapYear(year) ? 29 : 28;
    if (month == 4 || month == 6 || month == 9 || month == 11)
        return 30;
    return 31;
}

static bool isAfter(const Date& a, const Date& b)
{
    if (a.year != b.year)
        return a.year > b.year;
    if (a.month != b.month)
        return a.month > b.month;
    return a.day > b.day;
}

Date today()
{
    time_t now = chrono::system_clock::to_time_t(chrono::system_clock::now());
    tm* local = localtime(&now);
    return {local->tm_mday, local->tm_mon + 1, local->tm_year + 1900};
}

int currentYear()
{
    return today().year;
}

bool validateCNP (const string & cnp)
{
    char control[] = "279146358279";
    if (cnp.length () != 13)
        return false;
    for (int i = 0; i < 13; i++)
        if (cnp[i] < '0' || cnp[i] > '9')
            return false;
    if (cnp[0] < '1' || cnp[0] > '8')
        return false;
    int YY = (cnp[1] - '0') * 10 + (cnp[2] - '0');
    int MM = (cnp[3] - '0') * 10 + (cnp[4] - '0');
    if (MM < 1 || MM > 12)
        return false;
    int DD = (cnp[5] - '0') * 10 + (cnp[6] - '0');
    //day check for months with 31/30/29/28 days
    int year = cnpCentury(cnp[0]) + YY;
    if (DD < 1 || DD > daysInMonth(MM, year))
        return false;
    // Nobody can be born in the future
    if (isAfter({DD, MM, year}, today()))
        return false;
    int sum = 0;
    for (int i = 0; i < 12; i++)
        sum += (cnp[i] - '0') * (control[i] - '0');
    int check_digit = sum % 11;
    if (check_digit == 10)
        check_digit = 1;
    if (check_digit != (cnp[12] - '0'))
        return false;
    return true;
}

bool extractBirthDate(const string& cnp, Date& birth)
{
    if (cnp.length() != 13)
        return false;

    birth.year = cnpCentury(cnp[0]) + (cnp[1] - '0') * 10 + (cnp[2] - '0');
    birth.month = (cnp[3] - '0') * 10 + (cnp[4] - '0');
    birth.day = (cnp[5] - '0') * 10 + (cnp[6] - '0');
    return true;
}

bool parseDate(const string& text, Date& date)
{
    // Format: DD-MM-YYYY
    if (text.length() != 10 || text[2] != '-' || text[5] != '-')
        return false;
    for (int i = 0; i < 10; i++)
        if (i != 2 && i != 5 && (text[i] < '0' || text[i] > '9'))
            return false;

    date.day = stoi(text.substr(0, 2));
    date.month = stoi(text.substr(3, 2));
    date.year = stoi(text.substr(6, 4));

    if (date.year < 1900 || date.month < 1 || date.month > 12)
        return false;
    return date.day >= 1 && date.day <= daysInMonth(date.month, date.year);
}

bool validateDate(const string& date)
{
    Date parsed;
    return parseDate(date, parsed) && !isAfter(parsed, today());
}

int completedYears(const Date& from, const Date& to)
{
    int years = to.year - from.year;
    // The anniversary has not been reached yet this year
    if (to.month < from.month || (to.month == from.month && to.day < from.day))
        years--;
    return years;
}

bool parseInt(const string& text, int& value)
{
    try {
        size_t pos;
        value = stoi(text, &pos);
        return text.find_first_not_of(" \t", pos) == string::npos;
    } catch (const exception&) {
        return false;
    }
}

bool parseDouble(const string& text, double& value)
{
    try {
        size_t pos;
        value = stod(text, &pos);
        return text.find_first_not_of(" \t", pos) == string::npos;
    } catch (const exception&) {
        return false;
    }
}

void ensureParentDirectory(const string& filename)
{
    size_t slash = filename.find_last_of("/\\");
    if (slash == string::npos)
        return;

    // Create each folder on the path, ignoring the ones that already exist
    for (size_t pos = filename.find_first_of("/\\"); pos != string::npos && pos <= slash;
         pos = filename.find_first_of("/\\", pos + 1)) {
        string dir = filename.substr(0, pos);
        if (dir.empty())
            continue;
#ifdef _WIN32
        _mkdir(dir.c_str());
#else
        mkdir(dir.c_str(), 0755);
#endif
    }
}
