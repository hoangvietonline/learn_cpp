#include <iostream>
#define max_value 100
using namespace std;


int main()
{
    string a = "viet";
    string* ptr = &a;
    *ptr = "10";

    int player_score = 10;
    int player_lives = 3;
    int value = max_value /5;

    cout << "Initial Score: " << player_score << endl;
    cout << "Initial Lives: " << player_lives << endl;

    cout << "Score using postfix increment " << player_score++ << endl;
    cout << "Score using prefix increment " << ++player_score << endl;

    cout << "Lives using postfix decrement " << player_lives-- << endl;
    cout << "Score using prefix increment " << --player_lives << endl;

    cout <<"" << max_value << endl;

    cout << "address of a: " << &a << endl;
    cout << "address of ptr: " << &ptr << endl;
    cout << "value of ptr: " << ptr << endl;
    cout << "value pointer ptr: " << *ptr << endl;


    int highScores[4] = {5,7,8,10};
    int* scorePtr = highScores;
    
    cout << "address of highScores[0]: " << scorePtr << endl;
    cout << "address of highScores[1]: " << scorePtr +1 << endl;
    cout << "value of  highScores[0]" << *scorePtr << endl;
    cout << "value of  highScores[1]" << scorePtr[1] << endl;
    cout << "value of  highScores[1]" << *(scorePtr+1) << endl;
}