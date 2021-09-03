#include <iostream>
#include <iomanip>

#include "BusDataBase.h"
#include "test_runner.h"
//#include "parse_request_tests.h"
#include "json.h"

int main() {
    BusDataBase db;

    std::cout << std::setprecision(17);
    Json::Document js = Json::Load(std::cin);
    db.Build(js);
    db.AnswerRequests(js);
}