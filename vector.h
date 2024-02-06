#ifndef ZZF_VECTOR_H_
#define ZZF_VECTOR_H_


#include "uninitialized.h"
#include"alloc.h"
#include "iterator.h"
#include "util.h"
#include "exceptdef.h"

//未自主定义函数：copy copy_backward max
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
            start = alloc_and_fill(n, val);
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
        vector(size_type n, const T& val) {fill_initialize(n, val);}
        vector(int n, const T& val) {fill_initialize(n, val);}
        vector(long n, const T& val) {fill_initialize(n, val);}
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

        void insert(iterator pos, size_type n, const T&x);

        iterator erase(iterator pos){
            if (pos + 1 != end())
                std::copy(pos + 1, finish, pos);     //目前没有定义，第六章见
            --finish;
            destroy(finish);
            return pos;
        }

        iterator erase(iterator first, iterator last){
            iterator i = std::copy(last, finish, first);
            destroy(i, finish);
            finish = finish - (last - first);
            return first;
        }

        iterator find(iterator first, iterator last, const T& x){
            iterator tmp = first;
            while(tmp != last){
                if (*tmp == x)
                    return tmp;
                ++tmp;
            }
            return tmp;
        }

        void resize(size_type new_size, const T& x){
            if (new_size < size())
                erase(begin() + new_size, end());
            else
                insert(end(), new_size - size(), x);
        }

        void resize(size_type new_size) {resize(new_size, T());}

        void clear() {erase(begin(), end());}

    public://测试函数

        void void_test_out(){
            iterator cur = begin();
            while(cur != end()){
                auto tmp = cur;
                cur++;
                std::cout << *tmp << std::endl;
            }
        }

    };

    template <typename T, typename Alloc>//此处不能给alloc默认值
    void vector<T, Alloc>::insert_aux(iterator pos, const T& x){
        if (finish != end_of_storage){
            construct(finish, *(finish - 1));
            ++finish;
            T x_copy = x;
            std::copy_backward(pos, finish - 2, finish - 1);
            *pos = x_copy;
        }
        else{
            const size_type old_size = size();
            const size_type len = old_size != 0 ? 2 * old_size : 1;
            iterator new_start = data_allocator::allocate(len);
            iterator new_finish = new_start;
            try{
                new_finish = uninitialized_copy(start, pos, new_start);
                construct(new_finish, x);
                ++new_finish;
                new_finish = uninitialized_copy(pos, finish, new_finish);
            }
            catch(...){
                destroy(new_start, new_finish);
                data_allocator::deallocate(new_start, len);
                throw;
            }
            destroy(begin(), end());
            deallocate();

            start = new_start;
            finish = new_finish;
            end_of_storage = start + len;
        }
    }

    template <typename T, typename Alloc>//此处不能给alloc默认值
    void vector<T, Alloc>::insert(iterator pos, size_type n, const T&x){
        if (n != 0){
            if (size_type(end_of_storage - finish) >= n){
                T x_copy = x;
                const size_type elems_after = finish - pos;
                iterator old_finish = finish;
                //分为[finish - n, finish)和[pos, finish - n)俩部分往后移动
                if (elems_after > n){
                    uninitialized_copy(finish - n, finish, finish);
                    finish += n;
                    std::copy_backward(pos, old_finish - n, old_finish);
                    std::fill(pos, pos + n, x_copy);
                }
                //x分为两块填充
                else{
                    uninitialized_fill_n(finish, n - elems_after, x_copy);
                    finish += n - elems_after;
                    uninitialized_copy(pos, old_finish, finish);
                    finish += elems_after;
                    std::fill(pos, old_finish, x_copy);
                }
            }
            else{
                //若加入不是非常多则2倍，多则装满为止
                const size_type old_size = size();
                const size_type len = old_size + std::max(old_size, n);
                iterator new_start = data_allocator::allocate(len);
                iterator new_finish = new_start;
                try{
                    new_finish = uninitialized_copy(start, pos, new_start);
                    new_finish = uninitialized_fill_n(new_finish, n, x);
                    new_finish = uninitialized_copy(pos, finish, new_finish);
                }
                catch(...){
                    destroy(new_start, new_finish);
                    data_allocator::deallocate(new_start, len);
                    throw;
                }
                destroy(start, finish);
                deallocate();

                start = new_start;
                finish = new_finish;
                end_of_storage = start + len;
            }
        }
    }

    
} // namespace zzf_stl

#endif   