#ifndef TINYSTL_SET_H
#define TINYSTL_SET_H

// 该头文件包含 set / multiset

#include "rb_tree.h"
#include "functional.h"
#include "algobase.h"

namespace mystl
{

    // 模板类 set<Key, Compare> 以 rb_tree_ 作为底层容器, 键值不允许重复
    template <class Key, class Compare = mystl::less<Key>>
        class set
        {
            public:
                typedef Key         key_type;                   // 键的类型
                typedef Key         value_type;                 // 值的类型
                typedef Compare     key_compare;                // 键比较的函数对象类型
                typedef Compare     value_compare;              // 值比较的函数对象类型

            protected:
                typedef mystl::rb_tree<key_type, value_type,
                        mystl::identity<value_type>, key_compare> rep_type;
                typedef typename rep_type::iterator                 rep_iterator;
                rep_type tree_;                                   // 底层容器，红黑树

            public:
                typedef typename rep_type::pointer                  pointer;                    // 指向元素的指针类型
                typedef typename rep_type::const_pointer            const_pointer;              // 指向常量元素的指针类型
                typedef typename rep_type::const_reference          reference;                  // 引用类型
                typedef typename rep_type::const_reference          const_reference;            // 常量引用类型
                typedef typename rep_type::const_iterator           iterator;                   // 迭代器类型
                typedef typename rep_type::const_iterator           const_iterator;             // 常量迭代器类型
                typedef typename rep_type::const_reverse_iterator   reverse_iterator;           // 反向迭代器类型
                typedef typename rep_type::const_reverse_iterator   const_reverse_iterator;     // 常量反向迭代器类型
                typedef typename rep_type::size_type                size_type;                  // 元素数量类型
                typedef typename rep_type::difference_type          difference_type;            // 迭代器之间的距离类型
                typedef typename rep_type::node_allocator           allocator_type;             // 分配器类型

            public:
                // 默认构造函数
                set() = default;

                // 带有范围参数的构造函数
                template <class InputIter>
                    set(InputIter first, InputIter last)
                    : tree_()
                    { tree_.insert_unique(first, last); }

                // 带有初始化列表参数的构造函数
                set(std::initializer_list<value_type>ilist)
                    : tree_()
                { tree_.insert_unique(ilist.begin(), ilist.end()); }

                // 拷贝构造函数
                set(const set<Key>& other)
                    : tree_(other.tree_)
                {
                }

                // 移动构造函数
                set(set<Key>&& other) noexcept
                    : tree_(mystl::move(other.tree_))
                    {
                    }

                // 拷贝赋值运算符
                set<Key>& operator=(const set<Key>& rhs)
                {
                    tree_ = rhs.tree_;
                    return *this;
                }

                // 移动赋值运算符
                set<Key>& operator=(set<Key>&& rhs)
                {
                    tree_ = mystl::move(rhs.tree_);
                    return *this;
                }

                // 初始化列表赋值运算符
                set<Key>& operator=(std::initializer_list<value_type> ilist)
                {
                    tree_.clear();
                    tree_.insert_unique(ilist.begin(), ilist.end());
                    return *this;
                }

                // 返回键比较函数对象
                key_compare     key_comp()   const { return tree_.key_compare; }
                // 返回值比较函数对象
                value_compare   value_comp() const { return tree_.key_compare; }

                // 迭代器相关操作

                // 返回指向容器中第一个元素的迭代器
                iterator                begin()            noexcept
                { return tree_.begin(); }
                const_iterator          begin()      const noexcept
                { return tree_.begin(); }

                // 返回指向容器末尾（最后一个元素的下一个位置）的迭代器
                iterator                end()              noexcept
                { return tree_.end(); }
                const_iterator          end()        const noexcept
                { return tree_.end(); }

                // 返回指向容器末尾（最后一个元素）的反向迭代器
                reverse_iterator        rbegin()           noexcept
                { return reverse_iterator(end()); }
                const_reverse_iterator  rbegin()     const noexcept
                { return const_reverse_iterator(end()); }

                // 返回指向容器开始（第一个元素的前一个位置）的反向迭代器
                reverse_iterator        rend()             noexcept
                { return reverse_iterator(begin()); }
                const_reverse_iterator  rend()       const noexcept
                { return const_reverse_iterator(begin()); }

                // 返回指向容器开始的常量迭代器
                const_iterator          cbegin()     const noexcept
                { return begin(); }
                // 返回指向容器末尾（最后一个元素的下一个位置）的常量迭代器
                const_iterator          cend()       const noexcept
                { return end(); }
                // 返回指向容器末尾（最后一个元素）的反向常量迭代器
                const_reverse_iterator  crbegin()    const noexcept
                { return rbegin(); }
                // 返回指向容器开始（第一个元素的前一个位置）的反向常量迭代器
                const_reverse_iterator  crend()      const noexcept
                { return rend(); }

