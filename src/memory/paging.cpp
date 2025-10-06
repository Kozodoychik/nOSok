#include <memory/paging.hpp>
#include <memory/alloc.hpp>

extern uint32_t _kernel_start;
extern uint32_t _kernel_end;

namespace nosok {
    namespace mem {
        namespace paging {
            page_dir_entry page_dirs[1024] __attribute__((aligned(4096)));
            page_table_entry page_tables[1024*1024] __attribute__((aligned(4096)));

            void init() {
                for (int page = 0; page < 1024; page++) {
                    if (page*4 < _kernel_start || page*4 > _kernel_end) continue;
                    page_tables[page].present = true;
                    page_tables[page].read_write = true;
                    page_tables[page].address = (page*4) >> 12;
                }

                page_dirs[0].present = true;
                page_dirs[0].read_write = true;
                page_dirs[0].address = (uint32_t)page_tables >> 12;

                page_dirs[768].present = true;
                page_dirs[768].read_write = true;
                page_dirs[768].address = (uint32_t)page_tables >> 12;

                set_cr3(page_dirs);
            }

            void* map_page(uint32_t paddr, uint32_t vaddr, uint8_t flags) {
                int pd_index = (vaddr & 0xFFC00000) >> 22;
                int pt_index = ((vaddr & 0x003FF000) >> 12) + (pd_index * 1024);

                uint32_t paddr_aligned = paddr & 0xFFFFF000;

                if (!page_dirs[pd_index].present) return (void*)-1;

                page_tables[pt_index].present = true;
                page_tables[pt_index].address = paddr_aligned >> 12;
                page_tables[pt_index].flags = flags;

            }

            uint32_t vaddr_to_paddr(uint32_t paddr) {
                
            }

            void set_cr3(page_dir_entry* pdes) {
                asm (
                    "mov %0, %%eax\n\r"
                    "mov %%eax, %%cr3" :
                    :
                    "g"((uint32_t)pdes):
                    "eax"
                );
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