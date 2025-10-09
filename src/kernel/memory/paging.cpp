#include <memory/paging.hpp>
#include <memory/frame_alloc.hpp>

extern uint32_t _kernel_start;
extern uint32_t _kernel_end;

extern nosok::mem::paging::page_dir_entry page_directory;

namespace nosok {
    namespace mem {
        namespace paging {
            page_dir_entry* current_page_dir = &page_directory;

            void init() {

            }

            void* map_page(uint32_t paddr, uint32_t vaddr) {
                page_table_entry* page_table;
                int pd_index = (vaddr & 0xFFC00000) >> 22;
                int pt_index = (vaddr & 0x003FF000) >> 12;

                uint32_t paddr_aligned = paddr & 0xFFFFF000;

                if (!current_page_dir[pd_index].present) {
                    page_table_entry* frame = (page_table_entry*)nosok::mem::frames::alloc();
                    
                    current_page_dir[pd_index].address = (uint32_t)frame >> 12;
                    current_page_dir[pd_index].read_write = true;
                    current_page_dir[pd_index].present = true;

                    page_table = frame;
                }
                else {
                    page_table = (page_table_entry*)(current_page_dir[pd_index].address << 12);
                }

                page_table[pt_index].address = paddr >> 12;
                page_table[pt_index].read_write = true;
                page_table[pt_index].present = true;

                return (void*)vaddr;
            }

            void unmap_page(uint32_t vaddr) {
                page_table_entry* page_table;
                int pd_index = (vaddr & 0xFFC00000) >> 22;
                int pt_index = (vaddr & 0x003FF000) >> 12;

                if (!current_page_dir[pd_index].present) return;

                page_table = (page_table_entry*)(current_page_dir[pd_index].address << 12);
                
                page_table[pt_index] = {0};

                asm (
                    "invlpg (%0)":
                    :
                    "r"(vaddr):
                    "memory"
                );

            }

            uint32_t vaddr_to_paddr(uint32_t paddr) {
                
            }

            void set_cr3(page_dir_entry* pd) {
                asm (
                    "mov %0, %%eax\n\r"
                    "mov %%eax, %%cr3" :
                    :
                    "g"((uint32_t)pd):
                    "eax"
                );

                current_page_dir = pd;
            }

            uint32_t get_cr3() {
                uint32_t cr3;
                asm (
                    "mov %%cr3, %0":
                    "=r"(cr3):
                    :
                    "memory"
                );
                return cr3;
            }
        }
    }
}