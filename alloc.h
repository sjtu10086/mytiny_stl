#ifndef ZZF_ALLOC_H_
#define ZZF_ALLOC_H_

//二级空间配置器

#include <new.h>
#include <new>
#include <cstdlib>
#include "construct.h"

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

    static void construct(T* ptr) {zzf_stl::construct(ptr);}
    static void construct(T* ptr, const T& value) {zzf_stl::construct(ptr, value);}
    static void construct(T* ptr, T&& value) {zzf_stl::construct(ptr, std::move(value));}
/*
    template <class... Args>
    static void construct(T* ptr, Args&& ...args) {zzf_stl::construct(ptr, mystl::forward<Args>(args)...);}
*/
    static void destroy(T* ptr) {zzf_stl::destroy(ptr);}
    static void destroy(T* first, T* last) {zzf_stl::destroy(first, last);}
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

    //交给客户端调用
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

//仅考虑单线程
enum {ALIGN = 8};//小型区块上调边界
enum {MAX_BYTES = 128};//小型区块上限
enum {FREELISTS = MAX_BYTES / ALIGN};// free-list number

class default_alloc{
private:
    //向上取整
    static size_t ROUND_UP(size_t bytes){
        return (((bytes) + ALIGN - 1) & ~(ALIGN - 1));
    }

    union obj{
        union obj* free_list_link;
        char client_data[1];
    };
    static obj* volatile free_list[FREELISTS];

    //决定第n号free-list
    static size_t FREELIST_INDEX(size_t bytes){
        return (((bytes) + ALIGN - 1) / ALIGN - 1);
    }

    static void *refill(size_t n);
    static char* chunk_alloc(size_t size, int &nobjs);

    static char* start_free;
    static char* end_free;
    static size_t heap_size;

public:
    static void* allocate(size_t n);
    static void deallocate(void *p, size_t n);
    static void* reallocate(void *p, size_t old_sz, size_t new_sz);
};

char* default_alloc::start_free = 0;
char* default_alloc::end_free = 0;
size_t default_alloc::heap_size = 0;

//obj中都被初始化为空指针/val == 0
default_alloc::obj* volatile default_alloc::free_list[FREELISTS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void* default_alloc::allocate(size_t n){
    obj* volatile *my_free_list;//二级指针
    obj *result;
    if (n > (size_t) MAX_BYTES)
        return malloc_alloc::allocate(n);
    my_free_list = free_list + FREELIST_INDEX(n);
    result = *my_free_list;
    if (result == 0){
        void *r = refill(ROUND_UP(n));
        return r;
    }
    // result已经指向了第一个可用区块，区块的第一个字节是下一个可用区块也是下一个obj的地址，所以这句话就已经移除了第一个可用区块。
    *my_free_list = result -> free_list_link;
    return result;
}

void default_alloc::deallocate(void *p, size_t n){
    obj *q = (obj*) p;
    obj * volatile * my_free_list;
    if (n > (size_t) MAX_BYTES){
        malloc_alloc::deallocate(p, n);
        return;
    }
    my_free_list = free_list + FREELIST_INDEX(n);
    q -> free_list_link = *my_free_list;
    *my_free_list = q;
}

inline void* default_alloc::reallocate(void* p, size_t old_size, size_t new_size)
{
    deallocate(p, old_size);
    p = allocate(new_size);
    return p;
}

//从内存池中缺省取得20个新节点
void* default_alloc::refill(size_t n){
    int nobjs = 20;
    //nobjs传引用
    char* chunk = chunk_alloc(n, nobjs);
    obj * volatile * my_free_list;
    obj *result;
    obj *current_obj, *next_obj;
    int i;
    if (nobjs == 1)
        return chunk;
    my_free_list = free_list + FREELIST_INDEX(n);

    //在chunk空间中建立free list
    result = (obj*) chunk;
    *my_free_list = next_obj = (obj *)(chunk + n);
    for (i = 1; ;i++){
        current_obj = next_obj;
        next_obj = (obj*)((char*)next_obj + n);
        if (i + 1 == nobjs){
            current_obj -> free_list_link = 0;
            break;
        }
        else
            current_obj -> free_list_link = next_obj;
    }
    return result;
}

char* default_alloc::chunk_alloc(size_t size, int &nobjs){
    char * result;
    size_t total_bytes = size * nobjs;
    size_t bytes_left = end_free - start_free;
    if (bytes_left >= total_bytes){
        result = start_free;
        start_free += total_bytes;
        return result;
    }
    else if (bytes_left >= size){
        nobjs = bytes_left / size;
        total_bytes = size * nobjs;
        result = start_free;
        start_free += total_bytes;
        return result;
    }
    else{
        size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
        if (bytes_left > 0){//如果还有一点点，需求的内存不够，找一个够用的区块放进去
            obj * volatile * my_free_list = free_list + FREELIST_INDEX(bytes_left);
            ((obj*)start_free) -> free_list_link = *my_free_list;
            *my_free_list = (obj *)start_free;
        }

        start_free = (char*) malloc (bytes_to_get);
        if (start_free == 0){
            int i;
            obj * volatile * my_free_list, *p;
            for (i = size; i < MAX_BYTES; i += ALIGN){
                my_free_list = free_list + FREELIST_INDEX(i);
                p = *my_free_list;
                if (p != 0){
                    *my_free_list = p -> free_list_link;
                    start_free = (char*) p;
                    end_free = start_free + i;
                    return (chunk_alloc(size, nobjs));
                }
            }
            end_free = 0;
            start_free = (char*) malloc_alloc::allocate(bytes_to_get);
        }
        heap_size += bytes_to_get;
        end_free = start_free + bytes_to_get;
        return (chunk_alloc(size, nobjs));
    }
}


}
#endif