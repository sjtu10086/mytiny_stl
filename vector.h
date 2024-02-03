#ifndef ZZF_VECTOR_H_
#define ZZF_VECTOR_H_


#include "uninitialized.h"
#include"alloc.h"
#include "iterator.h"
#include "util.h"
#include "exceptdef.h"

namespace zzf_stl
{
    template <typename T, typename Alloc = zzf_stl::default_alloc>
    class vector{
    public:
        typedef T value_type;
        typedef T*  pointer;
        typedef T*  iterator;
        typedef T&  reference;
        typedef size_t  size_type;
        typedef ptrdiff_t difference_type;
    
    protected:
        typedef simple_alloc<value_type, default_alloc> data_allocator;
        iterator start;
        iterator finish;
        iterator end_of_storage;

        void insert_aux(iterator pos, const T& x);
        void deallocate(){
            if (start)
                data_allocator::deallocate(start, end_of_storage - start);
        }
        void fill_initialize(size_type n, const T& val){
            start = alloc_and_fill(n, value);
            finish = start + n;
            end_of_storage = finish;
        }

        iterator alloc_and_fill(size_type n, const T& x){
            iterator result = data_allocator::allocate(n);
            uninitialized_fill_n(result, n, x);
            return result;
        }
    public:
        iterator begin() {return start;}
        iterator end() {return finish;}
        size_type size() {return size_type(end() - begin());}
        size_type capacity() {return size_type(end_of_storage - begin());}
        bool empty() {return (begin() == end());}
        reference operator [](size_type n) {return *(begin() + n);}

        vector() : start(0), finish(0), end_of_storage(0) {}
        vector(size_type n, const T& val) {fill_initialize(n, value);}
        vector(int n, const T& val) {fill_initialize(n, value);}
        vector(long n, const T& val) {fill_initialize(n, value);}
        explicit vector(size_type n) {fill_initialize(n, T());}

        ~vector(){
            destroy(start, finish);
            deallocate();
        }

        reference front() {return *begin();}
        reference back() {return *(end() - 1);}
        void push_back(const T& x){
            if (finish != end_of_storage){
                construct(finish, x);
                ++finish;
            }
            else    
                insert_aux(end(), x);
        }

        void pop_back(){
            --finish;
            destroy(finish);
        }

        iterator erase(iterator pos){
            if (pos + 1 != end())
                copy(pos + 1, finish, pos);     //目前没有定义，第六章见
            --finish;
            destroy(finish);
            return pos;
        }

        void resize(size_type new_size, const T& x){
            if (new_size < size())
                erase(begin() + new_size, end());
            else
                insert(end(), new_size - size(), x);
        }

        void resize(size_type new_size) {resize(new_size, T());}

        void clear() {erase(begin(), end());}

    };
} // namespace zzf_stl

#endif   