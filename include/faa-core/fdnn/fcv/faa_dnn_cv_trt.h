//
// Created by HP on 2026/7/9.
//

#ifndef FASTSCREEN_FAA_DNN_CV_H
#define FASTSCREEN_FAA_DNN_CV_H
#include <string>
#include <gdlz/trt/cv/core/trt_cv_core.h>
#include <opencv2/core/mat.hpp>
#include "faa-core/faa_export.h"
namespace faa::fdnn::fcv {
    struct FaaDetCtx{
        gdlz::trt::icv::core::TensorRTCvCtx ctx;
    };

    struct FaaDetOutputData{
        gdlz::trt::core::cuda::TensorRTCudaData cuda_data;

        [[nodiscard]] auto toLength() const{
            return cuda_data.len;
        }
        [[nodiscard]] auto toHost() const{
            return cuda_data.data;
        }
    };

    // struct FaaDetParam{
    //     int32_t width;
    //     int32_t height;
    //     double  scalefactor;
    // };

    // struct FaaDetPostParam{
    //     int32_t height;
    //     int32_t width;
    //     float   threshold;
    //     float   unclip_ratio;
    //     int32_t PAD;
    // };


    class FAA_CPP_EXPORT FaaCvDnnForTensorRT{
        protected:

            std::unique_ptr<gdlz::trt::icv::core::TensorRTCvCoreFramework> framework;
        public:
            FaaCvDnnForTensorRT(const std::string& config_path);
            ~FaaCvDnnForTensorRT()=default;

            FaaCvDnnForTensorRT& setInput (const std::string& name,std::vector<int64_t> shape,FaaDetCtx& ctx);
            FaaCvDnnForTensorRT& setOutput(const std::string& name,gdlz::trt::core::cuda::DataType dataType,FaaDetCtx& ctx);
            FaaCvDnnForTensorRT& initEngine();
            FaaCvDnnForTensorRT& initCtx  (FaaDetCtx& ctx);

            FaaCvDnnForTensorRT& postImage(const std::string& name,cv::Mat& img,FaaDetCtx& ctx,gdlz::trt::core::cuda::DataType dataType);
            FaaCvDnnForTensorRT& clear    (FaaDetCtx& ctx);
            FaaCvDnnForTensorRT& forward  (FaaDetCtx& ctx);
            FaaCvDnnForTensorRT& getOutput(const std::string& name,FaaDetOutputData& output,FaaDetCtx& ctx,gdlz::trt::core::cuda::DataType dataType);

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
