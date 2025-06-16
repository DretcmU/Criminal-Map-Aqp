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

using namespace std;

// ===================
// ESTRUCTURA PRINCIPAL
// ===================
struct Dato {
    vector<double> columnas;
    uint64_t indice_h = 0;
    uint64_t key;
    int cluster_id = -1; // -1 = sin asignar, -2 = ruido
};

// ===================
// HILBERT ND
// ===================
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

// ===================
// LECTURA Y HILBERT
// ===================
// vector<Dato> leerColumnasDeCSVConHilbert(
//     const string& filename,
//     const vector<string>& columnasDeseadas,
//     int bitsHilbert = 16
// ) {
//     vector<Dato> datos;
//     ifstream archivo(filename);
//     string linea;

//     if (!archivo.is_open()) {
//         cerr << "No se pudo abrir el archivo: " << filename << endl;
//         return datos;
//     }

//     // Leer encabezado
//     getline(archivo, linea);
//     istringstream ssEncabezado(linea);
//     string columna;
//     unordered_map<string, int> indiceColumna;

//     int index = 0;
//     while (getline(ssEncabezado, columna, ',')) {
//         indiceColumna[columna] = index++;
//     }

//     vector<int> indicesSeleccionados;
//     for (const auto& nombre : columnasDeseadas) {
//         if (indiceColumna.find(nombre) != indiceColumna.end()) {
//             indicesSeleccionados.push_back(indiceColumna[nombre]);
//         } else {
//             cerr << "Columna no encontrada: " << nombre << endl;
//             return datos;
//         }
//     }

//     vector<vector<double>> columnasRaw(columnasDeseadas.size());
//     vector<vector<string>> filasCSV;

//     while (getline(archivo, linea)) {
//         istringstream ss(linea);
//         string valor;
//         vector<string> filaCompleta;

//         while (getline(ss, valor, ',')) {
//             filaCompleta.push_back(valor);
//         }

//         if (filaCompleta.size() > *max_element(indicesSeleccionados.begin(), indicesSeleccionados.end())) {
//             for (size_t i = 0; i < indicesSeleccionados.size(); ++i) {
//                 int idx = indicesSeleccionados[i];
//                 columnasRaw[i].push_back(stod(filaCompleta[idx]));
//             }
//             filasCSV.push_back(filaCompleta);
//         }
//     }

//     vector<double> minVals, maxVals;
//     for (const auto& col : columnasRaw) {
//         double minV = *min_element(col.begin(), col.end());
//         double maxV = *max_element(col.begin(), col.end());
//         minVals.push_back(minV);
//         maxVals.push_back(maxV);
//     }

//     int filanumN=0;
//     for (const auto& fila : filasCSV) {
//         if (stod(fila[1])==0.0 || stod(fila[2])==0.0) continue;
//         Dato dato;
//         vector<uint32_t> coords;
//         vector<uint32_t> pos;
//         int con = 0;

//         for (size_t i = 0; i < indicesSeleccionados.size(); ++i) {
//             int idx = indicesSeleccionados[i];
//             double val = stod(fila[idx]);

//             uint32_t norm = static_cast<uint32_t>(
//                 ((val - minVals[i]) / (maxVals[i] - minVals[i])) * ((1 << bitsHilbert) - 1)
//             );
//             //coords.push_back(norm);
//             if(con<2){
//                 cout<<minVals[0]<<" "<<maxVals[0]<<" "<<minVals[1]<<" "<<maxVals[1]<<endl;
//                 dato.columnas.push_back(val);
//                 // if (stod(fila[1])==0.0 || stod(fila[2])==0.0){
//                 //     cout<<"A"<<filanumN<<endl;
//                 //     cin>>con;
//                 // }
//                 pos.push_back(norm);
//                 con++;
//             }
//             else{
//                 coords.push_back(norm);
//             }
//             filanumN++;
//         }
//         dato.key = hilbertIndexND(pos, bitsHilbert);
//         dato.indice_h = hilbertIndexND(coords, bitsHilbert);
//         datos.push_back(dato);
//     }

