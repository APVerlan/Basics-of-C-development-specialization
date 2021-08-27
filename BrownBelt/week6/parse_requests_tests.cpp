#include "test_runner.h"
#include "BusDataBase.h"
#include "parse_request.h"

void TestPrepStr() {
    std::string str1 = "    Lol ", str2 = "     13214   ",
                str3 = " 123 ", str4 = " 1", str5 = "23";

    ASSERT(PreprocessString(str1) == "Lol");
    ASSERT(PreprocessString(str2) == "13214");
    ASSERT(PreprocessString(str3) == "123");
    ASSERT(PreprocessString(str4) == "1");
    ASSERT(PreprocessString(str5) == "23");
}

void TestParseAnsw() {
    std::vector<std::string> lines {"Bus 150:",
                                         "Bus ALFp 450 N    :",
                                         "Bus JKJHgsfd asfe_sfdsg :"};
    std::vector<std::string> correct = {"150", "ALFp 450 N", "JKJHgsfd asfe_sfdsg"};

    for (size_t i = 0; i < correct.size(); ++i) {
        std::unique_ptr<Request> request = ParseAnswRequest(lines[i]);
        ASSERT(request->type == RequestType::PrintRouteStats);
        ASSERT(request->GetName() == correct[i]);
    }
}

void TestParseBuildStop() {
    std::vector<std::string> lines_stop {"Stop Rasskazovka    : 55.632761   ,37.333324  ",
                                        "Stop   Biryulyovo Zapadnoye   :  55.574371   , 37.6517",
                                        "Stop   Biryusinka:   55.581065  , 37.64839",
                                        "Stop        Universam     :    55.587655  , 37.645687",
                                        "Stop Biryulyovo Tovarnaya: 55.592028 , 37.653656  ",
                                        "Stop Biryulyovo Passazhirskaya: 55.580999  , 37.659164  "};
    std::vector<std::string> correct_name = {"Rasskazovka", "Biryulyovo Zapadnoye", "Biryusinka",
                                             "Universam", "Biryulyovo Tovarnaya", "Biryulyovo Passazhirskaya"};
    std::vector<Point> correct_pt = {{55.632761, 37.333324}, {55.574371, 37.6517},
                                     {55.581065, 37.64839}, {55.587655, 37.645687},
                                     {55.592028, 37.653656}, {55.580999, 37.659164}};

    for (size_t i = 0; i < correct_name.size(); ++i) {
        std::unique_ptr<Request> request = ParseBuildRequest(lines_stop[i]);
        ASSERT(request->type == RequestType::AddStop);
        ASSERT(request->GetName() == correct_name[i]);
        ASSERT(std::abs(static_cast<Point *>(request->GetData())->latitude - correct_pt[i].latitude) < 0.0001);
        ASSERT(std::abs(static_cast<Point *>(request->GetData())->longitude - correct_pt[i].longitude) < 0.0001);
    }
}

void TestParseBuildRoute() {
    std::vector<std::string> lines_route {"Bus 256   : Biryulyovo Zapadnoye>Biryusinka    >Universam > Biryulyovo Tovarnaya>Biryulyovo Passazhirskaya>    Biryulyovo Zapadnoye      ",
                                         "Bus    750    : Tolstopaltsevo - Marushkino - Rasskazovka"
                                            };
    std::vector<std::vector<std::string>> correct_route = {{"Biryulyovo Zapadnoye",
                                                            "Biryusinka",
                                                            "Universam",
                                                            "Biryulyovo Tovarnaya",
                                                            "Biryulyovo Passazhirskaya",
                                                            "Biryulyovo Zapadnoye"},
                                                           {"Tolstopaltsevo",
                                                             "Marushkino",
                                                            "Rasskazovka"}
                                                            };
    std::vector<std::string> correct_num = {"256", "750"};
    for (size_t i = 0; i < correct_route.size(); ++i) {
        std::unique_ptr<Request> request = ParseBuildRequest(lines_route[i]);
        ASSERT(request->type == RequestType::AddRoute);
        ASSERT( correct_route[i] == static_cast<BusRoute *>(request->GetData())->route);
        ASSERT( correct_num[i] == request->GetName());
    }
}

