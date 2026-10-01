#ifndef TELEVISION_H
#define TELEVISION_H

#include "Appliance.h"

class Television : public Appliance {
    private:
        double diagonal; // cm

    public:
        Television(const string&, const string&, int, double, double);
        ~Television() override = default;

        void display() const override;

        double get_Diagonal() const;
};

#endif // TELEVISION_H
