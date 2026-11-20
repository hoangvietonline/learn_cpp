#include <cmath>
#include <cstring>
#include <exception>
#include <iostream>
#include <ostream>
#include <streambuf>
#include <type_traits>

#include "Device.h"
#include "Light.h"
#include "Thermostat.h"
#include "SecurityCam.h"
#include "Room.h"

static_assert(std::is_abstract<Device>::value, "Device must be abstract");
static_assert(std::has_virtual_destructor<Device>::value,
              "Device must have a virtual destructor");

namespace
{
    // Khong dung assert: cac kiem tra van chay khi build voi -DNDEBUG.
    void check(bool condition, const char *message)
    {
        if (!condition)
        {
            throw message;
        }
    }

    bool equalPower(double actual, double expected)
    {
        return std::fabs(actual - expected) < 0.000001;
    }

    // Thu output vao mang char de kiem tra status(), khong dung std::string.
    class TextBuffer : public std::streambuf
    {
        char text_[4096]{};

    public:
        TextBuffer()
        {
            setp(text_, text_ + sizeof(text_) - 1);
        }

        const char *text()
        {
            *pptr() = '\0';
            return text_;
        }
    };

    // Sau khi ham nay ket thuc, Room chi giu cac clone, khong phu thuoc bien cuc bo.
    void populate(Room &room, bool turnOn = false)
    {
        Light light(1, "Living light", 80);
        Thermostat thermostat(2, "Living thermostat", 22);
        SecurityCam camera(3, "Living camera", 1080, false);
        if (turnOn)
        {
            light.turnOn();
            thermostat.turnOn();
            camera.turnOn();
        }
        room += light;
        room += thermostat;
        room += camera;
    }

    Device &requireDevice(Room &room, uint32_t id)
    {
        Device *device = room.find(id);
        check(device != nullptr, "Expected device was not found");
        return *device;
    }

    void test01()
    {
        Room room("Living");
        populate(room);
        check(room.getCount() == 3, "Room must contain three devices");
        check(Device::count() == 3, "Only the three clones should remain alive");
        TextBuffer buffer;
        std::ostream output(&buffer);
        room.listDevices(output);
        check(output.good(), "Status output failed");
        check(std::strstr(buffer.text(), "Brightness: 80%") != nullptr,
              "Light status is missing");
        check(std::strstr(buffer.text(), "TargetTemp: 22") != nullptr,
              "Thermostat status is missing");
        check(std::strstr(buffer.text(), "Resolution: 1080") != nullptr,
              "Camera status is missing");
        check(equalPower(room.totalPower(), 0), "New devices should be OFF");
        const Room &view = room;
        check(view.find(1) != nullptr && view.find(999) == nullptr,
              "Const find returned an incorrect result");

        TextBuffer operatorBuffer;
        std::ostream operatorOutput(&operatorBuffer);
        operatorOutput << requireDevice(room, 1) << '\n';
        check(std::strstr(operatorBuffer.text(), "Brightness: 80%") != nullptr,
              "operator<< must call virtual status()");
    }

    void test02()
    {
        Room room("Living");
        populate(room);
        for (uint32_t id = 1; id <= 3; ++id)
        {
            requireDevice(room, id).turnOn();
        }
        check(equalPower(requireDevice(room, 1).powerUsage(), 8), "Light must use 8 W");
        check(equalPower(requireDevice(room, 2).powerUsage(), 800),
              "Thermostat at 22 C must use 800 W (difference equals 3)");
        check(equalPower(requireDevice(room, 3).powerUsage(), 5), "Camera must use 5 W");
        check(equalPower(room.totalPower(), 813), "Total must be 813 W");
    }

    void test03()
    {
        Room room("Living");
        populate(room, true);
        Device &camera = requireDevice(room, 3);
        check(camera.applySetting(1, 1), "Recording setting was rejected");
        check(equalPower(room.totalPower(), 816), "Recording total must be 816 W");
        camera.turnOff();
        check(equalPower(camera.powerUsage(), 0), "OFF camera must use 0 W even when recording");
        check(camera.applySetting(1, 0), "Stopping recording failed");
        camera.turnOn();
        check(equalPower(room.totalPower(), 813), "Total must return to 813 W");
    }

