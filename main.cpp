#include <iostream>
#include <fstream>
#include "bplustree.hpp"

/**
 *
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char **argv) {

    std::string input_file = "";
    if (argc > 1) {
        // first argument is the file name containing the input test data
        input_file = argv[1];
    }

    // ifstream is used to read input_file
    std::ifstream inf(input_file);

    // If output file stream couldn't be read
    if (!inf) {
        // Print an error and exit
        std::cerr << "ERROR! " << input_file << " could not be opened for reading!" << std::endl;
        exit(1);
    }

    //search & write output to file
    std::ofstream outf("output_file.txt");
    // If we couldn't open the output file stream for writing
    if (!outf) {
        // Print an error and exit
        std::cerr << "ERROR! output_file.txt could not be opened for writing!" << std::endl;
        exit(1);
    }

    //read first line from file - order of B+ tree = m
    std::string order;
    getline(inf, order);
    auto bplustree = new BPlusTree();
    bplustree->initialise(atoi(order.c_str()));

    int current_line = 0;
    // Read rest of the file skipping 1st line and parse the input
    while (inf) {
        if (current_line == 0) {
            current_line++;
        } else {
            // read data from the file into a string
            std::string string_input;
            inf >> string_input;

            std::string delimiter1 = "(";
            std::string delimiter2 = ",";
            std::string delimiter3 = ")";

            size_t position = string_input.find(delimiter1);

            /*Input parsed - 3 diff values stored in array - Insert/Search, key/key1, value/key2,
             key2 can be empty in case of exact search
             */
            std::string operation_type = string_input.substr(0, position);  // operation_type - Insert/Search
            string_input.erase(0, position + delimiter1.length());

            std::string input1, input2; // for parsing the keys/values from input
            //TODO: optimize code; can we use something like INTEGER_MAX for comparison in if statement??
            // For search there can be one or two keys as input, array initialized depending on that
            if (operation_type == "Search") {
                //no comma
                if (string_input.find(delimiter2) > (size_t) 1000) {
                    position = string_input.find(delimiter3);
                    input1 = string_input.substr(0, position);
                    string_input.erase(0, position + delimiter3.length());
                } else {
                    // range search
                    position = string_input.find(delimiter2);
                    input1 = string_input.substr(0, position);
                    string_input.erase(0, position + delimiter2.length());

                    position = string_input.find(delimiter3);
                    input2 = string_input.substr(0, position);
                    string_input.erase(0, position + delimiter3.length());
                }

                float key1 = atof(input1.c_str());

                if (input2 == "") {
                    std::vector<std::string> values = bplustree->search(key1);
                    // if search yields no result, then add NULL to output file
                    if (values.empty()) {
                        outf << "NULL" << std::endl;
                    } else {
                        for (std::vector<std::string>::const_iterator i = values.begin(); i < values.end(); ++i) {
                            outf << *i;
                            if (i < values.end() - 1)
                                outf << ","; // add comma only if value is not last value
                            else
                                outf << std::endl;
                        }
                    }
                } else {
                    float key2 = atof(input2.c_str());
                    std::vector<std::pair<float, std::string>> values = bplustree->search(key1, key2);
                    // if search yields no result, then add NULL to output file
                    if (values.empty()) {
                        outf << "NULL" << std::endl;
                    } else {
                        for (std::vector<std::pair<float, std::string>>::const_iterator i = values.begin();
                             i < values.end(); ++i) {
                            outf << "(" << values[i - values.begin()].first << "," << values[i - values.begin()].second
                                 << ")";
                            if (i < values.end() - 1)
                                outf << ","; // add comma only if value is not last value
                            else
                                outf << std::endl;
                        }
                    }

                }

            } else {
                position = string_input.find(delimiter2);
                input1 = string_input.substr(0, position);
                string_input.erase(0, position + delimiter2.length());

                position = string_input.find(delimiter3);
                input2 = string_input.substr(0, position);
                string_input.erase(0, position + delimiter3.length());

                //insert call
                float key = atof(input1.c_str());
                bplustree->insert(key, input2);
            }
        }
    }

    return 0;
}
