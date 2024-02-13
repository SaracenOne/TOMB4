#include "../tomb4/pch.h"
#include "function_stubs.h"
#include <string>
#include "platform.h"
#include "cmdline.h"

FILE *logF = nullptr;
FILE *global_logF = nullptr;

#ifdef DEBUG
#define MAX_MEMORY_ALLOCATIONS 4096
#define MAX_ALLOCATION_FILENAME 64

struct allocation_table_entry {
	char filename[64];
	int line_number = -1;
	void* buffer = nullptr;
};

allocation_table_entry allocation_table[MAX_MEMORY_ALLOCATIONS];
long alloc_count = 0;

void* system_malloc(long size, const char* filename, int line_number)
{
	alloc_count++;
	if (alloc_count >= MAX_MEMORY_ALLOCATIONS) {
		platform_fatal_error("Exceed maximum memory allocations!");
		return nullptr;
	}

	int first_free = -1;
	for (int i = 0; i < MAX_MEMORY_ALLOCATIONS; i++) {
		if (allocation_table[i].buffer == nullptr) {
			first_free = i;
			break;
		}
	}

	if (first_free < 0) {
		platform_fatal_error("Failed to an valid memory allocation table entry!");
		return nullptr;
	}

	void* ptr = malloc(size);
	if (!ptr) {
		platform_fatal_error("Failed to allocate memory!");
		return nullptr;
	}

	allocation_table[first_free].buffer = ptr;
	memcpy(allocation_table[first_free].filename, filename, MAX_ALLOCATION_FILENAME);
	allocation_table[first_free].line_number = line_number;

	return ptr;
}

void* system_realloc(void* ptr, long size, const char* filename, int line_number) {
	if (!ptr) {
		return system_malloc(size, filename, line_number);
	}

	void* new_ptr = realloc(ptr, size);
	if (!new_ptr) {
		platform_fatal_error("Failed to reallocate memory!");
		return nullptr;
	}

	for (int i = 0; i < MAX_MEMORY_ALLOCATIONS; i++) {
		if (allocation_table[i].buffer == ptr) {
			allocation_table[i].buffer = new_ptr;
			break;
		}
	}

	return new_ptr;
}

void system_free(void* ptr)
{
	if (!ptr) {
		platform_fatal_error("Attempted to free nullptr!");
	}

	for (int i = 0; i < MAX_MEMORY_ALLOCATIONS; i++) {
		if (allocation_table[i].buffer == ptr) {
			allocation_table[i].buffer = nullptr;
			break;
		}
	}

	free(ptr);
	alloc_count--;
	if (alloc_count) {
		if (alloc_count < 0) {
			platform_fatal_error("Memory allocation underflow!");
		}
	}
}
#endif

void system_report_stray_allocation()
{
#ifdef DEBUG
	if (alloc_count != 0) {
		for (int i = 0; i < MAX_MEMORY_ALLOCATIONS; i++) {
			if (allocation_table[i].buffer) {
				Log(0, "Leaked memory at %s:%i\n", allocation_table[i].filename, allocation_table[i].line_number);
			}
		}

		Log(0, "Attempted to exit application with %i stray allocations.", alloc_count);
	}
#endif
}

PHD_VECTOR CamPos;
PHD_VECTOR CamRot;

long nPolyType;

char* malloc_buffer;
char* malloc_ptr;
long malloc_size;
long malloc_free;

static long malloc_used;

static long rand_1 = 0xD371F947;
static long rand_2 = 0xD371F947;

long GetRandomControl()
{
	rand_1 = 0x41C64E6D * rand_1 + 12345;
	return (rand_1 >> 10) & 0x7FFF;
}

void SeedRandomControl(long seed)
{
	rand_1 = seed;
}

long GetRandomDraw()
{
	rand_2 = 0x41C64E6D * rand_2 + 12345;
	return (rand_2 >> 10) & 0x7FFF;
}

void SeedRandomDraw(long seed)
{
	rand_2 = seed;
}

void init_game_malloc()
{
	malloc_buffer = (char*)SYSTEM_MALLOC(MALLOC_SIZE);
	malloc_size = MALLOC_SIZE;
	malloc_ptr = malloc_buffer;
	malloc_free = MALLOC_SIZE;
	malloc_used = 0;
}

void* game_malloc(long size)
{
	char* ptr;

	size = (size + 3) & -4;

	if (size > malloc_free)
	{
		Log(0, "OUT OF MEMORY");
		return 0;
	}
	else
	{
		ptr = malloc_ptr;
		malloc_free -= size;
		malloc_used += size;
		malloc_ptr += size;
		memset(ptr, 0, size);
		return ptr;
	}
}

void GlobalLog(const char* s, ...)
{
#ifdef DO_LOG
	va_list list;
	char log_bufffer[8192];

	va_start(list, s);
	vsprintf(log_bufffer, s, list);
	va_end(list);

	std::string full_path = platform_get_userdata_path() + "log.txt";

	if (!global_logF)
		global_logF = fopen(full_path.c_str(), "w+");

	strcat(log_bufffer, "\n");
	fwrite(log_bufffer, strlen(log_bufffer), 1, global_logF);
#endif
}

void Log(ulong type, const char* s, ...)
{
#ifdef DO_LOG
	va_list list;
	char log_buffer[8192];

	va_start(list, s);
	vsprintf(log_buffer, s, list);
	va_end(list);

	if (!game_user_dir_path.empty()) {
		std::string full_path = game_user_dir_path + "log.txt";

		if (!logF)
			logF = fopen(full_path.c_str(), "w+");

		strcat(log_buffer, "\n");
		fwrite(log_buffer, strlen(log_buffer), 1, logF);
	} else {
		GlobalLog(log_buffer);
	}
#endif
}
