#include "BTree.hpp"
#include <iostream>

using namespace std;

int main() {
    int dimensions = 3;
    int bits = 5;
    BTree tree(2);

    vector<vector<uint32_t>> puntos = {
        {1, 2, 3},
        {4, 4, 4},
        {0, 0, 1},
        {7, 1, 3},
        {15, 15, 15}
    };
    cout << "Hilbert-BTree:\n";

    for (const auto& coords : puntos) {
        uint64_t h = hilbertIndexND(coords, bits);
        cout<<h<<endl;
        tree.insert(Point(h, coords, {"punto"}));
    }

    cout << "Hilbert-BTree:\n";
    tree.traverse();

    vector<uint32_t> buscar = {4, 3, 4};
    uint64_t hbuscar = hilbertIndexND(buscar, bits);

    Point* encontrado = tree.search(hbuscar);
    if (encontrado) {
        cout << "\nEncontrado: HilbertKey " << encontrado->key << " -> Coords: ";
        for (auto v : encontrado->coords) cout << v << " ";
        cout << endl;
    } else {
        cout << "\nNo encontrado.\n";
    }

    return 0;
}
