#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int main(){
    ifstream inFile;
    string str;
    int number;
    char letter;

    inFile.open("player.txt");
    
    if (inFile.fail())
    {
        cout << "File not found!" << endl;

    } else{
        while (!inFile.eof())
        {
            getline(inFile,str);
            cout << str << endl;
        }

        inFile.close();
        
    }
    cout << endl << endl;

    return 0;
}