#ifndef MYTINYSTL_LIST_H_
#define MYTINYSTL_LIST_H_

// 这个头文件包含了一个模板类 list
// list : 双向链表

// notes:
//
// 异常保证：
// mystl::list<T> 满足基本异常保证，部分函数无异常保证，并对以下等函数做强异常安全保证：
//   * emplace_front
//   * emplace_back
//   * emplace
//   * push_front
//   * push_back
//   * insert

#include <initializer_list>

#include "iterator.h"   // 迭代器相关操作
#include "memory.h"     // 内存相关操作
#include "functional.h" // 函数对象
#include "util.h"       // 工具函数
#include "exceptdef.h"  // 异常定义

namespace mystl
{

    template <class T> struct list_node_base;  // 前置声明
    template <class T> struct list_node;

    // 节点指针类型
    template <class T>
        struct node_traits
        {
            typedef list_node_base<T>* base_ptr;
            typedef list_node<T>*      node_ptr;
        };

    // list 的节点结构
    template <class T>
        struct list_node_base
        {
            typedef typename node_traits<T>::base_ptr base_ptr;
            typedef typename node_traits<T>::node_ptr node_ptr;

            base_ptr prev;  // 前一节点
            base_ptr next;  // 下一节点

            list_node_base() = default;

            // 把当前节点转换为派生类的节点指针
            node_ptr as_node()
            {
                return static_cast<node_ptr>(self());
            }

            // 将当前节点从链表中断开
            void unlink()
            {
                prev = next = self();
            }

            // 返回当前节点的指针
            base_ptr self()
            {
                return static_cast<base_ptr>(&*this);
            }
        };

    // list 的节点结构，继承自 list_node_base
    template <class T>
        struct list_node : public list_node_base<T>
    {
        typedef typename node_traits<T>::base_ptr base_ptr;
        typedef typename node_traits<T>::node_ptr node_ptr;

        T value;  // 数据域

        list_node() = default;
        list_node(const T& v)
            :value(v)
        {
        }
        list_node(T&& v)
            :value(mystl::move(v))
        {
        }

        // 把当前节点转换为基类的指针
        base_ptr as_base()
        {
            return static_cast<base_ptr>(&*this);
        }

        // 返回当前节点的指针
        node_ptr self()
        {
            return static_cast<node_ptr>(&*this);
        }
    };

    // list 的迭代器设计
    template <class T>
        struct list_iterator : public mystl::iterator<mystl::bidirectional_iterator_tag, T>
    {
        typedef T                                 value_type;  // 值类型
        typedef T*                                pointer;     // 指针类型
        typedef T&                                reference;   // 引用类型
        typedef typename node_traits<T>::base_ptr base_ptr;    // 基类指针类型
        typedef typename node_traits<T>::node_ptr node_ptr;    // 派生类指针类型
        typedef list_iterator<T>                  self;        // 迭代器自身类型

        base_ptr node_;  // 指向当前节点

        // 默认构造函数
        list_iterator() = default;

        // 构造函数，将指针 x 赋值给迭代器
        list_iterator(base_ptr x)
            :node_(x) {}

        // 构造函数，将指针 x 转换为派生类的指针并赋值给迭代器
        list_iterator(node_ptr x)
            :node_(x->as_base()) {}

        // 复制构造函数
        list_iterator(const list_iterator& rhs)
            :node_(rhs.node_) {}

        // 重载操作符

        // 解引用操作符，返回节点的数据域的引用
        reference operator*()  const { return node_->as_node()->value; }

        // 成员访问操作符，返回节点的数据域的指针
        pointer   operator->() const { return &(operator*()); }

        // 前置自增操作符，指向下一个节点
        self& operator++()
        {
            MYSTL_DEBUG(node_ != nullptr);
            node_ = node_->next;
            return *this;
        }

        // 后置自增操作符，指向下一个节点
        self operator++(int)
        {
            self tmp = *this;
            ++*this;
            return tmp;
        }

        // 前置自减操作符，指向前一个节点
        self& operator--()
        {
            MYSTL_DEBUG(node_ != nullptr);
            node_ = node_->prev;
            return *this;
        }

        // 后置自减操作符，指向前一个节点
        self operator--(int)
        {
            self tmp = *this;
            --*this;
            return tmp;
        }

        // 重载比较操作符

        // 判断两个迭代器是否相等，即指向同一个节点
        bool operator==(const self& rhs) const { return node_ == rhs.node_; }

        // 判断两个迭代器是否不相等，即指向不同的节点
        bool operator!=(const self& rhs) const { return node_ != rhs.node_; }
    };

    template <class T>
        struct list_const_iterator : public iterator<bidirectional_iterator_tag, T>
    {
        typedef T                                 value_type;   // 值类型
        typedef const T*                          pointer;      // 指针类型
        typedef const T&                          reference;    // 引用类型
        typedef typename node_traits<T>::base_ptr base_ptr;     // 基类指针类型
        typedef typename node_traits<T>::node_ptr node_ptr;     // 派生类指针类型
        typedef list_const_iterator<T>            self;         // 迭代器自身类型

        base_ptr node_;  // 指向当前节点

        // 默认构造函数
        list_const_iterator() = default;

        // 构造函数，将指针 x 赋值给迭代器
        list_const_iterator(base_ptr x)
            :node_(x) {}

        // 构造函数，将指针 x 转换为派生类的指针并赋值给迭代器
        list_const_iterator(node_ptr x)
            :node_(x->as_base()) {}

        // 复制构造函数
        list_const_iterator(const list_iterator<T>& rhs)
            :node_(rhs.node_) {}

        // 复制构造函数
        list_const_iterator(const list_const_iterator& rhs)
            :node_(rhs.node_) {}

        // 解引用操作符，返回节点的数据域的引用
        reference operator*()  const { return node_->as_node()->value; }

        // 成员访问操作符，返回节点的数据域的指针
        pointer   operator->() const { return &(operator*()); }

        // 前置自增操作符，指向下一个节点
        self& operator++()
        {
            MYSTL_DEBUG(node_ != nullptr);
            node_ = node_->next;
            return *this;
        }

        // 后置自增操作符，指向下一个节点
        self operator++(int)
        {
            self tmp = *this;
            ++*this;
            return tmp;
        }

        // 前置自减操作符，指向前一个节点
        self& operator--()
        {
            MYSTL_DEBUG(node_ != nullptr);
            node_ = node_->prev;
            return *this;
        }

        // 后置自减操作符，指向前一个节点
        self operator--(int)
        {
            self tmp = *this;
            --*this;
            return tmp;
        }

