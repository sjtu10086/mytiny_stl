#ifndef ZZF_RB_TREE_H_
#define ZZF_RB_TREE_H_
#include"iterator.h"
#include "alloc.h"
#include "util.h"
namespace zzf_stl
{
    typedef bool rb_tree_color;
    const rb_tree_color rb_tree_red = false;
    const rb_tree_color rb_tree_black = true;

    typedef struct rb_tree_node_base
    {
        typedef rb_tree_color color_type;
        typedef struct rb_tree_node_base* base_ptr;
        color_type color;
        base_ptr parent;
        base_ptr left;
        base_ptr right;

        static base_ptr minimum(base_ptr x){
            while (x -> left)
                x = x -> left;
            return x;
        }

        static base_ptr maximum(base_ptr x){
            while (x -> right)
                x = x -> right;
            return x;
        }
    } rb_tree_node_base; 

    template <class T>
    struct rb_tree_node : public rb_tree_node_base
    {
        typedef rb_tree_node<T>* link_type;
        T value;
    };

    struct rb_tree_iterator_base
    {
        typedef rb_tree_node_base::base_ptr base_ptr;
        typedef bidirectional_iterator_tag iterator_category;
        typedef ptrdiff_t difference_type;

        base_ptr node;
    

        void increment(){
            if (!(node -> right)){
                node = node -> right;
                while(node -> left){
                    node = node -> left;
                }
            }
            else{
                base_ptr y = node -> parent;
                while(node == y -> right){
                    node = y;
                    y = y -> parent;
                }
                if (node -> right != y)//防止都是nullptr，此时node即为max
                    node = y;
            }
        }

        void decrement(){
            if (node -> color == rb_tree_red && node -> parent -> parent == node){//若为end()/header
                node = node -> right;
            }
            else if(!(node -> left)){
                node = node -> left;
                while (node -> right)
                    node = node -> right;
            }
            else{
                base_ptr y = node -> parent;
                while(node == y -> left){
                    node = y;
                    y = y -> parent;
                }
                node = y;
            }
        }
    };

    template <class T, class Ref = T&, class Ptr = T*>
    struct rb_tree_iterator : public rb_tree_iterator_base{
        typedef T value_type;
        typedef Ref reference;
        typedef Ptr pointer;
        typedef rb_tree_iterator<T, T&, T*> iterator;
        typedef rb_tree_iterator<T, const T&, const T*> const_iterator;
        typedef rb_tree_iterator<T, Ref, Ptr> self;
        typedef rb_tree_node<T>* link_type;

        rb_tree_iterator() {}
        rb_tree_iterator(link_type x) {node = x;}
        rb_tree_iterator(const iterator& iter) {node = iter.node;}

        reference operator*() const {return link_type(node) -> value;}
        pointer operator->() const { return &(operator*());}
        self& operator++() {increment(); return *this;}
        self& operator--() {decrement(); return *this;}
        self operator++(int){
            self tmp = *this;
            increment();
            return tmp;
        }
        self operator++(int){
            self tmp = *this;
            decrement();
            return tmp;
        }
    };

    template <class Key, class Value, class Key_of_Value, class Compare, class Alloc = zzf_stl::default_alloc>
    class rb_tree{
    protected:
        typedef void* void_pointer;
        typedef rb_tree_node_base base_ptr;
        typedef rb_tree_node<Value> rb_tree_node;
        typedef simple_alloc<rb_tree_node, Alloc> rb_tree_node_allocator;
        typedef rb_tree_color color_type;

    public:
        typedef Key key_type;
        typedef Value value_type;
        typedef value_type* pointer;
        typedef const value_type* const_pointer;
        typedef value_type& reference;
        typedef const value_type& const_reference;
        typedef rb_tree_node* link_type;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

    protected:
        link_type get_node() {return rb_tree_node_allocator::allocate();}
        void put_node(link_type p) {return rb_tree_node_allocator::deallocate(p);}

        link_type create_node(const value_type& x){
            link_type tmp = get_node();
            construct(&tmp -> value, x);
            return tmp;
        }
        link_type clone_node(link_type p){
            link_type tmp = create_node(p -> value);
            tmp -> color = p -> color;
            tmp -> left = nullptr;
            tmp -> right = nullptr;
            return tmp;
        }
        void destroy_node(link_type p){
            destroy(&p -> value);
            put_node(p);
        }
    
