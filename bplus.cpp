#include <string>
#include <queue>

#include "bplus.hpp"

void BPlusTree::initialise(int order) {
    // an InternalNode can have at most "order" no. of children
    // i.e., InternalNodes and DataNodes can have at most (order - 1) no. of keys
    this->order = order;

    // initialise root as an empty (with FLT_MAX as the only key) data node
    root = new DataNode();
    head = tail = static_cast<DataNode *>(root);
}

int BPlusTree::get_order() {
    return order;
}

void BPlusTree::insert(float key_to_insert, std::string value) {
    Node *curr_node = root;
    std::cout << "before..." << std::endl;
    // reach a DataNode first
    while (curr_node->get_type().compare("DATA") != 0) {
        std::cout << "Entered an internal node" << std::endl;
        InternalNode *curr_internal_node = static_cast<InternalNode *>(curr_node);
        for (std::vector<float>::iterator i = curr_internal_node->keys.begin();
             i < curr_internal_node->keys.end(); ++i) {
            if (key_to_insert < *i) {
                curr_node = curr_internal_node->child_ptrs.at(i - curr_internal_node->keys.begin());
                std::cout << "Shifted to the child at " << i - curr_internal_node->keys.begin() << std::endl;
                break;
            }
        }
    }
    std::cout << "Trying to insert in DataNode..." << std::endl;
    static_cast<DataNode*>(curr_node)->insert(key_to_insert, value);
    std::cout << "Key " << key_to_insert << " inserted in DataNode sucessfully" << std::endl;

    if (curr_node->get_n_keys() == order)
    {
        std::cout << "Trying to split DataNode" << std::endl;
        auto split_result = static_cast<DataNode*>(curr_node)->split(order);
        std::cout << "DataNode split completed successfully" << std::endl;

        InternalNode *curr_parent = static_cast<InternalNode*>(curr_node->parent);
        bool needs_new_root = true;
        while (nullptr != curr_parent)
        {
            curr_parent->combine(split_result);
            // we need to split the parent if combine caused it to have "order"
            // no. of children
            if (curr_parent->get_n_keys() == order)
            {
                split_result = curr_parent->split(order);
                // curr_node is being updated as it will be used in the if-condition
                // below when needs_new_root is true
                curr_node = curr_parent;
                curr_parent = static_cast<InternalNode*>(curr_node->get_parent());
            }
            else
            {
                needs_new_root = false;
                break;
            }
        }

        if (needs_new_root)
        {
            InternalNode *new_root = split_result.first;
            auto new_root_right_child = split_result.second;
            // remember we didn't make any child/parent associations in DataNode::split?
            // we need to make them here
            new_root->insert_child(0, curr_node); // this is why we were updating curr_node in the while loop above
            new_root->insert_child(1, new_root_right_child);
            curr_node->set_parent(new_root);
            new_root_right_child->set_parent(new_root);
            root = new_root;
        }
        /*
        // if this is not the root node, we need to handle insert and (possibly)
        // split on its ancestors, up to the root
        if (nullptr != curr_data_node->parent)
        {
            // combine the result of the split with the old DataNode's parent
            // and split it (the parent) if required
            auto curr_parent = curr_data_node->parent;
            while ((nullptr != curr_parent) && (curr_parent->get_n_children() == order))
            {
                curr_parent->combine(split_result);
                auto in_split_result = curr_parent->split(order);
                curr_parent = curr_parent->get_parent();
            }
        }
        // if this is the root node, we need the new InternalNode to become the parent
        else
        {
            InternalNode *new_root = split_result.first;

            // remember we didn't make any child/parent associations in DataNode::split?
            // we need to make them here
            new_root->insert_child(0, curr_data_node);
            new_root->insert_child(1, new_data_node);
            curr_data_node->set_parent(new_root);
            new_data_node->set_parent(new_root);
            root = new_root;
        }
         */
    }
}

std::vector<std::string> BPlusTree::search(float key) {
    std::cout << "In search method:: " << std::endl;
    Node *curr_node = root;
    std::vector<std::string> search_output_arr;
    std::cout << "In search method:: before wgile " << curr_node->get_type().compare("DATA") << std::endl;
    // reach a DataNode first
    while (curr_node->get_type().compare("DATA") != 0) {
        std::cout << "In while::  " << std::endl;
        std::cout << "Entered an internal node" << std::endl;
        // cast Node to internal node
        InternalNode *curr_internal_node = static_cast<InternalNode *>(curr_node);
        for (std::vector<float>::iterator i = curr_internal_node->keys.begin();
             i < curr_internal_node->keys.end(); ++i) {
            // search key < key from internal node
            if (key < *i) {
                curr_node = curr_internal_node->child_ptrs.at(i - curr_internal_node->keys.begin());
                std::cout << "Shifted to the child at " << i - curr_internal_node->keys.begin() << std::endl;
                break;
            }
        }
    }

    std::cout << "In search method:: after while " << std::endl;
    //after reaching data node, convert the curr_node to type - DataNode
    DataNode *curr_data_node = static_cast<DataNode *>(curr_node);
    std::cout << "curr data" << std::endl;

    //values from
    while (curr_data_node != nullptr) {
        std::cout << "curr data in while" << std::endl;
        for (std::vector<float>::const_iterator i = curr_data_node->keys.begin(); i < curr_data_node->keys.end(); ++i) {
            std::cout << "*** for" << std::endl;
            if (key == *i) {
                std::cout << "*** if condition" << std::endl;
                search_output_arr.push_back(curr_data_node->values.at(i - curr_data_node->keys.begin()));
                std::cout << "*** post if condition" << std::endl;
            }
            std::cout << "*** out if condition" << std::endl;
        }
        std::cout << "*** for end if condition" << std::endl;
        curr_data_node = curr_data_node->right;
        std::cout << "*** curr_data_node" << std::endl;
    }
    std::cout << "*** outside while" << std::endl;
    return search_output_arr;
}

    /*
     * Ranged search
     *
     * Search for key such that key_start <= key <= key_end
     */
    std::string BPlusTree::search(float key_start, float key_end) {
        return nullptr;
    }

    // TODO remove this temporary method
    void BPlusTree::print_all_keys() {
        for (DataNode *dn = head; dn != nullptr; dn = dn->right)
        {
            dn->print_all_keys();
        }
    }

    void BPlusTree::print_tree_bfs()
    {
        std::queue<Node*> q;
        q.push(root);

        while (!q.empty())
        {
            InternalNode *curr_node = static_cast<InternalNode*>(q.front());
            q.pop();
            for (auto const &child: curr_node->child_ptrs)
            {
                if (child->get_type().compare("DATA") != 0)
                {
                    q.push(child);
                }
            }
            curr_node->print_all_keys();
        }
    }
