#include <cstring>
#include <iostream>

#include "Light.h"
#include "config.h"

Light::Light(uint32_t id, const char *name, int brightness)
    : Device(id, name), brightness_(0)
{
    setBrightness(brightness);
    TRACE("[ctor] Light(" << id_ << ")");
};

Light::~Light()
{
    TRACE("[dtor] Light(" << id_ << ")");
}

Light::Light(const Light &other) : Device(other), brightness_(other.brightness_) {
    TRACE("[ctor] Light(" << id_ << ")");
};

Light &Light::operator=(const Light &other)
{
    if (this == &other)
    {
        return *this;
    }

    Device::operator=(other);
    brightness_ = other.brightness_;

    return *this;
};

bool Light::setBrightness(int brightness)
{
    if (brightness < 0 || brightness > 100)
    {
        return false;
    }

    brightness_ = brightness;
    return true;
}

int Light::getBrightness() const
{
    return brightness_;
}

double Light::powerUsage() const
{
    if (!isOn_)
    {
        return 0.0;
    }

    return 10.0 * brightness_ / 100;
}

void Light::status(std::ostream &os) const
{
    os << "ID: " << id_
       << ", Name: " << name_
       << ", State: " << (isOn_ ? "ON" : "OFF")
       << ", Brightness: " << brightness_ << "%";
}

int Light::settingCount() const
{
    return 1;
};

const char *Light::settingName(int i) const
{
    if (i == 0)
    {
        return "Brightness";
    }
    return nullptr;
};

bool Light::applySetting(int i, int value)
{
    if (i == 0)
    {
        return setBrightness(value);
    }
    return false;
};

Light* Light::clone() const{
    return new Light(*this);
}