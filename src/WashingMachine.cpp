#include "../include/WashingMachine.h"

WashingMachine::WashingMachine(const string& b, const string& mod, int year, double price, int cap)
    : Appliance("WashingMachine", b, mod, year, price), capacity(cap) {}

void WashingMachine::display() const {
    cout << "\n=== WASHING MACHINE ===" << endl;
    cout << "Brand: " << get_Brand() << endl;
    cout << "Model: " << get_Model() << endl;
    cout << "Manufacture year: " << get_ManufactureYear() << endl;
    cout << "List price: " << get_ListPrice() << " RON" << endl;
    cout << "Capacity: " << capacity << " kg" << endl;
    cout << "Age: " << getAge() << " years" << endl;
}

int WashingMachine::get_Capacity() const {
    return capacity;
}
