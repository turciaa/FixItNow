#ifndef WASHINGMACHINE_H
#define WASHINGMACHINE_H

#include "Appliance.h"

class WashingMachine : public Appliance {
    private:
        int capacity; // kg

    public:
        WashingMachine(const string&, const string&, int, double, int);
        ~WashingMachine() override = default;

        void display() const override;

        int get_Capacity() const;
};

#endif // WASHINGMACHINE_H
