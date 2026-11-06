#include <iostream>
#include <string>
using namespace std;

class Animal
{
private:
    string name_;
    int age_;

public:
    Animal()
    {
        cout << "Default animal" << endl;
    }

    Animal(string name, int age) : name_(name), age_(age)
    {
        cout << "Create animal" << name << endl;
    }

    void setName(string &name)
    {
        name_ = name;
    }

    void setAge(int age)
    {
        if (age > 0)
        {
            age_ = age;
        }
    }

    string getName() const
    {
        return name_;
    }

    int getAge() const
    {
        return age_;
    }

    void displayInfo()
    {
        cout << "displayInfo "
             << "Name: " << name_
             << " Age: " << age_
             << endl;
    }

    virtual void makeSound() const = 0;
    virtual ~Animal() = default;
};

class Dog : public Animal
{
public:
    Dog() : Animal()
    {
        cout << "Default dog" << endl;
    }

    Dog(const string &name, const int &age) : Animal(name, age)
    {
        cout << "Create Dog" << name << endl;
    }

    void makeSound() const override
    {
        cout << "Woof Woof!" << endl;
    }
};

class Cat : public Animal
{
public:
    Cat() : Animal()
    {
        cout << "Default cat" << endl;
    }
    Cat(const string &name, const int &age) : Animal(name, age)
    {
        cout << "Create Dog" << name << endl;
    }

    void makeSound() const override
    {
        cout << "Meow Meow!" << endl;
    }
};

enum class asset_type
{
    texture,
    sound,
    animation,
    script
};

int main()
{
    Dog dog{"BIBI", 3};
    Cat cat{"KIKI", 2};
    dog.displayInfo();

    dog.setAge(5);
    dog.displayInfo();

    dog.setAge(-2);
    dog.makeSound();

    asset_type asset_value;

    asset_value = (asset_type::script);
    cout << static_cast<int>(asset_value) << endl;

    reference_wrapper<Animal> animals[] = {dog, cat};

    for (Animal &animal : animals)
    {
        animal.makeSound();
    }

    Cat dog1;
    Animal *animal = &dog1;
    Dog *dogptr = dynamic_cast<Dog *>(animal);

    dogptr->makeSound();
    return 0;
}