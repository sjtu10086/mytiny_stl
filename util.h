//工具类

#ifndef ZZF_UTIL_H_
#define ZZF_UTIL_H_
#include <iostream>
namespace zzf_stl{

    template <class T>
    typename std::remove_reference<T>::type&& move(T&& arg) noexcept
    {
    return static_cast<typename std::remove_reference<T>::type&&>(arg);
    }
    // swap
    template <class Tp>
    void swap(Tp& lhs, Tp& rhs)
    {
        auto tmp(zzf_stl::move(lhs));
        lhs = zzf_stl::move(rhs);
        rhs = zzf_stl::move(tmp);
    }
/*
    template <class Tp>
    void swap(Tp& lhs, Tp& rhs)
    {
        auto tmp = lhs;
        lhs = rhs;
        rhs = lhs;
    }
*/

}
#endif