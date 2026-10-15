//
// Created by HP on 2026/7/9.
//

#ifndef FASTSCREEN_FAA_DNN_CV_H
#define FASTSCREEN_FAA_DNN_CV_H
#include <string>
#include <gdlz/trt/cv/core/trt_cv_core.h>
#include <opencv2/core/mat.hpp>
#include "faa_core/faa_export.h"
#include "info/ilog/ilog.h"

namespace faa::fdnn::fcv {
    struct FaaCvCtx{
        gdlz::trt::icv::core::TensorRTCvCtx ctx;
    };

    struct FaaCvOutputData{
        gdlz::trt::core::cuda::TensorRTCudaData cuda_data;

        [[nodiscard]] auto toLength() const{
            return cuda_data.len;
        }
        [[nodiscard]] auto toHost() const{
            return cuda_data.data;
        }
    };


    class FAA_CPP_EXPORT FaaCvDnnForTensorRT{
        protected:

            std::unique_ptr<gdlz::trt::icv::core::TensorRTCvCoreFramework> framework;
        public:
            FaaCvDnnForTensorRT(const std::string& config_path);
            ~FaaCvDnnForTensorRT()=default;

            FaaCvDnnForTensorRT& setInput (const std::string& name,std::vector<int64_t> shape,FaaCvCtx& ctx);
            FaaCvDnnForTensorRT& setOutput(const std::string& name,gdlz::trt::core::cuda::DataType dataType,FaaCvCtx& ctx);
            FaaCvDnnForTensorRT& initEngine();
            FaaCvDnnForTensorRT& initCtx  (FaaCvCtx& ctx);
            FaaCvDnnForTensorRT& resetInput (const std::string& name, const std::vector<int64_t>& shape,FaaCvCtx& ctx);
            FaaCvDnnForTensorRT& resetOutput(const std::string& name, FaaCvCtx& ctx,gdlz::trt::core::cuda::DataType dataType);
            FaaCvDnnForTensorRT& postImage(const std::string& name,cv::Mat& img,FaaCvCtx& ctx,gdlz::trt::core::cuda::DataType dataType);
            FaaCvDnnForTensorRT& clear    (FaaCvCtx& ctx);
            FaaCvDnnForTensorRT& forward  (FaaCvCtx& ctx);
            FaaCvDnnForTensorRT& getOutput(const std::string& name,FaaCvOutputData& output,FaaCvCtx& ctx,gdlz::trt::core::cuda::DataType dataType);
            FaaCvDnnForTensorRT& getOutput(const std::string& name,FaaCvOutputData& output,FaaCvCtx& ctx,const std::string& dataType,info::log::Ilog* ilog = nullptr);

            std::vector<int64_t> getOutputShapes(const std::string& name,FaaCvCtx& ctx);

            template<typename P,typename... Args>
            FaaCvDnnForTensorRT& preProcess(P& processer,Args&&... args){
                processer.PreProcess(std::forward<Args>(args)...);
                return *this;
            }

            template<typename P,typename... Args>
            FaaCvDnnForTensorRT& postProcess(P& processer,Args&&... args){
                processer.PostProcess(std::forward<Args>(args)...);
                return *this;
            }

    };


}
#endif //FASTSCREEN_FAA_DNN_CV_H