                // 容量相关操作

                // 判断容器是否为空
                bool                    empty()      const noexcept { return tree_.empty(); }
                // 返回容器中元素的数量
                size_type               size()       const noexcept { return tree_.size(); }
                // 返回容器可能包含的最大元素数量
                size_type               max_size()   const noexcept { return tree_.max_size(); }

                // 修改容器操作

                // 在容器中构造新元素，使用参数构造函数的形式
                template <class ...Args>
                    mystl::pair<iterator, bool> emplace(Args&& ...args)
                    { return tree_.emplace_unique(mystl::forward<Args>(args)...); }

                // 在容器中构造新元素，使用参数构造函数的形式，并提供一个提示位置
                template <class ...Args>
                    iterator emplace_hint(iterator hint, Args&& ...args)
                    { return tree_.emplace_unique_use_hint((rep_iterator&)hint, mystl::forward<Args>(args)...); }

                // 插入元素到容器中，如果插入成功返回一个迭代器和 true，如果已经存在相同的键则返回迭代器和 false
                pair<iterator, bool> insert(const value_type& value)
                { return tree_.insert_unique(value); }

                // 插入元素到容器中，如果插入成功返回一个迭代器和 true，如果已经存在相同的键则返回迭代器和 false
                pair<iterator, bool> insert(value_type&& value)
                { return tree_.insert_unique(mystl::move(value)); }

                // 在指定位置插入元素到容器中，返回插入后的迭代器
                iterator insert(iterator hint, const value_type& value)
                { return tree_.insert_unique((rep_iterator&)hint, value); }

                // 在指定位置插入元素到容器中，返回插入后的迭代器
                iterator insert(iterator hint, value_type&& value)
                { return tree_.insert_unique((rep_iterator&)hint, mystl::move(value)); }

                // 插入范围内的元素到容器中
                template <class InputIter>
                    void insert(InputIter first, InputIter last)
                    { tree_.insert_unique(first, last); }

                // 删除指定位置上的元素
                void        erase(iterator postition)
                { tree_.erase((rep_iterator&)postition); }

                // 删除容器中指定键对应的元素，返回删除的数量（0 或 1）
                size_type   erase(const key_type& key)           { return tree_.erase_unique(key); }

                // 删除指定范围内的元素
                void        erase(iterator first, iterator last)
                { return tree_.erase((rep_iterator&)first, (rep_iterator&)last); }

                // 清空容器
                void        clear() { tree_.clear(); }
            public:
                // 查找操作

                // 查找容器中指定键的元素，返回指向该元素的迭代器，如果不存在则返回 end()
                iterator        find(const key_type& key)              { return tree_.find(key); }
                // 查找容器中指定键的元素，返回指向该元素的常量迭代器，如果不存在则返回 end()
                const_iterator  find(const key_type& key)        const { return tree_.find(key); }

                // 返回容器中等于指定键的元素的数量（0 或 1）
                size_type       count(const key_type& key)       const { return tree_.count_unique(key); }

                // 返回第一个大于或等于给定键的元素的迭代器
                iterator        lower_bound(const key_type& key)       { return tree_.lower_bound(key); }
                // 返回第一个大于或等于给定键的元素的常量引用
                const_reference lower_bound(const key_type& key) const { return tree_.lower_bound(key); }

                // 返回第一个大于给定键的元素的迭代器
                iterator        upper_bound(const key_type& key)       { return tree_.upper_bound(key); }
                // 返回第一个大于给定键的元素的常量引用
                const_reference upper_bound(const key_type& key) const { return tree_.upper_bound(key); }

                // 返回范围内等于给定键的元素的迭代器对
                pair<iterator, iterator>
                    equal_range(const key_type& key)
                    { return tree_.equal_range_unique(key); }

                // 返回范围内等于给定键的元素的常量迭代器对
                pair<const_iterator, const_iterator>
                    equal_range(const key_type& key) const
                    { return tree_.equal_range_unique(key); }

                // 交换两个容器的内容
                void swap(set<Key>& rhs) noexcept
                { tree_.swap(rhs.tree_); }
            public:
                // 比较操作

                // 判断容器是否与另一个容器相等
                bool operator==(const set<Key>& rhs) { return tree_ == rhs.tree_; }
                // 判断容器是否小于另一个容器
                bool operator<(const set<Key>& rhs)  { return tree_ < rhs.tree_; }

