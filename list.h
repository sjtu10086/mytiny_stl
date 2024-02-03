#ifndef ZZF_LIST_H_
#define ZZF_LIST_H_

#include <iostream>
#include "iterator.h"
#include "alloc.h"
//未区分头节点与普通节点
namespace zzf_stl
{
    template <class T>
    struct __list_node{
        __list_node<T>* prev;
        __list_node<T>* next;
        //void* prev;
        //void* next;
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

        operator link_type(){
            return node;
        }

        self& operator ++(){
            node = (*node).next;
            return *this;
        }

        self operator ++(int){
            auto temp = *this;
            ++(*this);
            return temp;
        }

        self& operator --(){
            node = (*node).prev;
            return *this;
        }

        self operator --(int){
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

    template <class T, class Alloc = zzf_stl::default_alloc>
    class list{
    protected:
        typedef __list_node<T> list_node;
    public:
        typedef list_node* link_type;
        //typedef Alloc                      allocator_type;
        typedef zzf_stl::simple_alloc<T, Alloc>          data_allocator;
        typedef zzf_stl::simple_alloc<list_node, Alloc>  list_node_allocator;

        typedef list_iterator<T>                         iterator;
        typedef typename iterator::size_type     size_type;
        typedef typename iterator::reference     reference;

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
    protected:
        link_type get_node() {return list_node_allocator::allocate();}
        void put_node(link_type p) {list_node_allocator::deallocate(p);}
        link_type create_node(const T&x){
            link_type p = get_node();
            data_allocator::construct(&p -> data, x);
            return p;
        }
        void destroy_node(link_type p){
            data_allocator::destroy(&p -> data);
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

        iterator find(iterator first, iterator last, const T& x){
            iterator tmp = first;
            while(tmp != last){
                if (*tmp == x)
                    return tmp;
                ++tmp;
            }
            return tmp;
        }

        iterator insert(iterator pos, const T& x){//插入模式为：原节点位于新节点后方
            link_type tmp;
            tmp = create_node(x);
            tmp -> next = pos.node;
            tmp -> prev = pos.node -> prev;
            pos.node -> prev -> next = tmp;
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

        void clear(){
            link_type cur = node -> next;
            while(cur != node){
                link_type tmp = cur;
                cur = cur -> next;
                destroy_node(tmp);
            }
            node -> next = node;
            node -> prev = node;
        }
        
        void remove(const T& val){
            iterator first = begin();
            iterator last = end();
            while(first != last){
                auto next = first;
                ++next;
                if (*first == val)
                    erase(first);
                first = next;
            }
        }

        void unique(){
            iterator first = begin();
            iterator last = end();
            if (first == last)
                return;
            auto next = first;
            while(++next != last){
                if (*first == *next)
                    erase(next);
                else    
                    first = next;
                next = first;
            }
        }

        void splice(iterator pos, list &x){
            if (!x.empty())
                transfer(pos, x.begin(), x.end());
        }

        void splice(iterator pos, list&, iterator i){
            auto j = i;
            ++j;
            if(pos == i || pos == j)
                return;
            transfer(pos, i, j);
        }

        void splice(iterator pos, list&, iterator first, iterator last){
            if (first != last)
                transfer(pos, first, last);
        }

        void reverse(){
            if (node -> next == node || node -> next -> next == node)
                return;
            auto first = begin();
            ++first;
            while(first != end()){
                iterator old = first;
                ++first;
                transfer(begin(), old, first);
            }
        }
        //暂未实现，等我看完排序
        void sort();
        void merge();
    protected://内部调用函数
        //将[first, last)中的所有元素移动到pos之前
        //测试完成
        void transfer(iterator pos, iterator first, iterator last){
            if (pos != last){
                last.node -> prev -> next = pos.node;//C
                first.node -> prev -> next = last.node;//A
                pos.node -> prev -> next = first.node;//B
                link_type tmp = pos.node->prev;
                pos.node->prev = last.node->prev;
                last.node->prev = first.node->prev;
                first.node->prev = tmp;
            }
        }

        //void swap(iterator lhs&, )




    public://测试函数

        void void_test_out(){
            link_type cur = node -> next;
            while(cur != node){
                link_type tmp = cur;
                cur = cur -> next;
                std::cout << tmp->data << std::endl;
            }
        }
    };
} // namespace zzf_stl

#endif