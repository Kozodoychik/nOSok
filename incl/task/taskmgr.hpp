#pragma once

#include <stdint.h>
#include <task/task.hpp>

namespace nosok {
    namespace tasks {

        void init();

        void create(void* entry);
        extern "C" void switch_task();

    }
}