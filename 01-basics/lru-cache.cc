//-----------------------------------------------------------------------------
//
// lru-cache implementation on C++
//
//-----------------------------------------------------------------------------
#include <iostream>
#include <cassert>

#include "lru-cache.hpp"


int slow_get_page_imitation (int key)
{
    return key;
}


int main ()
{
    int hits = 0;
    int n;
    size_t m;

    std::cin >> m >> n;
    assert (std::cin.good());
    caches::cache_t<int> c{m};

    for (int i = 0; i < n; i++)
    {
        int q;
        std::cin >> q;
        assert (std::cin.good());

        if (c.lookup_update(q, slow_get_page_imitation))
            hits++;
    }

    std::cout << hits << std::endl;
}