        // 重载比较操作符

        // 判断两个迭代器是否相等，即指向同一个节点
        bool operator==(const self& rhs) const { return node_ == rhs.node_; }

        // 判断两个迭代器是否不相等，即指向不同的节点
        bool operator!=(const self& rhs) const { return node_ != rhs.node_; }
    };

    // 模板类: list
    // 模板参数 T 代表数据类型
    template <class T>
        class list
        {
            public:
                // list 的嵌套型别定义
                typedef mystl::allocator<T>                      allocator_type;  // 分配器类型
                typedef mystl::allocator<T>                      data_allocator;  // 数据分配器类型
                typedef mystl::allocator<list_node_base<T>>      base_allocator;  // 基础节点分配器类型
                typedef mystl::allocator<list_node<T>>           node_allocator;  // 节点分配器类型

                typedef typename allocator_type::value_type      value_type;  // 数据类型
                typedef typename allocator_type::pointer         pointer;     // 指针类型
                typedef typename allocator_type::const_pointer   const_pointer;  // 常量指针类型
                typedef typename allocator_type::reference       reference;  // 引用类型
                typedef typename allocator_type::const_reference const_reference;  // 常量引用类型
                typedef typename allocator_type::size_type       size_type;  // 大小类型
                typedef typename allocator_type::difference_type difference_type;  // 差值类型

                typedef list_iterator<T>                         iterator;  // 迭代器类型
                typedef list_const_iterator<T>                   const_iterator;  // 常量迭代器类型
                typedef mystl::reverse_iterator<iterator>        reverse_iterator;  // 反向迭代器类型
                typedef mystl::reverse_iterator<const_iterator>  const_reverse_iterator;  // 常量反向迭代器类型

                typedef typename node_traits<T>::base_ptr        base_ptr;  // 基础指针类型
                typedef typename node_traits<T>::node_ptr        node_ptr;  // 节点指针类型

                allocator_type get_allocator() { return node_allocator(); }  // 获取分配器实例

            private:
                base_ptr  node_;  // 指向末尾节点
                size_type size_;  // 大小

            public:
                // 构造、复制、移动、析构函数
                list()
                { fill_init(0, value_type()); }  // 默认构造函数，创建一个空列表

                explicit list(size_type n)
                { fill_init(n, value_type()); }  // 创建一个包含 n 个默认值的列表

                list(size_type n, const T& value)
                { fill_init(n, value); }  // 创建一个包含 n 个指定值的列表

                template <class Iter, typename std::enable_if<
                    mystl::is_input_iterator<Iter>::value, int>::type = 0>
                    list(Iter first, Iter last)
                    { copy_init(first, last); }  // 根据迭代器范围 [first, last) 构造列表

                list(std::initializer_list<T> ilist)
                { copy_init(ilist.begin(), ilist.end()); }  // 根据初始化列表构造列表

                list(const list& rhs)
                { copy_init(rhs.cbegin(), rhs.cend()); }  // 拷贝构造函数

                list(list&& rhs) noexcept
                    :node_(rhs.node_), size_(rhs.size_)
                    {
                        rhs.node_ = nullptr;
                        rhs.size_ = 0;
                    }  // 移动构造函数

                list& operator=(const list& rhs)
                {
                    if (this != &rhs)
                    {
                        assign(rhs.begin(), rhs.end());
                    }
                    return *this;
                }  // 拷贝赋值运算符

                list& operator=(list&& rhs) noexcept
                {
                    clear();
                    splice(end(), rhs);
                    return *this;
                }  // 移动赋值运算符

                list& operator=(std::initializer_list<T> ilist)
                {
                    list tmp(ilist.begin(), ilist.end());
                    swap(tmp);
                    return *this;
                }  // 赋值初始化列表

                ~list()
                {
                    if (node_)
                    {
                        clear();
                        base_allocator::deallocate(node_);
                        node_ = nullptr;
                        size_ = 0;
                    }
                }  // 析构函数

            public:
                // 迭代器相关操作
                iterator               begin()         noexcept
                { return node_->next; }  // 返回起始迭代器

                const_iterator         begin()   const noexcept
                { return node_->next; }  // 返回常量起始迭代器

                iterator               end()           noexcept
                { return node_; }  // 返回结束迭代器

                const_iterator         end()     const noexcept
                { return node_; }  // 返回常量结束迭代器

                reverse_iterator       rbegin()        noexcept
                { return reverse_iterator(end()); }  // 返回反向起始迭代器

                const_reverse_iterator rbegin()  const noexcept
                { return reverse_iterator(end()); }  // 返回常量反向起始迭代器

                reverse_iterator       rend()          noexcept
                { return reverse_iterator(begin()); }  // 返回反向结束迭代器

                const_reverse_iterator rend()    const noexcept
                { return reverse_iterator(begin()); }  // 返回常量反向结束迭代器

                const_iterator         cbegin()  const noexcept
                { return begin(); }  // 返回常量起始迭代器

                const_iterator         cend()    const noexcept
                { return end(); }  // 返回常量结束迭代器

                const_reverse_iterator crbegin() const noexcept
                { return rbegin(); }  // 返回常量反向起始迭代器

                const_reverse_iterator crend()   const noexcept
                { return rend(); }  // 返回常量反向结束迭代器

                // 容量相关操作
                bool      empty()    const noexcept
                { return node_->next == node_; }  // 判断列表是否为空

                size_type size()     const noexcept
                { return size_; }  // 返回列表大小

                size_type max_size() const noexcept
                { return static_cast<size_type>(-1); }  // 返回列表的最大容量

                // 访问元素相关操作
                reference       front()
                {
                    MYSTL_DEBUG(!empty());
                    return *begin();
                }  // 返回首元素的引用

                const_reference front() const
                {
                    MYSTL_DEBUG(!empty());
                    return *begin();
                }  // 返回首元素的常量引用

                reference       back()
                {
                    MYSTL_DEBUG(!empty());
                    return *(--end());
                }  // 返回尾元素的引用

                const_reference back()  const
                {
                    MYSTL_DEBUG(!empty());
                    return *(--end());
                }  // 返回尾元素的常量引用

                // 调整容器相关操作

                // assign

                void     assign(size_type n, const value_type& value)
                { fill_assign(n, value); }  // 分配指定大小，全部赋值为 value

                template <class Iter, typename std::enable_if<
                    mystl::is_input_iterator<Iter>::value, int>::type = 0>
                    void     assign(Iter first, Iter last)
                    { copy_assign(first, last); }  // 根据迭代器范围 [first, last) 赋值

