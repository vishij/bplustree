#include <iostream>
#include "bplus.hpp"

int main()
{
    auto bptree = new BPlusTree();
    bptree->initialise(4);

    bptree->insert(3.5, "something");
    bptree->print_all_keys();

    return 0;
}
