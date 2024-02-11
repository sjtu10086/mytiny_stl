#ifndef ZZF_ALGOOTHER_H
#define ZZF_ALGOOTHER_H

namespace zzf_stl
{
    template <class InputIter, class T>
    InputIter
    find(InputIter first, InputIter last, const T& value)
    {
    while (first != last && *first != value)
        ++first;
    return first;
    }

} // namespace zzf_stl


#endif