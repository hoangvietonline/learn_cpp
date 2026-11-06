#include <iostream>
#include <vector>

int main()
{
    std::cout << __cplusplus << std::endl;
    std::vector<int> numbers;
    numbers.reserve(5);
    numbers.push_back(1);
    numbers.push_back(2);
    numbers.push_back(3);
    numbers.push_back(4);
    numbers.push_back(5);

    numbers.insert(numbers.end(), {10,20,3,76});

    for (std::size_t i = 0; i < numbers.size(); ++i)
    {
        const int &value = numbers[i];

        std::cout << "index" << i << ": " << value << std::endl;
    }

    std::cout << "size:" << numbers.size() << std::endl;
}
