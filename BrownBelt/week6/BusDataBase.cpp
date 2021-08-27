//
// Created by apisa on 27.08.2021.
//

#include "BusDataBase.h"

void    BusDataBase::ProcessBuildRequest(std::unique_ptr<Request> request) {
    if (request->type == RequestType::AddRoute) {
        bus_routes_[request->GetBusNum()] = std::move(request->GetRoute());
    } else if (request->type == RequestType::AddStop) {
        bus_stops_[request->GetName()] = request->GetCoord();
    }
}

void    BusDataBase::Build(std::istream &stream_i) {
    size_t n;
    std::string line;

    stream_i >> n;
    stream_i.get();

    for (size_t i = 0; i < n; ++i) {
        std::getline(stream_i, line);
        ProcessBuildRequest(ParseBuildRequest(line));
    }

    ProcessLengths();
}

void    BusDataBase::ProcessAnswRequest(std::unique_ptr<Request> request, std::ostream &stream_o) {
    stream_o << "Bus " << request->GetBusNum() << ": ";
    if (bus_routes_.count(request->GetBusNum()) > 0) {
        auto bus_route = bus_routes_[request->GetBusNum()];

        if (bus_route.type == "circle") {
            stream_o << bus_route.route.size();
        } else {
            stream_o << 2 * bus_route.route.size() - 1;
        }


        stream_o << " stops on route, " << bus_route.uniq_stops.size() << " unique stops, "
                 << bus_route.length << " route length\n";

    } else {
        stream_o << "not found\n";
    }
}

void BusDataBase::ProcessLengths() {
    for (auto& [bus_num, bus_route] : bus_routes_) {
        bus_route.length = 0;
        for (size_t i = 1; i < bus_route.route.size(); ++i) {
            bus_route.length +=
                GetDistance(bus_stops_[bus_route.route[i - 1]], bus_stops_[bus_route.route[i]]);
        }
        if (bus_route.type == "circle") {
            bus_route.length += GetDistance(bus_stops_[bus_route.route.front()],
                                            bus_stops_[bus_route.route.back()]);
        } else if (bus_route.type == "direct") {
            bus_route.length += bus_route.length;
        }
    }
}

void BusDataBase::AnswerRequests(std::istream &stream_i) {
    size_t n;
    std::string line;

    stream_i >> n;
    stream_i.get();

    for (size_t i = 0; i < n && std::getline(stream_i, line); ++i) {
        ProcessAnswRequest(ParseAnswRequest(line));
    }
}