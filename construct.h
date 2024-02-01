#ifndef ZZF_CONSTRUCT_H_
#define ZZF_CONSTRUCT_H_

//construct destroy

#include <new.h>
#include <new>
#include <type_traits>

namespace zzf_stl{

template <class T>
void construct(T* ptr)
{
    ::new ((void*)ptr) T();
}

template <typename T1, typename T2>
inline void construct(T1 *p,const T2& val){
    ::new((void*)p) T1(val);
}

/*
template <typename T, class... Args>
void construct(T* ptr, Args&&... args)
{
  ::new ((void*)ptr) T(zzf_stl::forward<Args>(args)...);
}*/

template <typename T>
inline void destroy(T *p){
    p -> ~T();
}

template <typename ForwardIter>
inline void destroy(ForwardIter first, ForwardIter last){
    for (; first != last; ++first)
        destroy(&*first);
}

//后续写了iter再补上
/*
template <typename ForwardIter>
inline void destroy(ForwardIter first, ForwardIter last){
    __destroy(first, last, value_type(first));
}

//判断元素的数值型别是否有trivial destructor
template <typename ForwardIter, typename T>
inline void __destroy(ForwardIter first, ForwardIter last, T*){
    //typedef typename __type_traits<T>::has_trivial_destructor trivial_destructor;
    __destroy_aux(first, last, std::is_trivially_destructible<
              typename iterator_traits<ForwardIter>::value_type>{});
}

template <typename ForwardIter>
inline void __destroy_aux(ForwardIter first, ForwardIter last, std::false_type){
    for (; first != last; ++first)
        destroy(&*first);
}

template <typename ForwardIter>
inline void __destroy_aux(ForwardIter first, ForwardIter last, std::true_type){}
*/
}
#endif

