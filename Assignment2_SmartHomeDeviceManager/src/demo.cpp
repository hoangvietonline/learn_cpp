#include <iostream>
#include <cstring>

#include "Device.h"
#include "Light.h"
#include "Thermostat.h"
#include "SecurityCam.h"
#include "Room.h"


class BadRoom
{
private:
    char *name_;

public:
    explicit BadRoom(const char *name)
    {
        name_ = new char[std::strlen(name) + 1];
        std::strcpy(name_, name);
    }

    ~BadRoom()
    {
        delete[] name_;
    }

    const char *getName() const
    {
        return name_;
    }
};

int main()
{
    std::cout << "=== 1. Thu tu tao / huy Thermostat ===\n";
    {
        Thermostat thermostat1{1, "thermostat1", 16};
    }

    std::cout << "\n=== 2. Danh sach thiet bi da hinh ===\n";
    std::cout << "So thiet bi truoc scope: " << Device::count() << '\n';
    {

        Light light{2, "Light", 80};
        Thermostat thermostat{3, "thermostat", 22};
        SecurityCam securityCam{4, "SecurityCam", 1080, false};

        light.turnOn();
        thermostat.turnOn();
        securityCam.turnOn();
        Room roomA{"RoomA"};
        roomA.addDevice(light);
        roomA.addDevice(thermostat);
        roomA.addDevice(securityCam);

        Device *lightPtr = roomA.find(2);
        lightPtr->status(std::cout);
        std::cout << '\n';

        Device *thermostatPtr = roomA.find(3);
        thermostatPtr->status(std::cout);
        std::cout << '\n';

        Device *securityCamPtr = roomA.find(4);
        securityCamPtr->status(std::cout);
        std::cout << '\n';

        std::cout << "Total Power: " << roomA.totalPower() << "W\n";

        std::cout << "\n=== 3. Deep copy room ===\n";
        {

            Room roomB(roomA);
            Device *lightPtrRoomB = roomB.find(2);
            lightPtrRoomB->applySetting(0, 20);

            std::cout << "Light in roomA: ";
            lightPtr->status(std::cout);
            std::cout << '\n';

            std::cout << "Light in roomB: ";
            std::cout << *lightPtrRoomB << '\n';

            std::cout << "Address of light A: " << lightPtr << '\n';
            std::cout << "Address of light B: " << lightPtrRoomB << '\n';
        }
        lightPtr->status(std::cout);
        std::cout << '\n';
        std::cout << "\n=== 4. Tu gan phong ===\n";
        std::cout << "Before self-assignment:\n";
        roomA.status(std::cout);
        Room &roomAlias = roomA;
        roomA = roomAlias;
        std::cout << "After self-assignment:\n";
        roomAlias.status(std::cout);

        std::cout << "\n=== 5. Minh hoa loi shallow copy ===\n";
        BadRoom badRoom("BadRoom1");
        std::cout << "Name memory address: "
                  << static_cast<const void *>(badRoom.getName()) << "\n";
        // BadRoom badRoom2(badRoom);
        // Copy constructor mặc định chỉ sao chép địa chỉ name_.
        // Hai đối tượng sẽ cùng trỏ tới một vùng nhớ.
        // Khi ra khỏi scope, cả hai destructor cùng delete[] vùng nhớ đó,
        // gây double delete.

        std::cout << "\n=== 6. Destructor ao ===\n";

        Device *device = new Light(6, "Color Light", 90);
        delete device;

        // Device co destructor virtual nen khi delete qua Device*,
        // destructor cua doi tuong thuc Light duoc goi truoc,
        // sau do destructor cua Device duoc goi.
        // Neu destructor Device khong virtual, xoa doi tuong Light
        // qua Device* gay undefined behavior (hanh vi khong xac dinh).
        // Tai nguyen rieng cua lop con co the khong duoc giai phong.

        std::cout << "\n=== 7. Kiem tra bo dem ===\n";
        std::cout << "So thiet bi truoc khi roi scope: "
                  << Device::count() << '\n';
    }

    // Moi Room va thiet bi tren stack o tren phai duoc huy tai day.
    std::cout << "So thiet bi sau scope: " << Device::count()
              << " (mong doi: 0)\n";
    return 0;
}
