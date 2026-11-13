#ifndef THERMOSTAT_H
#define THERMOSTAT_H

#include "Device.h"

class Thermostat : public Device
{
private:
    int targetTemp_;

public:
    Thermostat(uint32_t id, const char *name, int targetTemp);
    Thermostat(const Thermostat &other);
    Thermostat &operator=(const Thermostat &other);
    ~Thermostat() override;

    // getter and setter
    bool setTargetTemp(int targetTemp);
    int getTargetTemp() const;

    double powerUsage() const override;
    void status(std::ostream &os) const override;

    int settingCount() const override;
    const char *settingName(int i) const override;
    bool applySetting(int i, int value) override;
    Thermostat *clone() const override;
};

#endif