#include <task/taskmgr.hpp>
#include <task/task.hpp>
#include <memory/alloc.hpp>
#include <std/printf.hpp>
#include <cpu/interrupts.hpp>

extern "C" void switch_context(task_context* ctx);
extern "C" void init_first_task(task_context* ctx);

namespace nosok {
    namespace tasks {

        task_context* tasks_start;
        task_context* tasks_end;
        task_context* current_task;

        void init() {
            tasks_start = new task_context;
            tasks_end = tasks_start;
            tasks_start->next = tasks_start;

            init_first_task(tasks_start);

            current_task = tasks_start;
        }

        void create(void* entry) {
            asm volatile("cli");
            void* stack = nosok::mem::kmalloc(4096);

            task_context* task = new task_context;

            tasks_end->next = task;
            tasks_end = task;
            task->next = tasks_start;

            uint32_t* sp = (uint32_t*)stack + 4096;
            *sp-- = 0x286;
            *sp-- = 0x08;
            *sp-- = (uint32_t)entry;
            *sp-- = 0x0;
            *sp-- = 0x0;
            *sp-- = 0x0;
            *sp = 0x0;

            //nosok::io::printf("New task's ESP: %x", sp);

            task->esp = sp;

            asm volatile("sti");
        }

        void switch_task() {
            switch_context(current_task);
            current_task = current_task->next;
        }

    }
}