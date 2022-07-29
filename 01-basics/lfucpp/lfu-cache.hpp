#pragma once

#include <algorithm>
#include <iterator>
#include <list>
#include <memory>
#include <unordered_map>
namespace cache
{
template <typename T, typename KeyT = int> class lfu_t {

    class freq_node_t;
    class local_node_t {
      public:
        size_t freq_;
        KeyT key_;
        std::shared_ptr<freq_node_t> freq_node_;
        T data_;

      public:
        local_node_t (KeyT key, std::shared_ptr<freq_node_t> freq_node, T data)
            : key_ (key), freq_ (freq_node->freq ()), data_ (data)
        {
            freq_node_ = freq_node;
            freq_node_->local_push_front (std::move (*this));
        }

        size_t freq () const { return freq_; }
        KeyT key () const { return key_; }
        T data () const { return data_; }

        size_t set_freq (size_t freq) { return freq_ = freq; }
        void set_freq_node (std::shared_ptr<freq_node_t> freq_node) { freq_node_ = freq_node; }
        std::shared_ptr<freq_node_t> freq_node () { return freq_node_; }
    };

    using local_list_t    = typename std::list<local_node_t>;
    using local_list_it_t = typename std::list<local_node_t>::iterator;

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

        std::list<local_node_t> &local_list () { return local_list_; }   // copy??
        local_node_t &local_list_back () { return local_list_.back (); }
        local_node_t &local_list_front () { return local_list_.front (); }

        void local_push_front (const local_node_t &local)
        {
            // std::cout << "begin local_list_.size() = " << local_list_.size () << std::endl;
            // std::cout << "data = " << local.data_ << std::endl;
            local_list_.push_front (local);
            // std::cout << "end local_list_.size() = " << local_list_.size () << std::endl;
        }
        void local_push_back (const local_node_t &local) { local_list_.push_back (local); }
        void local_pop_back () { local_list_.pop_back (); }
    };

    std::list<std::shared_ptr<freq_node_t>> clist_;
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
                std::shared_ptr<freq_node_t> &last_freq = clist_.front ();
                local_node_t &to_erase                  = last_freq->local_list_back ();
                table_.erase (table_.find (to_erase.key ()));
                last_freq->local_list ().pop_back ();

                if ( last_freq->local_list ().empty () )
                    clist_.erase (clist_.begin ());
                size_--;
            }

            if ( clist_.empty () || (clist_.front ())->freq () != 1 )
                clist_.emplace_front (std::shared_ptr<freq_node_t> (new freq_node_t));
            auto first_freq = clist_.front ();
            /*first_freq->local_push_front (*/ local_node_t (key, first_freq, slow_get_page (key));
            /*);*/
            auto new_local_node = first_freq->local_list_front ();
            table_[key]         = first_freq->local_list ().begin ();
            size_++;

            return false;
        }

        auto found       = hit->second;
        auto parent_node = found->freq_node ();
        auto pos         = std::find (clist_.begin (), clist_.end (), found->freq_node ());
        auto next_pos    = std::next (pos);

        size_t cur_freq = found->freq () + 1;

        if ( next_pos == clist_.end () || (*next_pos)->freq () != cur_freq )
            clist_.emplace (next_pos, std::shared_ptr<freq_node_t> (new freq_node_t (cur_freq)));
        auto new_freq                   = *std::next (pos);
        local_list_t &parent_local_list = parent_node->local_list ();
        parent_local_list.splice (new_freq->local_list ().begin (), parent_local_list, found, std::next (found));
        found->set_freq (cur_freq);
        found->set_freq_node (new_freq);

        if ( parent_local_list.empty () )
            clist_.remove (parent_node);
        // some code there
        return true;
    }
};
}   // namespace cache