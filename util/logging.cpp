#include "components/util/logging.h"

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits>
#include "components/util/slice.h"

void AppendNumberTo(std::string* str, uint64_t num) {
    char buf[30];
    snprintf(buf, sizeof(buf), "%llu", (unsigned long long) num);
    str->append(buf);
}

void AppendEscapedStringTo(std::string* str, const Slice& value) {
    for (size_t i = 0; i < value.Size(); i++) {
        char c = value[i];
        if (c >= ' ' && c <= '~') { // ascii [32, 126]
            str->push_back(c);
        } else {
            char buf[10];
            snprintf(buf, sizeof(buf), "\\x%02x",
                     static_cast<unsigned int>(c) & 0xff);
            str->append(buf);
        }
    }
}

std::string NumberToString(uint64_t num) {
    std::string r;
    AppendNumberTo(&r, num);
    return r;
}

std::string EscapeString(const Slice& value) {
    std::string r;
    AppendEscapedStringTo(&r, value);
    return r;
}

bool ConsumeDecimalNumber(Slice* in, uint64_t* val) {
    // Constants that will be optimized away.
    constexpr const uint64_t kMaxUint64 = std::numeric_limits<uint64_t>::max();
    constexpr const char kLastDigitOfMaxUint64 = 
        '0' + static_cast<char>(kMaxUint64 % 10);

    uint64_t value = 0;

    // reinterpret_cast-ing from char* to unsigned char* to avoid signedness.
    const unsigned char* start = 
        reinterpret_cast<const unsigned char*>(in->Data());

    const unsigned char* end = start + in->Size();
    const unsigned char* current = start;
    for (; current != end; ++current) {
        const unsigned char ch = *current;
        if (ch < '0' || ch > '9')
            break;

        // Overflow check.
        // kMaxUint64 / 10 is also constant and will be optimized away.
        if (value > kMaxUint64 / 10 ||
            (value == kMaxUint64 / 10 && ch > kLastDigitOfMaxUint64)) {
            return false;
        }

        value = (value * 10) + (ch - '0');
    }

    *val = value;
    const size_t digits_consumed = current - start;
    in->RemovePrefix(digits_consumed);
    return digits_consumed != 0;
}

