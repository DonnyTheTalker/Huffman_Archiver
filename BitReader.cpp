#include "BitReader.h"
#include "BitWorker.h"

#include <algorithm>

uint32_t BitReader::ReadNBits(std::istream& in, const uint32_t n)
{
    while (cache_.size() < n)
    {
        ReadExtraByte(in);
    }

    std::vector<bool> bits;
    while (bits.size() < n)
    {
        bits.push_back(cache_.front());
        cache_.pop();
    }

    std::reverse(bits.begin(), bits.end());
    return BitWorker::BitsToUInt(bits);
}

void BitReader::ReadExtraByte(std::istream& in)
{
    unsigned char ch;
    in >> std::noskipws >> ch;
    auto bits = BitWorker::UIntToBits(static_cast<uint32_t>(ch), bit_values::BYTE_TO_BITS);
    std::reverse(bits.begin(), bits.end());

    for (const auto& bit : bits)
    {
        cache_.push(bit);
    }
}
