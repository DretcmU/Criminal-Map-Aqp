// #define CPPHTTPLIB_NO_UNIX_SOCKETS
// #define WIN32_LEAN_AND_MEAN
// #include <Windows.h>
#include "templates/httplib.h"
#include "templates/json.hpp"
#include <iostream>
#include <random>

using json = nlohmann::json;

int main() {
    std::cout<<"A";
    httplib::Server svr;
    
    svr.Options("/consultar", [](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "POST, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        res.status = 204; // No Content
    });
    
    svr.Post("/consultar", [](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
    
        try {
            json recibido = json::parse(req.body);
            double minLat = recibido["minLat"];
            double maxLat = recibido["maxLat"];
            double minLon = recibido["minLon"];
            double maxLon = recibido["maxLon"];
    
            std::cout << "Recibido: [" << minLat << ", " << maxLat << "] x ["
                      << minLon << ", " << maxLon << "]\n";
    
            // Generadores aleatorios
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> count_dist(3, 7);
            std::uniform_real_distribution<> lat_dist(minLat, maxLat);
            std::uniform_real_distribution<> lon_dist(minLon, maxLon);
            std::uniform_int_distribution<> color_dist(0, 2);
    
            int count = count_dist(gen);
            json respuesta = json::array();
    
            for (int i = 0; i < count; ++i) {
                double lat = lat_dist(gen);
                double lon = lon_dist(gen);
                int color = color_dist(gen);
    
                respuesta.push_back({
                    {"lat", lat},
                    {"lon", lon},
                    {"color", color}
                });
            }
    
            res.set_content(respuesta.dump(), "application/json");
        } catch (const std::exception& e) {
            res.status = 400;
            res.set_content(std::string("Error: ") + e.what(), "text/plain");
        }
    });

    std::cout << "Servidor en http://localhost:8080\\n";
    svr.listen("0.0.0.0", 8080);
    return 0;
}