//     return datos;
// }

void guardarMinMax(const string& filename, const vector<double>& minVals, const vector<double>& maxVals) {
    ofstream archivo(filename);
    if (!archivo.is_open()) {
        cerr << "No se pudo abrir el archivo para escribir: " << filename << endl;
        return;
    }

    archivo << minVals[0] << " " << minVals[1] << endl;
    archivo << maxVals[0] << " " << maxVals[1] << endl;

    archivo.close();
}

vector<Dato> leerColumnasDeCSVConHilbert(
    const string& filename,
    const vector<string>& columnasDeseadas,
    int bitsHilbert = 16
) {
    vector<Dato> datos;
    ifstream archivo(filename);
    string linea;

    if (!archivo.is_open()) {
        cerr << "No se pudo abrir el archivo: " << filename << endl;
        return datos;
    }

    // Leer encabezado
    getline(archivo, linea);
    istringstream ssEncabezado(linea);
    string columna;
    unordered_map<string, int> indiceColumna;
    int index = 0;

    while (getline(ssEncabezado, columna, ',')) {
        indiceColumna[columna] = index++;
    }

    // Verificar que todas las columnas deseadas existen
    vector<int> indicesSeleccionados;
    for (const auto& nombre : columnasDeseadas) {
        if (indiceColumna.find(nombre) != indiceColumna.end()) {
            indicesSeleccionados.push_back(indiceColumna[nombre]);
        } else {
            cerr << "Columna no encontrada: " << nombre << endl;
            return datos;
        }
    }

    // Leer todas las filas y filtrar válidas (columna 1 y 2 no deben ser 0.0 o vacías)
    vector<vector<string>> filasValidas;
    while (getline(archivo, linea)) {
        istringstream ss(linea);
        string valor;
        vector<string> filaCompleta;

        while (getline(ss, valor, ',')) {
            filaCompleta.push_back(valor);
        }

        // Validación: verificar que haya suficientes columnas
        if (filaCompleta.size() <= 2) continue;

        try {
            if (!filaCompleta[1].empty() && !filaCompleta[2].empty()) {
                double val1 = stod(filaCompleta[1]);
                double val2 = stod(filaCompleta[2]);

                if (val1 != 0.0 && val2 != 0.0) {
                    filasValidas.push_back(filaCompleta);
                }
            }
        } catch (...) {
            // Ignorar si no se puede convertir alguna columna
            continue;
        }
    }

    // Extraer valores numéricos para columnas deseadas
    vector<vector<double>> columnasRaw(columnasDeseadas.size());

    for (const auto& fila : filasValidas) {
        for (size_t i = 0; i < indicesSeleccionados.size(); ++i) {
            int idx = indicesSeleccionados[i];
            try {
                double val = stod(fila[idx]);
                columnasRaw[i].push_back(val);
            } catch (...) {
                columnasRaw[i].push_back(0.0);  // Si hay error, usa 0.0 (o podrías ignorar esa fila si prefieres)
            }
        }
    }

    // Calcular min y max para normalización
    vector<double> minVals, maxVals;
    for (const auto& col : columnasRaw) {
        double minV = *min_element(col.begin(), col.end());
        double maxV = *max_element(col.begin(), col.end());
        minVals.push_back(minV);
        maxVals.push_back(maxV);
    }

    int xdatoxd=0;
    // vector<double> min_norm = {39.9, -74.9};  // latitud mínima, longitud mínima
    // vector<double> max_norm = {41.1, -72.9};  // latitud máxima, longitud máxima
    // for(int i=0;i<2;i++){
    //     if(minVals[i]<min_norm[i]) minVals[i] = min_norm[i];
    //     if(maxVals[i]>max_norm[i]) maxVals[i] = max_norm[i];
    // }

    cout<<"Min: " <<minVals[0]<<" "<<minVals[1]<<endl;
    cout<<"Max: " <<maxVals[0]<<" "<<maxVals[1]<<endl;
    cout<<"mira: ";cin>>xdatoxd;

    guardarMinMax("minmax.txt", minVals, maxVals);

    // Construir objetos Dato
    for (size_t i = 0; i < filasValidas.size(); ++i) {
        const auto& fila = filasValidas[i];
        if(minVals[0]>stod(fila[1]) || stod(fila[1])>maxVals[0] || 
        minVals[1]>stod(fila[2]) || stod(fila[2])>maxVals[1]){
            continue;
        }
        Dato dato;
        vector<uint32_t> coords;
        vector<uint32_t> pos;

        for (size_t j = 0; j < indicesSeleccionados.size(); ++j) {
            int idx = indicesSeleccionados[j];
            double val = stod(fila[idx]);

            // Normalización
            uint32_t norm = static_cast<uint32_t>(
                ((val - minVals[j]) / (maxVals[j] - minVals[j])) * ((1 << bitsHilbert) - 1)
            );

            if (j < 2) {
                dato.columnas.push_back(val);
                pos.push_back(norm);
            } else {
                coords.push_back(norm);
            }
        }

        dato.key = hilbertIndexND(pos, bitsHilbert);
        dato.indice_h = hilbertIndexND(coords, bitsHilbert);
        datos.push_back(dato);
    }

    return datos;
}

