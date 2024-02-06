#ifndef ZZF_UNINITIALIZED_H_
#define ZZF_UNINITIALIZED_H_

//未完成
//学习到第六章再来修改
#include <type_traits>
#include "alloc.h"
namespace zzf_stl
{
    //uninitialized_fill
    template <class ForwardIterator, class T>
    inline void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x){
        __uninitialized_fill(first, last, x, std::remove_reference_t<decltype(*first)>());
    }

    template <class ForwardIterator, class T, class T1>
    inline void __uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x, T1*){
        typedef typename std::is_pod<T1>::value is_POD;
        __uninitialized_fill_aux(first, last, x, is_POD{});
    }

    template <class ForwardIterator, class T>
    inline void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, std::true_type){
        std::fill(first, last, x);
    }

    template <class ForwardIterator, class T>
    inline void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, std::false_type){
        ForwardIterator cur = first;
        for (; cur != last; ++cur){
            construct(&*cur, x);
        }
    }


    //------------------uninitialized_fill_n-----------------------------------------------------------
    /*
    template <class ForwardIterator, class Size, class T>
    inline ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T& x){
        return __uninitialized_fill_n(first, n, x, std::remove_reference_t<decltype(*first)>());
    }

    template <class ForwardIterator, class Size, class T, class T1>
    inline ForwardIterator __uninitialized_fill_n(ForwardIterator first, Size n, const T& x, T1*){
        typedef typename std::is_pod<T1>::value is_POD;
        return __uninitialized_fill_n_aux(first, n, x, is_POD{});
    }*/

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

    template <class ForwardIterator, class Size, class T>
    inline ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T& x){
        return __uninitialized_fill_n_aux(first, n, x, std::is_trivially_copy_assignable<
                                                                typename iterator_traits<ForwardIterator>::
                                                                value_type>{});
    }

    //---------------------uninitialized_copy--------------------------------------------------------
    template <class InputIterator, class ForwardIterator>
    inline ForwardIterator __uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, std::true_type){
        return std::copy(first, last, result);
    }

    template <class InputIterator, class ForwardIterator>
    inline ForwardIterator __uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, std::false_type){
        ForwardIterator cur = first;
        for (; first != last; ++cur, ++first){
            construct(&*cur, &*first);
        }
        return cur;
    }
    //不知道为什么仿造前面的做法有问题
    template <class InputIterator, class ForwardIterator>
    inline ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result){
        return __uninitialized_copy_aux(first, last, result, std::is_trivially_copy_assignable<
                                                                typename iterator_traits<ForwardIterator>::
                                                                value_type>{});
    }

    inline char* uninitialized_copy(const char* first, const char* last, char* result){
        memmove(result, first, last - first);
        return result + (last - first);
    }

    inline wchar_t* uninitialized_copy(const wchar_t* first, const wchar_t* last, wchar_t* result){
        memmove(result, first, (last - first) * sizeof(wchar_t));
        return result + (last - first);
    }


} // namespace zzf_stl


#endif