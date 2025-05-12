#include "BTree.hpp"
#include <iostream>

BTreeNode::BTreeNode(int t, bool leaf) {
    this->t = t;
    this->leaf = leaf;
}

Point* BTreeNode::search(uint64_t key) {
    int i = 0;
    while (i < keys.size() && key > keys[i].key)
        i++;

    if (i < keys.size() && key == keys[i].key)
        return &keys[i];

    if (leaf)
        return nullptr;

    return children[i]->search(key);
}

void BTreeNode::insertNonFull(Point k) {
    int i = keys.size() - 1;

    if (leaf) {
        keys.push_back(k);
        while (i >= 0 && k < keys[i]) {
            keys[i + 1] = keys[i];
            i--;
        }
        keys[i + 1] = k;
    } else {
        while (i >= 0 && k < keys[i])
            i--;

        i++;
        if (children[i]->keys.size() == 2 * t - 1) {
            splitChild(i, children[i]);
            if (k.key > keys[i].key)
                i++;
        }
        children[i]->insertNonFull(k);
    }
}

void BTreeNode::splitChild(int i, BTreeNode* y) {
    BTreeNode* z = new BTreeNode(y->t, y->leaf);

    for (int j = 0; j < t - 1; j++)
        z->keys.push_back(y->keys[j + t]);

    if (!y->leaf) {
        for (int j = 0; j < t; j++)
            z->children.push_back(y->children[j + t]);
    }

    Point midKey = y->keys[t - 1];

    y->keys.resize(t - 1);
    if (!y->leaf)
        y->children.resize(t);

    children.insert(children.begin() + i + 1, z);
    keys.insert(keys.begin() + i, midKey);
}

void BTreeNode::traverse() {
    int i;
    for (i = 0; i < keys.size(); i++) {
        if (!leaf)
            children[i]->traverse();

        cout << "Key: " << keys[i].key << " | Coords: ";
        for (auto v : keys[i].coords) cout << v << ", ";
        cout << " | Data: ";
        for (auto& d : keys[i].data) cout << d << " ";
        cout << endl;
    }

    if (!leaf)
        children[i]->traverse();
}

BTree::BTree(int t) {
    root = nullptr;
    this->t = t;
}

void BTree::insert(Point k) {
    if (!root) {
        root = new BTreeNode(t, true);
        root->keys.push_back(k);
    } else {
        if (root->keys.size() == 2 * t - 1) {
            BTreeNode* s = new BTreeNode(t, false);
            s->children.push_back(root);
            s->splitChild(0, root);

            int i = (k.key > s->keys[0].key) ? 1 : 0;
            s->children[i]->insertNonFull(k);
            root = s;
        } else {
            root->insertNonFull(k);
        }
    }
}

Point* BTree::search(uint64_t key) {
    return root ? root->search(key) : nullptr;
}

void BTree::traverse() {
    if (root) root->traverse();
}

BTreeNode::~BTreeNode() {
    for (auto child : children)
        delete child;
}

BTree::~BTree() {
    delete root;
}


uint64_t hilbertIndexND(const vector<uint32_t>& coords, int bits) {
    int n = coords.size();
    vector<uint32_t> x(coords);
    uint64_t index = 0;
    uint32_t mask = 1U << (bits - 1);

    for (int i = 0; i < bits; ++i) {
        uint32_t bitword = 0;
        for (int j = 0; j < n; ++j) {
            uint32_t bit = (x[j] & mask) ? 1 : 0;
            bitword |= (bit << (n - j - 1));
        }

        uint32_t gray = bitword ^ (bitword >> 1);
        index = (index << n) | gray;

        for (int j = 0; j < n; ++j) {
            if ((bitword >> (n - j - 1)) & 1) {
                x[j] ^= mask;
            }
        }

        mask >>= 1;
    }

    return index;
}
