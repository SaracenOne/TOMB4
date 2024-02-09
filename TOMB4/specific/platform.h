int strcicmp(char const* a, char const* b);
void find_file_with_substring(const char* dir_path, const char* substring, char* found_filename);
int ends_with(const char* str, const char* suffix);

void platform_fatal_error(const char* s, ...);