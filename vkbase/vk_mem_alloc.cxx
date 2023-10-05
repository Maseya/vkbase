#define VMA_IMPLEMENTATION
#pragma warning(push)

// This disables a 64-to-32 bit int downcast that occurs. This warning occurs in v3.0.1.
// Once a new release comes out, I can erase this.
#pragma warning(disable : 4244)
#include <vma/vk_mem_alloc.h>
#pragma warning(pop)