                void     assign(std::initializer_list<T> ilist)
                { copy_assign(ilist.begin(), ilist.end()); }  // 根据初始化列表赋值

                // emplace_front / emplace_back / emplace

                template <class ...Args>
                    void     emplace_front(Args&& ...args)
                    {
                        THROW_LENGTH_ERROR_IF(size_ > max_size() - 1, "list<T>'s size too big");  // 如果当前大小已经达到最大值，抛出异常
                        auto link_node = create_node(mystl::forward<Args>(args)...);  // 创建新节点
                        link_nodes_at_front(link_node->as_base(), link_node->as_base());  // 将新节点链接到首部
                        ++size_;  // 大小加一
                    }

                template <class ...Args>
                    void     emplace_back(Args&& ...args)
                    {
                        THROW_LENGTH_ERROR_IF(size_ > max_size() - 1, "list<T>'s size too big");  // 如果当前大小已经达到最大值，抛出异常
                        auto link_node = create_node(mystl::forward<Args>(args)...);  // 创建新节点
                        link_nodes_at_back(link_node->as_base(), link_node->as_base());  // 将新节点链接到尾部
                        ++size_;  // 大小加一
                    }

                template <class ...Args>
                    iterator emplace(const_iterator pos, Args&& ...args)
                    {
                        THROW_LENGTH_ERROR_IF(size_ > max_size() - 1, "list<T>'s size too big");  // 如果当前大小已经达到最大值，抛出异常
                        auto link_node = create_node(mystl::forward<Args>(args)...);  // 创建新节点
                        link_nodes(pos.node_, link_node->as_base(), link_node->as_base());  // 将新节点链接到指定位置之前
                        ++size_;  // 大小加一
                        return iterator(link_node);
                    }

                // insert

                iterator insert(const_iterator pos, const value_type& value)
                {
                    THROW_LENGTH_ERROR_IF(size_ > max_size() - 1, "list<T>'s size too big");  // 如果当前大小已经达到最大值，抛出异常
                    auto link_node = create_node(value);  // 创建新节点
                    ++size_;  // 大小加一
                    return link_iter_node(pos, link_node->as_base());
                }

                iterator insert(const_iterator pos, value_type&& value)
                {
                    THROW_LENGTH_ERROR_IF(size_ > max_size() - 1, "list<T>'s size too big");  // 如果当前大小已经达到最大值，抛出异常
                    auto link_node = create_node(mystl::move(value));  // 创建新节点
                    ++size_;  // 大小加一
                    return link_iter_node(pos, link_node->as_base());
                }

                iterator insert(const_iterator pos, size_type n, const value_type& value)
                {
                    THROW_LENGTH_ERROR_IF(size_ > max_size() - n, "list<T>'s size too big");  // 如果当前大小已经达到最大值，抛出异常
                    return fill_insert(pos, n, value);
                }

                template <class Iter, typename std::enable_if<
                    mystl::is_input_iterator<Iter>::value, int>::type = 0>
                    iterator insert(const_iterator pos, Iter first, Iter last)
                    {
                        size_type n = mystl::distance(first, last);  // 计算插入元素的个数
                        THROW_LENGTH_ERROR_IF(size_ > max_size() - n, "list<T>'s size too big");  // 如果当前大小已经达到最大值，抛出异常
                        return copy_insert(pos, n, first);
                    }

                // push_front / push_back

                void push_front(const value_type& value)
                {
                    THROW_LENGTH_ERROR_IF(size_ > max_size() - 1, "list<T>'s size too big");  // 如果当前大小已经达到最大值，抛出异常
                    auto link_node = create_node(value);  // 创建新节点
                    link_nodes_at_front(link_node->as_base(), link_node->as_base());  // 将新节点链接到首部
                    ++size_;  // 大小加一
                }

                void push_front(value_type&& value)
                {
                    emplace_front(mystl::move(value));  // 在链表头部插入一个节点，调用emplace_front()函数
                }

                void push_back(const value_type& value)
                {
                    THROW_LENGTH_ERROR_IF(size_ > max_size() - 1, "list<T>'s size too big");  // 检查是否超出最大容量
                    auto link_node = create_node(value);  // 创建一个节点，并将value赋值给节点的值域
                    link_nodes_at_back(link_node->as_base(), link_node->as_base());  // 将节点链接到链表尾部
                    ++size_;  // 节点数加一
                }

                void push_back(value_type&& value)
                {
                    emplace_back(mystl::move(value));  // 在链表尾部插入一个节点，调用emplace_back()函数
                }

                // 删除链表头部节点
                void pop_front()
                {
                    MYSTL_DEBUG(!empty());  // 检查链表是否为空
                    auto n = node_->next;  // 获取要删除的节点
                    unlink_nodes(n, n);  // 将节点从链表中断开
                    destroy_node(n->as_node());  // 销毁节点
                    --size_;  // 节点数减一
                }

                // 删除链表尾部节点
                void pop_back()
                {
                    MYSTL_DEBUG(!empty());  // 检查链表是否为空
                    auto n = node_->prev;  // 获取要删除的节点
                    unlink_nodes(n, n);  // 将节点从链表中断开
                    destroy_node(n->as_node());  // 销毁节点
                    --size_;  // 节点数减一
                }

                // 删除指定位置的元素
                template <class T>
                    typename list<T>::iterator 
                    list<T>::erase(const_iterator pos)
                    {
                        MYSTL_DEBUG(pos != cend());  // 检查迭代器是否有效
                        auto n = pos.node_;  // 获取要删除的节点
                        auto next = n->next;  // 获取下一个节点
                        unlink_nodes(n, n);  // 将节点从链表中断开
                        destroy_node(n->as_node());  // 销毁节点
                        --size_;  // 节点数减一
                        return iterator(next);  // 返回下一个节点的迭代器
                    }

                // 删除区间 [first, last) 内的元素
                template <class T>
                    typename list<T>::iterator 
                    list<T>::erase(const_iterator first, const_iterator last)
                    {
                        if (first != last)
                        {
                            unlink_nodes(first.node_, last.node_->prev);  // 将要删除的区间从链表中断开
                            while (first != last)
                            {
                                auto cur = first.node_;  // 获取当前要删除的节点
                                ++first;
                                destroy_node(cur->as_node());  // 销毁节点
                                --size_;  // 节点数减一
                            }
                        }
                        return iterator(last.node_);  // 返回最后一个节点的迭代器
                    }

