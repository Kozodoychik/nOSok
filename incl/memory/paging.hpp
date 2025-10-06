#include <stdint.h>

namespace nosok {
    namespace mem {
        namespace paging {

            typedef struct {

                union {
                    struct {
                        bool present         : 1;
                        bool read_write      : 1;
                        bool user_supervisor : 1;
                        bool write_through   : 1;
                        bool cache_disable   : 1;
                        bool accessed        : 1;
                        bool unused          : 1;
                        bool page_4mb        : 1;
                    };
                    uint8_t flags;
                };
                uint8_t unused_high     : 4;
                uint32_t address : 20;

            } page_dir_entry;

            typedef struct {

                bool present : 1;
                union {
                    struct {
                        bool read_write      : 1;
                        bool user_supervisor : 1;
                        bool write_through   : 1;
                        bool cache_disable   : 1;
                        bool accessed        : 1;
                        bool dirty           : 1;
                        bool pat             : 1;
                        bool global          : 1;
                    };
                    uint8_t flags;
                };
                
                uint8_t unused   : 3;
                uint32_t address : 20;

            } page_table_entry;

            void init();

            void* map_page(uint32_t paddr, uint32_t vaddr, uint8_t flags);
            void unmap_page(uint32_t vaddr);

            page_table_entry* get_pte(uint32_t vaddr);

            uint32_t vaddr_to_paddr(uint32_t vaddr);
            uint32_t paddr_to_vaddr(uint32_t paddr);

            void set_cr3(page_dir_entry* pdes);
            uint32_t get_cr3();

        }
    }
}