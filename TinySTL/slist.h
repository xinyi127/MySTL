#ifndef TINYSTL_SLIST_H
#define TINYSTL_SLIST_H

// 该头文件包含一个模板类 slist
// slist: 单向链表

#include <initializer_list>

#include "memory.h"
#include "iterator.h"
#include "functional.h"
#include "util.h"
#include "exceptdef.h"

namespace mystl
{

    // 单向链表节点的基类，只包含一个指向下一个节点的指针
    struct slist_node_base
    {
        slist_node_base* next;  // 指向下一个节点的指针
    };

    // 在给定节点的后面插入新节点
    inline slist_node_base* slist_make_link(slist_node_base* prev_node, slist_node_base* new_node)
    {
        new_node->next = prev_node->next;
        prev_node->next = new_node;
        return new_node;
    }

    // 返回给定节点的上一个节点
    inline slist_node_base* slist_previous(slist_node_base* head, const slist_node_base* node)
    {
        while (head && head->next != node)
            head = head->next;
        return head;
    }

    // 返回给定节点的上一个节点（const重载版本）
    inline const slist_node_base* slist_previous(const slist_node_base* head, const slist_node_base* node)
    {
        while (head && head->next != node)
            head = head->next;
        return head;
    }

    // 将一段节点插入到目标节点之后
    inline void slist_splice_after(slist_node_base* pos, slist_node_base* before_first, slist_node_base* before_last)
    {
        if (pos != before_first && pos != before_last)
        {
            slist_node_base* first = before_first->next;
            slist_node_base* after = pos->next;
            before_first->next = before_last->next;
            pos->next = first;
            before_last->next = after;
        }
    }

    // 反转给定节点之后的链表
    inline slist_node_base* slist_reverse(slist_node_base* node)
    {
        slist_node_base* result = node;
        node = node->next;
        result->next = nullptr;
        while (node)
        {
            slist_node_base* next = node->next;
            node->next = result;
            result = node;
            node = next;
        }
        return result;
    }

    // 单向链表节点的模板类
    template <class T>
        struct slist_node : public slist_node_base
    {
        T data;
    };

    // 单向链表迭代器的模板类
    template <class T, class Ref, class Ptr>
        struct slist_iterator
        {
            typedef slist_iterator<T, T&, T*> iterator;  // 迭代器类型
            typedef slist_iterator<T, const T&, const T*> const_iterator;  // const迭代器类型
            typedef slist_iterator<T, Ref, Ptr> self;  // 自身类型

            typedef ptrdiff_t difference_type;  // 迭代器之间的距离类型
            typedef T value_type;  // 值的类型
            typedef Ref reference;  // 引用的类型
            typedef Ptr pointer;  // 指针的类型
            typedef slist_node_base node_base;  // 节点基类
            typedef node_base* node_base_ptr;  // 节点基类指针类型
            typedef slist_node<T> list_node;  // 节点类型
            typedef list_node* list_node_ptr;  // 节点指针类型

            node_base_ptr node;  // 指向当前节点的指针

            // 构造函数，通过给定节点指针初始化
            slist_iterator(list_node_ptr x) : node(x)
            {
            }

            // 默认构造函数，将指针初始化为空
            slist_iterator() : node(nullptr)
            {
            }

            // 拷贝构造函数，通过给定迭代器初始化
            slist_iterator(const iterator& it)
                : node(it.node)
            {
            }

            // 指向下一个节点
            void incr()
            {
                node = node->next;
            }

            // 解引用操作符，返回当前节点的数据
            reference operator*() const
            {
                return ((list_node_ptr)node)->data;
            }

            // 重载箭头操作符，返回指向当前节点数据的指针
            pointer operator->() const
            {
                return &(operator*());
            }

            // 前置递增操作符，指向下一个节点
            self& operator++()
            {
                incr();
                return *this;
            }

            // 后置递增操作符，返回递增前的迭代器值
            self operator++(int)
            {
                auto tmp = *this;
                incr();
                return tmp;
            }

            // 判断两个迭代器是否相等
            bool operator==(const self& rhs)
            {
                return node == rhs.node;
            }

            // 判断两个迭代器是否不相等
            bool operator!=(const self& rhs)
            {
                return !(node == rhs.node);
            }
        };

    template <class T>
        class slist
        {
            public:

                typedef allocator<T>                                allocator_type;

                typedef typename allocator_type::value_type         value_type;
                typedef typename allocator_type::pointer            pointer;
                typedef typename allocator_type::const_pointer      const_pointer;
                typedef typename allocator_type::reference          reference;
                typedef typename allocator_type::const_reference    const_reference;
                typedef typename allocator_type::size_type          size_type;
                typedef typename allocator_type::difference_type    difference_type;