// ===================
// DISTANCIA EUCLIDEANA
// ===================
double distancia(const Dato& a, const Dato& b) {
    double suma = 0;
    for (size_t i = 0; i < a.columnas.size(); ++i) {
        double diff = a.columnas[i] - b.columnas[i];
        suma += diff * diff;
    }
    return sqrt(suma);
}

// ===================
// EXPANSIÓN DE CLUSTER
// ===================
void expandirCluster(Dato* punto, vector<Dato*>& vecinos, int cluster_id, double eps, int minPts, vector<Dato*>& grupo) {
    punto->cluster_id = cluster_id;
    vector<Dato*> cola = vecinos;

    while (!cola.empty()) {
        Dato* actual = cola.back();
        cola.pop_back();

        if (actual->cluster_id == -1 || actual->cluster_id == -2) {
            actual->cluster_id = cluster_id;

            // Buscar vecinos de este punto
            vector<Dato*> vecinos2;
            for (Dato* otro : grupo) {
                if (distancia(*actual, *otro) <= eps) {
                    vecinos2.push_back(otro);
                }
            }

            if (vecinos2.size() >= minPts) {
                cola.insert(cola.end(), vecinos2.begin(), vecinos2.end());
            }
        }
    }
}

// ===================
// DBSCAN EN UN GRUPO (MÓDULO PRINCIPAL)
// ===================
void dbscan(vector<Dato*>& grupo, double eps, int minPts, int& nextClusterId) {
    for (Dato* punto : grupo) {
        if (punto->cluster_id != -1) continue;

        vector<Dato*> vecinos;
        for (Dato* otro : grupo) {
            if (distancia(*punto, *otro) <= eps) {
                vecinos.push_back(otro);
            }
        }

        if (vecinos.size() < minPts) {
            punto->cluster_id = -2; // ruido
            continue;
        }

        // Expansión del clúster
        expandirCluster(punto, vecinos, nextClusterId, eps, minPts, grupo);
        nextClusterId++;
    }
}


void guardarEnBinario(const vector<Dato>& datos, const vector<string>& columnas, const string& nombreArchivo) {
    ofstream outBin(nombreArchivo, ios::binary);
    if (!outBin.is_open()) {
        cerr << "No se pudo crear el archivo binario de salida: " << nombreArchivo << endl;
        return;
    }

    size_t n_columnas = 2;//columnas.size();
    size_t n_datos = datos.size();
    outBin.write(reinterpret_cast<const char*>(&n_datos), sizeof(size_t));
    outBin.write(reinterpret_cast<const char*>(&n_columnas), sizeof(size_t));

    for (const Dato& d : datos) {
        // if(d.columnas[0]==0.0){
        //     cout<<1000000<<endl;
        // }
        outBin.write(reinterpret_cast<const char*>(d.columnas.data()), sizeof(double) * n_columnas);
        outBin.write(reinterpret_cast<const char*>(&d.key), sizeof(uint64_t));
        outBin.write(reinterpret_cast<const char*>(&d.cluster_id), sizeof(int));
    }

    outBin.close();
    cout << "Archivo binario guardado como '" << nombreArchivo << "'\n";
}

