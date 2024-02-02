#ifndef ZZF_LIST_H_
#define ZZF_LIST_H_

#include "iterator.h"
#include "alloc.h"
//未区分头节点与普通节点
namespace zzf_stl
{
    template <class T>
    struct __list_node{
        typedef __list_node<T>* prev;
        typedef __list_node<T>* next;
        T data;
    };

    template <class T, class Ref = T&, class Ptr = T*>
    struct list_iterator : public zzf_stl::iterator<zzf_stl::bidirectional_iterator_tag, T>{
        typedef list_iterator<T, T&, T*>    iterator;
        typedef list_iterator<T, Ref, Ptr>  self;

        typedef zzf_stl::bidirectional_iterator_tag    iterator_category;
        typedef T               value_type;
        typedef Ptr             pointer;
        typedef Ref             reference;

        typedef __list_node<T>*   link_type;;
        typedef size_t          size_type;
        typedef ptrdiff_t       difference_type;

        link_type node;//指向节点的指针

        list_iterator() = default;
        list_iterator(link_type l) : node(l) {}
        list_iterator(const iterator& iter) : node(iter.node) {}

        bool operator == (const self& rhs) const{
            return this->node == rhs.node;
        }

        bool operator != (const self& rhs) const{
            return this->node != rhs.node;
        }

        reference operator*()  const { return (*node).data;}

        //标准做法
        pointer   operator->() const { return &(operator*());}

        self& operator ++(){
            node = ((*node).next);
            return *this;
        }

        self& operator ++(int){
            auto temp = *this;
            ++(*this);
            return temp;
        }

        self& operator --(){
            node = ((*node).prev);
            return *this;
        }

        self& operator --(int){
            auto temp = *this;
            --(*this);
            return temp;
        }
    };
/*
    template <class T, class Ref = T&, class Ptr = T*>
    struct list_const_iterator : public zzf_stl::iterator<zzf_stl::bidirectional_iterator_tag, T>{
        typedef list_const_iterator <T, T&, T*>    iterator;
        typedef list_const_iterator <T, Ref, Ptr>  self;

        typedef zzf_stl::bidirectional_iterator_tag    iterator_category;
        typedef T               value_type;
        typedef Ptr             pointer;
        typedef Ref             reference;

        typedef __list_node<T>*   link_type;;
        typedef size_t          size_type;
        typedef ptrdiff_t       difference_type;

        link_type node;//指向节点的指针

        list_const_iterator() = default;
        list_const_iterator(link_type l) : node(l) {}
        list_const_iterator(const list_iterator<T>& iter) : node(iter.node) {}
        list_const_iterator(const iterator& iter) : node(iter.node) {}

        bool operator == (const self& rhs) const{
            return this->node == rhs.node;
        }

        bool operator != (const self& rhs) const{
            return this->node != rhs.node;
        }

        reference operator*()  const { return (*node).data;}

        //标准做法
        pointer   operator->() const { return &(operator*());}

        self& operator ++(){
            node = ((*node).next);
            return *this;
        }

        self& operator ++(int){
            auto temp = *this;
            ++(*this);
            return temp;
        }

        self& operator --(){
            node = ((*node).prev);
            return *this;
        }

        self& operator --(int){
            auto temp = *this;
            --(*this);
            return temp;
        }
    };
    */

    template <class T, class Alloc = zzf_stl::simple_alloc<T, zzf_stl::default_alloc>>
    class list{
    protected:
        typedef __list_node<T> list_node;
    public:
        typedef list_node* link_type;
        //typedef Alloc                      allocator_type;
        //typedef Alloc                      data_allocator;
        typedef Alloc                      list_node_allocator;

        typedef typename allocator_type::value_type      value_type;
        typedef typename allocator_type::pointer         pointer;
        typedef typename allocator_type::const_pointer   const_pointer;
        typedef typename allocator_type::reference       reference;
        typedef typename allocator_type::const_reference const_reference;
        typedef typename allocator_type::size_type       size_type;
        typedef typename allocator_type::difference_type difference_type;

        typedef list_iterator<T>                         iterator;
        //typedef list_const_iterator<T>                   const_iterator;

    protected:
        link_type node;
    
    public:
        //迭代器部分函数
        iterator begin() const {return (link_type)((*node).next);}
        iterator end() const {return node;}
        bool empty() const {return node == node -> next;}
        size_type size() const {return zzf_stl::distance(begin(), end());}
        reference front() {return *begin();}
        reference bank() {return *(--end());}

        //构造与内存管理
    private:
        link_type get_node() {return list_node_allocator::allocate();}
        void put_node(link_type p) {list_node_allocator::deallocate(p);}
        link_type create_node(const T&x){
            link_type p = get_node();
            list_node_allocator::construct(&p -> data, x);
            return p;
        }
        void destroy_node(link_type p){
            list_node_allocator::destroy(&p -> data);
            put_node(p);
        }
        void empty_initialize(){
            node = get_node();
            node -> next = node;
            node -> prev = node;
        }
    public:
        list() {empty_initialize();}

        //元素操作
        void push_front(const T& x) {insert(begin(), x);}

        void push_back(const T& x) {insert(end(), x);}

        void pop_front() {erase(begin());}

        void pop_back() {iterator tmp = end(); erase(--tmp);}

        iterator insert(iterator pos, const T& x){//插入模式为：原节点位于新节点后方
            link_type tmp;
            tmp = create_node(x);
            tmp -> next = pos.node;
            tmp -> prev = pos.node -> prev;
            (pos.node -> prev) -> next = tmp;
            pos.node -> prev = tmp;
            return tmp;
        }

        iterator erase(iterator pos){
            link_type next_node = pos.node -> next;
            link_type prev_node = pos.node -> prev;
            prev_node -> next = next_node;
            next_node -> prev = prev_node;
            destroy_node(pos);
            return iterator(next_node);
        }

        
        


    };
} // namespace zzf_stl

#endif