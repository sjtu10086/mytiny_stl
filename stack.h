#ifndef ZZF_STACK_H
#define ZZF_STACK_H
#include "deque.h"
namespace zzf_stl
{
    template <class T, class Sequence = zzf_stl::deque<T>>
    class stack{
        template <class U, class Seq_U>
        friend bool operator==(const stack<U, Seq_U>& v1, const stack<U, Seq_U>& v2);

        template <class U, class Seq_U>
        friend bool operator<(const stack<U, Seq_U>& v1, const stack<U, Seq_U>& v2);
    public:
        typedef typename Sequence::value_type value_type;
        typedef typename Sequence::size_type size_type;
        typedef typename Sequence::reference reference;
    private:
        Sequence c;
    public:
        bool empty() {return c.empty();}
        size_type size() {return c.size();}
        reference top() {return c.back();}
        void push(const value_type& x) {c.push_back(x);}
        void pop() {c.pop_back();}
    };

    template <class U, class Seq_U>
    bool operator==(const stack<U, Seq_U>& v1, const stack<U, Seq_U>& v2){
        return v1.c == v2.c;
    }

    template <class U, class Seq_U>
    bool operator<(const stack<U, Seq_U>& v1, const stack<U, Seq_U>& v2){
        return v1.c < v2.c;
    }
} // namespace zzf_stl


#endif