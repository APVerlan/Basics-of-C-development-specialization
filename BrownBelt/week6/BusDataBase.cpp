#include "BusDataBase.h"


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

    const auto &settings = request.GetRoot().AsMap().at("routing_settings");
    bus_velocity_ = settings.AsMap().at("bus_velocity").AsInt();
    bus_wait_time_ = settings.AsMap().at("bus_wait_time").AsInt();

    ProcessBuildRequestJSON(request);
    ProcessLengths();

    size_t i = 0;
    for (auto &[key, val] : bus_stops_) {
        stop_to_id_[key] = i++;
    }
    for (auto &[key, val] : stop_to_id_) {
        id_to_stops_[val] = key;
    }
    CreateRouter();
}

DirectedWeightedGraph<BusDataBase::Time> BusDataBase::CreateGraph() {

    DirectedWeightedGraph<Time> G(bus_stops_.size());
    //std::unordered_map<std::size_t, std::unordered_map<std::size_t, size_t>> added_edges;

    for (auto &[num, bus_route] : bus_routes_) {
        // add  edges with span_count = 1

        for (size_t i = 1; i < bus_route.route.size(); ++i) {
            Time time = bus_wait_time_ +
                    ((60.0 / 1000.0) / bus_velocity_) * static_cast<double>((bus_stops_.at(bus_route.route[i - 1]).dists.count(bus_route.route[i]) > 0) ?
                                                                            bus_stops_.at(bus_route.route[i - 1]).dists.at(bus_route.route[i]) :
                                                                            bus_stops_.at(bus_route.route[i]).dists.at(bus_route.route[i - 1]));
            Edge<Time> edge = {stop_to_id_[bus_route.route[i - 1]],
                         stop_to_id_[bus_route.route[i]],
                      time,
                      1};

            /*if (added_edges[edge.from].count(edge.to) == 0) {
                added_edges[edge.from][edge.to] = G.AddEdge(edge);
                edge_to_bus[added_edges[edge.from][edge.to]].insert(num);
            } else {
                edge_to_bus[added_edges[edge.from][edge.to]].insert(num);
            }*/
            edge_to_bus[G.AddEdge(edge)].insert(num);

            if (bus_route.type == "direct") {
                time = bus_wait_time_ +
                        ((60.0 / 1000.0) / bus_velocity_) * static_cast<double>((bus_stops_.at(bus_route.route[i]).dists.count(bus_route.route[i - 1]) > 0) ?
                                                                                bus_stops_.at(bus_route.route[i]).dists.at(bus_route.route[i - 1]) :
                                                                                bus_stops_.at(bus_route.route[i - 1]).dists.at(bus_route.route[i]));
                Edge<Time> reverse_edge = {stop_to_id_[bus_route.route[i]],
                          stop_to_id_[bus_route.route[i - 1]],
                          time,
                          1};

                /*if (added_edges[reverse_edge.from].count(reverse_edge.to) == 0) {
                    added_edges[reverse_edge.from][reverse_edge.to] = G.AddEdge(reverse_edge);
                    edge_to_bus[added_edges[reverse_edge.from][reverse_edge.to]].insert(num);
                } else {
                    edge_to_bus[added_edges[reverse_edge.from][reverse_edge.to]].insert(num);
                }*/
                edge_to_bus[G.AddEdge(reverse_edge)].insert(num);
            }
        }

        // add  edges with span_count > 1
        for (size_t i = 1; i < bus_route.route.size(); ++i) {
            Time time_direct = bus_wait_time_, time_reverse = bus_wait_time_;

            for (size_t j = i; j < bus_route.route.size(); ++j) {
                time_direct += ((60.0 / 1000.0) / bus_velocity_) * static_cast<double>((bus_stops_.at(bus_route.route[j - 1]).dists.count(bus_route.route[j]) > 0) ?
                                                                                bus_stops_.at(bus_route.route[j - 1]).dists.at(bus_route.route[j]) :
                                                                                bus_stops_.at(bus_route.route[j]).dists.at(bus_route.route[j - 1]));
                Edge<Time> edge = {stop_to_id_[bus_route.route[i - 1]],
                                   stop_to_id_[bus_route.route[j]],
                                   time_direct,
                                   j - i + 1};

                //if (added_edges[edge.from].count(edge.to) == 0) {
                   // added_edges[edge.from][edge.to] = G.AddEdge(edge);
                   // edge_to_bus[added_edges[edge.from][edge.to]].insert(num);
                /*} else {
                    edge_to_bus[added_edges[edge.from][edge.to]].insert(num);
                }*/
                edge_to_bus[G.AddEdge(edge)].insert(num);

                if (bus_route.type == "direct") {
                    time_reverse += ((60.0 / 1000.0) / bus_velocity_) * static_cast<double>((bus_stops_.at(bus_route.route[j]).dists.count(bus_route.route[j - 1]) > 0) ?
                                                                                   bus_stops_.at(bus_route.route[j]).dists.at(bus_route.route[j - 1]) :
                                                                                   bus_stops_.at(bus_route.route[j - 1]).dists.at(bus_route.route[j]));
                    Edge<Time> reverse_edge = {stop_to_id_[bus_route.route[j]],
                                               stop_to_id_[bus_route.route[i - 1]],
                                               time_reverse,
                                               j - i + 1};

                    //if (added_edges[reverse_edge.from].count(reverse_edge.to) == 0) {
                        //added_edges[reverse_edge.from][reverse_edge.to] = G.AddEdge(reverse_edge);
                       // edge_to_bus[added_edges[reverse_edge.from][reverse_edge.to]].insert(num);
                    /*} else {
                        edge_to_bus[added_edges[reverse_edge.from][reverse_edge.to]].insert(num);
                    }*/
                    edge_to_bus[G.AddEdge(reverse_edge)].insert(num);
                }
            }
        }

    }

    return G;
}