                // 判断容器是否不等于另一个容器
                bool operator!=(const set<Key>& rhs) { return !(*this == rhs); }
                // 判断容器是否大于另一个容器
                bool operator>(const set<Key>& rhs)  { return rhs < *this; }
                // 判断容器是否小于等于另一个容器
                bool operator<=(const set<Key>& rhs) { return !(rhs < *this); }
                // 判断容器是否大于等于另一个容器
                bool operator>=(const set<Key>& rhs) { return !(*this < rhs); }
        };

    // 模板类 multiset 键值允许重复
    template <class Key, class Compare = mystl::less<Key>>
        class multiset
        {
            public:
                // 定义类型别名
                typedef Key         key_type;               // 键值类型
                typedef Key         value_type;             // 值类型
                typedef Compare     key_compare;            // 键比较函数类型
                typedef Compare     value_compare;          // 值比较函数类型

            protected:
                typedef mystl::rb_tree<key_type, value_type, mystl::identity<value_type>, key_compare> rep_type; // 底层红黑树容器类型
                typedef typename rep_type::iterator                 rept_iterator;           // 迭代器类型
                rep_type tree_;                                                              // 底层红黑树对象

            public:
                typedef typename rep_type::pointer                  pointer;                // 指针类型
                typedef typename rep_type::const_pointer            const_pointer;          // 常量指针类型
                typedef typename rep_type::const_reference          reference;              // 引用类型
                typedef typename rep_type::const_reference          const_reference;        // 常量引用类型
                typedef typename rep_type::const_iterator           iterator;               // 迭代器类型
                typedef typename rep_type::const_iterator           const_iterator;         // 常量迭代器类型
                typedef typename rep_type::const_reverse_iterator   reverse_iterator;       // 反向迭代器类型
                typedef typename rep_type::const_reverse_iterator   const_reverse_iterator; // 常量反向迭代器类型
                typedef typename rep_type::size_type                size_type;              // 大小类型
                typedef typename rep_type::difference_type          difference_type;        // 差值类型
                typedef typename rep_type::node_allocator           allocator_type;         // 分配器类型

            public:
                // 构造函数
                multiset() = default;

                template <class InputIter>
                    multiset(InputIter first, InputIter last)
                    : tree_()
                    {
                        // 使用底层红黑树的插入函数插入[first, last)范围内的元素
                        tree_.insert_multi(first, last);
                    }

                multiset(std::initializer_list<value_type> ilist)
                    : tree_()
                {
                    // 使用底层红黑树的插入函数插入初始化列表中的元素
                    tree_.insert_multi(ilist.begin(), ilist.end());
                }

                multiset(const multiset<Key>& other)
                    : tree_(other.tree_)
                {
                    // 拷贝构造函数
                }

                multiset(multiset<Key>&& other) noexcept
                    : tree_(mystl::move(other.tree_))
                    {
                        // 移动构造函数
                    }

                // 赋值运算符重载
                multiset<Key>& operator=(const multiset<Key>& rhs)
                {
                    tree_ = rhs.tree_;
                    return *this;
                }

                multiset<Key>& operator=(multiset<Key>&& rhs)
                {
                    tree_ = mystl::move(rhs.tree_);
                    return *this;
                }

                multiset<Key>& operator=(std::initializer_list<value_type> ilist)
                {
                    // 清空当前multiset，并插入初始化列表中的元素
                    tree_.clear();
                    tree_.insert_multi(ilist.begin(), ilist.end());
                    return *this;
                }

                // 获取比较函数对象
                key_compare             key_comp()   const { return tree_.key_compare; }
                value_compare           value_comp() const { return tree_.key_compare; }

                // 迭代器相关操作
                iterator                begin()            noexcept { return tree_.begin(); }
                const_iterator          begin()      const noexcept { return tree_.begin(); }
                iterator                end()              noexcept { return tree_.end(); }
                const_iterator          end()        const noexcept { return tree_.end(); }

                reverse_iterator        rbegin()           noexcept { return reverse_iterator(end()); }
                const_reverse_iterator  rbegin()     const noexcept { return const_reverse_iterator(end()); }
                reverse_iterator        rend()             noexcept { return reverse_iterator(begin()); }
                const_reverse_iterator  rend()       const noexcept { return const_reverse_iterator(begin()); }

                const_iterator          cbegin()     const noexcept { return begin(); }
                const_iterator          cend()       const noexcept { return end(); }
                const_reverse_iterator  crbegin()    const noexcept { return rbegin(); }
                const_reverse_iterator  crend()      const noexcept { return rend(); }

                // 容量相关操作
                bool                    empty()      const noexcept { return tree_.empty(); }
                size_type               size()       const noexcept { return tree_.size(); }
                size_type               max_size()   const noexcept { return tree_.max_size(); }

