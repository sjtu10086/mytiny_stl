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
    template <class T, class Ref, class Ptr, size_t BufSiz>//每个迭代器控制map上的一个buf？
    struct __deque_iterator{    //未继承
    typedef __deque_iterator<T, T&, T*, BufSiz> iterator;
    typedef __deque_iterator<T, const T&, const T*, BufSiz> const_iterator;

    static size_t buffer_size() {return __deque_buf_size(BufSiz, sizeof(T));}

    typedef random_access_iterator_tag iterator_category;
    typedef T value_type;
    typedef Ptr pointer;
    typedef Ref reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef T** map_pointer;
    typedef __deque_iterator self;


    T* cur;//尾后
    T* first;
    T* last;
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
    bool operator< (const self& x) const {return (node == x.node ? (cur < x.cur) : (node < x.node));}

    inline static size_t __deque_buf_size(size_t n, size_t sz){//Bufsiz为0时即默认使用512 bytes缓冲
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
        deque() : start(), finish(), map(0), map_size(0) {fill_initialize(0, value_type());}
        deque(int n, const value_type& val) : start(), finish(), map(0), map_size(0) {fill_initialize(n, val);}
        iterator begin() {return start;}
        iterator end() {return finish;}
        reference operator[] (size_type n){return start[difference_type(n)];}
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

        void fill_initialize(size_type n, const value_type& val);
        void create_map_and_nodes(size_type num_elements);
        void push_back_aux(const value_type& t);
        void push_front_aux(const value_type& t);
        void pop_back_aux();
        void pop_front_aux();
        void reverse_map_at_back(size_type nodes_to_add = 1);
        void reverse_map_at_front(size_type nodes_to_add = 1);
        void reallocate_map(size_type, bool);
        size_type buffer_size() {return __deque_iterator<T, T&, T*, BufSiz>::buffer_size();}
    
    public:
        void push_back(const value_type& t);
        void push_front(const value_type& t);
        void pop_back();
        void pop_front();
        void clear();
        iterator erase(iterator);
        iterator erase(iterator, iterator);
        iterator insert(iterator, const T&);



    public:
        void void_test_out(){
            iterator cur = begin();
            while(cur != end()){
                auto tmp = cur;
                cur++;
                std::cout << *tmp << std::endl;
            }
        }
    };

//*************************init相关******************************************************************

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
        size_type num_nodes = num_elements / buffer_size() + 1;
        map_size = std::max(static_cast<size_type>(DEQUE_MAP_INIT_SIZE), num_nodes + 2);
        map = map_allocator::allocate(map_size);
        map_pointer nstart = map + (map_size - num_nodes) / 2;
        map_pointer nfinish = nstart + num_nodes - 1;
        map_pointer cur;
        try{
            for (cur = nstart; cur <= nfinish; cur++){
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

//****************************************push相关******************************************************
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
        if (start.cur != start.first){
            construct(start.cur - 1, t);
            --start.cur;
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

//****************************************pop相关******************************************************
    template <class T, class Alloc, size_t BufSiz>
    void deque<T, Alloc, BufSiz>::pop_back(){
        if (finish.cur != finish.first){//删除之后还有
            --finish.cur;//cur是尾后指针
            destroy(finish.cur);
        }
        else{
            pop_back_aux();
        }
    }

    template <class T, class Alloc, size_t BufSiz>
    void deque<T, Alloc, BufSiz>::pop_back_aux(){
        data_allocator::deallocate(finish.first);
        finish.set_node(finish.node - 1);
        finish.cur = finish.last - 1;
        destroy(finish.cur);
    }

    template <class T, class Alloc, size_t BufSiz>
    void deque<T, Alloc, BufSiz>::pop_front(){
        if (start.cur != start.last - 1){
            destroy(start.cur);
            ++start.cur;
        }
        else{
            pop_front_aux();
        }
    }

    template <class T, class Alloc, size_t BufSiz>
    void deque<T, Alloc, BufSiz>::pop_front_aux(){
        destroy(start.cur);
        data_allocator::deallocate(start.first);
        start.set_node(start.node + 1);
        start.cur = start.first;
    }
//***************************************reallocate*************************************************
    template <class T, class Alloc, size_t BufSiz>
    void deque<T, Alloc, BufSiz>::reverse_map_at_back(size_type nodes_to_add){
        if (nodes_to_add + 1 > map_size - (finish.node - map))
            reallocate_map(nodes_to_add, false);
    }

    template <class T, class Alloc, size_t BufSiz>
    void deque<T, Alloc, BufSiz>::reverse_map_at_front(size_type nodes_to_add){
        if (nodes_to_add > start.node - map)
            reallocate_map(nodes_to_add, true);
    }

    template <class T, class Alloc, size_t BufSiz>
    void deque<T, Alloc, BufSiz>::reallocate_map(size_type nodes_to_add, bool add_at_front){
        size_type old_num_nodes = finish.node - start.node + 1;
        size_type new_num_nodes = old_num_nodes + nodes_to_add;

        map_pointer new_nstart;
        if (map_size > 2 * new_num_nodes){
            new_nstart = map + (map_size - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
            if (new_nstart < start.node)
                std::copy(start.node, finish.node + 1, new_nstart);
            else
                std::copy_backward(start.node, finish.node + 1, new_nstart + old_num_nodes);
        }
        else{
            size_type new_map_size = map_size + std::max(map_size, nodes_to_add) + 2;
            map_pointer new_map = map_allocator::allocate(new_map_size);
            new_nstart = new_map + (new_map_size - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
            std::copy(start.node, finish.node + 1, new_nstart);
            map_allocator::deallocate(map, map_size);
            map = new_map;
            map_size = new_map_size;
        }

        start.set_node(new_nstart);
        finish.set_node(new_nstart + old_num_nodes - 1);
    }

//************************************其余一些功能函数********************************
    template <class T, class Alloc, size_t BufSiz>
    void deque<T, Alloc, BufSiz>::clear(){
        //除头尾之外的缓冲区
        for (map_pointer node = start.node + 1; node < finish.node; ++node){
            destroy(*node, *node + buffer_size());
            data_allocator::deallocate(*node, buffer_size());
        }

        if (start.node != finish.node){
            destroy(start.cur, start.last);
            destroy(finish.first, finish.cur);
            data_allocator::deallocate(finish.first, buffer_size());
        }
        else{
            destroy(start.cur, finish.cur);
        }
        finish = start;
    }

    template <class T, class Alloc, size_t BufSiz>
    deque<T, Alloc, BufSiz>::iterator deque<T, Alloc, BufSiz>::erase(iterator pos){
        iterator next = pos;
        ++next;
        difference_type index = pos - start;
        if (index < (size() >> 1)){
            std::copy_backward(start, pos, next);
            pop_front();
        }
        else{
            std::copy(next, finish, pos);
            pop_back();
        }
        return start + index;
    }

    template <class T, class Alloc, size_t BufSiz>
    deque<T, Alloc, BufSiz>::iterator deque<T, Alloc, BufSiz>::erase(iterator first, iterator last){
        if (first == start && last == finish){
            clear();
            return finish;
        }
        else{
            difference_type n = last - first;
            difference_type elements_before = first - start;
            if (elements_before < (size() - n) / 2){
                std::copy_backward(start, first, last);
                iterator new_start = start + n;
                destroy(start, new_start);
                for (map_pointer cur = start.node; cur < new_start.node; ++cur)
                    data_allocator::deallocate(*cur, buffer_size());//deallocate以buffer为单位
                start = new_start;
            }
            else{
                std::copy(last, finish, first);
                iterator new_finish = finish - n;
                destroy(new_finish, finish);
                for (map_pointer cur = new_finish.node + 1; cur <= finish.node; ++cur)
                    data_allocator::deallocate(*cur, buffer_size());
                finish = new_finish;
            }
            return start + elements_before;
        }
    }

    template <class T, class Alloc, size_t BufSiz>
    deque<T, Alloc, BufSiz>::iterator deque<T, Alloc, BufSiz>::insert(iterator pos, const T& x){
        if (pos.cur == start.cur){
            push_front(x);
            return start;
        }
        else if(pos.cur == finish.cur){
            push_back(x);
            iterator tmp = finish;
            --tmp;
            return tmp;
        }
        else{
            difference_type index = pos - start;
            value_type x_copy = x;
            if (index < size() / 2){
                push_front(front());
                iterator front1 = start;
                ++front1;
                iterator front2 = front1;
                ++front2;
                pos = start + index;
                iterator pos1 = pos;
                ++pos1;
                std::copy(front2, pos1, front1);
            }
            else{
                push_back(back());
                iterator back1 = finish;
                --back1;
                iterator back2 = back1;
                --back2;
                pos = start + index;
                std::copy_backward(pos, back2, back1);
            }
            *pos = x_copy;
            return pos;
        }
    }

} // namespace zzf_stl



#endif