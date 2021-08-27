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

    virtual void            *GetData() = 0;
    virtual std::string     &GetName() = 0;
};

struct AddStop : public Request {
    Point coord;
    std::string name;

    void            *GetData() { return static_cast<void *>(&coord); };
    std::string     &GetName() { return name; };
};

struct AddRoute : public Request {
    BusRoute route;
    std::string bus_num;

    void            *GetData() { return static_cast<void *>(&route); };
    std::string     &GetName() { return bus_num; };
};

struct PrintRouteStats : public Request {
    std::string bus_num;

    std::string     &GetName() { return bus_num; };
    void            *GetData() { return nullptr; };
};

#endif  // MY_PROJECT_REQUEST_H
