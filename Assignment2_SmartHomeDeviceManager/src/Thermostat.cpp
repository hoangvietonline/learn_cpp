#include <cstring>
#include <iostream>

#include "Thermostat.h"
#include "config.h"

Thermostat::Thermostat(uint32_t id, const char *name, int targetTemp)
    : Device(id, name), targetTemp_(16) {
    setTargetTemp(targetTemp);
    TRACE("[ctor] Thermostat(" << id_ << ")");
};

Thermostat::~Thermostat()
{
    TRACE("[dtor] Thermostat(" << id_ << ")");
}

Thermostat::Thermostat(const Thermostat &other) : Device(other), targetTemp_(16)
{
    setTargetTemp(other.targetTemp_);
    TRACE("[ctor] Thermostat(" << id_ << ")");
}

Thermostat &Thermostat::operator=(const Thermostat &other)
{
    if (this == &other)
    {
        return *this;
    }

    Device::operator=(other);
    targetTemp_ = other.targetTemp_;
    return *this;
};

bool Thermostat::setTargetTemp(int targetTemp)
{
    if (targetTemp < 16 || targetTemp > 30)
    {
        return false;
    }

    targetTemp_ = targetTemp;
    return true;
};
int Thermostat::getTargetTemp() const
{
    return targetTemp_;
};

double Thermostat::powerUsage() const
{
    if (!isOn_)
    {
        return 0.0;
    }

    if (std::abs(25 - targetTemp_) > 3)
    {
        return 1500.0;
    }
    return 800.0;
};
void Thermostat::status(std::ostream &os) const
{
    os << "ID: " << id_
       << ", Name: " << name_
       << ", State: " << (isOn_ ? "ON" : "OFF")
       << ", TargetTemp: " << targetTemp_ << "";
};

int Thermostat::settingCount() const
{
    return 1;
};

const char *Thermostat::settingName(int i) const
{
    if (i == 0)
    {
        return "Thermostat";
    }

    return nullptr;
};

bool Thermostat::applySetting(int i, int value)
{
    if (i == 0)
    {
        return setTargetTemp(value);
    }

    return false;
};

Thermostat* Thermostat::clone() const{
    return new Thermostat(*this);
}