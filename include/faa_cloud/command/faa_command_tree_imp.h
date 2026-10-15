//
// Created by HP on 2026/10/11.
//

#ifndef FASTSCREEN_FAA_COMMAND_TREE_IMP_H
#define FASTSCREEN_FAA_COMMAND_TREE_IMP_H
#include "faa_command_tree.h"
#include "faa_command_tokenizer.h"
namespace faa::cloud::command{

    inline FaaCommandTree& FaaCommandTree::add(
        std::unique_ptr<FaaCommandTreeNode> node,
        const std::vector<std::string>& path,
        info::log::Ilog* ilog) {

        if (node && this->root) {
           auto* curr = this->root.get();

           for (auto& p : path) {
               if (p == this->root->name) {
                   continue;
               }
               FaaCommandTreeNode* min_node = nullptr;
               for (auto& child : curr->children) {

                   if (child->name == p) {
                       min_node=child.get();
                       break;
                   }
               }

               if (min_node) {
                   curr=min_node;
               }else{
                   auto* old_curr = curr;
                   auto new_node = std::make_unique<FaaCommandTreeNode>(
                         FaaCommandTreeNode{
                             .param_slot_size = 0,
                             .token_type = operator_token ,
                             .name =  p,
                             .parent =  curr ,

                         }
                   );


                   curr=new_node.get();
                   if (ilog) {
                       ilog->info("send node name: {} ,parent name: {}",curr->name,curr->parent->name);

                   }

                   old_curr->children.push_back(std::move(new_node));
               }
           }

            // if (curr){
           node->parent=curr;
           curr->children.push_back(std::move(node));
            // }
            return *this;
        }

        return *this;
    }

    inline FaaCommandTree& FaaCommandTree::print(info::log::Ilog* ilog) {
        ilog->info("root size: {}",root->children.size());
        auto* curr = root.get();
        std::function<void(FaaCommandTreeNode*,std::string)> print = [&](const FaaCommandTreeNode* node,std::string cmd){

            if (node->name != root->name) {
                cmd+="/"+node->name;
            }

            if (node->children.empty()) {
                 ilog->info("cmd: {}",cmd);
                 return;

            }

            for (auto& child : node->children) {
                print(child.get(),cmd);
            }

        };

        std::string cmd;
        print(curr,cmd);

        return *this;
    }


    inline void FaaCommandTree::exec(const std::string& cmd, info::log::Ilog* ilog) {
        std::vector<std::string> tokens;
        tokenize(cmd,tokens,ilog);
        if (tokens.empty()) return;

        FaaCommandTreeNode* node = root.get();
        size_t pos = 0;

        // ---------- 阶段 1：只对 operator_token 下钻 ----------
        while (pos < tokens.size()) {
            FaaCommandTreeNode* next = nullptr;
            for (auto& c : node->children) {
                if (c->name == tokens[pos] &&
                    c->token_type == operator_token) {
                    next = c.get();
                    break;
                    }
            }
            if (!next) break;
            node = next;
            ++pos;
        }

        // ---------- 阶段 2：剩下的 tokens 在 keyword_token 里找参数槽 ----------
        std::map<std::string, std::vector<std::string>> params;

        while (pos < tokens.size()) {
            FaaCommandTreeNode* slot = nullptr;
            for (auto& c : node->children) {
                if (c->name == tokens[pos] &&
                    c->token_type == keyword_token) {
                    slot = c.get();
                    break;
                    }
            }
            if (!slot) {
                if (ilog) ilog->error("unknown param: {}", tokens[pos]);
                break;
            }

            ++pos;   // 吃掉槽名
            std::vector<std::string> vals;
            for (int k = 0; k < slot->param_slot_size && pos < tokens.size(); ++k) {
                vals.push_back(tokens[pos++]);
            }
            params[slot->name] = std::move(vals);
        }

        // ---------- 阶段 3：执行当前 node 的 callback ----------
        if (node->callback) {
            node->callback(params);
        } else if (ilog) {
            ilog->error("no callback for: {}", node->name);
        }
    }
}
#endif //FASTSCREEN_FAA_COMMAND_TREE_IMP_H