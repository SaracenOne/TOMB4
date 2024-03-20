#pragma once
#include "../global/types.h"

long GetRandomControl();
void SeedRandomControl(long seed);
long GetRandomDraw();
void SeedRandomDraw(long seed);
void init_game_malloc();
void* game_malloc(size_t size);

// For 32-bit backwards compatibility.
void reset_virtual_game_malloc_offset();
void increment_virtual_game_malloc_offset(size_t virtual_size);
size_t get_virtual_game_malloc_offset();

#ifndef DEBUG
#define SYSTEM_MALLOC(size) malloc(size)
#define SYSTEM_REALLOC(ptr, size) realloc(ptr, size)
#define SYSTEM_FREE(ptr) free(ptr)
#else
void* system_malloc(size_t size, const char* filename, int line_number);
void* system_realloc(void* ptr, size_t size, const char* filename, int line_number);
void system_free(void* ptr);
#define SYSTEM_MALLOC(size) system_malloc(size, __FILE__, __LINE__)
#define SYSTEM_REALLOC(ptr, size) system_realloc(ptr, size, __FILE__, __LINE__)
#define SYSTEM_FREE(ptr) system_free(ptr)
#endif

void system_report_stray_allocation();

void Log(ulong type, const char* s, ...);
void GlobalLog(const char* s, ...);

extern FILE* logF;
extern PHD_VECTOR CamPos;
extern PHD_VECTOR CamRot;
extern long nPolyType;
extern char* malloc_buffer;
extern char* malloc_ptr;
extern size_t malloc_size;
extern size_t malloc_free;
