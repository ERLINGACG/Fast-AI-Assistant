//
// Created by HP on 2026/11/9.
//
#include "faa_common/ilogger/faa_ilogger.h"
#include "faa_common/log/faa_logger.h"

#include <algorithm>
#include <fstream>
#include <iostream>
using faa::common::ilogger::FaaIloggerEnabled;
using faa::common::ilogger::FaaIlogger;
using faa::common::log::FaaLogger;



// void test_logger() {
//     logger.info("str {}", "world");
// }
class Test1 {

    FaaIlogger logger = FaaIlogger({});
    public:
         Test1()= default;

         void test(){
             logger.info("hello world");
             logger.error("hello world");
             logger.debug("hello world");
             logger.warn("hello world");
             std::this_thread::sleep_for(std::chrono::seconds(1));
             logger.debug("hello world");

         }
};
int main() {
    Test1 test1;
    test1.test();
    return 0;
}