#include <iostream>
#include <string>
#include <chrono>

using Clock = std::chrono::system_clock;
using TimpePoint = std::chrono::time_point<Clock>;
using Duration = std::chrono::duration<double>;

int createMonster()
{
    int health = 100;
    return health;
}

int createMonster();

int main()
{
    TimpePoint *start, *end;
    Duration *elapsed_seconds = new Duration();
    std::string input;
    start = new TimpePoint(Clock::now());



    while (input != "start")
    {
    std::cout << "Type `start` as quickly as you can and hit ENTER " << std::endl;
        if (!(std::cin >> input))
        {
            delete start;
            delete elapsed_seconds;
            return 
            1;
        }

        if (input == "start") break;

         std::cout << "Bạn nhập sai, please enter again!" << std::endl;
    }

    end = new TimpePoint(Clock::now());
    *elapsed_seconds = *end - *start;
    std::cout << "Reaction time: " << elapsed_seconds->count() << "s" << std::endl;
    std::cout << std::endl
              << std::endl;

    delete start;
    delete end;
    delete elapsed_seconds;


    int health = createMonster();

    std::cout << health << std::endl;
    return 0;
}


