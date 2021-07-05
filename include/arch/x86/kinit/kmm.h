#pragma once

#include <lib/stdint.h>

namespace kmm
{
	void init();
	void *alloc_frames(size_t number_of_frames);
	void free_frames(void* address);
};