void BusDataBase::CreateRouter() {
    router_.emplace(Router<Time>(CreateGraph()));
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

void    BusDataBase::ProcessStopBildRequestJSON(const std::map<std::string, Node> &request) {
    std::string name = request.at("name").AsString();

    bus_stops_[name].coord =
            {request.at("latitude").AsDouble(),
             request.at("longitude").AsDouble()};

    std::unordered_map<std::string, uint64_t> dist_map;
    auto &json_dist_map = request.at("road_distances").AsMap();

    for (auto &[key, val] : json_dist_map) {
        dist_map[key] = val.AsInt();
    }

    bus_stops_[name].dists = move(dist_map);
}

void    BusDataBase::ProcessBusBildRequestJSON(const std::map<std::string, Node> &request) {
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
        } else if (request.at("type").AsString() == "Bus") {
            ProcessBusBildRequestJSON(request);
        }
    }
}

void    BusDataBase::ProcessAnswBusRequestJSON(const std::map<std::string, Node> &request, std::ostream &stream_o) const {
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

void    BusDataBase::ProcessAnswStopRequestJSON(const std::map<std::string, Node> &request, std::ostream &stream_o) const {
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

std::optional<std::vector<std::size_t>>    BusDataBase::ComputingFastestRoute(size_t from, size_t to, Time &time) const {
    auto route_info = router_->BuildRoute(from, to);

    if (route_info.has_value()) {
        std::vector<size_t> route;

        for (size_t i = 0; i < route_info->edge_count; ++i) {
            route.push_back(router_->GetRouteEdge(route_info->id, i));
        }

        time = route_info->weight;
        return route;
    }
    return std::nullopt;
}

void    BusDataBase::PrintBusItem(std::string bus_num, size_t span_count, Time time, std::ostream &stream_o) const {
    stream_o << "{"
            << "\"span_count\": " << span_count << ", "
            << "\"type\": \"Bus\", "
            << "\"bus\": " << "\"" << bus_num << "\", "
            << "\"time\": " << time
            << "}";
}

void    BusDataBase::PrintWaitItem(std::string bus_stop, std::ostream &stream_o) const {
    stream_o << "{"
            << "\"type\": \"Wait\", "
            << "\"stop_name\": " << "\"" << bus_stop << "\", "
            << "\"time\": " << bus_wait_time_
            << "}, ";
}

std::string  BusDataBase::GetBestBus(std::vector<size_t> &route, size_t proc_edges) const {
    return *edge_to_bus.at(route[proc_edges]).begin();
    //size_t max = 1;

    /*for (auto &bus : buses) {
        size_t count = 0, i = proc_edges;

        while (i < route.size() && edge_to_bus.at(route[i]).count(bus)) {
            count++;
            i++;
        }

        if (max < count) {
            max = count;
            best_bus = bus;
        }
    }*/
}

void    BusDataBase::PrintItemsOfRoute(std::vector<size_t> &route, std::ostream &stream_o) const {
    size_t  proc_edges = 0;

    stream_o << "[";
    while (proc_edges < route.size()) {
        std::string bus_num = GetBestBus(route, proc_edges);;

        PrintWaitItem(id_to_stops_.at(router_->GetEdgeById(route[proc_edges]).from), stream_o);

        PrintBusItem(bus_num, router_->GetEdgeById(route[proc_edges]).span_count,
                     router_->GetEdgeById(route[proc_edges]).weight - bus_wait_time_, stream_o);

        proc_edges++;
        if (proc_edges < route.size()) {
            stream_o << ", ";
        }
    }
    stream_o << "]";
}

void    BusDataBase::ProcessAnswRouteRequestJSON(const std::map<std::string, Node> &request, std::ostream &stream_o) const {
    Time time = 0;
    std::optional<std::vector<size_t>> route = ComputingFastestRoute(stop_to_id_.at(request.at("from").AsString()),
                                                                     stop_to_id_.at(request.at("to").AsString()),
                                                                     time);

    stream_o << "\"request_id\": " << request.at("id").AsInt() << ", ";
    if (route != std::nullopt) {
        stream_o << "\"total_time\": " << time << ", "
        << "\"items\": ";
        PrintItemsOfRoute(route.value(), stream_o);
    } else {
        stream_o << "\"error_message\": \"not found\"";
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
        } else if (request.at("type").AsString() == "Route") {
            ProcessAnswRouteRequestJSON(request, stream_o);
        }
        stream_o << "}";
    }
    stream_o << "]";
}

/*
    * Function from old versions
    * Parth A, B, C
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
 */