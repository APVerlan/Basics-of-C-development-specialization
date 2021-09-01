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

using namespace Json;

class BusDataBase {
public:
    BusDataBase() = default;

    void    Build(const Document &request);
    void    ProcessBuildRequest(std::unique_ptr<Request> request);
    void    ProcessAnswRequest(std::unique_ptr<Request> request, std::ostream &stream_o = std::cout) const;
    void    AnswerRequests(const Document &request);
    void    ProcessBuildRequestJSON(const Document &request);
    void    ProcessAnswRequestJSON(const Document &request, std::ostream &stream_o = std::cout) const;

private:
    std::unordered_map<std::string, BusRoute>   bus_routes_;
    std::unordered_map<std::string, StopData>   bus_stops_;

    void    ProcessLengths();
    void    ProcessStopBildRequestJSON(const std::map<std::string, Node> &request);
    void    ProcessBusBildRequestJSON(const std::map<std::string, Node> &request);
    void    ProcessAnswBusRequestJSON(const std::map<std::string, Node> &request, std::ostream &stream_o) const;
    void    ProcessAnswStopRequestJSON(const std::map<std::string, Node> &request, std::ostream &stream_o) const;
};


#endif  // MY_PROJECT_BUSDATABASE_H
