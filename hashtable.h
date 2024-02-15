#ifndef ZZF_HASHTABLE_H_
#define ZZF_HASHTABLE_H_

#include <initializer_list>

#include "algo.h"
#include "alloc.h"
#include "vector.h"
#include "util.h"
#include "exceptdef.h"

namespace zzf_stl
{
    template <class Value>
    struct hashtable_node{
        hashtable_node* next;
        Value val;
    };

    template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc = zzf_stl::default_alloc>
    struct hashtable;

    template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
    struct hashtable_iterator{
        typedef hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc> hashtable;
        typedef hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc> iterator;
        typedef hashtable_node<Value> node;

        typedef forward_iterator_tag    iterator_category;
        typedef Value                   value_type;
        typedef ptrdiff_t               difference_type;
        typedef size_t                  size_type;
        typedef Value&                  reference;
        typedef Value*                  pointer;

        node* cur;
        hashtable* ht;

        hashtable_iterator(node* n, hashtable* tab) : cur(n), ht(tab) {}
        hashtable_iterator() {}
        reference operator*() const {return cur -> val;}
        pointer operator->() const {return &(operator*());}
        iterator& operator++();
        iterator operator++(int);
        bool operator==(const iterator& iter) const {return cur == iter.cur;}
        bool operator!=(const iterator& iter) const {return cur != iter.cur;}
    };

    template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
    hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>& 
    hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::operator++(){
        const node* old = cur;
        cur = cur -> next;
        if (!cur){
            size_type bucket = ht -> bkt_num(old -> val);
            while (!cur && ++bucket < ht -> buckets.size())
                cur = ht -> buckets[bucket];
        }
        return *this;
    }

    template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
    hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>
    hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::operator++(int){
        iterator tmp = *this;
        ++*this;
        return tmp;
    }

    template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
    class hashtable{
    public:
        typedef HashFcn hasher;
        typedef EqualKey key_equal;
        typedef size_t size_type;

    private:
        //函数对象
        hasher hash;
        key_equal equals;//判断键值相同与否
        ExtractKey get_key;//从节点中取出键值

        typedef hashtable_node<Value> node;
        typedef simple_alloc<node, Alloc> node_allocator;

        vector<node*, Alloc> buckets;
        size_type num_elements;

    public:
        static const int num_primes = 28;
        static unsigned long prime_list[num_primes] = 
        {
            53,         97,         193,            389,
            769,        1543,       3079,           6151,
            12289,      24593,      49157,          98317,
            196613,     393241,     786433,         1572869,
            3145739,    6291469,    12582917,       25165843,
            50331653,   100663319,  20136611,       402653189,
            805306457,  1610612741, 3221225473ul,   424967291ul
        }
        inline unsigned long next_prime(unsigned long n){
            const unsigned long* first = prime_list;
            const unsigned long* last = prime_list + num_primes;
            const unsigned long* pos = zzf_stl::lower_bound(first, last, n);
            return pos == last ? *(last - 1) : *pos;

        }
        inline size_type max_bucket_count() const {return prime_list[num_primes - 1];}
    public:
        hashtable(size_type n, const HashFcn& hf, const EqualKey& eql)
        : hash(hf), equals(eql), get_key(ExtractKey(), num_elements){   //ExtractKey未定义
            initialize_buckets(n);
        }

        size_type bucket_count() const {return buckets.size();}

    private:
        node* new_node(const value_type& obj){
            node* n = node_allocator::allocate();
            n -> next = nullptr;
            construct(&n -> val, obj);
        }

        void delete_node(node* n){
            destroy(&n -> val);
            node_allocator::deallocate(n);
        }

        size_type next_size(size_type n) const {return next_prime(n);}

        void initialize_buckets(size_type n){
            const size_type n_buckets = next_size(n);
            buckets.reserve(n_buckets);//需要vector中的reserve函数
            buckets.insert(buckets.end(), n_buckets, (node*) nullptr);
            num_elements = 0;
        }
    };


} // namespace zzf_stl


#endif