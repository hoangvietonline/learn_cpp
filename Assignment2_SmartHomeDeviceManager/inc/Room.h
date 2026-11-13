#ifndef ROOM_H
#define ROOM_H

#include "Device.h"

class Room
{
private:
    char *name_;
    Device **devices_;
    int count_;
    int capacity_;

public:
    explicit Room(const char *name);
    Room(const Room &other);
    Room &operator=(const Room &other);
    ~Room();

    // device management
    void addDevice(const Device &device);
    bool removeDevice(uint32_t id);

    Device *find(uint32_t id);
    const Device *find(uint32_t id) const;

    // Power
    double totalPower() const;

    // Getters
    const char *getName() const;
    int getCount() const;
    int getCapacity() const;

    // Operator overloading
    Room &operator+=(const Device &device);

    void status(std::ostream &os);
    void listDevices(std::ostream&) const;
};

#endif