                // 清空链表
                template <class T>
                    void list<T>::clear()
                    {
                        if (size_ != 0)
                        {
                            auto cur = node_->next;  // 获取第一个节点
                            for (base_ptr next = cur->next; cur != node_; cur = next, next = cur->next)
                            {
                                destroy_node(cur->as_node());  // 销毁节点
                            }
                            node_->unlink();  // 断开头尾节点的连接
                            size_ = 0;  // 节点数置零
                        }
                    }

                // 改变链表的大小
                template <class T>
                    void list<T>::resize(size_type new_size, const value_type& value)
                    {
                        auto i = begin();
                        size_type len = 0;
                        while (i != end() && len < new_size)
                        {
                            ++i;
                            ++len;
                        }
                        if (len == new_size)
                        {
                            erase(i, node_);  // 删除多余的节点
                        }
                        else
                        {
                            insert(node_, new_size - len, value);  // 在末尾插入新的节点
                        }
                    }

                // 将链表x插入到pos之前
                template <class T>
                    void list<T>::splice(const_iterator pos, list& x)
                    {
                        MYSTL_DEBUG(this != &x);  // 检查链表是否和x相同
                        if (!x.empty())
                        {
                            THROW_LENGTH_ERROR_IF(size_ > max_size() - x.size_, "list<T>'s size too big");  // 检查容量是否足够

                            auto f = x.node_->next;  // 获取x的第一个节点
                            auto l = x.node_->prev;  // 获取x的最后一个节点

                            x.unlink_nodes(f, l);  // 将x从原链表中断开
                            link_nodes(pos.node_, f, l);  // 将x插入到当前链表
                            size_ += x.size_;  // 节点数增加x的节点数
                            x.size_ = 0;  // x的节点数置零
                        }
                    }

                // 将it所指的节点插入到pos之前
                template <class T>
                    void list<T>::splice(const_iterator pos, list& x, const_iterator it)
                    {
                        if (pos.node_ != it.node_ && pos.node_ != it.node_->next)
                        {
                            THROW_LENGTH_ERROR_IF(size_ > max_size() - 1, "list<T>'s size too big");  // 检查容量是否足够

                            auto f = it.node_;  // 获取it所指的节点

                            x.unlink_nodes(f, f);  // 将it从原链表中断开
                            link_nodes(pos.node_, f, f);  // 将it插入到当前链表
                            ++size_;  // 节点数增加1
                            --x.size_;  // x的节点数减1
                        }
                    }

                // 移除值为value的元素
                template <class T>
                    void list<T>::remove(const value_type& value)
                    {
                        remove_if([&](const value_type& v) {return v == value; });  // 使用lambda表达式调用remove_if函数
                    }

                template <class T>
                    template <class UnaryPredicate>
                    void list<T>::remove_if(UnaryPredicate pred)
                    {
                        auto first = begin();
                        auto last = end();
                        while (first != last)
                        {
                            if (pred(*first))  // 如果pred返回true，则移除该节点
                            {
                                first = erase(first);
                            }
                            else
                            {
                                ++first;
                            }
                        }
                    }

                // 移除链表中相邻的重复元素，保留每组相同元素的第一个元素
                template <class T>
                    void list<T>::unique()
                    {
                        unique(mystl::equal_to<T>());  // 使用默认的比较函数调用unique函数
                    }

                template <class T>
                    template <class BinaryPredicate>
                    void list<T>::unique(BinaryPredicate pred)
                    {
                        auto first = begin();
                        auto last = end();
                        if (first == last)
                        {
                            return;
                        }
                        auto next = first;
                        while (++next != last)
                        {
                            if (pred(*first, *next))  // 如果两个元素相等，则移除第二个元素
                            {
                                erase(next);
                            }
                            else
                            {
                                first = next;
                            }
                            next = first;
                        }
                    }

                // 将链表x合并到当前链表，两个链表的元素必须已经有序
                template <class T>
                    void list<T>::merge(list& x)
                    {
                        merge(x, mystl::less<T>());  // 使用默认的比较函数调用merge函数
                    }

                template <class T>
                    template <class Compare>
                    void list<T>::merge(list& x, Compare comp)
                    {
                        if (this != &x)
                        {
                            THROW_LENGTH_ERROR_IF(size_ > max_size() - x.size_, "list<T>'s size too big");  // 检查容量是否足够

                            auto first1 = begin();
                            auto last1 = end();
                            auto first2 = x.begin();
                            auto last2 = x.end();

                            while (first1 != last1 && first2 != last2)
                            {
                                if (comp(*first2, *first1))  // 如果第二个链表的元素小于第一个链表的元素
                                {
                                    auto next = first2;
                                    ++next;
                                    splice(first1, x, first2);  // 将第二个链表的元素插入到第一个链表中
                                    first2 = next;
                                }
                                else
                                {
                                    ++first1;
                                }
                            }
                            if (first2 != last2)  // 如果第二个链表还有剩余元素，将其全部插入到第一个链表中
                            {
                                splice(last1, x, first2, last2);
                            }
                            size_ += x.size_;
                            x.size_ = 0;
                        }
                    }

                // 排序链表
                template <class T>
                    void list<T>::sort()
                    {
                        list_sort(begin(), end(), size(), mystl::less<T>());  // 使用默认的比较函数调用list_sort函数
                    }

                template <class T>
                    template <class Compared>
                    void list<T>::sort(Compared comp)
                    {
                        list_sort(begin(), end(), size(), comp);  // 调用list_sort函数进行排序
                    }

                // 反转链表
                template <class T>
                    void list<T>::reverse()
                    {
                        if (size_ <= 1)  // 如果节点数小于等于1，不需要反转
                        {
                            return;
                        }
                        auto first = node_->next;  // 获取第一个节点
                        auto last = node_->prev;  // 获取最后一个节点
                        while (first != last)
                        {
                            mystl::swap(first->prev, first->next);  // 交换前驱和后继指针
                            first = first->prev;
                            if (first->next == first)  // 如果只有两个节点，交换后，需要更新最后一个节点
                            {
                                last = first;
                            }
                        }
                    }

                // 创建节点
                template <class T>
                    template <class... Args>
                    typename list<T>::node_ptr list<T>::create_node(Args&&... args)
                    {
                        auto p = node_allocator::allocate();  // 分配内存
                        try
                        {
                            data_allocator::construct(p, mystl::forward<Args>(args)...);  // 构造节点
                            return p;
                        }
                        catch (...)
                        {
                            node_allocator::deallocate(p);  // 构造失败，释放分配的内存
                            throw;
                        }
                    }