    protected:
        size_type node_count;
        link_type header;
        Compare key_compare;

        link_type& root() const {return (link_type&) header -> parent;}
        link_type& left_most() const {return (link_type&) header -> left;}
        link_type& right_most() const {return (link_type&) header -> right;}

        static link_type& left(link_type x) {return (link_type&)(x -> left);}
        static link_type& right(link_type x) {return (link_type&)(x -> right);}
        static link_type& parent(link_type x) {return (link_type&)(x -> parent);}
        static reference value(link_type x) {return x -> value;}
        static const Key& key(link_type x) {return Key_of_Value()(value(x));}
        static color_type& color(link_type x) {return (color_type&)(x -> color);}

        static link_type& left(base_ptr x) {return (link_type&)(x -> left);}
        static link_type& right(base_ptr x) {return (link_type&)(x -> right);}
        static link_type& parent(base_ptr x) {return (link_type&)(x -> parent);}
        static reference value(base_ptr x) {return (link_type(x)) -> value;}
        static const Key& key(base_ptr x) {return Key_of_Value()(value((link_type(x))));}
        static color_type& color(base_ptr x) {return (color_type&)((link_type(x)) -> color);}

        static link_type minimum(link_type x) {return (link_type) rb_tree_node_base::minimum();}
        static link_type maximum(link_type x) {return (link_type) rb_tree_node_base::maximum();}

    public:
        typedef rb_tree_iterator<value_type, reference, pointer> iterator;

    private:
        iterator __insert(base_ptr x, base_ptr y, const value_type& v);
        link_type __copy(link_type x, link_type p);
        iterator __erase(iterator hint);

        void init(){
            header = get_node();
            color(header) = rb_tree_red;
            root() = nullptr;
            left_most() = header;
            right_most() = header;
        }

    public:
        rb_tree(const Compare & comp = Compare()) : node_count(0), key_compare(comp) {init();}
        ~rb_tree() {clear(); put_node(header);}
        rb_tree<Key, Value, Key_of_Value, Compare, Alloc>& operator= (const rb_tree<Key, Value, Key_of_Value, Compare, Alloc>&x);

    public:
        Compare key_comp() const {return key_compare;}
        iterator begin() {return left_most();}
        iterator end() {return header;}
        bool empty() {return node_count == 0;}
        size_type size() {return node_count;}
        size_type max_size() const {return size_type(-1);}

    public:
        pair<iterator, bool> insert_unique(const value_type& x);
        iterator insert_equal(const Value& x);
        iterator find(const Key& k);
        size_type erase_multi(const key_type& key);
        size_type erase_unique(const key_type& key);
        void erase(iterator first, iterator last);
        void clear();
    };

    template <class Key, class Value, class Key_of_Value, class Compare, class Alloc>
    typename rb_tree<Key, Value, Key_of_Value, Compare, Alloc>::iterator 
    rb_tree<Key, Value, Key_of_Value, Compare, Alloc>::find(const Key& k){
        link_type y = header;
        link_type x = root();

        while (x)
        {
            if (!key_compare(key(x), k)){
                y = x, x = x->left;
            }
            else
                x = x -> right;
        }

        iterator j = iterator(y);
        return (j == end() || key_compare(k, key(j.node))) ? end() : j;
    }

    template <class Key, class Value, class Key_of_Value, class Compare, class Alloc>
    typename rb_tree<Key, Value, Key_of_Value, Compare, Alloc>::iterator 
    rb_tree<Key, Value, Key_of_Value, Compare, Alloc>::insert_equal(const Value& v){
        link_type y = header;
        link_type x = root();
        while(x){
            y = x;
            x = key_compare(Key_of_Value()(v), key(x)) ? left(x) : right(x);
        }
        return __insert(x, y, v);
    }

    /*
        如果comp为true,而且刚好j又是整个树的最小值，这说明当前要插入的值比整个树的最小值还要小。于是就直接调用__insert(x, y, v) 函数进行插入。整个辅助函数，我们后面深入。

如果comp为true,但是y不是指向最小值。那么就让 j自减一下，此时 j指向的元素,此时有关系： 
j.value<y.value 
v>=j.value

    */
    template <class Key, class Value, class Key_of_Value, class Compare, class Alloc>
    pair<typename rb_tree<Key, Value, Key_of_Value, Compare, Alloc>::iterator, bool>
    rb_tree<Key, Value, Key_of_Value, Compare, Alloc>::insert_unique(const value_type& v){
        link_type y = header;
        link_type x = root();
        bool comp = true;
        while(x){
            y = x;
            comp = key_compare(Key_of_Value()(v), key(x));
            x = comp ? left(x) : right(x);
        }

        iterator j = iterator(y);
        if (comp){//离开循环时comp为真，即应插入左侧
            if (j == begin())
                return pair<iterator, bool>(__insert(x, y, v), true);
            else
                --j;
        }

        if (key_compare(key(j.node), Key_of_Value()(v)))
            return pair<iterator, bool>(__insert(x, y, v), true);
        return pair<iterator, bool>(j, false);

    }

