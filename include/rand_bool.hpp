#pragma once
#include <random>
#include <cstdint>

class RandBool
{
    std::mt19937_64 rng;
    uint64_t buffer = 0;
    int remaining = 0;

public:
    RandBool() : rng(std::random_device{}()) {}

    bool next()
    {
        if (remaining == 0)
        {
            buffer = rng();
            remaining = 64;
        }
        bool result = buffer & 1;
        buffer >>= 1;
        --remaining;
        return result;
    }
};
