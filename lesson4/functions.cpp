#include <iostream>
#include <functional>
#include <limits>
using namespace std;

void showMenu()
{
    cout << "=================================\n";
    cout << "Chương trình tính toán\n"
         << endl;
    cout << "1. Cộng\n"
         << endl;
    cout << "2. Trừ\n"
         << endl;
    cout << "3. Nhân\n"
         << endl;
    cout << "4. Chia\n"
         << endl;
    cout << "5. Huỷ chương trình\n"
         << endl;
    cout << "Chọn phép tính bạn muốn" << endl;
    cout << "=================================\n";
}

bool inputNumbers(float &a, float &b)
{
    cout << "Nhập a: ";
    if (!(cin >> a))
    {
        cout << "Dữ liệu không hợp lệ!" << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return false;
    }

    cout << "Nhập b: ";
    if (!(cin >> b))
    {
        cout << "Dữ liệu không hợp lệ!" << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return false;
    }
    return true;
}

void showResult(float result)
{
    cout << "Kết quả: " << result << endl;
}

void calculateAddition()
{
    float a, b;
    if (!inputNumbers(a, b))
        return;
    showResult(plus<float>{}(a, b));
}

void calculateSubtraction()
{
    float a, b;
    if (!inputNumbers(a, b))
        return;
    showResult(minus<float>{}(a, b));
}

void calculateMultiplication()
{
    float a, b;
    if (!inputNumbers(a, b))
        return;
    showResult(multiplies<float>{}(a, b));
}

void calculateDivision()
{
    float a, b;
    if (!inputNumbers(a, b))
        return;

    if (b == 0.0f)
    {
        cout << "Không thể chia cho 0!" << endl;
        return;
    }

    float result = divides<float>{}(a, b);
    showResult(result);
}

int main()
{
    int choose = 0;
    bool running = true;
    while (running)
    {
        showMenu();
        if (!(cin >> choose))
        {
            if (cin.eof())
                break;

            cout << "Vui lòng nhập số từ 1 đến 5!" << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        switch (choose)
        {
        case 1:
            calculateAddition();
            break;
        case 2:
            calculateSubtraction();
            break;
        case 3:
            calculateMultiplication();
            break;
        case 4:
            calculateDivision();
            break;
        case 5:
            running = false;
            break;
        default:
            cout << "Lựa chọn không hợp lệ!" << endl;
            break;
        }
    }
    return 0;
}