                // 销毁节点
                template <class T>
                    void list<T>::destroy_node(node_ptr p)
                    {
                        data_allocator::destroy(p);  // 销毁节点中的对象
                        node_allocator::deallocate(p);  // 释放分配的内存
                    }

                // 初始化函数
                template <class T>
                    void list<T>::fill_init(size_type n, const value_type& value)
                    {
                        auto cur = node_->next;  // 获取第一个节点
                        for (; n > 0; --n)
                        {
                            auto tmp = create_node(value);  // 创建节点
                            link_nodes_at_back(tmp->as_base(), tmp->as_base());  // 将节点链接到链表尾部
                        }
                    }

                template <class T>
                    template <class Iter>
                    void list<T>::copy_init(Iter first, Iter last)
                    {
                        auto f1 = first;
                        auto f2 = begin();
                        for (; f1 != last; ++f1, ++f2)
                        {
                            auto tmp = create_node(*f1);  // 创建节点
                            link_nodes_at_back(tmp->as_base(), tmp->as_base());  // 将节点链接到链表尾部
                        }
                    }

                // 在pos之前插入节点
                template <class T>
                    typename list<T>::iterator 
                    list<T>::link_iter_node(const_iterator pos, base_ptr node)
                    {
                        auto ret = pos.node_->prev;  // 获取pos的前一个节点
                        node->prev = ret;  // 将node的前驱指针设置为ret
                        node->next = ret->next;  // 将node的后继指针设置为ret的初始后继指针
                        ret->next->prev = node;  // 更新ret的初始后继节点的前驱指针
                        ret->next = node;  // 将ret的后继指针设置为node
                        return iterator(node);  // 返回插入节点的迭代器
                    }

                // 链接节点
                template <class T>
                    void list<T>::link_nodes(base_ptr p, base_ptr first, base_ptr last)
                    {
                        p->prev = first->prev;  // 将p的前驱指针设置为first的前驱指针
                        p->next = last;  // 将p的后继指针设置为last
                        first->prev->next = p;  // 更新first的前驱节点的后继指针
                        first->prev = last;  // 将first的前驱指针设置为last
                    }

                // 在链表头部链接节点
                template <class T>
                    void list<T>::link_nodes_at_front(base_ptr first, base_ptr last)
                    {
                        first->prev = node_;  // 将first的前驱指针设置为头节点
                        last->next = node_->next;  // 将last的后继指针设置为原始第一个节点
                        node_->next->prev = last;  // 更新原始第一个节点的前驱指针
                        node_->next = first;  // 将头节点的后继指针设置为first
                    }

                // 在链表尾部链接节点
                template <class T>
                    void list<T>::link_nodes_at_back(base_ptr first, base_ptr last)
                    {
                        last->next = node_;  // 将last的后继指针设置为头节点
                        first->prev = node_->prev;  // 将first的前驱指针设置为原始最后一个节点
                        node_->prev->next = first;  // 更新原始最后一个节点的后继指针
                        node_->prev = last;  // 将头节点的前驱指针设置为last
                    }

                // 断开节点连接
                template <class T>
                    void list<T>::unlink_nodes(base_ptr f, base_ptr l)
                    {
                        f->prev->next = l->next;  // 将f的前驱节点的后继指针设置为l的后继指针
                        l->next->prev = f->prev;  // 将l的后继节点的前驱指针设置为f的前驱指针
                    }

                // 填充赋值
                template <class T>
                    void list<T>::fill_assign(size_type n, const value_type& value)
                    {
                        auto i = begin();
                        for (; i != end() && n > 0; ++i, --n)
                        {
                            *i = value;  // 将value赋值给节点的值域
                        }
                        if (n > 0)
                        {
                            insert(i, n, value);  // 在i之前插入n个值为value的节点
                        }
                        else
                        {
                            erase(i, node_);  // 删除多余的节点
                        }
                    }

                template <class T>
                    template <class Iter>
                    void list<T>::copy_assign(Iter first, Iter last)
                    {
                        auto f1 = first;
                        auto f2 = begin();
                        for (; f1 != last && f2 != end(); ++f1, ++f2)
                        {
                            *f2 = *f1;  // 将*f1赋值给*f2
                        }
                        if (f1 != last)
                        {
                            insert(node_, f1, last);  // 在末尾插入[first, last)范围内的节点
                        }
                        else
                        {
                            erase(f2, node_);  // 删除多余的节点
                        }
                    }

                // 填充插入
                template <class T>
                    typename list<T>::iterator 
                    list<T>::fill_insert(const_iterator pos, size_type n, const value_type& value)
                    {
                        if (n == 0)
                        {
                            return iterator(pos.node_);  // 如果n为零，直接返回pos的迭代器
                        }
                        auto ret = link_iter_node(pos, create_node(value));  // 在pos之前插入一个值为value的节点
                        while (--n)
                        {
                            link_iter_node(pos, create_node(value));  // 在pos之前插入n个值为value的节点
                        }
                        return ret;  // 返回插入节点的迭代器
                    }

                template <class T>
                    template <class Iter>
                    typename list<T>::iterator 
                    list<T>::copy_insert(const_iterator pos, size_type n, Iter first)
                    {
                        if (n == 0)
                        {
                            return iterator(pos.node_);  // 如果n为零，直接返回pos的迭代器
                        }
                        auto ret = link_iter_node(pos, create_node(*first));  // 在pos之前插入一个值为*first的节点
                        while (--n)
                        {
                            link_iter_node(pos, create_node(*(++first)));  // 在pos之前插入n个值为*(first+1)的节点
                        }
                        return ret;  // 返回插入节点的迭代器
                    }

                // 排序链表
                template <class T>
                    template <class Compared>
                    typename list<T>::iterator list<T>::list_sort(iterator first, iterator last, size_type n, Compared comp)
                    {
                        if (n < 2)
                        {
                            return first;  // 如果节点数小于2，无需排序，直接返回
                        }
                        if (n == 2)
                        {
                            if (comp(*--last, *first))
                            {
                                mystl::swap(first.node_->value, last.node_->value);  // 如果有两个节点，如果需要交换两个节点的值域
                            }
                            return first;
                        }

                        auto count = n >> 1;  // 将节点一分为二
                        auto mid = first;
                        mystl::advance(mid, count);  // 获取中间节点
                        auto result = first;
                        mystl::advance(result, count);  // 获取第二个子链表的起始节点

                        first = list_sort(first, mid, count, comp);  // 对第一个子链表进行排序
                        result = list_sort(result, last, n - count, comp);  // 对第二个子链表进行排序

                        auto cut = list_merge(first, result, comp);  // 合并两个已排序的子链表

                        return mystl::next(cut);  // 返回合并后的尾节点的迭代器
                    }

