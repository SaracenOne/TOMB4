#include "../tomb4/pch.h"
#include "function_stubs.h"
#include <string>
#include "platform.h"
#include "cmdline.h"

FILE *logF = nullptr;
FILE *global_logF = nullptr;

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
	malloc_buffer = (char*)malloc(MALLOC_SIZE);
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
