#include <iostream>
#include <vector>
#include <cmath>
#include <cstdint>

using namespace std;

// Convierte un vector de coordenadas a un índice de Hilbert
uint64_t hilbertIndexND(const vector<uint32_t>& coords, int bits) {
    int n = coords.size();
    vector<uint32_t> x(coords);
    vector<uint32_t> M(n);
    uint64_t index = 0;

    uint32_t mask = 1U << (bits - 1);

    for (int i = 0; i < bits; ++i) {
        uint32_t bitword = 0;
        for (int j = 0; j < n; ++j) {
            uint32_t bit = (x[j] & mask) ? 1 : 0;
            bitword |= (bit << (n - j - 1));
        }

        // Gray code
        uint32_t gray = bitword ^ (bitword >> 1);
        index = (index << n) | gray;

        // Update the coordinates
        uint32_t t = bitword;
        for (int j = 0; j < n; ++j) {
            if ((t >> (n - j - 1)) & 1) {
                x[j] ^= mask; // Flip bit
            }
        }

        mask >>= 1;
    }

    return index;
}

int main() {
    int dimensions = 3;
    int bits = 5; // Cada coordenada debe estar en [0, 31]

    vector<uint32_t> coords(dimensions, 0);
    coords[0] = 7;
    coords[1] = 12;
    coords[2] = 15;
    // ... Puedes asignar más coordenadas según tu caso

    uint64_t index = hilbertIndexND(coords, bits);
    cout << "Hilbert index (30D): " << index << endl;

    return 0;
}