    inline void __rb_tree_rotate_left(rb_tree_node_base*x, rb_tree_node_base*& root){
        rb_tree_node_base* y = x -> right;
        x -> right = y -> left;
        if (y -> left)
            y -> left -> parent = x;

        y -> parent = x -> parent;
        if (x == root)
            root = y;
        else if (x == x -> parent -> left)
            x -> parent -> left = y;
        else
            x -> parent -> right = y;
        y -> left = x;
        x -> parent = y;
    }

    inline void __rb_tree_rotate_right(rb_tree_node_base*x, rb_tree_node_base*& root){
        rb_tree_node_base* y = x -> right;
        x -> left = y -> right;
        if (y -> right)
            y -> right -> parent = x;
        y -> parent = x -> parent;
        if (x == root)
            root = y;
        else if (x == x -> parent -> right)
            x -> parent -> right = y;
        else    
            x -> parent -> left = y;
        y -> right = x;
        x -> parent = y;  
    }

    inline void __rb_tree_rebanlance(rb_tree_node_base* x, rb_tree_node_base*& root){//慢慢理解吧
        x -> color = rb_tree_red;
        while (x != root && x -> parent -> color == rb_tree_red){//父节点为红
            if (x -> parent == x -> parent -> parent -> left){
                rb_tree_node_base* y = x -> parent -> parent -> right;

                if (y && y -> color == rb_tree_red){//叔叔节点存在并且为红
                    x -> parent -> color = rb_tree_black;
                    y -> color = rb_tree_black;
                    x -> parent -> parent -> color = rb_tree_red;
                    x = x -> parent -> parent;
                }

                else{//叔叔节点为黑/空
                    if (x == x -> parent -> right){
                        x = x -> parent;
                        __rb_tree_rotate_left(x, root);
                    }
                    x -> parent -> color = rb_tree_black;
                    x -> parent -> parent -> color = rb_tree_red;
                    __rb_tree_rotate_right(x -> parent -> parent, root);
                }
            }
            else{
                rb_tree_node_base* y = x -> parent -> parent -> left;

                if (y && y -> color == rb_tree_red){
                    x -> parent -> color = rb_tree_black;
                    y -> color = rb_tree_black;
                    x -> parent -> parent -> color = rb_tree_red;
                    x = x -> parent -> parent;
                }
                else{
                    if (x == x -> parent -> left){
                        x = x -> parent;
                        __rb_tree_rotate_right(x, root);
                    }
                    x -> parent -> color = rb_tree_black;
                    x -> parent -> parent -> color = rb_tree_red;
                    __rb_tree_rotate_left(x -> parent -> parent, root);
                }
            }
        }
        root -> color = rb_tree_black;
    }

    template <class Key, class Value, class Key_of_Value, class Compare, class Alloc>
    typename rb_tree<Key, Value, Key_of_Value, Compare, Alloc>::iterator
    rb_tree<Key, Value, Key_of_Value, Compare, Alloc>::__insert(base_ptr x_, base_ptr y_, const value_type& v){
        link_type x = (link_type) x_;
        link_type y = (link_type) y_;
        link_type z;
        //Key_of_Value是一个函数对象
        if (y == header || x != 0 || key_compare(Key_of_Value()(v)), key(y)){
            z = create_node(v);
            if (y == header){
                root() = z;
                rightmost() = z;
            }
            else if (y == leftmost())
                leftmost() = z;
        }
        else{
            z = create_node(v);
            right(y) = z;
            if (y == rightmost())
                rightmost() = z;
        }
        parent(z) = y;
        left(z) = nullptr;
        right(z) = nullptr;
        __rb_tree_rebanlance(z, header -> parent);
        ++node_count;
        return iterator(z);
    }



    

} // namespace zzf_stl







#endif