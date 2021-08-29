#include "BusDataBase.h"
#include "parse_request.h"

void    BusDataBase::ProcessBuildRequest(std::unique_ptr<Request> request) {
    if (request->type == RequestType::AddRoute) {
        bus_routes_[request->GetName()] = std::move(*static_cast<BusRoute *>(request->GetData()));
        for (auto& item : bus_routes_[request->GetName()].route) {
            bus_stops_[item].buses.insert(request->GetName());
        }
    } else if (request->type == RequestType::AddStop) {
        bus_stops_[request->GetName()] = *static_cast<StopData *>(request->GetData());
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

void    BusDataBase::ProcessAnswRequest(std::unique_ptr<Request> request, std::ostream &stream_o) const {
    if (request->type == RequestType::PrintRouteStats) {
        stream_o << "Bus " << request->GetName() << ": ";
        if (bus_routes_.count(request->GetName()) != 0) {
            auto bus_route = bus_routes_.at(request->GetName());

            if (bus_route.type == "circle") {
                stream_o << bus_route.route.size();
            } else {
                stream_o << 2 * bus_route.route.size() - 1;
            }

            stream_o << " stops on route, " << bus_route.uniq_stops.size() << " unique stops, "
                     << bus_route.length << " route length, " <<  bus_route.length / bus_route.geo_length << " curvature\n";

        } else {
            stream_o << "not found\n";
        }
    } else  if (request->type == RequestType::PrintStopStats) {
        stream_o << "Stop " << request->GetName() << ": ";
        if (bus_stops_.count(request->GetName()) != 0) {
            if (bus_stops_.at(request->GetName()).buses.empty()) {
                stream_o << "not buses\n";
            } else {
                stream_o << "buses";
                for (auto& item : bus_stops_.at(request->GetName()).buses) {
                    stream_o << " " << item;
                }
            }
        }
        else {
            stream_o << "not found\n";
        }
    }
}

void BusDataBase::ProcessLengths() {
    for (auto& [bus_num, bus_route] : bus_routes_) {
        for (size_t i = 1; i < bus_route.route.size(); ++i) {
            bus_route.geo_length +=
                GetDistance(bus_stops_.at(bus_route.route[i - 1]).coord,
                            bus_stops_.at(bus_route.route[i]).coord);
            bus_route.length +=
                    (bus_stops_.at(bus_route.route[i - 1]).dists.count(bus_route.route[i]) ?
                    bus_stops_.at(bus_route.route[i - 1]).dists.at(bus_route.route[i]) :
                     bus_stops_.at(bus_route.route[i]).dists.at(bus_route.route[i - 1]));
        }

        if (bus_route.type == "direct") {
            bus_route.length += bus_route.length;
            bus_route.geo_length += bus_route.geo_length;
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