                // 插入元素
                template <class... Args>
                    iterator emplace(Args&&... args)
                    {
                        // 调用底层红黑树的插入函数
                        return tree_.emplace_multi(mystl::forward<Args>(args)...);
                    }

                template <class... Args>
                    iterator emplace_hint(iterator hint, Args&&... args)
                    {
                        // 调用底层红黑树的使用提示的插入函数
                        return tree_.template emplace_multi_use_hint((rept_iterator&)hint, mystl::forward<Args>(args)...);
                    }

                iterator insert(const value_type& value)
                {
                    // 调用底层红黑树的插入函数
                    return tree_.insert_multi(value);
                }

                iterator insert(value_type&& value)
                {
                    // 调用底层红黑树的插入函数
                    return tree_.insert_multi(mystl::move(value));
                }

                iterator insert(iterator hint, const value_type& value)
                {
                    // 调用底层红黑树的使用提示的插入函数
                    return tree_.insert_multi((rept_iterator&)hint, value);
                }

                iterator insert(iterator hint, value_type&& value)
                {
                    // 调用底层红黑树的使用提示的插入函数
                    return tree_.insert_multi((rept_iterator&)hint, mystl::move(value));
                }

                template <class InputIter>
                    void insert(InputIter first, InputIter last)
                    {
                        // 调用底层红黑树的插入函数，插入[first, last)范围内的元素
                        tree_.insert_multi(first, last);
                    }

                // 删除元素
                void erase(iterator position)
                {
                    // 调用底层红黑树的删除函数
                    tree_.erase((rept_iterator&)position);
                }

                size_type erase(const key_type& key)
                {
                    // 调用底层红黑树的删除函数，删除键值为key的元素
                    return tree_.erase_multi(key);
                }

                void erase(iterator first, iterator last)
                {
                    // 调用底层红黑树的删除函数，删除[first, last)范围内的元素
                    return tree_.erase((rept_iterator&)first, (rept_iterator&)last);
                }

                void clear()
                {
                    // 清空底层红黑树
                    tree_.clear();
                }

            public:
                // 查找操作
                iterator        find(const key_type& key)
                {
                    // 在底层红黑树中查找键值为key的元素，并返回迭代器
                    return tree_.find(key);
                }

                const_iterator  find(const key_type& key) const
                {
                    // 在底层红黑树中查找键值为key的元素，并返回常量迭代器
                    return tree_.find(key);
                }

                size_type       count(const key_type& key) const
                {
                    // 在底层红黑树中计算键值为key的元素个数
                    return tree_.count_multi(key);
                }

                iterator        lower_bound(const key_type& key)
                {
                    // 返回在底层红黑树中第一个不小于key的键值的位置
                    return tree_.lower_bound(key);
                }

                const_reference lower_bound(const key_type& key) const
                {
                    // 返回在底层红黑树中第一个不小于key的键值的位置
                    return tree_.lower_bound(key);
                }

                iterator        upper_bound(const key_type& key)
                {
                    // 返回在底层红黑树中第一个大于key的键值的位置
                    return tree_.upper_bound(key);
                }

                const_reference upper_bound(const key_type& key) const
                {
                    // 返回在底层红黑树中第一个大于key的键值的位置
                    return tree_.upper_bound(key);
                }

                // 返回键值范围等于[key]的元素的区间
                pair<iterator, iterator> equal_range(const key_type& key)
                {
                    // 返回一个迭代器对，第一个迭代器指向第一个键值大于等于key的元素，第二个迭代器指向第一个键值大于key的元素
                    return tree_.equal_range_multi(key);
                }

                pair<const_iterator, const_iterator> equal_range(const key_type& key) const
                {
                    // 返回一个常量迭代器对，第一个迭代器指向第一个键值大于等于key的元素，第二个迭代器指向第一个键值大于key的元素
                    return tree_.equal_range_multi(key);
                }

                // 交换操作
                void swap(multiset<Key>& rhs) noexcept
                {
                    // 交换底层红黑树
                    tree_.swap(rhs.tree_);
                }

            public:
                // 比较操作符重载
                bool operator==(const multiset<Key>& rhs) { return tree_ == rhs.tree_; }
                bool operator<(const multiset<Key>& rhs) { return tree_ < rhs.tree_; }
                bool operator!=(const multiset<Key>& rhs) { return !(*this == rhs); }
                bool operator>(const multiset<Key>& rhs) { return rhs < *this; }
                bool operator<=(const multiset<Key>& rhs) { return !(rhs < *this); }
                bool operator>=(const multiset<Key>& rhs) { return !(*this < rhs); }
        };

}


#endif //TINYSTL_SET_H
