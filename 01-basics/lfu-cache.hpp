#pragma once

#include <list>
#include <unordered_map>
#include <iterator>
#include <algorithm>

namespace lfu 
{

template <typename T>struct freq_node_t;

template <typename T> struct local_node_t
{
    size_t freq;
    int key;
    freq_node_t<T> *freq_node;
    T data;

    local_node_t (size_t);
    local_node_t (size_t, int, freq_node_t<T>*);
    local_node_t () : freq(1) {}
};

template <typename T>
local_node_t<T>::local_node_t(size_t fr) : freq{fr} {}

template <typename T>
local_node_t<T>::local_node_t(size_t fr, int key, freq_node_t<T>* freq_node_ar) : freq{fr}, key(key)
{
    freq_node = new freq_node_t<T>(freq_node_ar->freq);
    freq_node->local_list = freq_node_ar->local_list;
}

template <typename T>
int operator==(local_node_t<T> first, const local_node_t<T> second)
{
    return first.freq == second.freq;
}

// "using" operator with template down there
template <typename T>
using local_list_t = std::list<struct local_node_t<T>>;

template <typename T>struct freq_node_t
{
    size_t freq;

    local_list_t<T> local_list;

    freq_node_t(size_t);
    freq_node_t() : freq(1) {}
};

template <typename T>
freq_node_t<T>::freq_node_t(size_t fr) : freq{fr} {}

template <typename T>
int operator==(freq_node_t<T> first, const freq_node_t<T> second)
{
    return first.freq == second.freq;
}

template <typename T, typename KeyT = int> struct lfu_t
{
    using freq_list_it_t = typename std::list<freq_node_t<T>>::iterator;
    
    private:
    std::list<freq_node_t<T>> clist_;

    using local_list_it_t = typename std::list<struct local_node_t<T>>::iterator;
    std::unordered_map <KeyT, local_list_it_t> table_;
    
    public:
    size_t sz_;

    lfu_t (size_t sz) : sz_(sz) {}

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
                freq_node_t<T>& last_freq = clist_.back();
                local_node_t<T>& to_erase = last_freq.local_list.back();
                table_.erase(table_.find(to_erase.key));
                if (last_freq.local_list.size() == 0)
                {
                    clist_.erase(std::prev(clist_.end()));
                }
            }

            freq_node_t<T> first_freq;
            if (clist_.size() == 0 || (clist_.front()).freq != 1)
            {
                clist_.push_front(first_freq);
            }
            
            local_node_t<T> *new_local_node = new local_node_t<T> (1, key, &first_freq);
            new_local_node->data = slow_get_page(key);
            new_local_node->freq_node->local_list.push_front(*new_local_node);
            table_[key] = first_freq.local_list.begin();
            return false;
        }
        auto found = hit->second;

        freq_node_t<T>& parent_node = *(*found).freq_node;
        parent_node.local_list.remove(*found);
        size_t cur_freq = parent_node.freq + 1;
        if (parent_node.local_list.size() == 0)
        {
            clist_.remove(parent_node);
        }

        freq_list_it_t it = std::find (clist_.begin(), std::prev(clist_.end()), parent_node);
        freq_node_t<T>& next_freq = *(std::next(it));

        if (next_freq.freq != cur_freq)
        {
            freq_node_t<T> new_freq(cur_freq);
            clist_.insert(std::next(it, 1), new_freq);
            next_freq = new_freq;
        }

        local_node_t<T> *new_local_node = new local_node_t<T>(cur_freq, key, &next_freq);
        new_local_node->data = slow_get_page(key);
        next_freq.local_list.push_front(*new_local_node);
        return true;
    }
};

}