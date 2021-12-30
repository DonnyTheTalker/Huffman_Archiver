#include "BitWriter.h"
#include "BitWorker.h"

#include <algorithm>

void BitWriter::WriteBits(std::ostream& out, const std::vector<bool>& bits)
{
    for (const auto& bit : bits)
    {
        cache_.push(bit);
    }
    WriteToStream(out);
}

void BitWriter::WriteNBits(std::ostream& out, const uint32_t ch, const uint32_t n)
{
    auto bits = BitWorker::UIntToBits(ch, n);
    WriteBits(out, bits);
}

void BitWriter::WriteUInt(std::ostream& out, const uint32_t ch)
{
    auto bits = BitWorker::UIntToBits(ch, bit_values::BYTE_TO_BITS);
    std::reverse(bits.begin(), bits.end());
    WriteBits(out, bits);
}

void BitWriter::WriteToStream(std::ostream& out)
{
    if (cache_.size() < bit_values::BYTE_TO_BITS)
    {
        return;
    }

    std::vector<bool> byte;
    for (uint32_t i = 0; i < bit_values::BYTE_TO_BITS; ++i)
    {
        byte.push_back(cache_.front());
        cache_.pop();
    }

    out << static_cast<unsigned char>(BitWorker::BitsToUInt(byte));
}

void BitWriter::WriteAll(std::ostream& out)
{
    while (cache_.size() % bit_values::BYTE_TO_BITS != 0)
    {
        cache_.push(false);
    }

    while (!cache_.empty())
    {
        WriteToStream(out);
    }
}