    void test04()
    {
        Light light(1, "Light", 80);
        Thermostat thermostat(2, "Thermostat", 22);
        SecurityCam camera(3, "Camera", 1080, false);
        check(!light.setBrightness(120) && light.getBrightness() == 80,
              "Invalid brightness must leave the old value unchanged");
        check(!thermostat.setTargetTemp(35) && thermostat.getTargetTemp() == 22,
              "Invalid temperature must leave the old value unchanged");
        check(!camera.setResolution(999) && camera.getResolution() == 1080,
              "Invalid resolution must leave the old value unchanged");
        check(!light.setBrightness(-1), "Negative brightness must be rejected");
        check(!thermostat.setTargetTemp(15), "Temperature below 16 must be rejected");
        check(light.setBrightness(0) && light.setBrightness(100), "Brightness bounds must be accepted");
        check(thermostat.setTargetTemp(16) && thermostat.setTargetTemp(30),
              "Temperature bounds must be accepted");
        check(camera.setResolution(720) && camera.setResolution(2160),
              "Supported resolutions must be accepted");

        light.turnOn();
        check(light.setBrightness(25) && equalPower(light.powerUsage(), 2.5),
              "Light power must preserve fractional watts");
        thermostat.turnOn();
        check(thermostat.setTargetTemp(28) && equalPower(thermostat.powerUsage(), 800),
              "Temperature difference of 3 must use 800 W");
        check(thermostat.setTargetTemp(29) && equalPower(thermostat.powerUsage(), 1500),
              "Temperature difference greater than 3 must use 1500 W");

        check(light.settingCount() == 1 && thermostat.settingCount() == 1 && camera.settingCount() == 2,
              "Incorrect number of settings");
        check(light.settingName(0) && thermostat.settingName(0) && camera.settingName(1),
              "Valid settings must have names");
        check(!light.applySetting(0, 120) && light.getBrightness() == 25,
              "Virtual settings must validate brightness");
        check(!thermostat.applySetting(0, 35) && thermostat.getTargetTemp() == 29,
              "Virtual settings must validate temperature");
        check(!camera.applySetting(0, 999) && camera.getResolution() == 2160,
              "Virtual settings must validate resolution");
        check(!camera.applySetting(1, 2) && !camera.getIsRecording(),
              "Recording accepts only 0 or 1");
        check(!light.applySetting(-1, 0) && !thermostat.applySetting(1, 22) && !camera.applySetting(2, 0), "Unknown settings must be rejected");
    }

    void test05()
    {
        Room original("Living");
        populate(original, true);
        Room copied(original);
        check(copied.getName() != original.getName() && std::strcmp(copied.getName(), original.getName()) == 0,
              "Room names must be deep copied");
        for (uint32_t id = 1; id <= 3; ++id)
        {
            Device &a = requireDevice(original, id);
            Device &b = requireDevice(copied, id);
            check(&a != &b && a.getName() != b.getName(), "Devices and names must be independent");
            check(std::strcmp(a.getName(), b.getName()) == 0, "Copied name differs from source");
        }
        check(requireDevice(copied, 1).applySetting(0, 20), "Changing copied light failed");
        check(equalPower(requireDevice(original, 1).powerUsage(), 8) && equalPower(requireDevice(copied, 1).powerUsage(), 2),
              "Original light must stay at 80%, copy must become 20%");
        check(equalPower(copied.totalPower(), 807), "Copied room must use 807 W");

        // Kiem tra ca phep gan: phong dich da co thiet bi cu.
        Room assigned("Old room");
        Light oldLight(99, "Old light", 10);
        assigned += oldLight;
        assigned = original;
        check(assigned.getCount() == 3 && assigned.find(99) == nullptr,
              "Assignment must replace old devices");
        check(requireDevice(assigned, 1).applySetting(0, 20), "Changing assigned light failed");
        check(equalPower(original.totalPower(), 813) && equalPower(assigned.totalPower(), 807),
              "Assigned room must be independent");
        Room empty("Empty");
        assigned = empty;
        check(assigned.getCount() == 0 && equalPower(assigned.totalPower(), 0),
              "Assignment from empty room must release old devices");
    }

    void test06()
    {
        Room original("Living");
        populate(original, true);
        {
            Room copied(original);
            check(requireDevice(copied, 1).applySetting(0, 20), "Changing copy failed");
        }
        check(Device::count() == 3, "Destroying the copy must release its three devices");
        check(equalPower(original.totalPower(), 813), "Original must survive destruction of copy");
        check(requireDevice(original, 1).applySetting(0, 50), "Original must remain usable");
        check(equalPower(original.totalPower(), 810), "Original must update normally");
    }

