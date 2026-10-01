#ifndef APPLIANCE_H
#define APPLIANCE_H

#include <iostream>
#include <string>
using namespace std;

class Appliance {
    protected:
        string type;
        string brand;
        string model;
        int manufactureYear;
        double listPrice;

    public:
        Appliance(const string&, const string&, const string&, int, double);
        virtual ~Appliance() = default;

        virtual void display() const = 0;

        // Getters
        string get_Type() const;
        string get_Brand() const;
        string get_Model() const;
        int get_ManufactureYear() const;
        double get_ListPrice() const;

        int getAge() const;
};

#endif // APPLIANCE_H
