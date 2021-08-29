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

class BusDataBase {
public:
    BusDataBase() = default;

    void    Build(std::istream& stream_i);
    void    ProcessBuildRequest(std::unique_ptr<Request> request);
    void    ProcessAnswRequest(std::unique_ptr<Request> request, std::ostream &stream_o = std::cout) const;
    void    AnswerRequests(std::istream &stream_i);

private:
    std::unordered_map<std::string, BusRoute>   bus_routes_;
    std::unordered_map<std::string, StopData>   bus_stops_;
    void    ProcessLengths();
};


#endif  // MY_PROJECT_BUSDATABASE_H
