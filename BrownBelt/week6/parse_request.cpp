/*
 *  This code for old versions
 *  Parth A, B, C
 *
#include "BusDataBase.h"

std::string PreprocessString(std::string str) {
    size_t i = 0;
    while (std::isspace(str.back())) {
        str.pop_back();
    }
    while (std::isspace(str[i])) {
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
        std::string num;

        request.type = RequestType::PrintRouteStats;

        std::getline(stream_i, num, ':');
        request.bus_num = PreprocessString(num);
        return std::make_unique<PrintRouteStats>(request);
    } else if (type == "Stop") {
        PrintStopStats request;
        std::string name;

        request.type = RequestType::PrintStopStats;

        std::getline(stream_i, name, ':');
        request.name = PreprocessString(name);
        return std::make_unique<PrintStopStats>(request);
    } else return nullptr;
}

std::unique_ptr<Request>    ParseBuildRequest(std::string& str) {
    std::string type;
    std::istringstream stream_i(str);
    stream_i >> type;

    if (type == "Stop") {
        AddStop request;
        request.type = RequestType::AddStop;

        std::string name, data;

        std::getline(stream_i, name, ':');

        request.name = PreprocessString(name);

        std::getline(stream_i, data, ',');
        data = PreprocessString(data);
        request.data.coord.latitude = std::stod(data);

        std::getline(stream_i, data, ',');
        data = PreprocessString(data);
        request.data.coord.longitude = std::stod(data);

        if (!stream_i.str().empty()) {
            std::string stop;
            while(std::getline(stream_i, data, 'm')) {
                stream_i >> stop;
                std::getline(stream_i, stop, ',');
                request.data.dists[PreprocessString(stop)] = std::stoi(PreprocessString(data));
            }
        }

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
    } else {
        return nullptr;
    }
}
 */