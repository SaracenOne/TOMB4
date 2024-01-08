#include "../tomb4/pch.h"
#include "platform.h"

#ifdef _WIN32
#include <windows.h>
#elif defined(_POSIX_VERSION)
#include <dirent.h>
#else
#error "Platform not supported"
#endif
#include "function_stubs.h"
#include "winmain.h"

int count_matching_characters(const char* s1, const char* s2) {
	int count = 0;
	while (*s1 && *s2 && *s1 == *s2) {
		count++;
		s1++;
		s2++;
	}
	return count;
}

int strcicmp(char const* a, char const* b) {
	for (;; a++, b++) {
		int d = tolower((unsigned char)*a) - tolower((unsigned char)*b);
		if (d != 0 || !*a)
			return d;
	}
}

void find_file_with_substring(const char* dir_path, const char* substring, char* found_filename) {
#ifdef _WIN32
	char win32_path[256];
	wsprintf(win32_path, "%s\\*", dir_path);

	WIN32_FIND_DATA find_file_data;
	HANDLE hFind = FindFirstFile(win32_path, &find_file_data);

	if (hFind == INVALID_HANDLE_VALUE) {
		perror("Error finding file");
		return;
	}

	do {
		if (find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			continue; // Skip directories
		}

		int cmp_count = count_matching_characters(substring, find_file_data.cFileName);
		int str_len = strlen(substring);

		if (cmp_count >= str_len) {
			strncpy(found_filename, find_file_data.cFileName, 256 - 1);
			found_filename[256 - 1] = '\0';
			break;
		}
	} while (FindNextFile(hFind, &find_file_data));

	FindClose(hFind);
#elif defined(_POSIX_VERSION)
	DIR* dirp = opendir(dir_path);
	if (dirp == NULL) {
		perror("Error opening directory");
		return;
	}

	struct dirent* dp;
	while ((dp = readdir(dirp)) != NULL) {
		if (dp->d_type == DT_DIR) {
			continue; // Skip directories
		}

		int cmp_count = count_matching_characters(substring, dp->d_name);
		int str_len = strlen(substring);

		if (cmp_count >= str_len) {
			strncpy(found_filename, dp->d_name, 256 - 1);
			found_filename[256 - 1] = '\0';
			break;
		}
	}

	closedir(dirp);
#endif
}

void platform_fatal_error(const char* s, ...) {
	va_list list;
	char buf[4096];

	va_start(list, s);
	vsprintf(buf, s, list);
	strcat(buf, "\n");
	va_end(list);

	Log(0, "Fatal Error: %s", buf);

#ifdef USE_SDL
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
		"Tomb4Main.",
		buf,
		NULL);
#elif _WIN32
	MessageBox(0, buf, "Tomb4Main", 0);
#endif

	exit(-1);
}