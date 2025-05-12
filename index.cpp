#define CPPHTTPLIB_NO_UNIX_SOCKETS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "templates/httplib.h"
#include "templates/json.hpp"
#include <iostream>

using json = nlohmann::json;

int main() {
    httplib::Server svr;

    svr.Post("/consultar", [](const httplib::Request& req, httplib::Response& res) {
        try {
            json recibido = json::parse(req.body);
            double minLat = recibido["minLat"];
            double maxLat = recibido["maxLat"];
            double minLon = recibido["minLon"];
            double maxLon = recibido["maxLon"];

            std::cout << "Recibido: [" << minLat << ", " << maxLat << "] x ["
                      << minLon << ", " << maxLon << "]\\n";

            json respuesta = json::array();
            respuesta.push_back({{"lat", minLat + 0.001}, {"lon", minLon + 0.001}});
            respuesta.push_back({{"lat", maxLat - 0.001}, {"lon", maxLon - 0.001}});

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
