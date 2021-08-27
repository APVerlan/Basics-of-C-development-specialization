//
// Created by apisa on 27.08.2021.
//

#ifndef MY_PROJECT_REQUEST_H
#define MY_PROJECT_REQUEST_H

#include <string>
#include <unordered_set>
#include <vector>

enum class RequestType {
    AddRoute,
    AddStop,
    PrintRouteStats
};

struct BusRoute {
    std::string type;
    std::vector<std::string> route;
    std::unordered_set<std::string> uniq_stops;
    double length = 0.;
};

struct Request {
    RequestType type;

    virtual Point&              GetCoord()  = 0;
    virtual std::string&        GetName() = 0;
    virtual std::string&        GetBusNum() = 0;
    virtual BusRoute&           GetRoute() = 0;
};

struct AddStop : public Request {
    Point coord;
    std::string name;

    Point&          GetCoord() { return coord; };
    std::string&    GetName() { return name; };
    std::string&    GetBusNum() {};
    BusRoute&       GetRoute() {};
};

struct AddRoute : public Request {
    BusRoute route;
    std::string bus_num;

    BusRoute&       GetRoute() { return route; };
    std::string&    GetBusNum() { return bus_num; };
    Point&          GetCoord() {};
    std::string&    GetName() {};
};

struct PrintRouteStats : public Request {
    std::string bus_num;

    std::string&    GetBusNum() { return bus_num; };
    Point&          GetCoord() {};
    std::string&    GetName() {};
    BusRoute&       GetRoute() {};
};

#endif  // MY_PROJECT_REQUEST_H
