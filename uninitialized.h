#ifndef ZZF_UNINITIALIZED_H_
#define ZZF_UNINITIALIZED_H_

//未完成
//学习到第六章再来修改
#include <type_traits>
#include "alloc.h"
namespace zzf_stl
{
    template <class ForwardIterator, class Size, class T>
    inline ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T& x){
        return __uninitialized_fill_n(first, n, x, std::remove_reference_t<decltype(*first)>());
    }

    template <class ForwardIterator, class Size, class T, class T1>
    inline ForwardIterator __uninitialized_fill_n(ForwardIterator first, Size n, const T& x, T1*){
        typename std::is_pod<T1>::value is_POD;
        //return __uninitialized_fill_n_aux(first, n, x, is_POD);
        return __uninitialized_fill_n_aux(first, n, x, std::false_type);
    }

    template <class ForwardIterator, class Size, class T>
    inline ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, std::true_type){
        return std::fill_n(first, n, x);
    }

    template <class ForwardIterator, class Size, class T>
    inline ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, std::false_type){
        ForwardIterator cur = first;
        for (; n > 0; ++cur){
            construct(&*cur, x);
        }
        return cur;
    }
} // namespace zzf_stl


#endif