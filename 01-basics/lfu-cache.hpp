#pragma once

#include <list>
#include <unordered_map>
#include <iterator>

namespace lfu 
{

template <typename T>struct freq_node_t;

template <typename T> struct local_node_t
{
    size_t freq;
    struct freq_node_t<T> freq_node;
    T data;

    local_node_t (size_t fr)
    {
        freq = fr;
    }
};

// "using" operator with template down there
template <typename T>
using local_list_t = std::list<struct local_node_t<T>>;

template <typename T>struct freq_node_t
{
    size_t freq;

    local_list_t<T> local_list;

    freq_node_t(size_t fr)
    {
        freq = fr;
    }
};


template <typename T, typename KeyT = int> struct lfu_t
{
    using local_list_it_t = typename std::list<struct local_node_t<T>>::iterator;
    using freq_list_it_t = typename std::list<struct freq_node_t<T>>::iterator;
    
    private:
    std::list<freq_node_t<T>> clist_;

    std::unordered_map <KeyT, local_list_it_t> table_;
    
    public:
    size_t sz_;

    lfu_t (size_t sz)
    {
        sz_ = sz;
    }

    bool full ()
    {
        return (clist_.size() == sz_);
    }


    template <typename F> bool lfu_get (KeyT key, F slow_get_page)
    {
        auto hit = table_.find(key);

        if (hit == table_.end())
        {
            if (full())
            {
                freq_node_t last_freq = clist_.back();
                local_list_t<T> last_local_list = last_freq.local_list;
                table_.erase(last_local_list.back());
                if (last_freq.local_list.size() == 0)
                {
                    clist_.remove(last_freq);
                }
            }

            freq_node_t first_freq = clist_.front();
            if (first_freq.freq != 1)
            {
                freq_node_t<T> new_first_freq(1);
                clist_.push_front(new_first_freq);
                first_freq = clist_.front();
            }
            
            local_node_t<T> new_local_node(1);
            new_local_node.data = slow_get_page(key);
            new_local_node.freq_node = first_freq;
            first_freq.local_list.push_front(new_local_node);
            return false;
        }
        local_node_t<T> found = *hit;

        freq_node_t parent_node = found.freq_node;
        parent_node.local_list.erase(found);
        int cur_freq = parent_node.freq + 1;
        if (parent_node.local_list.size() == 0)
        {
            clist_.remove(parent_node);
        }

        freq_list_it_t it = std::find (clist_.begin(), clist_.end(), parent_node);
        
        std::next(it, 1);

        freq_node_t next_freq = *it;

        if (next_freq.freq != cur_freq)
        {
            freq_node_t <T> new_freq(cur_freq);
            clist_.insert(std::next(it, 1), new_freq);
            next_freq = new_freq;
        }

        local_node_t<T> new_local_node(cur_freq);
        new_local_node.data = slow_get_page(key);
        new_local_node.freq_node = next_freq;
        next_freq.local_list.push_front(new_local_node);
        return true;
    }
};

}