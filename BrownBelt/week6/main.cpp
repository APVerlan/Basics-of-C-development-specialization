#include <iostream>
#include <iomanip>

#include "BusDataBase.h"
#include "test_runner.h"

void TestPrepStr();
void TestParseAnsw();
void TestParseBuildStop();

int main() {
    BusDataBase db;
    TestRunner tr;

    RUN_TEST(tr, TestPrepStr);
    RUN_TEST(tr, TestParseAnsw);
    RUN_TEST(tr, TestParseBuildStop);


    std::cout << std::setprecision(6);
    db.Build(std::cin);
    db.AnswerRequests(std::cin);
}