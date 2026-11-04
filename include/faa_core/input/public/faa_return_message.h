//
// Created by HP on 2026/10/27.
//

#ifndef FASTAIASSISTANT_ROOT_FAA_RETURN_MESSAGE_H
#define FASTAIASSISTANT_ROOT_FAA_RETURN_MESSAGE_H
#include <string>
namespace faa::core::input {

    struct FaaReturnMessage {
        std::string error;
    };
}
#endif //FASTAIASSISTANT_ROOT_FAA_RETURN_MESSAGE_H