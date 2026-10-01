#ifndef FRIDGE_H
#define FRIDGE_H

#include "Appliance.h"

class Fridge : public Appliance {
    private:
        bool freezer;

    public:
        Fridge(const string&, const string&, int, double, bool);
        ~Fridge() override = default;

        void display() const override;

        bool get_Freezer() const;
};

#endif // FRIDGE_H
