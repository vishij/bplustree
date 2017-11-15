#include <iostream>
#include <fstream>
#include <cstdlib> // for exit()
#include "bplus.hpp"

int main(int argc, char** argv)
{

    using namespace std;
    string input_file = "";
    if (argc > 1) {
        //TODO: remove print
        // first argument is the file name containing the input test data
        std::cout << argv[1] << std::endl;
        input_file = argv[1];
    }

    // ifstream is used to read input_file
    ifstream inf(input_file);

    // If output file stream couldn't be read
    if (!inf)
    {
        // Print an error and exit
        cerr << "ERROR! " << input_file << " could not be opened for reading!" << endl;
        exit(1);
    }

    //TODO: add code to read values
    // While not end of file
    while (inf)
    {
        //TODO: remove print
        // read stuff from the file into a string and print it
        string strInput;
        inf >> strInput;
        cout << strInput << endl;
    }

    //TODO: processing code: inserts and splits

    auto bptree = new BPlusTree();
    bptree->initialise(4);

    bptree->insert(3.5, "something");
    bptree->print_all_keys();

    ofstream outf("output_file.txt");

    // If we couldn't open the output file stream for writing
    if (!outf)
    {
        // Print an error and exit
        cerr << "ERROR! output_file.txt could not be opened for writing!" << endl;
        exit(1);
    }

    // TODO: write data to output
//    outf << "This is line 1" << endl;
//    outf << "This is line 2" << endl;

    return 0;
}
