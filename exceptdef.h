#ifndef ZZF_EXCEPTDEF_H_
#define ZZF_EXCEPTDEF_H_

#include <stdexcept>

#include <cassert>

namespace zzf_stl
{

    inline void debug(bool expr)
    {
        assert(expr);
    }

    inline void throw_length_error_if(bool expr, const std::string& what)
    {
        if (expr) throw std::length_error(what);
    }

    inline void throw_out_of_range_if(bool expr, const std::string& what)
    {
        if (expr) throw std::out_of_range(what);
    }

    inline void throw_runtime_error_if(bool expr, const std::string& what)
    {
        if (expr) throw std::runtime_error(what);
    }

} // namepsace zzf_stl

#endif // !ZZF_EXCEPTDEF_H_