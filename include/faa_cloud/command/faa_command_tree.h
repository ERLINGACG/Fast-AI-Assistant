//
// Created by HP on 2026/10/9.
//

#ifndef FASTSCREEN_FAA_COMMAND_TREE_H
#define FASTSCREEN_FAA_COMMAND_TREE_H
#include <map>

#include "faa_command_tree_node.h"
#include "info/ilog/ilog.h"

namespace faa::cloud::command {




    // template<typename... Args>
    class FaaCommandTree {

    protected:

        std::unique_ptr<FaaCommandTreeNode> root = std::make_unique<FaaCommandTreeNode>(
            FaaCommandTreeNode{
                .param_slot_size = 0,
                .token_type = root_token,
                .name = "/",
            }
        );

        std::vector<std::string> history;


    public:
        FaaCommandTree()= default;

        FaaCommandTree& add( std::unique_ptr<FaaCommandTreeNode> node, const std::vector<std::string>& path,info::log::Ilog* log = nullptr);
        FaaCommandTree& print(info::log::Ilog* ilog);



        void exec(const std::string& cmd, info::log::Ilog* ilog = nullptr);




        // auto& add(std::unique_ptr<FaaCommandTreeNode> node,
        //   const std::vector<std::string>& path,
        //   info::log::Ilog* ilog = nullptr) {
        //     if (!node) {
        //         if (ilog) ilog->error("node is null");
        //         return *this;
        //     }
        //     if (!root) {
        //         if (ilog) ilog->error("root is null");
        //         return *this;
        //     }
        //
        //     FaaCommandTreeNode* current = root.get();
        //
        //     for (const auto& token : path) {
        //         if (token.empty() || token == "/") continue;
        //
        //         // 1) 在当前节点下找同名子节点
        //         FaaCommandTreeNode* next = nullptr;
        //         for (auto& child : current->children) {
        //             if (child->name == token) { next = child.get(); break; }
        //         }
        //
        //         // 2) 没找到就新建一个
        //         if (!next) {
        //             auto new_node = std::make_unique<FaaCommandTreeNode>(FaaCommandTreeNode{
        //                 .param_len  = 0,
        //                 .token_type = operator_token,
        //                 .name       = token,
        //             });
        //             new_node->parent = current;
        //             next = new_node.get();                     // ← move 前取 raw
        //             current->children.push_back(std::move(new_node));
        //         }
        //
        //         // 3) 无论找到还是新建，都下钻到 next
        //         current = next;
        //     }
        //
        //     // 4) 最后把 node 挂到 current 下
        //     node->parent = current;
        //     current->children.push_back(std::move(node));
        //     return *this;
        // }

        // void debugNode(info::log::Ilog* ilog,
        //        FaaCommandTreeNode* node,
        //        const std::string& path) {
        //     if (!ilog || !node) return;
        //
        //     std::string full = path + "/" + node->name;
        //
        //     // 叶子 → 打印完整路径
        //     if (node->children.empty()) {
        //         ilog->debug("leaf: {}", full);
        //         return;
        //     }
        //
        //     // 不是叶子 → 继续下钻
        //     for (auto& child : node->children) {
        //         if (!child) continue;
        //         debugNode(ilog, child.get(), full);
        //     }
        // }
        //
        // void debug(info::log::Ilog* ilog = nullptr) {
        //     if (!ilog || !root) return;
        //     ilog->debug("debug command tree");
        //     debugNode(ilog, root.get(), "");
        // }
        //
        //
        // auto chick(const std::string& line){
        //     if ( line.empty() || line[0] != '/'){
        //         return false;
        //     }
        //     return true;
        // }

        // std::vector<std::string> lex(const std::string& line) {
        //     if (line.empty() || line[0] != '/') return {};
        //
        //     std::string primitive = line.substr(1);
        //     std::vector<std::string> tokens;
        //     std::string token_block;
        //     bool in_quote = false;
        //
        //     for (char c : primitive) {
        //         if (c == '"') {
        //             if (in_quote) {
        //                 tokens.push_back(token_block);   // 引号闭合，当前块成 token
        //                 token_block.clear();
        //             }
        //             in_quote = !in_quote;
        //             continue;
        //         }
        //         if (in_quote) {
        //             token_block += c;
        //             continue;
        //         }
        //         if (c == ' ' || c == '\t' || c == '-' ) {
        //             if (!token_block.empty()) {
        //                 tokens.push_back(token_block);
        //                 token_block.clear();
        //             }
        //             continue;
        //         }
        //         token_block += c;
        //     }
        //
        //     // 收尾：最后一个 token 还在 token_block 里
        //     if (!token_block.empty()) {
        //         tokens.push_back(token_block);
        //     }
        //     return tokens;
        // }

        // auto exec(const std::string& line,info::log::Ilog* ilog = nullptr) {
        //     if (!chick(line) ){
        //         return;
        //     }
        //
        //     auto tokens = lex(line);
        //     if (ilog) ilog->info("exec tokens size: {}",tokens.size());
        //
        //     auto start_cmd = tokens.begin();
        //     if (start_cmd == tokens.end()) {
        //         return;
        //     }
        //     auto* current = this->root.get();
        //     std::map<std::string,std::vector<std::string>> params;
        //     for (const auto& start : current->children ){
        //         if (start->name == *start_cmd){
        //             current = start.get();
        //             break;
        //         }
        //     }
        //
        //     if (ilog) ilog->info("main command: {} , param_len: {}",current->name,current->param_len);
        //     const auto call_back = call_back_map.find(current->name);
        //     if (call_back == call_back_map.end()){
        //         if (ilog) ilog->error("command not found: {}",current->name);
        //         return;
        //     }
        //
        //     // int pos = 1;
        //     FaaCommandTreeNode* param_node = current;
        //     size_t pos = 1;
        //     while (pos < tokens.size()) {
        //         const std::string& keyword = tokens[pos];
        //
        //         // 找 keyword 对应的子节点
        //         FaaCommandTreeNode* slot = nullptr;
        //         for (auto& child : param_node->children) {
        //             if (child->name == keyword) { slot = child.get(); break; }
        //         }
        //
        //         if (!slot) {
        //             if (ilog) ilog->error("unknown keyword: {}", keyword);
        //             return;
        //         }
        //
        //         if (slot->token_type == operator_token) {
        //             param_node = slot;
        //             pos += 1;                     // operator 消费 1 个
        //             continue;
        //         }
        //
        //         // keyword：从 pos+1 取 param_len 个
        //         std::vector<std::string> vals;
        //         for (size_t k = 0; k < slot->param_len; ++k) {
        //             size_t idx = pos + 1 + k;
        //             if (idx >= tokens.size()) {
            // for (auto const& param : current->children) {
            //     param_pos[param->name] = 0;
            //     param_node = param.get();
        //             }
        //             vals.push_back(tokens[idx]);
        //         }
        //         params[slot->name] = std::move(vals);
        //         pos += 1 + slot->param_len;       // 跳过 keyword + 它的值
        //     }
        //     call_back->second(params);
        //
        // }
    };

}
#include "faa_command_tree_imp.h"
#endif //FASTSCREEN_FAA_COMMAND_TREE_H