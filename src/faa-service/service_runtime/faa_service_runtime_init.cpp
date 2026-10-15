//
// Created by HP on 2026/7/21.
//
#include "faa_core/win_utils/faa_win_utils.h"
#include "../../../include/faa_service/runtime/faa_servuce_runtime_init.h"

bool faa::service::rt::init::faa_wgc_init(FaaWgcCapResources& resources, const std::string& target_key, const Ilog& log){
    resources.
          window_capture=std::make_unique<FAAWindowCapture>(FAAWindowCapture(target_key,log)
               .initD3d11(resources.d3d_ctx)
               .initWgcRtCtx(resources.wgc_rt).
                startSession());
    return resources.window_capture->is_init();
}

void faa::service::rt::init::faa_ocr_init(FaaServiceOcrResources& resources,nlohmann::json& config)
{
    if(resources.is_init){
        return;
    }
    resources.is_init=true;
    auto& det_dnn=resources.det_dnn;
    auto& rec_dnn=resources.rec_dnn;
    for (const auto& it : config.items()){
           auto v=it.value();
           const auto& k=it.key();

           auto entrance=v["entrance"];
           auto exit    =v["exit"];
           auto height  =v["height"];
           auto wight   =v["width"];
           auto path    =v["path"];
           auto layers   =v["layers"];
           auto vocab_path             =v.value("vocab_path","");

           if (k == "det"){

               det_dnn.init(entrance["name"].get<std::string>(),
                            entrance["type"].get<std::string>(),
                            exit["name"].get<std::string>(),
                            exit["type"].get<std::string>(),
                            height.get<int>(),
                            wight.get<int>(),
                            std::make_unique<FaaCvDnnForTensorRT>(
                                path.get<std::string>()
                            )
               );
              for( const auto& info:layers.items()){
                  if (info.value().contains("input")){

                      // log.debug("layers:{}",info.value().dump());
                      auto input=info.value()["input"];
                      det_dnn.input(
                           input["name"].get<std::string>(),
                           input["dims"].get<std::vector<int64_t>>()
                      );
                  }

                  if (info.value().contains("output")){
                      // log.debug("layers:{}",info.value().dump());
                      auto output=info.value()["output"];
                      det_dnn.output(
                           output["name"].get<std::string>(),
                           output["type"].get<std::string>()
                      );
                  }
              }
           }

           if (k == "rec"){
               rec_dnn.init(entrance["name"].get<std::string>(),
                            entrance["type"].get<std::string>(),
                            exit["name"].get<std::string>(),
                            exit["type"].get<std::string>(),
                            height.get<int>(),
                            wight.get<int>(),
                            std::make_unique<FaaCvDnnForTensorRT>(
                                path.get<std::string>()
                            )
               );
               rec_dnn.proc=std::make_unique<FaaOcrRecProc>(vocab_path);
               for( const auto& info:layers.items()){
                  if (info.value().contains("input")){
                      //
                      // log.debug("layers:{}",info.value().dump());
                      auto input=info.value()["input"];
                      rec_dnn.input(
                           input["name"].get<std::string>(),
                           input["dims"].get<std::vector<int64_t>>()
                      );
                  }

                  if (info.value().contains("output")){
                      // log.debug("layers:{}",info.value().dump());
                      auto output=info.value()["output"];
                      rec_dnn.output(
                           output["name"].get<std::string>(),
                           output["type"].get<std::string>()
                      );
                  }
               }
           }


    }
}

bool faa::service::rt::init::win32_input_init(std::string target_key,
    std::map<std::string, std::unique_ptr<input::windows::WindowsInput>>& inputs)
{
    if (!inputs.contains(target_key)){
        auto handle=core::win_utils::GetWindowHandle(target_key);
        if (!handle){
            return false;
        }
        auto input= std::make_unique<input::windows::WindowsInput>(handle);
        inputs.emplace(target_key,std::move(input));
    }
    return true;
}
