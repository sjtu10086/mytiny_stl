#ifndef ZZF_ALLOC_H_
#define ZZF_ALLOC_H_

//暂未进行使用

#include <new.h>
#include <new>
#include <cstdlib>

#if !defined(__THROW_BAD_ALLOC)
    #include <iostream>
    #define __THROW_BAD_ALLOC std::cerr << "out of memory" << std::endl
#endif

namespace zzf_stl{
template <typename T, typename Alloc>
class simple_alloc{
public:
    typedef T           value_type;
    typedef T*          pointer;
    typedef const T*    const_pointer;
    typedef T&          reference;
    typedef const T&    const_reference;
    typedef size_t      size_type;
    typedef ptrdiff_t   difference_type;

    template <typename U, typename Alloc2>
    struct rebind{
        typedef simple_alloc<U, Alloc2> other;
    };

    static T* allocate(size_t n) {return n == 0? 0 : (T*) Alloc::allocate(n * sizeof(T));}
    static T* allocate() {return (T*) Alloc::allocate(sizeof(T));}
    static void deallocate(T* p, size_t n) {if (n != 0) Alloc::deallocate(p, n * sizeof(T));}
    static void deallocate(T* p) {Alloc::deallocate(p, sizeof(T));}
};

//malloc_based allocator
class malloc_alloc{
private:
    static void* oom_malloc(size_t);
    static void* oom_remalloc(void*, size_t);
    static void (* malloc_alloc_oom_handler)();

public:
    static void* allocate(size_t n){
        void *result = malloc(n);
        if (result == 0)
            result = oom_malloc(n);
        return result;
    }

    static void deallocate(void* p, size_t n){
        free(p);
    }

    static void* reallocate(void* p, size_t new_size){
        void *result = realloc(p, new_size);
        if (result == 0)
            result = oom_remalloc(p, new_size);
        return result;
    }

    static void (*set_malloc_handler(void (*f)())) (){
        void (* old)() = malloc_alloc_oom_handler;
        malloc_alloc_oom_handler = f;
        return old;
    }

    static void (*get_malloc_handler()) (){
        void (* now)() = malloc_alloc_oom_handler;
        return now;
    }
};

void (* malloc_alloc::malloc_alloc_oom_handler)() = 0;

void* malloc_alloc::oom_malloc(size_t n){
    void (* my_malloc_handler)();
    void *result;
    while(true){
        my_malloc_handler = malloc_alloc_oom_handler;
        if (my_malloc_handler == 0) { __THROW_BAD_ALLOC;}
        (*my_malloc_handler)();
        result = malloc(n);
        if (result) return result;
    }
}
void* malloc_alloc::oom_remalloc(void* p, size_t n){
    void (* my_malloc_handler)();
    void *result;
    while(true){
        my_malloc_handler = malloc_alloc_oom_handler;
        if (my_malloc_handler == 0) { __THROW_BAD_ALLOC;}
        (*my_malloc_handler)();
        result = realloc(p, n);
        if (result) return result;
    }
}


}
#endif