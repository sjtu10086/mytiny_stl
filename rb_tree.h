#ifndef ZZF_RB_TREE_H_
#define ZZF_RB_TREE_H_
#include"iterator.h"
#include "alloc.h"
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
        iterator insert(base_ptr x, base_ptr y, const value_type& v);
        link_type copy(link_type x, link_type p);

        iterator erase(link_type x);
        iterator erase(link_type x);
        void clear();

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
        iterator insert_equal(const value_type& x);
    };


    


    

} // namespace zzf_stl







#endif