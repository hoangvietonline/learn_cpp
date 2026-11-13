#include "config.h"
#include "Device.h"
#include <cstring>
#include <iostream>

int Device::s_count_ = 0;

Device::Device(uint32_t id, const char *name) : id_(id), name_(nullptr), isOn_(false)
{
    name_ = new char[std::strlen(name) + 1];
    std::strcpy(name_, name);
    ++s_count_;
    TRACE("[ctor] Device(" << id_ << ")");
}

Device::~Device()
{
    TRACE("[dtor] Device(" << id_ << ")");
    delete[] name_;
    --s_count_;
}

Device::Device(const Device &other) : id_(other.id_), name_(nullptr), isOn_(other.isOn_)
{
    name_ = new char[std::strlen(other.name_) + 1];
    std::strcpy(name_, other.name_);

    ++s_count_;
    TRACE("[ctor] Device(" << id_ << ")");
};

Device &Device::operator=(const Device &other)
{
    if (this == &other)
    {
        return *this;
    }

    char* newName = new char[std::strlen(other.name_) + 1];
    std::strcpy(newName, other.name_);
    delete [] name_;
    name_ = newName;

    id_ = other.id_;
    isOn_ = other.isOn_;
    return *this;
};

void Device::turnOn(){
    isOn_ = true;
}

void Device::turnOff(){
    isOn_ = false;
}

uint32_t Device::getId() const {
    return id_;
}

const char* Device::getName() const {
    return name_;
}

bool Device::isOn() const{
    return isOn_;
}

int Device::count() {
    return s_count_;
}

std::ostream& operator<<(std::ostream& os, const Device& device)
{
    device.status(os); // Gọi status() đúng loại thiết bị nhờ virtual
    return os;
}