                // 合并两个已排序的子链表
                template <class T>
                    template <class Compared>
                    typename list<T>::base_ptr 
                    list<T>::list_merge(base_ptr first1, base_ptr first2, Compared comp) 
                    {
                        if (first1 == node_)
                        {
                            return first2;  // 如果第一个子链表为空，直接返回第二个子链表
                        }
                        if (first2 == node_)
                        {
                            return first1;  // 如果第二个子链表为空，直接返回第一个子链表
                        }

                        base_ptr last1 = first1->prev;  // 获取第一个子链表的最后一个节点
                        base_ptr last2 = first2->prev;  // 获取第二个子链表的最后一个节点

                        while (first1 != last1 && first2 != last2)
                        {
                            if (comp(first2->value, first1->value))  // 如果第二个子链表的节点值小于第一个子链表的节点值
                            {
                                auto next = first2;
                                ++next;
                                link_nodes(first1->prev, first2, first2);
                                first2 = next;
                            }
                            else
                            {
                                ++first1;
                            }
                        }
                        if (first2 != last2)
                        {
                            link_nodes(last1, first2, last2->next);
                        }
                        return first1;  // 返回合并后的尾节点
                    }

                // list<T>类的成员函数实现

                // 将另一个list x中[first, last)范围内的节点插入到当前list的pos位置之前
                template <class T>
                    void list<T>::splice(const_iterator pos, list& x, const_iterator first, const_iterator last)
                    {
                        if (first != last && this != &x)
                        {
                            size_type n = mystl::distance(first, last); // 计算要移动的节点数量

                            // 如果当前list的大小加上要移动的节点数量超过了最大容量，则抛出长度错误异常
                            THROW_LENGTH_ERROR_IF(size_ > max_size() - n, "list<T>'s size too big");

                            auto f = first.node_; // 要移动的第一个节点
                            auto l = last.node_->prev; // 要移动的最后一个节点的前一个节点

                            x.unlink_nodes(f, l); // 将要移动的节点从原list中分离出来
                            link_nodes(pos.node_, f, l); // 将这些节点插入到当前list的pos位置之前

                            size_ += n; // 更新当前list的大小
                            x.size_ -= n; // 更新原list的大小
                        }
                    }

                // 移除满足谓词pred为true的所有元素
                template <class T>
                    template <class UnaryPredicate>
                    void list<T>::remove_if(UnaryPredicate pred)
                    {
                        auto f = begin(); // 遍历开始的迭代器
                        auto l = end(); // 遍历结束的迭代器

                        // 遍历整个list
                        for (auto next = f; f != l; f = next)
                        {
                            ++next; // 下一个要访问的节点

                            if (pred(*f)) // 如果当前元素满足谓词pred为true
                            {
                                erase(f); // 移除该元素
                            }
                        }
                    }

                // 移除list中满足谓词pred为true的重复元素
                template <class T>
                    template <class BinaryPredicate>
                    void list<T>::unique(BinaryPredicate pred)
                    {
                        auto i = begin(); // 遍历开始的迭代器
                        auto e = end(); // 遍历结束的迭代器
                        auto j = i;
                        ++j;

                        while (j != e) // 遍历整个list
                        {
                            if (pred(*i, *j)) // 如果两个相邻元素满足谓词pred为true
                            {
                                erase(j); // 移除第二个元素
                            }
                            else
                            {
                                i = j; // 否则，将i指向第二个元素
                            }

                            j = i; // 将j指向i所指的元素
                            ++j; // 将j指向下一个元素
                        }
                    }

                // 将另一个list x合并到当前list中，并按照comp为true的顺序排序
                template <class T>
                    template <class Compare>
                    void list<T>::merge(list& x, Compare comp)
                    {
                        if (this != &x)
                        {
                            THROW_LENGTH_ERROR_IF(size_ > max_size() - x.size_, "list<T>'s size too big");

                            auto f1 = begin(); // 当前list的遍历开始的迭代器
                            auto l1 = end(); // 当前list的遍历结束的迭代器
                            auto f2 = x.begin(); // 另一个list的遍历开始的迭代器
                            auto l2 = x.end(); // 另一个list的遍历结束的迭代器

                            while (f1 != l1 && f2 != l2) // 同时遍历两个list
                            {
                                if (comp(*f2, *f1)) // 如果第二个list的当前元素小于第一个list的当前元素
                                {
                                    // 找到第二个list中comp为true的一段区间
                                    auto next = f2;
                                    ++next;
                                    for (; next != l2 && comp(*next, *f1); ++next)
                                        ;
                                    auto f = f2.node_; // 要移动的第一个节点
                                    auto l = next.node_->prev; // 要移动的最后一个节点的前一个节点
                                    f2 = next;

                                    // 将这些节点从第二个list中分离出来，并插入到当前list的合适位置
                                    x.unlink_nodes(f, l);
                                    link_nodes(f1.node_, f, l);

                                    ++f1; // 更新当前list的遍历位置
                                }
                                else
                                {
                                    ++f1; // 更新当前list的遍历位置
                                }
                            }

                            // 连接第二个list中剩余的节点到当前list的末尾
                            if (f2 != l2)
                            {
                                auto f = f2.node_; // 剩余的第二个list的第一个节点
                                auto l = l2.node_->prev; // 剩余的第二个list的最后一个节点的前一个节点
                                x.unlink_nodes(f, l);
                                link_nodes(l1.node_, f, l);
                            }

                            size_ += x.size_; // 更新当前list的大小
                            x.size_ = 0; // 更新第二个list的大小
                        }
                    }

                // 将list反转
                template <class T>
                    void list<T>::reverse()
                    {
                        if (size_ <= 1) // 如果list的大小小于等于1，无需反转
                        {
                            return;
                        }

                        auto i = begin(); // 遍历开始的迭代器
                        auto e = end(); // 遍历结束的迭代器

                        while (i.node_ != e.node_) // 遍历整个list
                        {
                            mystl::swap(i.node_->prev, i.node_->next); // 交换相邻节点的指针
                            i.node_ = i.node_->prev; // 更新遍历位置
                        }

                        mystl::swap(e.node_->prev, e.node_->next); // 交换首尾节点的指针
                    }

                /*****************************************************************************************/
                // 辅助函数

