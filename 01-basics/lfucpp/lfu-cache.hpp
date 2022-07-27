#pragma once

#include <algorithm>
#include <iterator>
#include <list>
#include <memory>
#include <unordered_map>
namespace cache
{
template <typename T, typename KeyT = int> class lfu_t {
    class local_node_t;
    class freq_node_t {
        size_t freq_;
        std::list<local_node_t> local_list_;

      public:
        freq_node_t () : freq_ (1) {}
        freq_node_t (size_t freq) : freq_ (freq) {}

        size_t freq () const { return freq_; }

        size_t set_freq (size_t freq)
        {
            freq_ = freq;
            return freq_;
        }

        std::list<local_node_t> &local_list () const { return local_list_; }   // copy??

        void local_push_front (local_node_t &&local) { local_list_.push_front (local); }
        void local_push_back (local_node_t &&local) { local_list_.push_back (local); }
    };

    class local_node_t {
        size_t freq_;
        KeyT key_;
        std::shared_ptr<freq_node_t> freq_node_;
        T data_;

      public:
        local_node_t (KeyT key, std::shared_ptr<freq_node_t> freq_node) : key_ (key), freq_ (freq_node.freq ())
        {
            freq_node_ = freq_node;
            freq_node_->local_push_front (*this);
        }

        size_t freq () const { return freq_; }
        KeyT key () const { return key_; }
        T data () const { return data_; }
    };

    using local_list_it_t = typename std::list<local_node_t>::iterator;

    std::list<freq_node_t> clist_;
    std::unordered_map<KeyT, local_list_it_t> table_;

    size_t capacity_;
    size_t size_;

  public:
    lfu_t (size_t capacity) : capacity_ (capacity) {}

    size_t size () const { return size_; }
    size_t capacity () const { return capacity_; }

    bool full () { return capacity_ == size_; }

    template <typename F> bool lookup_update (KeyT key, F slow_get_page)
    {
        auto hit = table_.find (key);

        if ( hit == table_.end () )   // 1. cache miss
        {
            if ( full () )
            {
                // some code there
            }
            // some code there
        }

        // 2. cache hit

        // some code there
    }
};
}   // namespace cache