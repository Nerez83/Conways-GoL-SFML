#pragma once

namespace bit
{
    typedef uint8_t Cell;

    // set n to 1
    inline void set(Cell &num, int n)
    {
        num = num | ((Cell)1 << n);
    }
    // set n to 0
    inline void clear(Cell &num, int n)
    {
        num = num & ~((Cell)1 << n);
    }
    // switch n from 1 to 0 or from 0 to 1
    inline void toggle(Cell &num, int n)
    {
        num = num ^ ((Cell)1 << n);
    }
    // get n
    inline bool read(Cell &num, int n)
    {
        return (bool)(num & ((Cell)1 << n));
    }
}
