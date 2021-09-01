#include "BusDataBase.h"
#include "parse_request.h"

void    BusDataBase::ProcessBuildRequest(std::unique_ptr<Request> request) {
    if (request->type == RequestType::AddRoute) {
        bus_routes_[request->GetName()] = std::move(*static_cast<BusRoute *>(request->GetData()));
        for (auto& item : bus_routes_[request->GetName()].route) {
            bus_stops_[item].buses.insert(request->GetName());
        }
    } else if (request->type == RequestType::AddStop) {
        bus_stops_[request->GetName()].coord = static_cast<StopData *>(request->GetData())->coord;
        bus_stops_[request->GetName()].dists = static_cast<StopData *>(request->GetData())->dists;
    }
}

void    BusDataBase::Build(const Document &request) {
    /* Old version
     *
     * size_t n;
     * std::string line;
     *
     * stream_i >> n;
     * stream_i.get();
     *
     * for (size_t i = 0; i < n; ++i) {
     *      std::getline(stream_i, line);
     *      ProcessBuildRequest(ParseBuildRequest(line));
     *  }
     *
     *  ProcessLengths();
     *  */
    ProcessBuildRequestJSON(request);
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
                stream_o << "no buses\n";
            } else {
                stream_o << "buses";
                for (auto& item : bus_stops_.at(request->GetName()).buses) {
                    stream_o << " " << item;
                }
                stream_o << std::endl;
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
                    ((bus_stops_.at(bus_route.route[i - 1]).dists.count(bus_route.route[i]) > 0) ?
                    bus_stops_.at(bus_route.route[i - 1]).dists.at(bus_route.route[i]) :
                     bus_stops_.at(bus_route.route[i]).dists.at(bus_route.route[i - 1]));
        }

        if (bus_route.type == "direct") {
            bus_route.geo_length += bus_route.geo_length;
            for (size_t i = 1; i < bus_route.route.size(); ++i) {
                bus_route.length += ((bus_stops_.at(bus_route.route[i]).dists.count(bus_route.route[i - 1]) > 0) ?
                                     bus_stops_.at(bus_route.route[i]).dists.at(bus_route.route[i - 1]) :
                                     bus_stops_.at(bus_route.route[i - 1]).dists.at(bus_route.route[i]));
            }
        }
    }
}

void BusDataBase::AnswerRequests(const Document &request) {
   /* Old version
    *
    * size_t n;
    * std::string line;
    *
    * stream_i >> n;
    * stream_i.get();
    *
    * for (size_t i = 0; i < n && std::getline(stream_i, line); ++i) {
    * ProcessAnswRequest(ParseAnswRequest(line));
    * }
    */

    ProcessAnswRequestJSON(request);
}

void    BusDataBase::ProcessStopBildRequestJSON(const std::map<std::string, Node> request) {
    std::string name = request.at("name").AsString();

    bus_stops_[name].coord =
            {request.at("latitude").AsDouble(),
             request.at("longitude").AsDouble()};

    std::unordered_map<std::string, int> dist_map;
    auto &json_dist_map = request.at("road_distances").AsMap();

    for (auto &[key, val] : json_dist_map) {
        dist_map[key] = val.AsInt();
    }

    bus_stops_[name].dists = move(dist_map);
}

void    BusDataBase::ProcessBusBildRequestJSON(const std::map<std::string, Node> request) {
    std::string name = request.at("name").AsString();

    for( auto &item : request.at("stops").AsArray()) {
        bus_routes_[name].route.push_back(item.AsString());
    }
    bus_routes_[name].type = request.at("is_roundtrip").AsBool() ? "circle" : "direct";
    bus_routes_[name].uniq_stops.insert(bus_routes_[name].route.begin(),
                                        bus_routes_[name].route.end()
                                        );

    for (auto& item : bus_routes_[name].route) {
        bus_stops_[item].buses.insert(name);
    }
}

void    BusDataBase::ProcessBuildRequestJSON(const Document &js) {
    auto &base_requests = js.GetRoot().AsMap().at("base_requests").AsArray();

    for (auto &node: base_requests) {
        auto &request = node.AsMap();

        if (request.at("type").AsString() == "Stop") {
            ProcessStopBildRequestJSON(request);
        } else if ("Bus") {
            ProcessBusBildRequestJSON(request);
        }
    }
}

void    BusDataBase::ProcessAnswBusRequestJSON(const std::map<std::string, Node> request, std::ostream &stream_o) const {
    std::string name = request.at("name").AsString();

    if (bus_routes_.count(name) != 0) {
        stream_o << "\"request_id\": " << request.at("id").AsInt() << ", "
                 << "\"stop_count\": " << ((bus_routes_.at(name).type == "circle") ? bus_routes_.at(name).route.size() : 2 * bus_routes_.at(name).route.size() - 1) << ", "
                 << "\"unique_stop_count\": " << bus_routes_.at(name).uniq_stops.size() << ", "
                 << "\"route_length\": " << bus_routes_.at(name).length << ", "
                 << "\"curvature\": " << bus_routes_.at(name).length / bus_routes_.at(name).geo_length;
    } else {
        stream_o << "\"request_id\": " << request.at("id").AsInt() << ", "
                 << "\"error_message\": \"not found\"";
    }
}

void    BusDataBase::ProcessAnswStopRequestJSON(const std::map<std::string, Node> request, std::ostream &stream_o) const {
    std::string name = request.at("name").AsString();

    if (bus_stops_.count(name) != 0) {
        stream_o << "\"request_id\": " << request.at("id").AsInt() << ", ";
        stream_o << "\"buses\": ";
        if (bus_stops_.at(name).buses.empty()) {
            stream_o << "[]";
        } else {
            stream_o << "[";

            for (auto &item: bus_stops_.at(name).buses) {
                stream_o << "\"" << item << "\"";
                if (item != *(std::prev(bus_stops_.at(name).buses.end())))
                    stream_o << ", ";
            }
            stream_o << "]";
        }
    } else {
        stream_o << "\"request_id\": " << request.at("id").AsInt() << ", "
                 << "\"error_message\": \"not found\"";
    }
}

void    BusDataBase::ProcessAnswRequestJSON(const Document &json, std::ostream &stream_o) const {
    stream_o << "[";

    for (size_t i = 0; i < json.GetRoot().AsMap().at("stat_requests").AsArray().size(); ++i) {
        auto &request = json.GetRoot().AsMap().at("stat_requests").AsArray()[i].AsMap();

        if (i != 0)
            stream_o << ", ";
        stream_o << "{";
        if (request.at("type").AsString() == "Bus") {
            ProcessAnswBusRequestJSON(request, stream_o);
        } else if (request.at("type").AsString() == "Stop") {
            ProcessAnswStopRequestJSON(request, stream_o);
        }
        stream_o << "}";
    }
    stream_o << "]";
}