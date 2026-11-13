#include <iostream>

class Student {
    public:
        int* age;

        Student(int a){
            age = new int(a);    
        }

        Student(const Student &other){
            age = new int(*other.age);
        }

        Student& operator=(const Student &other){
            if (this != &other)
            {
                int* newAge = new int (*other.age);
                delete age;
                age = newAge; 
            }

            return *this;
            
        }

        ~Student(){
            delete age;
        }
};

int main (){
    Student student1(12);
    Student student2 = student1;

    return 0;
}