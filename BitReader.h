#pragma once

#include <cstdint>
#include <iostream>
#include <queue>

class BitReader
{
public:
    BitReader() = default;

    uint32_t ReadNBits(std::istream& in, uint32_t n);

protected:
    void ReadExtraByte(std::istream& in);
    std::queue<bool> cache_;
};
