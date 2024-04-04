#include "proj.hpp"
using namespace std;

int main(int argc, char* argv[]) {
    if (argc >= 5) {
        string filename = argv[1];
        ifstream infile(filename);

        if (!infile.is_open()) {
            cerr << "Failed to open file: " << filename << endl;
            return 1;
        }
        
        string line;
        int starting_instruction = stoi(argv[2]);
        int simulating_instruction = stoi(argv[3]);
        int width = stoi(argv[4]);
        int cycle_count = 0;
        while (getline(infile, line) && (cycle_count < simulating_instruction)) {
            
            cycle_count++;
        }
        
        infile.close(); 
        
    } else {
        cout << "Insufficient command line arguments provided." << endl;
    }

    return 0;
}