                // 创建一个节点
                template <class T>
                    template <class ...Args>
                    typename list<T>::node_ptr
                    list<T>::create_node(Args&& ...args)
                    {
                        node_ptr p = node_allocator::allocate(1); // 分配内存空间
                        try
                        {
                            // 在分配的内存空间上构造对象
                            data_allocator::construct(mystl::address_of(p->value), mystl::forward<Args>(args)...);
                            p->prev = nullptr;
                            p->next = nullptr;
                        }
                        catch (...)
                        {
                            node_allocator::deallocate(p); // 构造失败，释放内存空间
                            throw;
                        }

                        return p;
                    }

                // 销毁一个节点
                template <class T>
                    void list<T>::destroy_node(node_ptr p)
                    {
                        data_allocator::destroy(mystl::address_of(p->value)); // 销毁对象
                        node_allocator::deallocate(p); // 释放内存空间
                    }

                // 使用n个元素初始化容器
                template <class T>
                    void list<T>::fill_init(size_type n, const value_type& value)
                    {
                        node_ = base_allocator::allocate(1); // 分配内存空间
                        node_->unlink(); // 将节点和自身连接

                        size_ = n; // 更新list的大小

                        try
                        {
                            for (; n > 0; --n)
                            {
                                auto node = create_node(value); // 创建一个节点
                                link_nodes_at_back(node->as_base(), node->as_base()); // 将节点链接到list的末尾
                            }
                        }
                        catch (...)
                        {
                            clear(); // 清空list
                            base_allocator::deallocate(node_); // 释放内存空间
                            node_ = nullptr;
                            throw;
                        }
                    }

                // 以 [first, last) 初始化容器
                template <class T>
                    template <class Iter>
                    void list<T>::copy_init(Iter first, Iter last)
                    {
                        node_ = base_allocator::allocate(1);  // 分配一个节点，作为链表的头结点
                        node_->unlink();  // 断开头结点与自身的连接
                        size_type n = mystl::distance(first, last);  // 计算[first, last)的元素个数
                        size_ = n;  // 更新容器大小
                        try
                        {
                            for (; n > 0; --n, ++first)
                            {
                                auto node = create_node(*first);  // 创建一个值为*first的节点
                                link_nodes_at_back(node->as_base(), node->as_base());  // 将节点链接到链表尾部
                            }
                        }
                        catch (...)
                        {
                            clear();
                            base_allocator::deallocate(node_);  // 分配释放内存
                            node_ = nullptr;
                            throw;
                        }
                    }

                // 在 pos 处连接一个节点
                template <class T>
                    typename list<T>::iterator 
                    list<T>::link_iter_node(const_iterator pos, base_ptr link_node)
                    {
                        if (pos == node_->next)  // 若pos为头结点之后的第一个节点
                        {
                            link_nodes_at_front(link_node, link_node);  // 将节点链接到链表头部
                        }
                        else if (pos == node_)  // 若pos为头结点
                        {
                            link_nodes_at_back(link_node, link_node);  // 将节点链接到链表尾部
                        }
                        else  // 若pos不是头结点或头结点之后的第一个节点
                        {
                            link_nodes(pos.node_, link_node, link_node);  // 将节点链接到pos之前的位置
                        }
                        return iterator(link_node);  // 返回指向新插入节点的迭代器
                    }

                // 在 pos 处连接 [first, last] 的结点
                template <class T>
                    void list<T>::link_nodes(base_ptr pos, base_ptr first, base_ptr last)
                    {
                        pos->prev->next = first;  // 使pos之前节点的next指向first
                        first->prev = pos->prev;  // 使first的prev指向pos之前节点
                        pos->prev = last;  // 使pos的prev指向last
                        last->next = pos;  // 使last的next指向pos
                    }

                // 在头部连接 [first, last] 结点
                template <class T>
                    void list<T>::link_nodes_at_front(base_ptr first, base_ptr last)
                    {
                        first->prev = node_;  // 使first的prev指向头结点
                        last->next = node_->next;  // 使last的next指向头结点之后的第一个节点
                        last->next->prev = last;  // 使头结点之后的第一个节点的prev指向last
                        node_->next = first;  // 使头结点的next指向first
                    }

                // 在尾部连接 [first, last] 结点
                template <class T>
                    void list<T>::link_nodes_at_back(base_ptr first, base_ptr last)
                    {
                        last->next = node_;  // 使last的next指向头结点
                        first->prev = node_->prev;  // 使first的prev指向尾节点
                        first->prev->next = first;  // 使尾节点的next指向first
                        node_->prev = last;  // 使头结点的prev指向last
                    }

                // 容器与 [first, last] 结点断开连接
                template <class T>
                    void list<T>::unlink_nodes(base_ptr first, base_ptr last)
                    {
                        first->prev->next = last->next;  // 使first之前节点的next指向last之后的节点
                        last->next->prev = first->prev;  // 使last之后节点的prev指向first之前的节点
                    }

                // 用 n 个元素为容器赋值
                template <class T>
                    void list<T>::fill_assign(size_type n, const value_type& value)
                    {
                        auto i = begin();
                        auto e = end();
                        for (; n > 0 && i != e; --n, ++i)
                        {
                            *i = value;  // 对已存在的元素进行赋值
                        }
                        if (n > 0)  // 若还有剩余元素需要赋值
                        {
                            insert(e, n, value);  // 在末尾插入剩余元素
                        }
                        else
                        {
                            erase(i, e);  // 删除多余的元素
                        }
                    }

                // 复制[f2, l2)为容器赋值
                template <class T>
                    template <class Iter>
                    void list<T>::copy_assign(Iter f2, Iter l2)
                    {
                        auto f1 = begin();
                        auto l1 = end();
                        for (; f1 != l1 && f2 != l2; ++f1, ++f2)
                        {
                            *f1 = *f2;  // 对已存在的元素进行赋值
                        }
                        if (f2 == l2)  // 若[f2, l2)为空
                        {
                            erase(f1, l1);  // 删除多余的元素
                        }
                        else  // 若[f2, l2)不为空
                        {
                            insert(l1, f2, l2);  // 在末尾插入剩余元素
                        }
                    }

