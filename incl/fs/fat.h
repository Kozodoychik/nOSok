#include <stdint.h>


typedef struct {
    uint8_t     drive_num;
    uint8_t     nt_flags;
    uint8_t     signature;
    uint32_t    volume_id;
    char        volume_label[11];
    char        fs_id[8];
}__attribute__((packed)) fat_ebpb;

typedef struct {
    uint8_t     short_jmp[3];
    char        oem_id[8];
    uint16_t    bytes_per_sector;
    uint8_t     secs_per_cluster;
    uint16_t    reserved_count;
    uint8_t     fat_count;
    uint16_t    root_dir_entries;
    uint16_t    total_sectors;
    uint8_t     media_descr;
    uint16_t    secs_per_fat;
    uint16_t    secs_per_track;
    uint16_t    head_count;
    uint32_t    hidden_count;
    uint32_t    large_secs_count;
    fat_ebpb    ebpb;
}__attribute__((packed)) fat_bpb;

typedef struct {
    char        filename[11];
    uint8_t     attrib;
    uint8_t     nt_reserved;
    uint8_t     creation_time_sec;
    uint16_t    creation_time;
    uint16_t    creation_date;
    uint16_t    last_access_date;
    uint16_t    first_cluster_hi;
    uint16_t    modification_time;
    uint16_t    modification_date;
    uint16_t    first_cluster_lo;
    uint32_t    file_size;
} fat_directory_entry;