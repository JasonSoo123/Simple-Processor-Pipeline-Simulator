#include "proj.hpp"
using namespace std;


int main (int argc, char* argv[])
{
    if (argc >= 5)
    {
        string filename = argv[1];
        ifstream infile(filename);
        string line;
        //read line
        int starting_instruction = atof(argv[2]);
        int simulating_instruction = atof(argv[3]);
        int width = atof(argv[4]);

    }
    else
    {
        cout << "Insufficent command line arguments provided."<<endl;
    }
}