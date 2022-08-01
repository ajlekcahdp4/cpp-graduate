#pragma once

#include <algorithm>
#include <iterator>
#include <list>
#include <memory>
#include <unordered_map>
namespace cache
{
template <typename T, typename KeyT = int> class lfu_t {

    //=====================================local_node_t=========================================
    class freq_node_t;
    class local_node_t {
      public:
        size_t freq_;
        KeyT key_;
        freq_node_t *freq_node_;
        T data_;

      public:
        local_node_t (KeyT key, freq_node_t &freq_node, T data)
            : key_ (key), freq_ (freq_node.freq ()), data_ (data), freq_node_ (&freq_node)
        {
        }

        size_t freq () const { return freq_; }
        KeyT key () const { return key_; }
        T data () const { return data_; }

        size_t set_freq (size_t freq) { return freq_ = freq; }
        void set_freq_node (freq_node_t &freq_node) { freq_node_ = &freq_node; }
        freq_node_t &freq_node () { return *freq_node_; }
    };

    using local_list_t    = typename std::list<local_node_t>;
    using local_list_it_t = typename std::list<local_node_t>::iterator;

    //=====================================freq_node_t=========================================
    class freq_node_t {
      public:
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

        std::list<local_node_t> &local_list () { return local_list_; }
        local_node_t &local_list_back () { return local_list_.back (); }
        local_node_t &local_list_front () { return local_list_.front (); }

        void local_emplace_front (local_node_t &&local) { local_list_.emplace_front (local); }

        bool operator== (const freq_node_t &other) { return freq_ == other.freq_; }
    };

    //=========================================lfu_t=============================================
    std::list<freq_node_t> clist_;
    std::unordered_map<KeyT, local_list_it_t> table_;

    size_t capacity_;
    size_t size_;

  public:
    lfu_t (size_t capacity) : capacity_ (capacity), size_ (0) {}
    size_t size () const { return size_; }
    size_t capacity () const { return capacity_; }

    bool full () { return capacity_ == size_; }

    // update cache according to a request key
    template <typename F> bool lookup_update (KeyT key, F slow_get_page)
    {
        auto hit = table_.find (key);

        if ( hit == table_.end () )   // 1. cache miss
        {
            if ( full () )   // if cache is full we must evict some element from cache
                erase ();

            insert (key, slow_get_page);

            return false;
        }
        // 2. cache hit

        auto found = hit->second;
        promote (found);   // promoting requested element

        return true;
    }

  private:
    // evict less popular element from cache
    void erase ()
    {
        // find to erase
        freq_node_t &last_freq = clist_.front ();
        local_node_t &to_erase = last_freq.local_list_back ();

        // erase
        table_.erase (table_.find (to_erase.key ()));
        last_freq.local_list ().pop_back ();
        size_--;

        // remove if empty
        if ( last_freq.local_list ().empty () )
            clist_.pop_front ();
    }

    // insert requested element in cache
    template <typename F> void insert (KeyT key, F slow_get_page)
    {
        // insert or find freq_node with freq == 0
        if ( clist_.empty () || clist_.front ().freq () != 1 )
            clist_.emplace_front (freq_node_t (1));
        auto &first_freq = clist_.front ();

        // insert new request into first_freq
        first_freq.local_emplace_front (local_node_t (key, first_freq, slow_get_page (key)));
        local_node_t &new_local_node = first_freq.local_list_front ();
        table_[key]                  = first_freq.local_list ().begin ();
        size_++;
    }

    // promote requested element in cache
    void promote (local_list_it_t &found)
    {
        auto &parent_node               = found->freq_node ();
        local_list_t &parent_local_list = parent_node.local_list ();

        auto pos      = std::find (clist_.begin (), clist_.end (), parent_node);
        auto next_pos = std::next (pos);

        // find new freq node for promoted local node
        auto cur_freq = found->freq () + 1;
        if ( next_pos == clist_.end () || next_pos->freq () != cur_freq )
            clist_.emplace (next_pos, cur_freq);
        auto &new_freq               = *std::next (pos);
        local_list_t &new_local_list = new_freq.local_list ();

        // actually promote
        new_local_list.splice (new_local_list.begin (), parent_local_list, found);
        found->set_freq (cur_freq);
        found->set_freq_node (new_freq);

        // remove if empty
        if ( parent_local_list.empty () )
            clist_.erase (pos);
    }
};
}   // namespace cache