#ifndef COMPONENTS_UTIL_HASH_H_
#define COMPONENTS_UTIL_HASH_H_

#include <stddef.h>
#include <stdint.h>

uint32_t Hash(const char* data, size_t n, uint32_t seed);

#endif // COMPONENTS_UTIL_HASH_H_
