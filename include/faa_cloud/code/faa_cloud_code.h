//
// Created by HP on 2026/8/24.
//

#ifndef FASTSCREEN_FAA_CLOUD_CODE_H
#define FASTSCREEN_FAA_CLOUD_CODE_H
namespace faa::cloud::code {

    enum FaaCloudCode{
       success=0,
       failed=-1,
       network_error=-2,
       config_error=-3,
       unknown_error=-4,
    };

}
#endif //FASTSCREEN_FAA_CLOUD_CODE_H