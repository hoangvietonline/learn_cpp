#include <iostream>
#include <cstring>

template <typename T>
T add(T a,T b){
    return a + b;
};

template <typename T>
class Box
{
private:
    T value_;
public:
    Box(T value): value_(value){

    };

    T getValue()const{
        return value_;
    }

    ~Box (){};
};

double divide(double a, double b){
    if (b == 0){
        throw std::invalid_argument("Mau so phai khac 0");
    }

    return a /b;
}

int main(){
    Box<std::string> box {"kaka"};
    std::cout << box.getValue() << std::endl;

    try
    {
        double c = divide(10,0);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}