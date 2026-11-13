#ifndef LIGHT_H
#define LIGHT_H

#include "Device.h"

class Light : public Device
{
private:
    int brightness_;

public:
    Light(uint32_t id, const char *name, int brightness);
    // deep copy
    Light(const Light &other);
    Light &operator=(const Light &other);
    ~Light() override;

    // getter and setter
    bool setBrightness(int brightness);
    int getBrightness() const;

    double powerUsage() const override;
    void status(std::ostream &os) const override;

    int settingCount() const override;
    const char *settingName(int i) const override;
    bool applySetting(int i, int value) override;
    Light *clone() const override;
};

#endif