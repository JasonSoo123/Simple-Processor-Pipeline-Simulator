#include "proj.hpp"
using namespace std;


int main(int argc, char* argv[]) 
{
    if (argc >= 5) 
    {
        string filename = argv[1];
        ifstream infile(filename);

        if (!infile.is_open()) 
        {
            cout << "Failed to open file: " << filename << endl;
            return 1;
        }
        
        string line;
        int starting_instruction = stoi(argv[2]);
        int simulating_instruction = stoi(argv[3]);
        int width = stoi(argv[4]);

        if ((width>5)||(width<0)||(starting_instruction<0)||(simulating_instruction<0))
        {
            cout << "Input Error. Terminating Simulation...\n" << endl;
        }

        int cycle_count = 0;
        struct Pipeline * pipeline = InitalizePipeline();
        while (getline(infile, line) && (cycle_count < simulating_instruction)) 
        {
            
            cycle_count++;
        }
        
        infile.close(); 
        
    } 
    else 
    {
        cout << "Insufficient command line arguments provided." << endl;
    }

    return 0;
}
