//
// Created by HP on 2026/7/9.
//

#ifndef FASTSCREEN_FAA_DNN_EXECUTOR_H
#define FASTSCREEN_FAA_DNN_EXECUTOR_H
#include <string>
#include <vector>
#include <gdlz/trt/core/cuda/trt_cuda_data.h>
#include "faa_core/faa_export.h"
namespace faa::fdnn::rt{
    template<typename B,typename C> // <Backend,Context>
    class FAA_CPP_EXPORT FaaDnnExecutor{
        std::unique_ptr<B> backend;
        public:
           explicit FaaDnnExecutor(std::unique_ptr<B> backend) : backend(std::move(backend)) {}

          B& initEngine(){
              return backend->initEngine();
          }


          B& initCtx(C& ctx){
              return backend->initCtx(ctx);
          }


          template<typename... Args>
          B& preProcess(Args&&... args){
              return backend->preProcess(std::forward<Args>(args)...);
          }


          B& setInput(const std::string& name,std::vector<int64_t> shape,C& ctx){
              return backend->setInput(name,shape,ctx);
          }


          B& setOutput(const std::string& name,gdlz::trt::core::cuda::DataType dataType,C& ctx){
              return backend->setOutput(name,dataType,ctx);
          }

          template<typename... Args>
          B& resetInput(Args&&... args){
              return backend->resetInput(std::forward<Args>(args)...);
          }

          template<typename... Args>
          B& postImage(Args&&... args){
              return backend->postImage(std::forward<Args>(args)...);
          }


          B& forward(C& ctx){
              return backend->forward(ctx);
          }

          B& clear(C& ctx){
              return backend->clear(ctx);
          }

          template<typename... Args>
          B& getOutput(Args&&... args){
              return backend->getOutput(std::forward<Args>(args)...);
          }

          template<typename... Args>
          B& postProcess(Args&&... args){
              return backend->postProcess(std::forward<Args>(args)...);
          }

          std::vector<int64_t> getOutputShapes(const std::string& name,C& ctx){
              return backend->getOutputShapes(name,ctx);
          }



    };
}
#endif //FASTSCREEN_FAA_DNN_EXECUTOR_H