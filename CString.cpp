typedef unsigned int size_t;

void* memcpy(void* dest, const void* src, size_t n);
void* memmove(void* dest, const void* src, size_t n);
char* strcpy(char* dest, const char* src);
char* strncpy(char* dest, const char* src, size_t n);
char* strcat(char* dest, const char* src);
char* strncat(char* dest, const char* src, size_t n);
int memcmp(const void* ptr1, const void* ptr2, size_t n);
int strcmp(const char* str1, const char* str2);
int strcoll(const char* str1, const char* str2);
int strncmp(const char* str1, const char* str2, size_t n);
const char* strstr(const char* str1, const char* str2);
void* memset(void* ptr, int value, size_t n);
size_t strlen(const char* str);

void* memcpy(void* dest, const void* src, size_t n) {
	assert(dest);
	assert(src);
	int nint = n / 4, nchar = n % 4;
	int* destint = (int*)dest, * srcint = (int*)src;
	while (nint--) {
		*destint++ = *srcint++;
	}
	while (nchar--) {
		*((char*)destint++) = *((char*)srcint++);
	}
	return dest;
}

void* memmove(void* dest, const void* src, size_t n) {
	assert(dest);
	assert(src);
	int nint = n / 4, nchar = n % 4;
	int* destint = (int*)dest, * srcint = (int*)src;
	if (destint < srcint) {
		while (nint--) {
			*destint++ = *srcint++;
		}
		while (nchar--) {
			*((char*)destint++) = *((char*)srcint++);
		}
	}
	else {
		n -= nchar;
		while (nchar--) {
			*((char*)destint + n + nchar) = *((char*)srcint + n + nchar);
		}
		while (nint--) {
			*(destint + nint) = *(srcint + nint);
		}
	}
	return dest;
}

char* strcpy(char* dest, const char* src) {
	memmove(dest, src, strlen(src) + 1);
	return dest;
}

char* strncpy(char* dest, const char* src, size_t n) {
	memmove(dest, src, n);
	return dest;
}

char* strcat(char* dest, const char* src) {
	assert(dest);
	char* ret = dest;
	while (*dest++ != '\0');
	strcpy(dest, src);
	return ret;
}

char* strncat(char* dest, const char* src, size_t n) {
	assert(dest);
	char* ret = dest;
	while (*dest++ != '\0');
	memmove
}

size_t strlen(const char* str) {
	size_t ret = 0;
	while (*str++!='\0') ++ret;
	return ret;
}