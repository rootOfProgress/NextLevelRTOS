#ifndef HEALTH_H
#define HEALTH_H

typedef struct MemoryLifetime
{
    unsigned int num_of_allocs;
    unsigned int num_of_deallocs;
    unsigned int ram_size;
    unsigned int total_byte_alloced;
    unsigned int total_byte_used;    
} MemoryLifetime_t;

typedef struct OsMetaInformation
{
    unsigned int git_hash;
    unsigned int magic;
    short debug_mode;
    short systick_enabled;
    unsigned int os_version;   
} OsMetaInformation_t;

typedef struct ProcessLifetime
{
    unsigned int tasks_cleanup_pending : 4, 
                 failed_tasks : 4, 
                 finished_tasks : 8, 
                 waiting_tasks : 8, 
                 running_tasks : 8;  
} ProcessLifetime_t;

typedef struct OsLifetime 
{
    MemoryLifetime_t memoryStat;
    OsMetaInformation_t osMeta;
    ProcessLifetime_t processStat;
} OsLifetime_t;

#endif