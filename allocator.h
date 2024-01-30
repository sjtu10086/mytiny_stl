#ifndef ZZF_ALLOCATOR_H_
#define ZZF_ALLOCATOR_H_

#include<new>
#include<cstddef>
#include<cstdlib>
#include<climits>
#include<iostream>
namespace zzf_stl{

template <typename T>
class allocator{
public:
    typedef T           value_type;
    typedef T*          pointer;
    typedef const T*    const_pointer;
    typedef T&          reference;
    typedef const T&    const_reference;
    typedef size_t      size_type;
    typedef ptrdiff_t   difference_type;

    template <typename U>
    struct rebind{
        typedef allocator<U> other;
    };
    pointer allocate(size_type n = 1);
    void deallocate(pointer p, size_type n = 1);

    void construct(pointer p, const_reference val);
    void destroy(pointer p);

    pointer address(reference val){return pointer(&val);}
    const_pointer address(const_reference val){return const_pointer(&val);}

    size_type max_size() const {return size_type(UINT_MAX/sizeof(T));}
};

template <typename T>
T* allocator<T>::allocate(size_t n){
    if (n == 0)
        return nullptr;
    T* temp = (T*) (::operator new((size_t)(n * sizeof(T))));
    return temp;
}

template <typename T>
void allocator<T>::deallocate(T* p, size_t n){
    if (p == nullptr){
        std::cout << "out of memory" << std::endl;
        return;
    }
    ::operator delete(p);
}

template <typename T>
void allocator<T>::construct(T* p, const T& val){
    new(p) T(val);
}

template <typename T>
void allocator<T>::destroy(T* p){
    p -> ~T();
}

}//end of namespace
#endif