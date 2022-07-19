#pragma once

#include <iterator>
#include <list>
#include <unordered_map>

namespace caches
{

template <typename T, typename keyT = int> struct cache_t
{
    size_t size_;
    std::list<T> cache_list_;

    using ListIt = typename std::list<T>::iterator;
    std::unordered_map<keyT, ListIt> hash_tab_;

    // Structure methods down there
    cache_t (size_t sz) : size_ (sz) {}

    bool full () const { return (cache_list_.size () == size_); }

    template <typename F> bool lookup_update (keyT key, F slow_get_page)
    {
        auto hit = hash_tab_.find (key);

        if ( hit == hash_tab_.end () )
        {
            if ( full () )
            {
                hash_tab_.erase (cache_list_.back ());
                cache_list_.pop_back ();
            }
            cache_list_.push_front (slow_get_page (key));
            hash_tab_[key] = cache_list_.begin ();
            return false;
        }

        auto eltit = hit->second;
        if ( eltit != cache_list_.begin () )
            cache_list_.splice (cache_list_.begin (), cache_list_, eltit, std::next (eltit));
        return true;
    }
};

}   // namespace caches