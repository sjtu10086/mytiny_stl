#ifndef ZZF_DEQUE_H_
#define ZZF_DEQUE_H_

#include "uninitialized.h"
#include"alloc.h"
#include "iterator.h"//先放这儿吧
#include "util.h"
#include "exceptdef.h"

#ifndef DEQUE_MAP_INIT_SIZE
#define DEQUE_MAP_INIT_SIZE 8
#endif

namespace zzf_stl
{
    template <class T, class Ref, class Ptr, size_t Bufsiz>//每个迭代器控制map上的一个buf？
    struct __deque_iterator{    //未继承
    typedef __deque_iterator<T, T&, T*, Bufsiz> iterator;
    typedef __deque_iterator<T, const T&, const T*, Bufsiz> const_iterator;

    static size_t buffer_size() {return __deque_buf_size(BufSiz, sizeof(T));}

    typedef random_access_iterator_tag iterator_category;
    typedef T value_type;
    typedef Ptr pointer;
    typedef Ref reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef T** map_pointer;
    typedef __deque_iterator self;


    T* cur;
    T* first;//first前有备用空间
    T* last;//last后有备用空间，push时可以使用
    map_pointer node;//当前迭代器指向的buf，每个node对应一个buf

    void set_node(map_pointer new_node){
        node = new_node;
        first = *node;
        last = first + difference_type(buffer_size());
    }

    reference operator*() const {return *cur;}
    reference operator ->() const {return &(operator*());}

    difference_type operator-(const self& x) const {
        //相差的整个的buf + 两段小的
        return difference_type(buffer_size()) * (node - x.node - 1) + (cur - first) + (x.last - x.cur);
    }

    self& operator++(){
        ++cur;
        if (cur == last){
            set_node(node + 1);
            cur = first;
        }
        return *this;
    }

    self operator++(int){
        self tmp = *this;
        ++*this;
        return tmp;
    }

    self& operator--(){
        if (cur == first){
            set_node(node - 1);
            cur = last;
        }
        --cur;
        return *this;
    }

    self operator--(int){
        self tmp = *this;
        --*this;
        return tmp;
    }

    self& operator+=(difference_type n){
        difference_type offset = n + (cur - first);//相对于first偏移量
        if (offset >= 0 && offset < difference_type(buffer_size()))
            cur += n;
        else{
            difference_type node_offset = offset > 0 ? offset / difference_type(buffer_size()) 
                                                        : - difference_type((-offset - 1) / buffer_size()) + 1;
            set_node(node + node_offset);
            cur = first + (offset - node_offset * difference_type(buffer_size()));
        }
        return *this;
    }
    self operator+(difference_type n) const {
        self tmp = * this;
        return tmp += n;
    }
    self operator-=(difference_type n) {return *this += -n;}
    self operator-(difference_type n) const {
        self tmp = * this;
        return tmp -= n;
    }
    reference operator [](difference_type n) const {return *(*this + n);}

    bool operator== (const self& x) const {return cur == x.cur;}
    bool operator!= (const self& x) const {return !(*this == x);}
    bool operator< (const self& x) const {return (node == x.node ? (cur < x,cur) : (node < x.node);)}

    inline size_t __deque_buf_size(size_t n, size_t sz){//Bufsiz为0时即默认使用512 bytes缓冲
        return n != 0 ? n : (sz < 512 ? size_t(512 / sz) : 1);
    }


    };

    template <class T, class Alloc = zzf_stl::default_alloc, size_t BufSiz = 0>
    class deque{
    public:
        typedef T value_type;
        typedef value_type* pointer;
        typedef value_type& reference;
        typedef ptrdiff_t difference_type;
        typedef size_t size_type;

        typedef __deque_iterator<T, T&, T*, BufSiz> iterator;
    
    protected:
        typedef pointer* map_pointer;

        iterator start;
        iterator finish;
        map_pointer map;
        size_type map_size;

    public:
        iterator begin() {return start;}
        iterator end() {return finish;}
        reference operator[] (size_type n){return start[difference_type(n);]}
        reference front() {return *start;}
        reference back() {  //后尝试改为 return *(finish - 1);
            iterator tmp = finish;
            --tmp;
            return *tmp;
        }

        size_type size() const {return finish - start;}
        size_type max_size() const {return size_type(-1);}
        bool empty() const {return finish == start;}

    protected:
        typedef simple_alloc<value_type, Alloc> data_allocator;
        typedef simple_alloc<pointer, Alloc> map_allocator;

        deque(int n, const value_type& val) : start(), finish(), map(0), map_size(0) {fill_initialize(n, val);}