vector<Dato> leerDesdeBinario(const string& nombreArchivo) {
    vector<Dato> datos;
    ifstream inBin(nombreArchivo, ios::binary);
    if (!inBin.is_open()) {
        cerr << "No se pudo abrir el archivo binario: " << nombreArchivo << endl;
        return datos;
    }

    size_t n_datos, n_columnas;
    inBin.read(reinterpret_cast<char*>(&n_datos), sizeof(size_t));
    inBin.read(reinterpret_cast<char*>(&n_columnas), sizeof(size_t));

    for (size_t i = 0; i < n_datos; ++i) {
        Dato d;
        if(d.columnas[0]==0.0){
            cout<<1000000<<endl;
        }
        d.columnas.resize(n_columnas);
        inBin.read(reinterpret_cast<char*>(d.columnas.data()), sizeof(double) * n_columnas);
        inBin.read(reinterpret_cast<char*>(&d.key), sizeof(uint64_t));
        inBin.read(reinterpret_cast<char*>(&d.cluster_id), sizeof(int));
        datos.push_back(d);
    }

    inBin.close();
    cout << "Archivo binario leído correctamente: " << nombreArchivo << "\n";
    return datos;
}

int main() {
    string archivoCSV = "processed_data_subset_500k.csv";
    vector<string> columnas = {
        "pickup_latitude", "pickup_longitude",
        "passenger_count", "trip_distance", "fare_amount",
        "extra", "mta_tax", "tip_amount", "tolls_amount",
        "improvement_surcharge", "total_amount"
    };
    int bitsHilbert = 16;
    int agrupamiento_por_bits = 2; // numero de digitios que restaremos al (indice hilbert x cantidad de columnas)

    vector<Dato> datos = leerColumnasDeCSVConHilbert(archivoCSV, columnas, bitsHilbert);
    int a;
    for(auto&d : datos)    {
    if(d.columnas[0]==0.0){
            cout<<1000000<<endl;
            cin>>a;
        }
        // for(auto&val : d.columnas)
        // cout<<val<<" ";
        // cout<<d.indice_h<<endl;
    }

    // Agrupar punteros por bits altos
    map<uint64_t, vector<Dato*>> gruposHilbert;

    for (Dato& dato : datos) {
        uint64_t grupo_id = dato.indice_h >> (bitsHilbert * (columnas.size()) - agrupamiento_por_bits);
        gruposHilbert[grupo_id].push_back(&dato);
    }

    // Aplicar DBSCAN a cada grupo
    int nextClusterId = 0, minPts=7;
    double epsilom = 0.2;
    for (auto it = gruposHilbert.begin(); it != gruposHilbert.end(); ++it) {
        uint64_t grupo_id = it->first;
        vector<Dato*>& grupo = it->second;
        dbscan(grupo, epsilom, minPts, nextClusterId);
    }

    // Mostrar resumen
    map<int, int> conteoClusters;
    for (const auto& d : datos) {
        conteoClusters[d.cluster_id]++;
    }

    long con=0;
    for (auto it = conteoClusters.begin(); it != conteoClusters.end(); ++it) {
        int id = it->first;
        int count = it->second;
        cout << "Cluster " << id << ": " << count << " puntos\n";
        con++;
    }
    //cout<<conteoClusters.size()<<endl;
    cout<<con<<endl;
    ofstream out("numGrups.txt");
    out << con<< endl;
    out.close();
    // for (size_t i = 0; i < 150 && i < datos.size(); ++i) {
    //     cout << "Fila " << i << " cluster = " << datos[i].cluster_id << endl;
    // }

    guardarEnBinario(datos,columnas, "BinDatos.bin");
    cout<<"size: "<<datos.size()<<endl;
    return 0;
}