#ifndef MY_PROJECT_BUSDATABASE_H
#define MY_PROJECT_BUSDATABASE_H

#include <iostream>
#include <memory>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <string>
#include <string_view>
#include <sstream>

#include "geometry.h"
#include "request.h"
#include "json.h"
#include "router.h"

using namespace Json;
using namespace Graph;

class BusDataBase {
public:
    BusDataBase() = default;

    void    Build(const Document &request);
    void    AnswerRequests(const Document &request);
    void    ProcessBuildRequestJSON(const Document &request);
    void    ProcessAnswRequestJSON(const Document &request, std::ostream &stream_o = std::cout) const;

    /*
    * Function from old versions
    * Parth A, B, C
     *
    void    ProcessBuildRequest(std::unique_ptr<Request> request);
    void    ProcessAnswRequest(std::unique_ptr<Request> request, std::ostream &stream_o = std::cout) const;
    */



private:
    using Time = size_t;
    using Edge = Edge<size_t>;

    std::unordered_map<std::string, BusRoute>   bus_routes_;
    std::unordered_map<std::string, StopData>   bus_stops_;
    std::unordered_map<std::string, size_t>     stop_to_id_;
    std::unordered_map<size_t, std::string>     id_to_stop_;

    std::unordered_map<size_t, std::unordered_set<std::string>>     edge_to_bus;

    std::optional<Router<Time>>    router_ = std::nullopt;

    size_t bus_wait_time_ = 6;
    size_t bus_velocity_ = 40;

    void    ProcessLengths();
    void    CreateRouter();
    DirectedWeightedGraph<BusDataBase::Time>    CreateGraph();

    void    ProcessAnswBusRequestJSON(const std::map<std::string, Node> &request, std::ostream &stream_o) const;
    void    ProcessAnswStopRequestJSON(const std::map<std::string, Node> &request, std::ostream &stream_o) const;
    void    ProcessAnswRouteRequestJSON(const std::map<std::string, Node> &request, std::ostream &stream_o) const;
    void    ProcessStopBildRequestJSON(const std::map<std::string, Node> &request);
    void    ProcessBusBildRequestJSON(const std::map<std::string, Node> &request);
    std::vector<std::string>    ComputingFastestRoute(size_t from, size_t to, size_t &time) const;

    using Graph = DirectedWeightedGraph<Time>;
};


#endif  // MY_PROJECT_BUSDATABASE_H
