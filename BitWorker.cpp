#include "BitWorker.h"

uint32_t BitWorker::BitsToUInt(const std::vector<bool>& bits)
{
    uint32_t result = 0;
    for (const auto& bit : bits)
    {
        result = (result * 2) + static_cast<uint32_t>(bit);
    }
    return result;
}

std::vector<bool> BitWorker::UIntToBits(const uint32_t ch, const uint32_t min_len)
{
    std::vector<bool> result;
    auto num = static_cast<uint32_t>(ch);

    do {
        result.push_back(num % 2);
        num /= 2;
    } while (num > 0);

    while (result.size() < min_len)
    {
        result.push_back(false);
    }

    return result;
}
