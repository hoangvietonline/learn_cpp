#include <iostream>
#include <limits>

#include "Device.h"
#include "Light.h"
#include "Thermostat.h"
#include "SecurityCam.h"
#include "Room.h"

const int MAX_ROOMS = 10;

// Tra ve false neu het du lieu (EOF), de chuong trinh thoat sach se.
bool readInt(const char *prompt, int &value)
{
    while (true)
    {
        std::cout << prompt;
        if (std::cin >> value)
        {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return true;
        }

        if (std::cin.eof() || std::cin.bad())
        {
            return false;
        }

        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input. Please enter an integer.\n";
    }
}

void showMenu()
{
    std::cout << "\n=== SMART HOME DEVICE MANAGER ===\n"
              << "1. Create room\n"
              << "2. Add device\n"
              << "3. List devices\n"
              << "4. Turn device on/off\n"
              << "5. Change device setting\n"
              << "6. Show total power\n"
              << "7. Copy room\n"
              << "8. Delete device\n"
              << "9. Delete room\n"
              << "0. Exit\n";
}

// Tra ve chi so mang (0-based), hoac -1 neu khong chon duoc phong.
int selectRoom(Room *const rooms[], int roomCount)
{
    if (roomCount == 0)
    {
        std::cout << "No rooms available. Create a room first.\n";
        return -1;
    }

    std::cout << "\nAvailable rooms:\n";
    for (int i = 0; i < roomCount; ++i)
    {
        std::cout << i + 1 << ". " << rooms[i]->getName() << '\n';
    }

    int number = 0;
    if (!readInt("Select room number: ", number))
    {
        return -1;
    }
    if (number < 1 || number > roomCount)
    {
        std::cout << "Invalid room number.\n";
        return -1;
    }
    return number - 1;
}

bool readName(const char *prompt, char *name, int capacity);

void createRoom(Room *rooms[], int &roomCount)
{
    if (roomCount >= MAX_ROOMS)
    {
        std::cout << "Room limit reached. Unable to create a new room.\n";
        return;
    }

    char name[100]{};
    if (!readName("Enter room name: ", name, sizeof(name)))
    {
        return;
    }

    rooms[roomCount] = new Room{name};
    ++roomCount;
    std::cout << "Room " << name << " created successfully.\n";
}

// Nhap ten co khoang trang; tra ve false khi het du lieu.
bool readName(const char *prompt, char *name, int capacity)
{
    while (true)
    {
        std::cout << prompt;
        if (!std::cin.getline(name, capacity))
        {
            if (std::cin.eof() || std::cin.bad())
            {
                return false;
            }
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Name is too long. Please try again.\n";
            continue;
        }

        bool hasText = false;
        for (int i = 0; name[i] != '\0'; ++i)
        {
            if (name[i] != ' ' && name[i] != '\t' && name[i] != '\r')
            {
                hasText = true;
                break;
            }
        }
        if (hasText)
        {
            return true;
        }
        std::cout << "Name cannot be empty.\n";
    }
}

bool readIntInRange(const char *prompt, int minimum, int maximum, int &value)
{
    while (readInt(prompt, value))
    {
        if (value >= minimum && value <= maximum)
        {
            return true;
        }
        std::cout << "Enter a value from " << minimum << " to " << maximum << ".\n";
    }
    return false;
}

void addDevice(Room &room, uint32_t &nextDeviceId)
{
    if (nextDeviceId > static_cast<uint32_t>(std::numeric_limits<int>::max()))
    {
        std::cout << "Device ID limit reached.\n";
        return;
    }

    std::cout << "\nDevice types:\n"
              << "1. Light\n"
              << "2. Thermostat\n"
              << "3. Security camera\n";

    int type = 0;
    if (!readIntInRange("Select device type: ", 1, 3, type))
    {
        return;
    }
    char name[100]{};
    if (!readName("Enter device name: ", name, sizeof(name)))
    {
        return;
    }

    int oldCount = room.getCount();
    switch (type)
    {
    case 1:
    {
        int brightness = 0;
        if (!readIntInRange("Brightness (0-100): ", 0, 100, brightness))
        {
            return;
        }
        Light device(nextDeviceId, name, brightness);
        room.addDevice(device);
        break;
    }
    case 2:
    {
        int temperature = 0;
        if (!readIntInRange("Target temperature (16-30 C): ", 16, 30, temperature))
        {
            return;
        }
        Thermostat device(nextDeviceId, name, temperature);
        room.addDevice(device);
        break;
    }
    case 3:
    {
        int resolution = 0;
        while (true)
        {
            if (!readInt("Resolution (720/1080/2160): ", resolution))
            {
                return;
            }
            if (resolution == 720 || resolution == 1080 || resolution == 2160)
            {
                break;
            }
            std::cout << "Invalid resolution. Choose 720, 1080 or 2160.\n";
        }
        int recording = 0;
        if (!readIntInRange("Recording (0: OFF, 1: ON): ", 0, 1, recording))
        {
            return;
        }
        SecurityCam device(nextDeviceId, name, resolution, recording == 1);
        room.addDevice(device);
        break;
    }
    }

    if (room.getCount() == oldCount)
    {
        std::cout << "Could not add device.\n";
        return;
    }
    std::cout << "Device added successfully (ID: " << nextDeviceId << ").\n";
    room.find(nextDeviceId)->status(std::cout);
    std::cout << '\n';
    ++nextDeviceId;
}

