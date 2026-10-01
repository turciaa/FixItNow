#include "../include/Television.h"

Television::Television(const string& b, const string& mod, int year, double price, double diag)
    : Appliance("Television", b, mod, year, price), diagonal(diag) {}

void Television::display() const {
    cout << "\n=== TELEVISION ===" << endl;
    cout << "Brand: " << get_Brand() << endl;
    cout << "Model: " << get_Model() << endl;
    cout << "Manufacture year: " << get_ManufactureYear() << endl;
    cout << "List price: " << get_ListPrice() << " RON" << endl;
    cout << "Diagonal: " << diagonal << " cm" << endl;
    cout << "Age: " << getAge() << " years" << endl;
}

double Television::get_Diagonal() const {
    return diagonal;
}
