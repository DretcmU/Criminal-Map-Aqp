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

void cargarMinMax(const string& filename, vector<double>& min_norm, vector<double>& max_norm) {
    ifstream archivo(filename);
    if (!archivo.is_open()) {
        cerr << "No se pudo abrir el archivo para leer: " << filename << endl;
        return;
    }

    min_norm.resize(2);
    max_norm.resize(2);

    archivo >> min_norm[0] >> min_norm[1];
    archivo >> max_norm[0] >> max_norm[1];

    archivo.close();
}

int main() {
    BTree* arbol = initHilbertTree("../preprocessing/BinDatos.bin",2);

    // cout << "Hilbert-BTree:\n";
    // arbol->traverse();

    vector<double> min_coord = {40.150, -74.2591}; // lat, lon
    vector<double> max_coord = {40.806, -74.2704}; // lat, lon
    
    int bits = 16;

    // vector<double> min_norm = {35.9426,-81.1019 };
    // vector<double> max_norm = {57.2693, -0.139907};

    vector<double> min_norm, max_norm;
    cargarMinMax("../preprocessing/minmax.txt", min_norm, max_norm);
    cout << "min_norm: " << min_norm[0] << ", " << min_norm[1] << endl;
    cout << "max_norm: " << max_norm[0] << ", " << max_norm[1] << endl;
    vector<Point> encontrados = buscarRangoHilbert(*arbol, min_coord, max_coord, bits, min_norm,max_norm); 

    return 0;
}
