#include <iostream>
#include <string>
using namespace std;

class SmartDevice
{
private:
    string serialNumber;

protected:
    string brand;

public:
    SmartDevice(string serialNumber, string brand) : serialNumber(serialNumber), brand(brand)
    {
        cout << "[Contructor] SmartDevice created with serial number: " << serialNumber << " and brand: " << brand << endl;
    }

    string getSerialNumber() const
    {
        return serialNumber;
    }

    virtual void operate() = 0;

    virtual ~SmartDevice()
    {
        cout << "[Destructor] SmartDevice with serial number: " << serialNumber << " and brand: " << brand << " is being destroyed." << endl;
    }
};

class SmartLight : public SmartDevice
{
    private:
        int brightness;
    public:
        SmartLight(string serialNumber,string brand,int brightness) : SmartDevice(serialNumber,brand),brightness(brightness)
        {
            cout << "[Contructor] SmartLight created with brightness: " << brightness << endl;
        }
        void operate() override
        {
            cout << "Operating SmartLight with brightness: " << brightness << endl;
        }

};

int main()
{
    SmartDevice* myLight = new SmartLight("SL12345", "Philips", 75);

    myLight->operate();
    cout << "Serial Number: " << myLight->getSerialNumber() << endl;
    delete myLight; // This will call the destructor of SmartLight and then SmartDevice
    return 0;
}