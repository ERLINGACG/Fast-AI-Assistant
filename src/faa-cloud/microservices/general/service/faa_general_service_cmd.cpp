//
// Created by HP on 2026/10/11.
//
#include <opencv2/imgcodecs.hpp>

#include "faa_cloud/microservices/general/service/faa_cloud_general.h"
using faa::cloud::microservices::general::FaaCloudGeneralServices;

void FaaCloudGeneralServices::addCommand_loop(info::log::Ilog* log) {
    this->addThread(
        "command_loop",
        std::thread([this,log]() {

                   std::string line;
                   while (this->getSignalMap().get(this->isRunning)) {
                        std::getline(std::cin, line);
                        this->cmd.exec(line,log);
                   }
              }
        ),
        base::resource::ThreadsMap::join,
        log
   );
}

void FaaCloudGeneralServices::addCommand(info::log::Ilog* log) {
    this->addExitCmd(log);
    this->addIsCaptureCmd(log);
    this->addGetFramesCmd(log);
    this->cmd.print(log);
}

void FaaCloudGeneralServices::addExitCmd(info::log::Ilog* log) {
    auto exit_cmd = std::make_unique<command::FaaCommandTreeNode>(
        command::FaaCommandTreeNode{
            .param_slot_size = 0,
            .token_type = command::operator_token,
            .name = "exit",
            .callback = [this,log] (const std::map<std::string, std::vector<std::string>>& params) {
                if (log) log->info("start exit");

                this->signals.set(this->isRunning,false);
                for (auto& val : this->signals.signals | std::views::keys) {
                    this->signals.set(val,false);
                    log->info("set signal {} to {}",val,signals.get(val));
                }
                this->network.network->close();
                return 0;
            }
        }
    );
    this->cmd.add(std::move(exit_cmd),{"/"},log);
}

void FaaCloudGeneralServices::addIsCaptureCmd(info::log::Ilog* log) {

    auto params = std::make_unique<command::FaaCommandTreeNode>(
                     command::FaaCommandTreeNode{
                       .param_slot_size = 1,
                       .token_type = command::keyword_token,
                       .name = "params",
                     }
               );





    auto is_cap = std::make_unique<command::FaaCommandTreeNode>(
        command::FaaCommandTreeNode{
            .param_slot_size = 0,
            .token_type = command::operator_token,
            .name = "cap",
            .callback = [this,log] (const std::map<std::string, std::vector<std::string>>& params) {
                if (log) log->info("start capture");

                auto isStart = false;
                std::string target = "";


                for (const auto& param : params) {
                    if (param.first == "params") {
                        if (param.second.size() != 1){
                            if (log) log->error("params size error");
                            continue;
                        }
                        std::string params_json_str = param.second.front();
                        try {
                            auto json = nlohmann::json::parse(params_json_str);
                            if (log) log->debug("params json_str: {}",json.dump(2));

                            target  = json.value("target","");
                            isStart = json.value("isStart",this->getSignalMap().get(this->isCapturing));

                        } catch (std::exception& e){
                            if (log) log->error("params json error:{}",e.what());
                        }
                        break;
                    }

                }
                if (!target.empty()){
                    cap.set(target);
                }

                this->signals.set(this->isCapturing,isStart);
                log->debug("name {} isStart:{}",cap.getTarget(),signals.get(this->isCapturing));



                return 0;
            }

        }
    );

    is_cap->children.emplace_back(std::move(params));

    this->cmd.add(std::move(is_cap),{"/"},log);

}

void FaaCloudGeneralServices::addGetFramesCmd(info::log::Ilog* log)
{
    auto get_frames = std::make_unique<command::FaaCommandTreeNode>(
                     command::FaaCommandTreeNode{
                       .param_slot_size = 0,
                       .token_type = command::operator_token,
                       .name = "get_frame_count",
                       .callback =   [this,log] (const std::map<std::string, std::vector<std::string>>& params) {
                           if (log) log->debug("get_frame_count");
                           log->info("frames:{}",this->cap.frame_resources.frames.size());
                           return 0;
                       }
                     }
               );

    auto save = std::make_unique<command::FaaCommandTreeNode>(
        command::FaaCommandTreeNode{
            .param_slot_size = 0,
            .token_type = command::operator_token,
            .name = "save_frame",
            .callback =   [this,log] (const std::map<std::string, std::vector<std::string>>& params) {
                           if (log) log->debug("save_frame");
                           cv::Mat frame;

                           this->cap.frame_resources.peek(frame);
                           if (frame.empty()){
                               log->warn("frame is empty");
                               return -1;
                           }
                           cv::imwrite("frame.png",frame);
                           log->info("save frame done");
                           return 0;
                       }
        }
    );
    this->cmd.add(std::move(get_frames),{"/"},log);
    this->cmd.add(std::move(save),{"/"},log);
}

