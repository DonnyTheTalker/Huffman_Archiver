#pragma once

#include <cstdint>
#include <iostream>
#include <queue>

class BitWriter
{
public:
    BitWriter() = default;

    void WriteBits(std::ostream& out, const std::vector<bool>& bits);

    void WriteNBits(std::ostream& out, uint32_t ch, uint32_t n);

    void WriteAll(std::ostream& out);

    void WriteUInt(std::ostream& out, uint32_t ch);

protected:
    void WriteToStream(std::ostream& out);

    std::queue<bool> cache_;
};
