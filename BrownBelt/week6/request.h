#ifndef MY_PROJECT_REQUEST_H
#define MY_PROJECT_REQUEST_H

#include <string>
#include <unordered_set>
#include <vector>

enum class RequestType {
    AddRoute,
    AddStop,
    PrintRouteStats,
    PrintStopStats
};

struct StopData {
    Point coord;
    std::set<std::string> buses;
    std::unordered_map<std::string, uint64_t> dists;
};

struct BusRoute {
    std::string type;
    std::vector<std::string> route;
    std::unordered_set<std::string> uniq_stops;
    unsigned long long length = 0;
    double geo_length = 0.;
};

struct Request {
    RequestType type;

    virtual void            *GetData() = 0;
    virtual std::string     &GetName() = 0;
};

struct AddStop : public Request {
    StopData data;
    std::string name;

    void            *GetData() { return static_cast<void *>(&data); };
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

struct PrintStopStats : public Request {
    std::string name;

    std::string     &GetName() { return name; };
    void            *GetData() { return nullptr; };
};

#endif  // MY_PROJECT_REQUEST_H