                typedef slist_iterator<T, T&, T*>                   iterator;
                typedef slist_iterator<T, const T&, const T*>       const_iterator;

            private:
                typedef slist_node<T>                               list_node;
                typedef slist_node_base                             list_node_base;
                typedef allocator<list_node>                        list_node_allocator;
                typedef allocator<T>                                data_allocator;

                list_node_base* head_;
                size_type       size_;

            private:

                template <class ...Args>
                    list_node* create_node(Args&& ...args)
                    {
                        list_node* p = list_node_allocator::allocate(1);
                        try {
                            data_allocator::construct(&(p->data), mystl::forward<Args>(args)...);
                            p->next = nullptr;
                        }
                        catch (...) {
                            list_node_allocator::deallocate(p);
                            throw ;
                        }
                        return p;
                    }


                static void destory_node(list_node* node)
                {
                    mystl::destroy(&node->data);
                    list_node_allocator::deallocate(node);
                }

                void fill_initialize(size_type n, const value_type& x)
                {
                    head_ = create_node(0);
                    head_->next = nullptr;
                    size_ = 0;
                    try {
                        insert_after_fill(head_, n, x);
                    }
                    catch (...) {
                        clear();
                    }
                }

                template <class InputIter>
                    void range_initialize(InputIter first, InputIter last)
                    {
                        head_ = create_node(0);
                        head_->next = nullptr;
                        size_ = 0;
                        try {
                            insert_after_range(head_, first, last);
                        }
                        catch (...) {
                            clear();
                        }
                    }

            public:
                slist() : size_(0)
            {
                head_ = create_node(0);
                head_->next = nullptr;
            }

                explicit slist(size_type n)
                { fill_initialize(n, value_type()); }

                slist(size_type n, const value_type& value)
                { fill_initialize(n, value); }

                template <class InputIter, typename std::enable_if<
                    mystl::is_input_iterator<InputIter>::value, int>::type = 0>
                    slist(InputIter first, InputIter last)
                    { range_initialize(first, last); }

                slist(std::initializer_list<value_type> ilist)
                { range_initialize(ilist.begin(), ilist.end()); }

                slist(const slist& other)
                { range_initialize(other.begin(), other.end()); }

                slist(slist&& other) : head_(other.head_), size_(other.size_)
            {
                other.head_ = nullptr,
                    other.size_ = 0;
            }

                slist& operator=(const slist& rhs)
                {
                    if (this != &rhs)
                    {
                        clear();
                        assign(rhs.begin(), rhs.end());
                    }
                    return *this;
                }

                slist& operator=(slist&& rhs)
                {
                    clear();
                    splice(iterator((list_node*)(head_->next)), rhs);
                    return *this;
                }

                slist& operator=(std::initializer_list<value_type> ilist)
                {
                    slist tmp(ilist.begin(), ilist.end());
                    swap(tmp);
                    return *this;
                }

                ~slist()
                {
                    if (head_)
                    {
                        clear();
                        list_node_allocator::deallocate((list_node*)(head_));
                        head_ = nullptr;
                        size_ = 0;
                    }
                }

            public:
                iterator        begin()           noexcept
                { return (list_node*)(head_->next); }

                const_iterator  begin() const     noexcept
                { return (list_node*)(head_->next); }

                iterator        end()             noexcept
                { return iterator(nullptr); }

                const_iterator  end()   const     noexcept
                { return iterator(nullptr);}

                const_iterator cbegin() const noexcept
                { return begin(); }
                const_iterator cend()   const noexcept
                { return end(); }

                size_type size() const noexcept { return size_; }
                size_type max_size() const noexcept
                { return static_cast<size_type>(-1); }

                bool empty() const noexcept
                { return head_->next == nullptr; }

                void swap(slist& other)
                {
                    mystl::swap(head_, other.head_);
                    mystl::swap(size_, other.size_);
                }

                bool operator==(const slist& rhs);
                bool operator<(const slist& rhs);

            public:
                reference front()
                { return ((list_node*)(head_->next))->data; }

                const_reference front() const
                { return ((list_node*)(head_->next))->data; }

                template <class ...Args>
                    void emplace_front(Args&& ...args)
                    {
                        slist_make_link(head_, create_node(mystl::forward<Args>(args)...));
                        ++size_;
                    }

                void push_front(const value_type& value)
                {
                    slist_make_link(head_, create_node(value));
                    ++size_;
                }

                void push_front(value_type&& value)
                { emplace_front(mystl::move(value)); }

                void pop_front()
                {
                    list_node* node = (list_node*)(head_->next);
                    head_->next = node->next;
                    destory_node(node);
                    --size_;
                }

