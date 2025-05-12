#ifndef BTREE_HPP
#define BTREE_HPP

#include <vector>
#include <string>
#include <cstdint>

using namespace std;

struct Point {
    uint64_t key;                 // Índice de Hilbert
    vector<uint32_t> coords;     // Coordenadas originales
    vector<string> data;         // Datos asociados

    Point() : key(0), coords(), data() {}
    Point(uint64_t k, vector<uint32_t> c, vector<string> d)
        : key(k), coords(c), data(d) {}

    bool operator<(const Point& other) const {
        return key < other.key;
    }

    bool operator==(const Point& other) const {
        return key == other.key;
    }
};

class BTreeNode {
public:
    vector<Point> keys;
    vector<BTreeNode*> children;
    bool leaf;
    int t;

    BTreeNode(int t, bool leaf);
    Point* search(uint64_t key);
    void insertNonFull(Point k);
    void splitChild(int i, BTreeNode* y);
    void traverse();
    ~BTreeNode();
};

class BTree {
public:
    BTreeNode* root;
    int t;

    BTree(int t);
    void insert(Point k);
    Point* search(uint64_t key);
    void traverse();
    ~BTree();
};

uint64_t hilbertIndexND(const vector<uint32_t>& coords, int bits);

#endif // BTREE_HPP
