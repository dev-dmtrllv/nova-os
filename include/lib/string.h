#pragma once

#include <lib/stdint.h>

void *memcpy(void *destination, const void *source, size_t num);
char* itoa(int num, char* str, int base);