                // 在 pos 处插入 n 个元素
                template <class T>
                    typename list<T>::iterator 
                    list<T>::fill_insert(const_iterator pos, size_type n, const value_type& value)
                    {
                        iterator r(pos.node_);
                        if (n != 0)
                        {
                            const auto add_size = n;
                            auto node = create_node(value);  // 创建一个值为value的节点
                            node->prev = nullptr;
                            r = iterator(node);
                            iterator end = r;
                            try
                            {
                                // 前面已经创建了一个节点，还需 n - 1 个
                                for (--n; n > 0; --n, ++end)
                                {
                                    auto next = create_node(value);  // 创建一个值为value的节点
                                    end.node_->next = next->as_base();  // 将节点链接到链表尾部
                                    next->prev = end.node_;  // 使next的prev指向上一个节点
                                }
                                size_ += add_size;  // 更新容器大小
                            }
                            catch (...)
                            {
                                auto enode = end.node_;
                                while (true)
                                {
                                    auto prev = enode->prev;
                                    destroy_node(enode->as_node());  // 销毁节点
                                    if (prev == nullptr)
                                        break;
                                    enode = prev;
                                }
                                throw;
                            }
                            link_nodes(pos.node_, r.node_, end.node_);  // 将新节点链接到链表中
                        }
                        return r;  // 返回指向新插入节点的迭代器
                    }

                // 在 pos 处插入 [first, last) 的元素
                template <class T>
                    template <class Iter>
                    typename list<T>::iterator 
                    list<T>::copy_insert(const_iterator pos, size_type n, Iter first)
                    {
                        iterator r(pos.node_); // 用于记录插入位置的迭代器
                        if (n != 0)
                        {
                            const auto add_size = n; // 插入元素的个数
                            auto node = create_node(*first); // 创建新节点，并将第一个元素赋值给新节点的值域
                            node->prev = nullptr; // 修改新节点的前向指针为 nullptr
                            r = iterator(node); // 更新记录插入位置的迭代器
                            iterator end = r; // 用于记录插入结束位置的迭代器
                            try
                            {
                                // 依次插入后续元素
                                for (--n, ++first; n > 0; --n, ++first, ++end)
                                {
                                    auto next = create_node(*first); // 创建新节点，并将当前元素赋值给新节点的值域
                                    end.node_->next = next->as_base();  // 将新节点链接到已有链表中
                                    next->prev = end.node_; // 设置新节点的前向指针
                                }
                                size_ += add_size; // 更新链表大小
                            }
                            catch (...)
                            {
                                // 如果发生异常，需要回滚已经插入的节点
                                auto enode = end.node_;
                                while (true)
                                {
                                    auto prev = enode->prev;
                                    destroy_node(enode->as_node()); // 销毁节点
                                    if (prev == nullptr)
                                        break;
                                    enode = prev;
                                }
                                throw;
                            }
                            link_nodes(pos.node_, r.node_, end.node_); // 将插入的链表段链接到原链表中
                        }
                        return r; // 返回记录插入位置的迭代器
                    }

                // 对 list 进行归并排序，返回一个迭代器指向区间最小元素的位置
                template <class T>
                    template <class Compared>
                    typename list<T>::iterator
                    list<T>::list_sort(iterator f1, iterator l2, size_type n, Compared comp)
                    {
                        if (n < 2)
                            return f1;

                        if (n == 2)
                        {
                            if (comp(*--l2, *f1))
                            {
                                auto ln = l2.node_;
                                unlink_nodes(ln, ln);
                                link_nodes(f1.node_, ln, ln);
                                return l2;
                            }
                            return f1;
                        }

                        auto n2 = n / 2;
                        auto l1 = f1;
                        mystl::advance(l1, n2);
                        auto result = f1 = list_sort(f1, l1, n2, comp);  // 前半段的最小位置
                        auto f2 = l1 = list_sort(l1, l2, n - n2, comp);  // 后半段的最小位置

                        // 把较小的一段区间移到前面
                        if (comp(*f2, *f1))
                        {
                            auto m = f2;
                            ++m;
                            for (; m != l2 && comp(*m, *f1); ++m)
                                ;
                            auto f = f2.node_;
                            auto l = m.node_->prev;
                            result = f2;
                            l1 = f2 = m;
                            unlink_nodes(f, l);
                            m = f1;
                            ++m;
                            link_nodes(f1.node_, f, l);
                            f1 = m;
                        }
                        else
                        {
                            ++f1;
                        }

                        // 合并两段有序区间
                        while (f1 != l1 && f2 != l2)
                        {
                            if (comp(*f2, *f1))
                            {
                                auto m = f2;
                                ++m;
                                for (; m != l2 && comp(*m, *f1); ++m)
                                    ;
                                auto f = f2.node_;
                                auto l = m.node_->prev;
                                if (l1 == f2)
                                    l1 = m;
                                f2 = m;
                                unlink_nodes(f, l);
                                m = f1;
                                ++m;
                                link_nodes(f1.node_, f, l);
                                f1 = m;
                            }
                            else
                            {
                                ++f1;
                            }
                        }
                        return result; // 返回最小元素的位置
                    }

                // 重载比较操作符
                template <class T>
                    bool operator==(const list<T>& lhs, const list<T>& rhs)
                    {
                        auto f1 = lhs.cbegin();
                        auto f2 = rhs.cbegin();
                        auto l1 = lhs.cend();
                        auto l2 = rhs.cend();
                        for (; f1 != l1 && f2 != l2 && *f1 == *f2; ++f1, ++f2)
                            ;
                        return f1 == l1 && f2 == l2; // 当且仅当所有元素都相等才返回 true
                    }

                template <class T>
                    bool operator<(const list<T>& lhs, const list<T>& rhs)
                    {
                        return mystl::lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend()); // 使用 mystl::lexicographical_compare 进行比较
                    }

                template <class T>
                    bool operator!=(const list<T>& lhs, const list<T>& rhs)
                    {
                        return !(lhs == rhs); // 当且仅当不相等时返回 true
                    }

                template <class T>
                    bool operator>(const list<T>& lhs, const list<T>& rhs)
                    {
                        return rhs < lhs; // 使用 operator< 进行比较
                    }

                template <class T>
                    bool operator<=(const list<T>& lhs, const list<T>& rhs)
                    {
                        return !(rhs < lhs); // 当且仅当 rhs 不小于 lhs 时返回 true
                    }

                template <class T>
                    bool operator>=(const list<T>& lhs, const list<T>& rhs)
                    {
                        return !(lhs < rhs); // 当且仅当 lhs 不小于 rhs 时返回 true
                    }

                // 重载 mystl 的 swap
                template <class T>
                    void swap(list<T>& lhs, list<T>& rhs) noexcept
                    {
                        lhs.swap(rhs); // 调用 swap 成员函数进行交换
                    }

        } // namespace mystl
#endif // !MYTINYSTL_LIST_H_
