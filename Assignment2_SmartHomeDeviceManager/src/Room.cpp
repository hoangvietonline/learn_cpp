#include <iostream>
#include "Room.h"
#include "Device.h"
#include "config.h"

Room::Room(const char *name) : name_(nullptr), devices_(nullptr), count_(0), capacity_(2)
{
    name_ = new char[std::strlen(name) + 1];
    std::strcpy(name_, name);
    devices_ = new Device *[capacity_] {};
    TRACE("[ctor] Room(" << name_ << ")");
};

Room::Room(const Room &other)
    : name_(nullptr), devices_(nullptr), count_(other.count_), capacity_(other.capacity_)
{
    name_ = new char[std::strlen(other.name_) + 1];
    std::strcpy(name_, other.name_);
    devices_ = new Device *[capacity_] {};

    for (int i = 0; i < other.count_; ++i)
    {
        devices_[i] = other.devices_[i]->clone();
    }
    TRACE("[ctor] Room(" << name_ << ")");
};

Room &Room::operator=(const Room &other)
{
    if (this != &other)
    {
        char *newName = new char[std::strlen(other.name_) + 1];
        std::strcpy(newName, other.name_);
        delete[] name_;
        name_ = newName;

        Device **newDevices = new Device *[other.capacity_] {};
        for (int i = 0; i < other.count_; ++i)
        {
            newDevices[i] = other.devices_[i]->clone();
        }

        for (int i = 0; i < count_; ++i)
        {
            delete devices_[i];
        }

        delete[] devices_;
        devices_ = newDevices;

        capacity_ = other.capacity_;
        count_ = other.count_;
    }

    return *this;
};

Room::~Room()
{
    TRACE("[dtor] Room(" << name_ << ")");
    for (int i = 0; i < count_; ++i)
    {
        delete devices_[i];
    }

    delete[] name_;
    delete[] devices_;
};

// char *name_;
// Device **devices_;
// int count_;
// int capacity_;

// device management
void Room::addDevice(const Device &device)
{
    if (count_ == capacity_)
    {
        if (capacity_ > std::numeric_limits<int>::max() / 2)
            return;
        int newCapacity = capacity_ == 0 ? 2 : capacity_ * 2;

        Device **newDevices = new Device *[newCapacity];

        for (int i = 0; i < count_; ++i)
        {
            newDevices[i] = devices_[i];
        }

        delete[] devices_;
        devices_ = newDevices;
        capacity_ = newCapacity;
    }

    devices_[count_] = device.clone();
    ++count_;
};
bool Room::removeDevice(uint32_t id)
{
    int position = -1;

    for (int i = 0; i < count_; ++i)
    {
        if (devices_[i]->getId() == id)
        {
            position = i;
            break;
        }
    }

    if (position < 0)
    {
        return false;
    }

    delete devices_[position];

    for (int i = position; i < count_ - 1; i++)
    {
        devices_[i] = devices_[i + 1];
    }
    --count_;
    return true;
};

Device *Room::find(uint32_t id)
{
    for (int i = 0; i < count_; ++i)
    {
        if (devices_[i]->getId() == id)
        {
            return devices_[i];
        }
    }

    return nullptr;
};
const Device *Room::find(uint32_t id) const
{
    for (int i = 0; i < count_; ++i)
    {
        if (devices_[i]->getId() == id)
        {
            return devices_[i];
        }
    }

    return nullptr;
};

// Power
double Room::totalPower() const
{
    double total = 0.0;

    for (int i = 0; i < count_; ++i)
    {
        total += devices_[i]->powerUsage();
    }
    return total;
};

const char *Room::getName() const
{
    return name_;
};
int Room::getCount() const
{
    return count_;
};
int Room::getCapacity() const
{
    return capacity_;
};

// Operator overloading
Room &Room::operator+=(const Device &device)
{
    addDevice(device);
    return *this;
};

void Room::status(std::ostream &os)
{
    os << "Name: " << name_
       << ", Count: " << (count_)
       << ", total Power: " << totalPower() << "W\n";
}

void Room::listDevices(std::ostream& os) const{
    for (int i = 0;i < count_;++i){
        devices_[i]->status(os);
    }
}