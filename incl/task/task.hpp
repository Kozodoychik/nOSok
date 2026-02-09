#pragma once

#include <stdint.h>

typedef struct task_context {
    void* esp;
    uint32_t cr3;
    uint32_t pid;
    task_context* next;
}__attribute__((packed)) task_context;