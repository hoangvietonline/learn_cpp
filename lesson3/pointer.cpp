#include <iostream>

struct Charactor {
    int health;
    int strength;
};

Charactor* createCharactor(int health, int strength){
    return new Charactor{health,strength};
}

Charactor createCharactorNoPoint(int health, int strength){
    return Charactor{health,strength};
}

int main(){
    Charactor hero = createCharactorNoPoint(100,50); 
    std::cout << "health: "<< hero.health << std::endl;
    std::cout << "strength: "<< hero.strength << std::endl;

    float playHealth = 100.0f;
    void* genericPointer = &playHealth;

    float* healthPointer = static_cast<float*>(genericPointer);

    std::cout << "genericPointer: "<< genericPointer << std::endl;
    std::cout << "genericPointer: "<< healthPointer << std::endl;

    return 0;
}