
#ifndef DEVICE_H
#define DEVICE_H

#include <cstdint>
#include <iosfwd>

class Device
{
protected:
    uint32_t id_;
    char *name_;
    bool isOn_;

    Device(const Device &other);
    Device &operator=(const Device &other);

private:
    static int s_count_;

public:
    Device(uint32_t id, const char *name);
    virtual ~Device();

    void turnOn();
    void turnOff();

    uint32_t getId() const;
    const char *getName() const;
    bool isOn() const;

    static int count();

    virtual double powerUsage() const = 0;
    virtual void status(std::ostream &os) const = 0;

    virtual int settingCount() const = 0;
    virtual const char *settingName(int i) const = 0;
    virtual bool applySetting(int i, int value) = 0;

    virtual Device *clone() const = 0;
};

std::ostream &operator<<(std::ostream &os,
                         const Device &device);

#endif
