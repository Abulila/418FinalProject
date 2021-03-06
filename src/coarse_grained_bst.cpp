#include <iostream>
#include <vector>
#include <stack>

#include "coarse_grained_bst.h"

BST::BST() : root(nullptr) {
    pthread_mutex_init(&bst_lock, nullptr);
}


BST::~BST() {
    free_node(root);
    pthread_mutex_destroy(&bst_lock);
}


void BST::free_node(node* current_node) {
    if (current_node == nullptr)
        return;
    free_node(current_node->left);
    free_node(current_node->right);
    delete current_node;
}


bool BST::insert_node(
        node* current_node, int val, node* new_node) {
    if (val < current_node->value) {
        if (current_node->left == nullptr) {
            current_node->left = new_node;
            return true;
        } else {
            return insert_node(current_node->left, val, new_node);
        }
    } else if (val > current_node->value) {
        if (current_node->right == nullptr) {
            current_node->right = new_node;
            return true;
        } else {
            return insert_node(current_node->right, val, new_node);
        }
    }
    return false;
}


bool BST::insert(int x) {
    pthread_mutex_lock(&bst_lock);

    node* new_node = new node;
    new_node->value = x;
    new_node->left = nullptr;
    new_node->right = nullptr;

    bool ret = false;

    if (root == nullptr) {
        root = new_node;
        ++size;
        ret = true;
    } else if ((ret = insert_node(root, x, new_node))) {
        ++size;
    }

    pthread_mutex_unlock(&bst_lock);
    return ret;
}

int BST::num_children(node* n) {
    int x = 0;
    if (n->left != nullptr) ++x;
    if (n->right != nullptr) ++x;
    return x;
}

//@REQUIRES: n != nullptr
int BST::find_min(node* n) {
    if (n->left == nullptr)
        return n->value;
    return find_min(n->left);
}

bool BST::remove_node(node* current_node, node* parent, direction d, int val) {
    if (current_node == nullptr)
        return false;

    if (val == current_node->value) {
        node* new_node;
        int new_val;
        switch (num_children(current_node)) {
            case 0:
                if (parent != nullptr && d == LEFT) {
                    parent->left = nullptr;
                } else if (parent != nullptr && d == RIGHT) {
                    parent->right = nullptr;
                }
                else
                    root = nullptr;
                delete current_node;
                break;
            case 1:
                new_node = (current_node->left == nullptr ?
                                current_node->right : current_node->left);
                if (parent != nullptr && d == LEFT)
                    parent->left = new_node;
                else if (parent != nullptr && d == RIGHT)
                    parent->right = new_node;
                else
                    root = new_node;
                delete current_node;
                break;
            case 2:
                new_val = find_min(current_node->right);
                remove_node(current_node->right, current_node, RIGHT, new_val);
                current_node->value = new_val;
                break;
        }
        --size;
        return true;
    } else if (val < current_node->value) {
        return remove_node(current_node->left, current_node, LEFT, val);
    } else {
        return remove_node(current_node->right, current_node, RIGHT, val);
    }
}

bool BST::remove(int x) {
    pthread_mutex_lock(&bst_lock);

    bool result = remove_node(root, nullptr, LEFT, x);

    pthread_mutex_unlock(&bst_lock);
    return result;
}

bool BST::search_node(node* current_node, int val) {
    if (current_node == nullptr) {
        return false;
    } else if (val == current_node->value) {
        return true;
    } else if (val < current_node->value) {
        return search_node(current_node->left, val);
    } else {
        return search_node(current_node->right, val);
    }
}

bool BST::contains(int x) {
    pthread_mutex_lock(&bst_lock);

    bool result = search_node(root, x);

    pthread_mutex_unlock(&bst_lock);
    return result;
}

void BST::fill_inorder(node* current_node, std::vector<int>* in_order) {
    if (current_node == nullptr)
        return;
    fill_inorder(current_node->left, in_order);
    in_order->push_back(current_node->value);
    fill_inorder(current_node->right, in_order);
}

std::vector<int> BST::in_order_traversal() {
    pthread_mutex_lock(&bst_lock);

    std::vector<int> out;
    fill_inorder(root, &out);

    pthread_mutex_unlock(&bst_lock);

    return out;
}
