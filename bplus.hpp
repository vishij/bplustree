#include <string>
#include "bplus.hpp"
#include "node.hpp"

void BPlusTree::initialise(int order)
{
    this->order = order;

    root = new InternalNode();
}

void BPlusTree::insert(float key_to_insert, std::string value)
{
    Node *currNode = root;

    while (currNode->get_type().compare("DATA") != 0)
    {
        for (std::vector<float>::iterator i = curr_node->keys.begin(); i != curr_node->keys.end(); ++i)
        {
            if (key_to_insert < *i)
            {
                currNode = currNode->child_ptrs.at(i - curr_node->keys.begin());
                break;
            }
        }
    }
}

std::string BPlusTree::search(float key)
{

}

/*
 * Ranged search
 *
 * Search for key such that key_start <= key <= key_end
 */
std::string BPlusTree::search(float key_start, float key_end)
{
}
