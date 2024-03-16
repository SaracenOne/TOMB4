#include "../tomb4/pch.h"
#include "platform.h"

#ifdef _WIN32
#include <windows.h>
#elif defined(_POSIX_VERSION)
#include <dirent.h>
#else
#error "Platform not supported"
#endif
#include "cmdline.h"
#include "function_stubs.h"
#include "winmain.h"

std::string userdata_path_dir = "";

std::string working_dir_path;
std::string game_user_dir_path;
std::string savegame_dir_path;
std::string screenshots_dir_path;

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

void platform_find_file_with_substring(const char* dir_path, const char* substring, char* found_filename) {
#ifdef _WIN32
	char win32_path[4096];
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

		size_t cmp_count = count_matching_characters(substring, find_file_data.cFileName);
		size_t str_len = strlen(substring);

		if (cmp_count >= str_len) {
			size_t filename_length = strlen(find_file_data.cFileName);

			if (filename_length < 256) {
				strcpy(found_filename, find_file_data.cFileName);
				found_filename[filename_length] = '\0';
				break;
			} else {
				Log(1, "Audio filename %s too long!", find_file_data.cFileName);
				return;
			}
		}
	} while (FindNextFile(hFind, &find_file_data));

	FindClose(hFind);
#elif defined(_POSIX_VERSION)
	DIR* dirp = opendir(concatinated_path);
	if (dirp == NULL) {
		perror("Error opening directory");
		return;
	}

	struct dirent* dp;
	while ((dp = readdir(dirp)) != NULL) {
		if (dp->d_type == DT_DIR) {
			continue; // Skip directories
		}

		size_t cmp_count = count_matching_characters(substring, dp->d_name);
		size_t str_len = strlen(substring);

		if (cmp_count >= str_len) {

			int filename_length = strlen(dp->d_name);

			if (filename_length < 256) {
				strcpy(found_filename, dp->d_name);
				found_filename[filename_length] = '\0';
				break;
			} else {
				Log(1, "Audio filename %s too long!", dp->d_name);
				return;
			}
		}
	}

	closedir(dirp);
#endif
}

int platform_string_ends_with(const char* str, const char* suffix) {
	if (!str || !suffix)
		return 0;
	size_t len_str = strlen(str);
	size_t len_suffix = strlen(suffix);
	if (len_suffix > len_str)
		return 0;
	return strncmp(str + len_str - len_suffix, suffix, len_suffix) == 0;
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
		"Tomb4Main",
		buf,
		NULL);
#elif _WIN32
	MessageBox(0, buf, "Tomb4Main", 0);
#endif

	exit(-1);
}

bool platform_create_directory(const char* path) {
	char tmp[256];
	char* p = NULL;
	size_t len;

	snprintf(tmp, sizeof(tmp), "%s", path);
	len = strlen(tmp);
	if (tmp[len - 1] == *PATH_SEPARATOR) {
		tmp[len - 1] = 0;
	}
	for (p = tmp + 1; *p; p++) {
		if (*p == *PATH_SEPARATOR) {
			*p = 0;
#ifdef _WIN32
			DWORD attr = GetFileAttributes(tmp);
			if (attr == INVALID_FILE_ATTRIBUTES) {
				int res = _mkdir(tmp);
				int er = errno;

				if (res != 0 && er != EEXIST) {
					return false;
				}
			}

			if (!(attr & FILE_ATTRIBUTE_DIRECTORY)) {
				int res = _mkdir(tmp);
				int er = errno;

				if (res != 0 && er != EEXIST) {
					return false;
				}
			}
#else
			if (mkdir(tmp, S_IRWXU) != 0 && errno != EEXIST) {
				return false;
			}
#endif
			* p = *PATH_SEPARATOR;
		}
	}
#ifdef _WIN32
	if (_mkdir(tmp) != 0 && errno != EEXIST) {
		return false;
	}
#else
	if (mkdir(tmp, S_IRWXU) != 0 && errno != EEXIST) {
		return false;
	}
#endif

	return true;
}

std::string platform_get_userdata_path() {
	if (userdata_path_dir.empty()) {
		FILE *portable_file = fopen("portable.txt", "r");
		if (portable_file) {
			fclose(portable_file);
			userdata_path_dir = ".";
			userdata_path_dir += PATH_SEPARATOR;
		} else {
			userdata_path_dir = SDL_GetPrefPath("", "Tomb4Plus");
		}
	}

	return userdata_path_dir;
}