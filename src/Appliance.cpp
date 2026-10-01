#include "../include/Appliance.h"
#include "../include/Utils.h"

Appliance::Appliance(const string& t, const string& b, const string& mod, int year, double price)
    : type(t), brand(b), model(mod), manufactureYear(year), listPrice(price) {}

string Appliance::get_Type() const {
    return type;
}

string Appliance::get_Brand() const {
    return brand;
}

string Appliance::get_Model() const {
    return model;
}

int Appliance::get_ManufactureYear() const {
    return manufactureYear;
}

double Appliance::get_ListPrice() const {
    return listPrice;
}

int Appliance::getAge() const {
    return currentYear() - manufactureYear;
}
