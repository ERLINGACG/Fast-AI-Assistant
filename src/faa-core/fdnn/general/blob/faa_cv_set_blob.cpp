//
// Created by HP on 2026/11/3.
//
#include "faa_core/fdnn/general/blob/faa_cv_set_blob.h"
#include "faa_core/frame/faa_mat.h"
#include "faa_core/frame/private/faa_mat_impi.h"
#include <opencv2/dnn.hpp>


void faa::core::fdnn::general::blob::setBlob(frame::FaaMat& blob, frame::FaaMat& orgImg, const FaaBlobParam& param)
{
    cv::dnn::blobFromImage(
       orgImg.impl()->mat,
       blob.impl()->mat,
       param.scalefactor,
       cv::Size(param.w,param.h),
       cv::Scalar(
           param.mean.b,
           param.mean.g,
           param.mean.r
           ),
       param.swapRB,
       param.crop
   );
}
