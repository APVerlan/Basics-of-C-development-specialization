#include "BusDataBase.h"

std::string PreprocessString(std::string str) {
    size_t i = 0;
    while (str.back() == ' ') {
        str.pop_back();
    }
    while (str[i] == ' ') {
        i++;
    }
    return str.substr(i);
}

std::unique_ptr<Request>    ParseAnswRequest(std::string& str) {
    std::string type;
    std::istringstream stream_i(str);
    stream_i >> type;

    if (type == "Bus") {
        PrintRouteStats request;
        request.type = RequestType::PrintRouteStats;

        std::string stop, num;

        std::getline(stream_i, num, ':');
        request.bus_num = PreprocessString(num);
        return std::make_unique<PrintRouteStats>(request);
    } else return nullptr;
}

std::unique_ptr<Request>    ParseBuildRequest(std::string& str) {
    std::string type;
    std::istringstream stream_i(str);
    stream_i >> type;

    if (type == "Stop") {
        AddStop request;
        request.type = RequestType::AddStop;

        std::string name, crd;

        std::getline(stream_i, name, ':');

        request.name = PreprocessString(name);

        std::getline(stream_i, crd, ',');
        crd = PreprocessString(crd);
        request.coord.latitude = std::stod(crd);

        stream_i >> crd;
        crd = PreprocessString(crd);
        request.coord.longitude = std::stod(crd);
        return std::make_unique<AddStop>(request);
    } else if (type == "Bus") {
        AddRoute request;
        request.type = RequestType::AddRoute;

        std::string stop, num;

        std::getline(stream_i, num, ':');
        request.bus_num = PreprocessString(num);

        char c;
        std::string line = stream_i.str();
        if (std::find(line.begin(), line.end(), '>') != line.end()) {
            request.route.type = "circle";
            c = '>';
        } else {
            request.route.type = "direct";
            c = '-';
        }

        for (size_t i = 0; std::getline(stream_i, stop, c); ++i) {
            stop = std::move(PreprocessString(stop));

            request.route.route.push_back(stop);
            request.route.uniq_stops.insert(stop);
        }
        return std::make_unique<AddRoute>(request);
    } else return nullptr;
}