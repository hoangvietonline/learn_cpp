#include "config.h"
#include "SecurityCam.h"
#include <iostream>

SecurityCam::SecurityCam(uint32_t id, const char *name, int resolution, bool isRecording)
    : Device(id, name), resolution_(720), isRecording_(false)
{
    setResolution(resolution);
    setIsRecording(isRecording);
    TRACE("[ctor] SecurityCam(" << id_ << ")");
}

SecurityCam::SecurityCam(const SecurityCam &other)
    : Device(other), resolution_(other.resolution_), isRecording_(other.isRecording_)
{
    TRACE("[ctor] SecurityCam(" << id_ << ")");
}

SecurityCam &SecurityCam::operator=(const SecurityCam &other)
{
    if (this == &other)
    {
        return *this;
    }
    Device::operator=(other);
    resolution_ = other.resolution_;
    isRecording_ = other.isRecording_;
    return *this;
}

SecurityCam::~SecurityCam()
{
    TRACE("[dtor] SecurityCam(" << id_ << ")");
}

constexpr bool isValidResolution(int value) noexcept
{
    switch (value)
    {
    case static_cast<int>(Resolution::Resolution_720):
    case static_cast<int>(Resolution::Resolution_1080):
    case static_cast<int>(Resolution::Resolution_2160):
        return true;
    default:
        return false;
    }
}

bool SecurityCam::setResolution(int resolution)
{
    if (!isValidResolution(resolution))
    {
        return false;
    }
    resolution_ = resolution;
    return true;
}

int SecurityCam::getResolution() const
{
    return resolution_;
}

bool SecurityCam::setIsRecording(bool isRecording)
{
    isRecording_ = isRecording;
    return true;
}

bool SecurityCam::getIsRecording() const
{
    return isRecording_;
}

double SecurityCam::powerUsage() const
{
    if (!isOn_)
    {
        return 0.0;
    }
    return isRecording_ ? 8.0 : 5.0;
}

void SecurityCam::status(std::ostream &os) const
{
    os << "ID: " << id_
       << ", Name: " << name_
       << ", State: " << (isOn_ ? "ON" : "OFF")
       << ", Resolution: " << resolution_
       << ", Recording: " << (isRecording_ ? "YES" : "NO");
}

int SecurityCam::settingCount() const
{
    return 2;
}

const char *SecurityCam::settingName(int i) const
{
    if (i == 0)
    {
        return "Resolution";
    }
    if (i == 1)
    {
        return "Recording (0: OFF, 1: ON)";
    }
    return nullptr;
}

bool SecurityCam::applySetting(int i, int value)
{
    if (i == 0)
    {
        return setResolution(value);
    }
    if (i == 1 && (value == 0 || value == 1))
    {
        return setIsRecording(value == 1);
    }
    return false;
}

SecurityCam *SecurityCam::clone() const
{
    return new SecurityCam(*this);
}
