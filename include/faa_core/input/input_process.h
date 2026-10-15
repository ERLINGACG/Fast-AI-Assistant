//
// Created by HP on 2026/7/6.
//

#ifndef FASTSCREEN_INPUT_PROCESS_H
#define FASTSCREEN_INPUT_PROCESS_H
#include <memory>

namespace faa::input{
    template<typename I>
    class InputProxy {
        I& inputer;
        public:
            explicit InputProxy(I& inputer):inputer(inputer){}

    };
}
#endif //FASTSCREEN_INPUT_PROCESS_H