                iterator previous(const_iterator pos)
                {
                    return iterator(dynamic_cast<list_node*>(slist_previous(head_, pos.node)));
                }

                const_iterator previous(const_iterator pos) const
                {
                    return const_iterator(dynamic_cast<list_node*>(slist_previous(head_, pos.node)));
                }

            private:
                list_node* insert_after(list_node_base* pos, const value_type& value)
                {
                    ++size_;
                    return dynamic_cast<list_node*>(slist_make_link(pos, create_node(value)));
                }

                list_node* emplace_after(list_node_base* pos, value_type&& value)
                {
                    ++size_;
                    return (list_node*)(slist_make_link(pos, create_node(mystl::move(value))));
                }

                void insert_after_fill(list_node_base* pos,
                        size_type n, const value_type& value)
                {
                    for (size_type i = 0; i < n; ++i)
                        pos = slist_make_link(pos, create_node(value));
                    size_ += n;
                }

                template <class InputIter>
                    void insert_after_range(list_node_base* pos, InputIter first, InputIter last)
                    {
                        for (; first != last; ++first)
                            pos = slist_make_link(pos, create_node(*first)),
                                ++size_;
                    }

                list_node_base* erase_after(list_node_base* pos)
                {
                    auto next = (list_node*)(pos->next);
                    auto next_next = next->next;
                    pos->next = next_next;
                    destory_node(next);
                    --size_;
                    return next_next;
                }

                // 删除 (before_first, last_node) 中间元素, 返回 last_node
                list_node_base* erase_after(list_node_base* before_first,
                        list_node_base* last_node)
                {
                    for (auto cur = (list_node*)(before_first->next); cur != last_node; )
                    {
                        list_node* tmp = cur;
                        cur = (list_node*)(cur->next);
                        destory_node(tmp);
                        --size_;
                    }
                    before_first->next = last_node;
                    return last_node;
                }

            public:
                // 在 pos 后插入值 value
                iterator insert_after(iterator pos, const value_type& value)
                {
                    return insert_after(pos.node, value);
                }

                iterator emplace_after(iterator pos, value_type&& value)
                {
                    return emplace_after(pos.node, mystl::move(value));
                }

                iterator insert_after(iterator pos)
                {
                    return insert_after(pos.node, value_type());
                }

                void insert_after(iterator pos, size_type n, const value_type& value)
                {
                    insert_after_fill(pos.node, n, value);
                }

                template <class InputIter>
                    void insert_after(iterator pos, InputIter first, InputIter last)
                    {
                        insert_after_range(pos.node, first, last);
                    }

                iterator insert(iterator pos, const value_type& value)
                {
                    return insert_after(slist_previous(head_, pos.node), value);
                }

                iterator emplace(iterator pos, value_type&& value)
                {
                    return emplace_after(slist_previous(head_, pos.node), mystl::move(value));
                }

                iterator insert(iterator pos, value_type&& value)
                { return emplace(pos, mystl::move(value)); }

                iterator insert(iterator pos)
                {
                    return insert_after(slist_previous(head_, pos.node), value_type());
                }

                void insert(iterator pos, size_type n, const value_type& value)
                {
                    insert_after_fill(slist_previous(head_, pos.node), n, value);
                }

                template  <class InputIter>
                    void insert(iterator pos, InputIter first, InputIter last)
                    {
                        insert_after_range(slist_previous(head_, pos.node), first, last);
                    }

            public:

                iterator erase_after(iterator pos)
                {
                    return dynamic_cast<list_node*>(erase_after(pos.node));
                }

                iterator erase_after(iterator before_first, iterator last)
                {
                    return dynamic_cast<list_node*>(erase_after(before_first.node, last.node));
                }

                iterator erase(iterator pos)
                {
                    return (list_node*)(erase_after(slist_previous(head_, pos.node)));
                }

                iterator erase(iterator first, iterator last)
                {
                    return (list_node*)(erase_after(slist_previous(head_, first.node), last.node));
                }

                template <class InputIter>
                    void assign(InputIter first, InputIter last);

                void assign(std::initializer_list<value_type> ilist)
                {
                    assign(ilist.begin(), ilist.end());
                }

                void resize(size_type new_size, const value_type& value);
                void resize(size_type new_size) { resize(new_size, value_type()); }
                void clear() { erase_after(head_, nullptr); }

            public:

                // 将区间 [before_first + 1, before_last + 1) 移动到 pos 之后
                void splic_after(iterator pos, iterator before_first, iterator before_last)
                {
                    if (before_first != before_last)
                        slist_splice_after(pos.node, before_first.node, before_last.node);
                    size_ += mystl::distance(before_first, before_last);
                }

