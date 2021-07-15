#pragma once

#include <lib/stdint.h>

void *memcpy(void *destination, const void *source, size_t num);
void *memset(void *destination, int value, size_t num);
char *itoa(int32_t num, char *str, int base);
char *ltoa(int64_t num, char *str, int base);
char *utoa(unsigned int num, char *str, int base);
char *ltoa(uint64_t num, char *str, int base);
size_t strlen(char *str);
int strncmp(char* str1, char* str2, size_t length);
int strcmp(char* str1, char* str2);