        void fill_initialize(size_type n, const value_type& val);
        void create_map_and_nodes(size_type num_elements);
        void push_back_aux(const value_type& t);
        void push_front_aux(const value_type& t);
        void reverse_map_at_back(size_type);
        void reverse_map_at_front(size_type);
        void reallocate_map(size_type, bool);
    
    public:
        void push_back(const value_type& t);
        void push_front(const value_type& t);
    };

    template <class T, class Alloc, size_t BufSiz>
    void deque<T, Alloc, BufSiz>::fill_initialize(size_type n, const value_type& val){
        create_map_and_nodes(n);    //产生好deque的相关结构
        map_pointer cur;
        try{
            for (cur = start.node; cur < finish.node; ++cur)
                uninitialized_fill(*cur, *cur + buffer_size(), val);
            uninitialized_fill(finish.first, finish.cur, val);
        }
        catch(...){
            
        }
    }   

    template <class T, class Alloc, size_t BufSiz>
    void deque<T, Alloc, BufSiz>::create_map_and_nodes(size_type num_elements){
        //节点数 = 元素个数/缓冲区大小 + 1
        //刚好整除时多分配一个节点
        size_type num_nodes = num_elements / __deque_iterator::buffer_size() + 1;
        map_size = max(static_cast<size_type>(DEQUE_MAP_INIT_SIZE), num_nodes + 2);
        map = map_allocator::allocate(map_size);
        map_pointer nstart = map + (map_size - num_nodes) / 2;
        map_pointer nfinish = nstart + num_nodes - 1;
        map_pointer cur;
        try{
            for (cur = nstart, cur <= nfinish; cur++){
                *cur = data_allocator::allocate(buffer_size());
            }
        }
        catch(...){
        }
        start.set_node(nstart);
        finish.set_node(nfinish);
        start.cur = start.first;
        finish.cur = finish.first + num_elements % buffer_size();
    }

    template <class T, class Alloc, size_t BufSiz>
    void deque<T, Alloc, BufSiz>::push_back(const value_type& t){
        if (finish.cur != finish.last - 1){
            construct(finish.cur, t);
            ++finish.cur;
        }
        else{
            push_back_aux(t);
        }
    }

    template <class T, class Alloc, size_t BufSiz>
    void deque<T, Alloc, BufSiz>::push_back_aux(const value_type& t){
        value_type t_copy = t;
        reverse_map_at_back();
        *(finish.node + 1) = data_allocator::allocate(buffer_size());
        try{
            construct(finish.cur, t_copy);
            finish.set_node(finish.node + 1);
            finish.cur = finish.first;
        }
        catch(...){

        }
    }

    template <class T, class Alloc, size_t BufSiz>
    void deque<T, Alloc, BufSiz>::push_front(const value_type& t){
        if (start.cur != finish.first){
            construct(start.cur - 1, t);
            --finish.cur;
        }
        else{
            push_front_aux(t);
        }
    }

    template <class T, class Alloc, size_t BufSiz>
    void deque<T, Alloc, BufSiz>::push_front_aux(const value_type& t){
        value_type t_copy = t;
        reverse_map_at_front();
        *(start.node - 1) = data_allocator::allocate(buffer_size());
        try{
            start.set_node(start.node - 1);
            start.cur = start.last - 1;
            construct(start.cur, t_copy);
        }
        catch(...){
            start.set_node(start.node + 1);
            start.cur = start.first;
            //deallocate_node
        }
    }

    template <class T, class Alloc, size_t BufSiz>
    void deque<T, Alloc, BufSiz>::reverse_map_at_back(size_type nodes_to_add = 1){
        if (nodes_to_add + 1 > map_size - (finish.node - map))
            reallocate_map(nodes_to_add, false);
    }

    template <class T, class Alloc, size_t BufSiz>
    void deque<T, Alloc, BufSiz>::reverse_map_at_front(size_type nodes_to_add = 1){
        if (nodes_to_add > start.node - map)
            reallocate_map(nodes_to_add, true);
    }

    //没写完，但是大脑停止运转了
    template <class T, class Alloc, size_t BufSiz>
    void deque<T, Alloc, BufSiz>::reallocate_map(size_type nodes_to_add, bool add_at_front){
        size_type old_num_nodes = finish.node - start.node + 1;
        size_type new_num_nodes = old_num_nodes + nodes_to_add;

        map_pointer new_nstart;
        if (map_size > 2 * new_num_nodes){

        }
    }

} // namespace zzf_stl



#endif