void listDevices(Room &room)
{
    room.listDevices(std::cout);
}

void turnDeviceOnOff(Room &room)
{
    int id = 0;
    if (!readIntInRange("Enter device ID: ", 1,
                        std::numeric_limits<int>::max(), id))
    {
        return;
    }

    int state = 0;
    if (!readIntInRange("Device state (0: OFF, 1: ON): ", 0, 1, state))
    {
        return;
    }

    Device *device = room.find(id);
    if (device != nullptr)
    {
        (state == 1) ? device->turnOn() : device->turnOff();
    }
    else
    {
        std::cout << "No device with that ID was found in this room.\n";
    }
}

void changeDeviceSetting(Room &room)
{
    int id = 0;
    if (!readIntInRange("Enter device ID: ", 1,
                        std::numeric_limits<int>::max(), id))
    {
        return;
    }

    Device *device = room.find(id);
    if (device != nullptr)
    {
        int settingCount = device->settingCount();
        for (int i = 0; i < settingCount; ++i)
        {
            std::cout << i << ": " << device->settingName(i) << '\n';
        }

        int i = 0;
        if (!readIntInRange("Enter setting index: ", 0, settingCount - 1, i))
        {
            return;
        }

        int value = 0;
        if (!readInt("Enter new value: ", value))
        {
            return;
        }

        if (device->applySetting(i, value))
        {
            std::cout << "Setting updated successfully.\n";
        }
        else
        {
            std::cout << "The value is invalid or the setting could not be applied.\n";
        }
    }
    else
    {
        std::cout << "No device with that ID was found in this room.\n";
    }
}

void showTotalPower(Room &room)
{
    std::cout << "Total Power: " << room.totalPower() << "W\n";
}

void copyRoom(Room *rooms[], int &roomCount, int sourceIndex)
{
    if (sourceIndex < 0 || sourceIndex >= roomCount)
    {
        std::cout << "Invalid room index.\n";
        return;
    }
    if (roomCount >= MAX_ROOMS)
    {
        std::cout << "Room limit reached. Unable to copy the room.\n";
        return;
    }

    rooms[roomCount] = new Room(*rooms[sourceIndex]);
    std::cout << "Room copied successfully.\n";
    rooms[roomCount]->listDevices(std::cout);

    ++roomCount;
}

void deleteDevice(Room &room)
{
    int id = 0;
    if (!readIntInRange("Enter device ID: ", 1,
                        std::numeric_limits<int>::max(), id))
    {
        return;
    }

    bool isSuccess = room.removeDevice(id);
    isSuccess ? std::cout << "Device deleted successfully.\n" : std::cout << "Device not found.\n";
}

void deleteRoom(Room *rooms[], int &roomCount, int index)
{
    if (index < 0 || index >= roomCount)
    {
        std::cout << "Invalid room index.\n";
        return;
    }

    delete rooms[index];

    for (int i = index; i < roomCount - 1; ++i)
    {
        rooms[i] = rooms[i + 1];
    }

    --roomCount;
    rooms[roomCount] = nullptr;

    std::cout << "Room deleted successfully.\n";
}

int main()
{
    Room *rooms[MAX_ROOMS]{};
    int roomCount = 0;
    uint32_t nextDeviceId = 1;

    while (true)
    {
        showMenu();
        int choice = 0;
        if (!readInt("Your choice: ", choice) || choice == 0)
        {
            break;
        }

        if (choice < 1 || choice > 9)
        {
            std::cout << "Invalid choice.\n";
            continue;
        }

        if (choice == 1)
        {
            createRoom(rooms, roomCount);
            continue;
        }

        int index = selectRoom(rooms, roomCount);
        if (index < 0)
        {
            continue;
        }

        switch (choice)
        {
        case 2:
            addDevice(*rooms[index], nextDeviceId);
            break;
        case 3:
            listDevices(*rooms[index]);
            break;
        case 4:
            turnDeviceOnOff(*rooms[index]);
            break;
        case 5:
            changeDeviceSetting(*rooms[index]);
            break;
        case 6:
            showTotalPower(*rooms[index]);
            break;
        case 7:
            copyRoom(rooms, roomCount, index);
            break;
        case 8:
            deleteDevice(*rooms[index]);
            break;
        case 9:
            deleteRoom(rooms, roomCount, index);
            break;
        }
    }

    // Giai phong tat ca phong khi chon Exit hoac gap EOF.
    for (int i = 0; i < roomCount; ++i)
    {
        delete rooms[i];
    }

    std::cout << "Live devices after cleanup: " << Device::count() << '\n';
    std::cout << "Goodbye!\n";
    return 0;
}
