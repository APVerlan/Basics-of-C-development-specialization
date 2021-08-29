#include <iostream>
#include <iomanip>

#include "BusDataBase.h"
#include "test_runner.h"
#include "parse_request_tests.h"

int main() {
    BusDataBase db;
    TestRunner tr;

    RUN_TEST(tr, TestPrepStr);
    RUN_TEST(tr, TestParseAnsw);
    RUN_TEST(tr, TestParseBuildStop);
    RUN_TEST(tr, TestParseBuildRoute);


    std::cout << std::setprecision(7);
    db.Build(std::cin);
    db.AnswerRequests(std::cin);
}