    void test07()
    {
        Room room("Living");
        populate(room, true);
        Device *before = room.find(1);
        Room &alias = room;
        room = alias;
        check(room.getCount() == 3 && room.find(1) == before, "Self-assignment changed devices");
        check(std::strcmp(room.getName(), "Living") == 0 && equalPower(room.totalPower(), 813),
              "Self-assignment changed room data");
        check(Device::count() == 3, "Self-assignment changed live count");
    }

    void test08()
    {
        Room room("Living");
        populate(room, true);
        check(!room.removeDevice(999), "Unknown ID must return false (not found)");
        check(room.getCount() == 3 && equalPower(room.totalPower(), 813),
              "Removing unknown ID must not change room");
        // Them kiem tra xoa giua mang, dau mang, cuoi mang va phong rong.
        check(room.removeDevice(2) && room.find(2) == nullptr, "Removing middle device failed");
        check(room.find(1) && room.find(3) && room.getCount() == 2,
              "Removing middle device damaged remaining devices");
        check(Device::count() == 2 && equalPower(room.totalPower(), 13), "Removed device was not released");
        check(room.removeDevice(1) && room.removeDevice(3), "Removing remaining devices failed");
        check(room.getCount() == 0 && !room.removeDevice(3), "Removing from empty room failed");
    }

    void test09()
    {
        Room room("Growing room");
        check(room.getCapacity() == 2, "Initial capacity must be 2 for TC-09");
        for (uint32_t id = 1; id <= 10; ++id)
        {
            Light light(id, "Growing light", 50);
            light.turnOn();
            room.addDevice(light);
        }
        check(room.getCount() == 10 && room.getCapacity() >= 10, "Array did not grow correctly");
        for (uint32_t id = 1; id <= 10; ++id)
        {
            check(equalPower(requireDevice(room, id).powerUsage(), 5), "Device lost during growth");
        }
        check(equalPower(room.totalPower(), 50), "Ten lights must use 50 W");
        TextBuffer buffer;
        std::ostream output(&buffer);
        room.listDevices(output);
        const char *cursor = buffer.text();
        int listed = 0;
        while ((cursor = std::strstr(cursor, "Brightness:")) != nullptr)
        {
            ++listed;
            ++cursor;
        }
        check(listed == 10, "Listing must contain all ten devices");
    }

    void test10()
    {
        check(Device::count() == 0, "Live count must start at zero");
        {
            Room room("Living");
            populate(room, true);
            check(Device::count() == 3, "Live count must include room-owned devices");
            Room copied(room);
            check(Device::count() == 6, "Copies must increase live count");
        }
        check(Device::count() == 0, "Live count must end at zero");
        // Destructor ao: xoa clone qua con tro lop cha.
        {
            Light light(1, "Original", 80);
            Device *copy = light.clone();
            const bool independent = copy != &light && copy->getName() != light.getName();
            delete copy;
            check(independent, "clone() must produce an independent device");
            check(Device::count() == 1, "Deleting via Device* must release the clone");
        }
        check(Device::count() == 0, "All devices must be destroyed");
    }
} // namespace

int main()
{
    const struct TestCase
    {
        const char *name;
        void (*run)();
    }

    tests[] = {
        {"TC-01: Three device types, status and operators", test01},
        {"TC-02: Turn ON, total power 813 W", test02},
        {"TC-03: Recording, total power 816 W", test03},
        {"TC-04: Invalid values, boundaries and settings", test04},
        {"TC-05: Deep copy constructor and assignment", test05},
        {"TC-06: Destroy copy, original remains usable", test06},
        {"TC-07: Self-assignment", test07},
        {"TC-08: Remove unknown and existing IDs", test08},
        {"TC-09: Grow from capacity 2 to ten devices", test09},
        {"TC-10: Cleanup, clone and virtual destructor", test10},
    };

    int passed = 0;
    int failed = 0;
    for (const TestCase &test : tests)
    {
        std::cout << "\n[RUN] " << test.name << '\n';
        try
        {
            test.run();
            check(Device::count() == 0, "Device leak after test");
            ++passed;
            std::cout << "[PASS] " << test.name << '\n';
        }
        catch (const char *message)
        {
            ++failed;
            std::cout << "[FAIL] " << test.name << ": " << message << '\n';
        }
        catch (const std::exception &error)
        {
            ++failed;
            std::cout << "[FAIL] " << test.name << ": " << error.what() << '\n';
        }
    }
    std::cout << "\nResult: " << passed << " passed, " << failed << " failed.\n"
              << "Live devices at exit: " << Device::count() << '\n';
    return (failed == 0 && Device::count() == 0) ? 0 : 1;
}
