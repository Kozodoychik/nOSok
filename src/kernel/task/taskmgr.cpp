#include <task/taskmgr.hpp>
#include <task/task.hpp>
#include <memory/alloc.hpp>
#include <std/printf.hpp>
#include <cpu/interrupts.hpp>

extern "C" void switch_context(task_context* ctx, task_context* next_ctx);
extern "C" void init_first_task(task_context* ctx);

namespace nosok {
    namespace tasks {

        task_context* tasks_start;
        task_context* tasks_end;
        task_context* current_task;

        void init() {
            task_context* task = new task_context;

            tasks_start = task;
            tasks_end = task;
            tasks_start->next = task;

            init_first_task(task);

            nosok::io::printf("New task's ESP: %x\n", tasks_start->esp);

            current_task = tasks_start;
        }

        void create(void* entry) {
            asm volatile ("cli");
            void* stack = nosok::mem::kmalloc(4096);

            task_context* task = new task_context;

            tasks_end->next = task;
            tasks_end = task;
            task->next = tasks_start;

            uint32_t* sp = (uint32_t*)stack + 4096;
            uint32_t jmp_sp = (uint32_t)sp;
            *sp-- = 0x08;
            *sp-- = (uint32_t)entry;
            *sp-- = 0x0;
            *sp-- = 0x0;
            *sp-- = 0x0;
            *sp-- = 0x0;
            *sp-- = 0x0;
            *sp-- = 0x0;
            *sp = 0x0;

            task->esp = sp;

            asm volatile ("sti");
        }

        void switch_task() {
            asm volatile ("cli");
            task_context* prev_task = current_task;
            current_task = current_task->next;
            switch_context(prev_task, current_task);
        }

    }
}