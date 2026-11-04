//
// Created by HP on 2026/10/26.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_INPUT_MS_H
#define FASTAIASSISTANT_ROOT_FAA_INPUT_MS_H
#include "faa_cloud/microservices/base/service/faa_cloud_base_services.h"
#include "faa_core/input/keyboard/faa_keyboard_input.h"
#include "faa_core/input/mouse/faa_mouse_input.h"

namespace faa::cloud::microservices::inputs{

    class FaaCloudInputMS : public base::FaaCloudBaseServices<FaaCloudInputMS> {

        info::log::Ilog* log = nullptr;
        core::input::keyboard::FaaKeyboardInput keyboardInput{};
        core::input::mouse::FaaMouseInput       mouseInput{};

    public:

        void setLog(info::log::Ilog* log);
        void addAllOper();
        void addAllThread();

        void addNetworkLoop();

        void addHelloOperat();
        void addSetTagOperat();
        void addActionOperat();
        void addKeyboardApplyOperat();
        void addMouseApplyOperat();


    };

}
#endif //FASTAIASSISTANT_ROOT_FAA_INPUT_MS_H