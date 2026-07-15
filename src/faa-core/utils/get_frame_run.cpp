//
// Created by HP on 2026/7/7.
//
#include <vector>
#include <map>

#include "faa-core/wgc_cap/faa_wgc_cap.h"
#include "info/ilog/ilog.h"
#include <opencv2/opencv.hpp>
using std::string;
using std::vector;
using std::map;
using faa::core::cap::wgc::D3DDeviceContext;
using faa::core::cap::wgc::WGCRunTimeContext;
using faa::core::cap::wgc::FAAWindowCapture;
using faa::core::cap::wgc::WgcTextureData;
using info::log::Ilog;
using info::log::IlogInit;


int main(int argc, char* argv[]){
    auto log=Ilog([]()->IlogInit{ return IlogInit{
     .enabled_debug = false,
    };});
    vector<string> args(argv+1, argv + argc);
    auto d3d_ctx=D3DDeviceContext();
    auto wgc_rt=WGCRunTimeContext();
    string target_window_name;
    string save_path;

    int count=0;
    int delays=0;
    for (auto& arg : args){
        if (arg == "-help"){
            log.info("help");
            return 0;
        }
        if (arg == "-target"){
            if (count+1 >= args.size()){
                break;
            }
            target_window_name = args[count+1];
            log.info("target window name is {}",target_window_name);

        }
        if (arg == "-save"){
            if (count+1 >= args.size()){
                break;
            }
            save_path = args[count+1];
            log.info("save path is {}",save_path);
        }
        if (arg == "-delay"){
            if (count+1 >= args.size()){
                break;
            }
            delays =stoi(args[count+1]);
            log.info("delay is {}",delays);
        }
        count++;
    }
    Sleep(3000);
    if (target_window_name.empty()){
        log.error("target window name is empty");
        return 1;
    }

    auto wgc_cap=FAAWindowCapture(target_window_name, log).
                        initD3d11(d3d_ctx).
                        initWgcRtCtx(wgc_rt).
                        startSession();

    auto texture_data=WgcTextureData();
    vector<cv::Mat> textures;
    auto start_time=std::chrono::high_resolution_clock::now();

    while (true){
        cv::Mat mat;
        auto get_frame_start_time=std::chrono::high_resolution_clock::now();
        while (mat.empty()){
            wgc_cap.getFrame(texture_data, mat);
        }
        auto get_frame_end_time=std::chrono::high_resolution_clock::now();
        log.info("get frame success, cost {} ms", std::chrono::duration_cast<std::chrono::milliseconds>(get_frame_end_time-get_frame_start_time).count());
        cv::imshow("frame", mat);
        int key = cv::waitKey(1);       // 等待1ms并返回按键值
        if (key != -1) {                // 如果有按键（无论什么键）
            break;                      // 退出循环
        }
        auto endTime=std::chrono::high_resolution_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(endTime-start_time).count() >= delays){
            textures.push_back(mat);
            start_time=endTime;
        }
    }

    log.info("textures size : {}", textures.size());
    log.info("save path is {}", save_path);

    int save_count=0;
    for (const auto& mat: textures){
            string save_name=save_path+"/"+target_window_name+"_"+std::to_string(save_count++)+".jpg";
            // cv::imwrite(save_name, mat);
            log.info("save images is {}", save_name);
    }
    return 0;
}
