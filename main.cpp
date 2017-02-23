#include <iostream>
#include <fstream>
using namespace std;

int main (int argc, char **argv) {

    string name;

    if (argc == 1)
        name = "in.txt";
    else
        name = string(argv[0]);

    string line;
    ifstream file (name);

    while ( getline (file,line) )
    {
        //do magic
    }

    return 0;
}