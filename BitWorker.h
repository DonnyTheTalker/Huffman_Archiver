#pragma once

#include <cstdint>
#include <vector>

namespace bit_values
{
    const uint32_t BYTE_TO_BITS = 8;
    const uint32_t ONE_BIT = 1;
    const uint32_t NINE_BITS = 9;
}

class BitWorker
{
public:
    static std::vector<bool> UIntToBits(uint32_t ch, uint32_t min_len);
    static uint32_t BitsToUInt(const std::vector<bool>& bits);
};
