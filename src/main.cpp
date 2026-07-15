#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/version.hpp>
#include <opencv2/video/video.hpp>
#include "info/ilog/ilog.h"
using info::log::IlogInit;
int main()
{
    auto log=info::log::Ilog([&](){
        return IlogInit{
            true,
            true,
            true,
            true,
            true
        };
    });
    log.info("Hello, world! {}", "FastScreen");


    return 0;
}
