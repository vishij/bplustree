#include <iostream>
#include <fstream>
#include "bplus.hpp"

int main(int argc, char** argv)
{
    using namespace std;
    string input_file = "";
    if (argc > 1) {
        //TODO: remove print
        // first argument is the file name containing the input test data
//        std::cout << argv[1] << std::endl;
        input_file = argv[1];
    }

    // ifstream is used to read input_file
    ifstream infile(input_file);

    // If output file stream couldn't be read
    if (!infile)
    {
        // Print an error and exit
        cerr << "ERROR! " << input_file << " could not be opened for reading!" << endl;
        exit(1);
    }

    //read first line from file - order of B+ tree = m
    string order;
    getline(infile, order);
        //TODO: remove comment
    cout << "order : " << order << endl;
    auto bptree = new BPlusTree();
    bptree->initialise(atoi(order.c_str()));

    int current_line = 0;
    //TODO: add code to read values
    // Read rest of the file skipping 1st  line and parse the input
    while (infile)
    {
        if(current_line == 0) {
            current_line++;
        } else {
            //TODO: remove print
            // read stuff from the file into a string and print it
            string str_input;
            infile >> str_input;
            string parsed_input[3];
            string delimiter1 = "(";
            string delimiter2 = ",";
            string delimiter3 = ")";

            size_t pos = str_input.find(delimiter1);
            cout<< "pos : " << pos << endl;

            /*Input parsed - 3 diff values stored in array - Insert/Search, key/key1, value/key2,
             key2 can be empty in case of exact search
             */
            string op_type = str_input.substr(0, pos);  // operation type - Insert/Search
            cout << "1 : " << op_type << endl;
            str_input.erase(0, pos + delimiter1.length());
            cout << "ip2 : " << str_input << endl;

            string input1, input2; // for parsing the keys/values from input
//            cout << (input1 == "") << " ;;;; " << (input2 == "");
            //TODO: optimize code; can we put something like INTEGER_MAX for comparison in if stmt??
            // For search there can be one or two keys as input, array initialized depending on that
            if(op_type == "Search") {
                //no comma
                if(str_input.find(delimiter2) > (size_t)1000) {
                    cout << "VISHI : " << str_input.find(delimiter2) << endl;
                    pos = str_input.find(delimiter3);
                    input1 = str_input.substr(0, pos);
                    cout << "2 : " << input1 << endl;
                    str_input.erase(0, pos + delimiter3.length());
                } else {
                    // range search
                    pos = str_input.find(delimiter2);
                    cout<< "pos : " << pos << endl;
                    input1 = str_input.substr(0, pos);
                    cout << "2 : " << input1 << endl;
                    str_input.erase(0, pos + delimiter2.length());
                    cout << "ip3 : " << str_input << endl;

                    pos = str_input.find(delimiter3);
                    input2 = str_input.substr(0, pos);
                    cout << "3 : " << input2 << endl;
                    str_input.erase(0, pos + delimiter3.length());
                }

                //search
                float key1 = atof(input1.c_str());
                if(input2 == "") {
                    cout << "ATOF :" << atof(input1.c_str()) << endl;
                    string value  = bptree->search(key1);
                } else {
                    float key2 = atof(input2.c_str());
                    bptree->search(key1, key2);
                }

                //write output to file
                ofstream outfile("output_file.txt");
                // If we couldn't open the output file stream for writing
                if (!outfile)
                {
                    // Print an error and exit
                    cerr << "ERROR! output_file.txt could not be opened for writing!" << endl;
                    exit(1);
                }

            } else {
                pos = str_input.find(delimiter2);
                cout<< "pos : " << pos << endl;
                input1 = str_input.substr(0, pos);
                cout << "2 : " << input1 << endl;
                str_input.erase(0, pos + delimiter2.length());
                cout << "ip3 : " << str_input << endl;

                pos = str_input.find(delimiter3);
                input2 = str_input.substr(0, pos);
                cout << "3 : " << input2 << endl;
                str_input.erase(0, pos + delimiter3.length());

                //insert call
                float key = atof(input1.c_str());
                cout << "Inserting value : " << atoi(order.c_str());
                bptree->insert(key, input2);
                bptree->print_all_keys();
            }

            //TODO: remove
            /*if(parsed_input[0] == "Insert") {
                cout << "Inserting value : " << atoi(order.c_str());
                bptree->insert(3.5, "something");
                bptree->print_all_keys();
            } else if(parsed_input[0] == "Search") {
                cout << "Searching value : ";
                float key1 = atof(input1.c_str());
                if(input2 == "") {
                    cout << "ATOF :" << atof(input1.c_str()) << endl;
                    bptree->search(key1);
                } else {
                    float key2 = atof(input2.c_str());
                    bptree->search(key1, key2);
                }
            }*/

            cout << "final str : " << str_input << endl;
        }
    }

    //TODO: processing code: inserts and splits
    ofstream outfile("output_file.txt");

    // If we couldn't open the output file stream for writing
    if (!outfile)
    {
        // Print an error and exit
        cerr << "ERROR! output_file.txt could not be opened for writing!" << endl;
        exit(1);
    }

    // TODO: write data to output
//    outfile << "This is line 1" << endl;
//    outfile << "This is line 2" << endl;


   /* auto bptree = new BPlusTree();
    bptree->initialise(4);
    std::cout << "B+ tree has bee initialised with order " << bptree->get_order() << std::endl;

    bptree->insert(3.5, "something");

    for (int i=1; i <= 20; ++i)
    {
        bptree->insert(i + 0.5, "some_random_value");
    }
    bptree->print_all_keys();*/
    return 0;
}
