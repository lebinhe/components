#ifndef COMPONENTS_UTIL_CODING_H_
#define COMPONENTS_UTIL_CODING_H_

#include <stdint.h>
#include <string.h>

#include <string>

#include "components/util/slice.h"

void PutFixed32(std::string* dst, uint32_t value);
void PutFixed64(std::string* dst, uint64_t value);
void PutVarint32(std::string* dst, uint32_t value);
void PutVarint64(std::string* dst, uint64_t value);
void PutLengthPrefixedSlice(std::string* dst, const Slice& value);

bool GetVarint32(Slice* input, uint32_t* value);
bool GetVarint64(Slice* input, uint64_t* value);
bool GetLengthPrefixedSlice(Slice* input, Slice* result);

const char* GetVarint32Ptr(const char* p, const char* limit, uint32_t* v);
const char* GetVarint64Ptr(const char* p, const char* limit, uint64_t* v);

int VarintLength(uint64_t v);

void EncodeFixed32(char* dst, uint32_t value);
void EncodeFixed64(char* dst, uint64_t value);

char* EncodeVarint32(char* dst, uint32_t value);
char* EncodeVarint64(char* dst, uint64_t value);

inline uint32_t DecodeFixed32(const char* ptr) { 
    // little endian
    uint32_t result;
    memcpy(&result, ptr, sizeof(result));
    return result;
}

inline uint64_t DecodeFixed64(const char* ptr) {
    // little endian
    uint64_t result;
    memcpy(&result, ptr, sizeof(result));
    return result;
}

const char* GetVarint32PtrFallback(const char* p,
                                   const char* limit,
                                   uint32_t* value);

inline const char* GetVarint32Ptr(const char* p,
                                  const char* limit,
                                  uint32_t* value) {
    if (p < limit) {
        uint32_t result = *(reinterpret_cast<const unsigned char*>(p));
        if ((result & 128) == 0) {
            *value = result;
            return p + 1;
        }
    }
    return GetVarint32PtrFallback(p, limit, value);
}

#endif // COMPONENTS_UTIL_CODING_H_
