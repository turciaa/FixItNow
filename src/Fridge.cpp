#include "../include/Fridge.h"

Fridge::Fridge(const string& b, const string& mod, int year, double price, bool frz)
    : Appliance("Fridge", b, mod, year, price), freezer(frz) {}

void Fridge::display() const {
    cout << "\n=== FRIDGE ===" << endl;
    cout << "Brand: " << get_Brand() << endl;
    cout << "Model: " << get_Model() << endl;
    cout << "Manufacture year: " << get_ManufactureYear() << endl;
    cout << "List price: " << get_ListPrice() << " RON" << endl;
    cout << "Freezer: " << (freezer ? "YES" : "NO") << endl;
    cout << "Age: " << getAge() << " years" << endl;
}

bool Fridge::get_Freezer() const {
    return freezer;
}
