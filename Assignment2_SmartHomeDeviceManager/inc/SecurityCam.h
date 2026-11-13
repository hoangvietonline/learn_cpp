#ifndef SECURITYCAM_H
#define SECURITYCAM_H

#include "Device.h"
#include <cstdint>

enum class Resolution : std::uint16_t
{
    Resolution_720 = 720,
    Resolution_1080 = 1080,
    Resolution_2160 = 2160
};

class SecurityCam : public Device
{
private:
    int resolution_;
    bool isRecording_;

public:
    SecurityCam(uint32_t id, const char *name, int resolution, bool isRecording);
    SecurityCam(const SecurityCam &other);
    SecurityCam &operator=(const SecurityCam &other);
    ~SecurityCam() override;

    // getter and setter
    bool setResolution(int resolution);
    int getResolution() const;

    bool setIsRecording(bool isRecording);
    bool getIsRecording() const;

    double powerUsage() const override;
    void status(std::ostream &os) const override;

    int settingCount() const override;
    const char *settingName(int i) const override;
    bool applySetting(int i, int value) override;
    SecurityCam *clone() const override;
};

#endif