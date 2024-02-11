#ifndef ZZF_PRIORITY_QUEUE_H
#define ZZF_PRIORITY_QUEUE_H
#include"vector.h"
#include"heap_algo.h"

namespace zzf_stl
{
    template <class T, class Sequence = zzf_stl::vector<T>, class Compare = std::less<typename Sequence::value_type>>
    class priority_queue{
    public:
        typedef typename Sequence::value_type value_type;
        typedef typename Sequence::size_type size_type;
        typedef typename Sequence::reference reference;
    protected:
        Sequence c;
        Compare comp;

        auto comp_max = [](T a, T b){return a > b;}
    public:
        priority_queue() : c() {}
        explicit priority_queue(const Compare& x) : c(), comp(x) {}


        template <class InputIterator>
        priority_queue(InputIterator first, InputIterator last, const Compare& x)
            : c(first, last), comp(x)
        {
            make_heap(c.begin(), c.end(), comp);
        }

        template <class InputIterator>
        priority_queue(InputIterator first, InputIterator last)
            : c(first, last)
        {
            make_heap(c.begin(), c.end());
        }

        bool empty() const {return c.empty();}
        size_type size() const {return c.size();}
        reference top() const {return c.front();}
        void push(const value_type& val){
            c.push_back(x);
            push_heap(c.begin(), c.end(), comp);
        }
        void pop(){
            pop_heap(c.begin(), c.end(), comp);
            c.pop_back();
        }
            

    };
} // namespace zzf_stl


#endif