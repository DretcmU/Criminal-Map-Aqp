#include "BTree.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <map>
#include <string>
#include <cmath>
#include <limits>
#include <algorithm>
#include <cstdint>
#include <set>
#include <functional>

using namespace std;

int main() {
    BTree* arbol = initHilbertTree("../preprocessing/BinDatos.bin",2);

    // cout << "Hilbert-BTree:\n";
    // arbol->traverse();

    vector<double> min_coord = {40.4774, -74.2591}; // lat, lon
    vector<double> max_coord = {44.5176, -72.7004}; // lat, lon
    
    int bits = 16;

    vector<Point> encontrados = buscarRangoHilbert(*arbol, min_coord, max_coord, bits); 

    return 0;
}
