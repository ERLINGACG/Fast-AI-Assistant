//
// Created by HP on 2026/10/9.
//

#ifndef FASTSCREEN_FAA_COMMAND_TREE_NODE_H
#define FASTSCREEN_FAA_COMMAND_TREE_NODE_H
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace faa::cloud::command {


    enum TokenType {
        root_token,
        keyword_token,
        operator_token,
    };




    struct FaaCmdParamSlot {

        TokenType type;
        std::string token_name;
        std::string value;
    };

    // template<typename... Args>
    struct FaaCommandTreeNode {

        int32_t   param_slot_size;
        TokenType token_type;
        std::string name;

        FaaCommandTreeNode* parent;
        std::vector    <std::unique_ptr<FaaCommandTreeNode>> children;

        std::function<int(std::map<std::string,std::vector<std::string>>&)> callback;

    };
}
#endif //FASTSCREEN_FAA_COMMAND_TREE_NODE_H