                // 将 prev 后面的元素移动到 pos 之后
                void splice_after(iterator pos, iterator prev)
                {
                    slist_splice_after(pos.node, prev.node, prev.node->next);
                    ++size_;
                }

                // 将其他 silist 移动到 pos 之后
                void splice(iterator pos, slist& other)
                {
                    if (other.head_->next)
                    {
                        slist_splice_after(slist_previous(head_, pos.node),
                                other.head_,
                                slist_previous(other.head_, 0));
                        size_ += other.size();
                    }
                }

                // 将 other 中的 i 移动到 pos 后
                void splice(iterator pos, slist& other, iterator i)
                {
                    ++size_;
                    slist_splice_after(slist_previous(head_, pos.node),
                            slist_previous(other.head_, i.node),
                            i.node);
                }

                // 将 [first, last) 移动到 pos 后
                void splice(iterator pos, slist& other, iterator first, iterator last)
                {
                    if (first != last)
                    {
                        size_ += mystl::distance(first, last);
                        slist_splice_after(slist_previous(head_, pos.node),
                                slist_previous(other.head_, first.node),
                                slist_previous(other.head_, last.node));
                    }
                }

            public:

                void reverse()
                {
                    if (head_->next)
                        head_->next = slist_reverse(head_->next);
                }

                void remove(const value_type& val);
                void unique();
                void merge(slist& other);
                void sort();
        };

    template <class T>
        template <class InputIter>
        void slist<T>::assign(InputIter first, InputIter last)
        {
            list_node_base* p1 = head_;
            list_node* n1 = (list_node*)(head_->next);
            for (; n1 != nullptr && first != last; ++first)
            {
                n1->data = *first;
                p1 = n1;
                n1 = (list_node*)(n1->next);
            }
            if (first == last)
                erase_after(p1, 0);
            else
                insert_after_range(p1, first, last);
        }

    template <class T>
        inline bool slist<T>::operator==(const slist& rhs)
        {
            list_node* n1 = dynamic_cast<list_node*>(head_->next);
            list_node* n2 = dynamic_cast<list_node*>(rhs.head_->next);
            while (n1 && n2 && n1->data == n2->data)
            {
                n1 = dynamic_cast<list_node*>(n1->next);
                n2 = dynamic_cast<list_node*>(n2->next);
            }
            return n1 == nullptr && n2 == nullptr;
        }

    template <class T>
        inline bool slist<T>::operator<(const slist<T>& rhs)
        {
            return mystl::lexicographical_compare(begin(), end(), rhs.begin(), rhs.end());
        }

    template <class T>
        void slist<T>::resize(size_type new_size, const value_type& value)
        {
            slist tmp(new_size, value);
            swap(tmp);
        }

    template <class T>
        void slist<T>::remove(const value_type& value)
        {
            list_node_base* cur = head_;
            while (cur && cur->next)
            {
                if (((list_node*)(cur->next))->data == value)
                    erase_after(cur);
                else
                    cur = cur->next;
            }
        }

    template <class T>
        void slist<T>::unique()
        {
            list_node_base* cur = head_->next;
            if (cur)
            {
                while (cur->next)
                {
                    if (((list_node*)(cur->next))->data ==
                            ((list_node*)(cur))->data)
                        erase_after(cur);
                    else
                        cur = cur->next;
                }
            }
        }

    template <class T>
        inline void swap(slist<T>& lhs, slist<T>& rhs)
        {
            lhs.swap(rhs);
        }

    template <class T>
        void slist<T>::merge(slist<T>& other)
        {
            list_node_base* n1 = head_;
            while (n1->next && other.head_->next)
            {
                if (((list_node*)(other.head_->next))->data <
                        ((list_node*)(n1->next))->data)
                    slist_splice_after(n1, other.head_, other.head_->next);
                n1 = n1->next;
            }
            if (other.head_->next)
            {
                n1->next = other.head_->next;
                other.head_->next = nullptr;
            }
        }

    template <class T>
        void slist<T>::sort()
        {
            if (head_->next && head_->next->next)
            {
                slist<T> carry;
                slist<T> counter[64];
                int fill = 0;
                while (!empty())
                {
                    slist_splice_after(carry.head_, head_, head_->next);
                    int i = 0;
                    while (i < fill && !counter[i].empty())
                    {
                        counter[i].merge(carry);
                        carry.swap(counter[i]);
                        ++i;
                    }
                    carry.swap(counter[i]);
                    if (i == fill)
                        ++fill;
                }
                for (int i = 1; i < fill; ++i)
                    counter[i].merge(counter[i - 1]);
                this->swap(counter[fill - 1]);
            }
        }

};

#endif //TINYSTL_SLIST_H
