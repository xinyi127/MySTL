#ifndef TINYSTL_MAP_H
#define TINYSTL_MAP_H

// 这个头文件包含了两个模板类 map / mulitmap
// map:      映射, 元素具有键值和实值, 元素在 map 中按照键值默认升序序排列, 且键值在 map 中唯一
// multimap: 映射, 元素具有键值和实值, 元素在 multimap 中按照键值默认升序排序, 键值在 multimap 中不唯一

#include "rb_tree.h"
#include "functional.h"


namespace mystl
{

    // 模板类 map 键值唯一
    // 参数一表示键值类型, 参数二表示对应的实际值类型, 参数三表示确定键值优先级的比较方式, 默认采取 < 比较
    template <class Key, class T, class Compare = mystl::less<T>>
        class map
        {
            public:
                typedef Key                         key_type;       // 键值类型
                typedef T                           data_type;      // 实际值类型
                typedef T                           mapped_type;    // 映射值类型，与实际值类型相同
                typedef mystl::pair<const Key, T>   value_type;     // 键值对类型
                typedef Compare                     key_compare;    // 键值比较方式

                // 定义仿函数, 用来进行元素的比较
                class value_compare
                    : public mystl::binary_function<value_type, value_type, bool>
                {
                    friend class map<Key, T, Compare>;
                    private:
                    Compare comp;                                   // 键值比较方式对象
                    value_compare(Compare c) : comp(c) {}           // 构造函数接收一个键值比较方式对象
                    public:
                    bool operator()(const value_type& lhs, const value_type& rhs) const
                    {
                        return comp(lhs.first, rhs.first);          // 调用键值比较方式对象进行比较
                    }
                };

                // 用于从键值对中提取键值的仿函数
                class select1st
                    : public mystl::unarg_function<value_type, T>
                {
                    friend class map<Key, T, Compare>;

                    public:
                    T operator()(const value_type& value) const
                    {
                        return value.first;                         // 返回键值
                    }

                };

            private:
                typedef mystl::rb_tree<key_type, value_type,
                        select1st, key_compare> rep_type;    // 底层红黑树实现容器类型
                rep_type t;                                                 // 红黑树对象

            public:
                typedef typename rep_type::pointer                  pointer;                // 指针类型
                typedef typename rep_type::const_pointer            const_pointer;          // 常量指针类型
                typedef typename rep_type::reference                reference;              // 引用类型
                typedef typename rep_type::const_reference          const_reference;        // 常量引用类型
                typedef typename rep_type::iterator                 iterator;               // 迭代器类型
                typedef typename rep_type::const_iterator           const_iterator;         // 常量迭代器类型
                typedef typename rep_type::reverse_iterator         reverse_iterator;       // 逆序迭代器类型
                typedef typename rep_type::const_reverse_iterator   const_reverse_iterator; // 常量逆序迭代器类型
                typedef typename rep_type::size_type                size_type;              // 元素数量类型
                typedef typename rep_type::difference_type          difference_type;        // 元素差值类型
                typedef typename rep_type::allocator_type           allocator_type;         // 分配器类型


            public:
                // 构造 / 复制 / 移动 / 重载赋值运算符
                map() = default;

                template<class InputIter>
                    map(InputIter first, InputIter last)
                    :t()
                    { t.insert_unique(first, last); }

                map(std::initializer_list<value_type> ilist)
                    :t()
                { t.insert_unique(ilist.begin(), ilist.end()); }

                explicit map(const map<Key, T, Compare>& other)
                    :t(other.t)
                {
                }

                explicit map(map<Key, T, Compare>&& other)
                    :t(mystl::move(other.t))
                {
                }

                map<Key, T, Compare>& operator=(const map<Key, T, Compare>& rhs)
                {
                    t = rhs.t;
                    return *this;
                }

                map<Key, T, Compare>& operator=(map<Key, T, Compare>&& rhs)
                {
                    t = rhs.t;
                    return *this;
                }

                map<Key, T, Compare>& operator=(std::initializer_list<value_type> ilist)
                {
                    t.clear();
                    t.insert_unique(ilist.begin(), ilist.end());
                    return *this;
                }

                key_compare     key_comp()      const { return key_compare(); }           // 返回键值比较方式对象
                value_compare   value_comp()    const { return value_compare(key_comp()); }  // 返回元素比较方式对象
                allocator_type  get_allocator() const { return allocator_type(); }       // 返回分配器对象

                // 返回迭代器
                iterator        begin()                   noexcept { return t.begin(); }                  // 返回首个元素的迭代器
                const_iterator  begin()             const noexcept { return t.begin(); }                  // 返回首个元素的常量迭代器
                iterator        end()                     noexcept { return t.end(); }                    // 返回尾后迭代器
                const_iterator  end()               const noexcept { return t.end(); }                    // 返回尾后常量迭代器

                reverse_iterator        rbegin()          noexcept { return reverse_iterator(end()); }    // 返回逆序起始迭代器
                const_reverse_iterator  rbegin()    const noexcept { return const_reverse_iterator(end()); } // 返回逆序起始常量迭代器
                reverse_iterator        rend()            noexcept { return reverse_iterator(begin()); }  // 返回逆序尾后迭代器
                const_reverse_iterator  rend()      const noexcept { return const_reverse_iterator(begin()); } // 返回逆序尾后常量迭代器

                const_iterator          cbegin()    const noexcept { return begin(); }                     // 返回首个元素的常量迭代器
                const_iterator          cend()      const noexcept { return end(); }                       // 返回尾后常量迭代器
                const_reverse_iterator  crbegin()   const noexcept { return rbegin(); }                    // 返回逆序起始常量迭代器
                const_reverse_iterator  crend()     const noexcept { return rend(); }                      // 返回逆序尾后常量迭代器


                bool                    empty()     const noexcept { return t.empty(); }                   // 判断容器是否为空
                size_type               size()      const noexcept { return t.size(); }                    // 返回容器中元素的数量
                size_type               max_size()  const noexcept { return t.max_size(); }                // 返回容器可能包含的最大元素数量

                // 访问内部元素
                mapped_type& at(const key_type& key)
                {
                    iterator it = lower_bound(key);

                    // 不存在元素则抛出异常
                    THROW_OUT_OF_RANGE_IF(it == end() || key_comp()(it->first, key),
                            "map<Key, T> no such element exists");
                    return it->second;
                }

                const mapped_type& at(const key_type& key) const
                {
                    iterator it = lower_bound(key);

                    // 不存在元素则抛出异常
                    THROW_OUT_OF_RANGE_IF(it == end() || key_comp()(it->first, key),
                            "map<Key, T> no such element exists");
                    return it->second;
                }

                mapped_type& operator[](const key_type& key)
                {
                    iterator it = lower_bound(key);
                    if (it == end() || key_comp()(key, it->first))
                        it = emplace_hint(it, key, T());
                    return it->second;
                }

                mapped_type& operator[](key_type&& key)
                {
                    iterator it = lower_bound(key);
                    if (it == end() || key_comp()(key, it->first))
                        it = emplace_hint(it, mystl::move(key), T());
                    return it->second;
                }

                // 插入删除相关

                // 插入元素，参数为构造键值对需要的参数
                template <class ...Args>
                    mystl::pair<iterator, bool> emplace(Args&& ...args)
                    {
                        return t.emplace_unique(mystl::forward<Args>(args)...);
                    }

                // 在给定迭代器位置插入元素，参数为构造键值对需要的参数和给定的迭代器位置
                template <class ...Args>
                    iterator emplace_hint(iterator hint, Args&& ...args)
                    {
                        return t.emplace_unique_use_hint(hint, mystl::forward<Args>(args)...);
                    }

                // 插入元素，参数为键值对
                mystl::pair<iterator, bool> insert(const value_type& value)
                {
                    return t.insert_unique(value);
                }

                // 插入元素，参数为右值引用的键值对
                mystl::pair<iterator, bool> insert(value_type&& value)
                {
                    return t.insert_unique(mystl::move(value));
                }

                // 在给定迭代器位置插入元素，参数为键值对和给定的迭代器位置
                iterator insert(iterator hint, const value_type& value)
                {
                    return t.insert_unique(hint, value);
                }

                // 在给定迭代器位置插入元素，参数为右值引用的键值对和给定的迭代器位置
                iterator insert(iterator hint, value_type&& value)
                {
                    return t.insert_unique(hint, mystl::move(value));
                }

                // 插入一段范围内的元素，参数为迭代器范围
                template <class InputIter>
                    void insert(InputIter first, InputIter last)
                    {
                        t.insert_unique(first, last);
                    }

                // 移除指定位置的元素
                void        erase(iterator position)             { t.erase(position); }

                // 移除指定键值的元素
                size_type   erase(const key_type& key)           { return t.erase_unique(key); }

                // 移除一段范围内的元素，参数为起始和结束迭代器
                void        erase(iterator first, iterator last) { t.erase(first, last); }

                // 清空容器内的所有元素
                void        clear()                              { t.clear(); }

                // 查找指定键值的元素，返回指向该元素的迭代器
                iterator        find(const key_type& key)               { return t.find(key); }
                const_iterator  find(const key_type& key)        const  { return t.find(key); }

                // 统计指定键值的元素数量
                size_type       count(const key_type& key)       const  { return t.count_unique(key); }

                // 返回第一个不小于指定键值的元素的迭代器
                iterator        lower_bound(const key_type& key)        { return t.lower_bound(key); }
                const_iterator  lower_bound(const key_type& key) const  { return t.lower_bound(key); }

                // 返回第一个大于指定键值的元素的迭代器
                iterator        upper_bound(const key_type& key)        { return t.upper_bound(key); }
                const_iterator  upper_bound(const key_type& key) const  { return t.upper_bound(key); }

                // 返回与指定键值匹配的元素范围，以 pair 形式返回两个指示界限的迭代器
                mystl::pair<iterator, iterator> equal_range(const key_type& key)
                { return t.equal_range_unique(key); }

                // 返回与指定键值匹配的元素范围，以 pair 形式返回两个指示界限的常量迭代器
                mystl::pair<const iterator, const iterator> equal_range(const key_type& key) const
                { return t.equal_range_unique(key); }

                // 交换两个容器的内容
                void swap(map<Key, T, Compare>& rhs) noexcept
                { t.swap(rhs.t); }

            public:
                // 运算符重载
                bool operator==(const map<Key, T, Compare>& rhs) const { return this->t == rhs.t; }     // 判断相等运算
                bool operator<(const map<Key, T, Compare>& rhs) const  { return this->t < rhs.t; }      // 判断小于运算
                bool operator!=(const map<Key, T, Compare>& rhs) const { return !(this->t == rhs.t); }  // 判断不等运算
                bool operator>(const map<Key, T, Compare>& rhs) const  { return rhs.t < this->t; }      // 判断大于运算
                bool operator<=(const map<Key, T, Compare>& rhs) const { return !(rhs.t < this->t); }   // 判断小于等于运算
                bool operator>=(const map<Key, T, Compare>& rhs) const { return !(this->t < rhs.t); }   // 判断大于等于运算
        };

    template <class Key, class T, class Compare>
        void swap(map<Key, T, Compare>& lhs, map<Key, T, Compare>& rhs)
        {
            lhs.swap(rhs);  // 调用左侧map对象的swap成员函数，将其与右侧map对象交换
        }

    /************************************************************************************/

    // 模板类 multimap, 键值允许重复
    // 参数一表示键值类型, 参数二表示对应的实际值类型, 参数三表示确定键值优先级的比较方式, 默认采取 < 比较
    template <class Key, class T, class Compare = mystl::less<T>>
        class multimap
        {
            public:
                typedef Key                         key_type;                   // 键值类型
                typedef T                           data_type;                  // 实际值类型
                typedef T                           mapped_type;                // 映射类型
                typedef mystl::pair<const Key, T>   value_type;                 // 键值对类型
                typedef Compare                     key_compare;                // 键的比较方式

                // 定义仿函数, 用来进行元素的比较
                class value_compare
                    : public mystl::binary_function<value_type, value_type, bool>
                {
                    friend class map<Key, T, Compare>;
                    private:
                    Compare comp;                                   // 存储键的比较方式
                    value_compare(Compare c) : comp(c) {}           // 构造函数
                    public:
                    bool operator()(const value_type& lhs, const value_type& rhs) const
                    {
                        return comp(lhs.first, rhs.first);          // 调用键的比较方式
                    }
                };

                // 选择键作为第一个元素的仿函数
                class select1st
                    : public mystl::unarg_function<value_type, T>
                {
                    friend class map<Key, T, Compare>;

                    public:
                    T operator()(const value_type& value) const
                    {
                        return value.first;                         // 返回键值对的键
                    }

                };

            private:
                typedef mystl::rb_tree<key_type, value_type,
                        select1st, key_compare> rep_type;  // 使用红黑树作为底层存储结构
                rep_type t;                                             // 存储底层红黑树

            public:
                typedef typename rep_type::pointer                  pointer;             // 指针类型
                typedef typename rep_type::const_pointer            const_pointer;       // 常量指针类型
                typedef typename rep_type::reference                reference;           // 引用类型
                typedef typename rep_type::const_reference          const_reference;     // 常量引用类型
                typedef typename rep_type::iterator                 iterator;            // 迭代器类型
                typedef typename rep_type::const_iterator           const_iterator;      // 常量迭代器类型
                typedef typename rep_type::reverse_iterator         reverse_iterator;    // 反向迭代器类型
                typedef typename rep_type::const_reverse_iterator   const_reverse_iterator;  // 常量反向迭代器类型
                typedef typename rep_type::size_type                size_type;           // 大小类型
                typedef typename rep_type::difference_type          difference_type;     // 差值类型
                typedef typename rep_type::allocator_type           allocator_type;      // 分配器类型


            public:
                // 构造 / 复制 / 移动 / 重载赋值运算符
                multimap() = default;                                          // 默认构造函数

                template<class InputIter>
                    multimap(InputIter first, InputIter last)
                    :t()
                    { t.insert_multi(first, last); }                                // 利用迭代器范围构造

                multimap(std::initializer_list<value_type> ilist)
                    :t()
                { t.insert_multi(ilist.begin(), ilist.end()); }                 // 利用初始化列表构造

                explicit multimap(const multimap<Key, T, Compare>& other)
                    :t(other.t)
                {
                }                                                              // 复制构造函数

                explicit multimap(multimap<Key, T, Compare>&& other)
                    :t(mystl::move(other.t))
                {
                }                                                              // 移动构造函数

                multimap<Key, T, Compare>& operator=(const multimap<Key, T, Compare>& rhs)
                {
                    t = rhs.t;                                                 // 赋值运算符重载
                    return *this;
                }

                multimap<Key, T, Compare>& operator=(multimap<Key, T, Compare>&& rhs)
                {
                    t = rhs.t;                                                 // 移动赋值运算符重载
                    return *this;
                }

                multimap<Key, T, Compare>& operator=(std::initializer_list<value_type> ilist)
                {
                    t.clear();                                                  // 清空当前容器
                    t.insert_multi(ilist.begin(), ilist.end());                  // 重新插入元素
                    return *this;
                }

                key_compare     key_comp()      const { return key_compare(); }   // 获取键的比较方式
                value_compare   value_comp()    const { return value_compare(key_comp()); }  // 获取元素的比较方式
                allocator_type  get_allocator() const { return allocator_type(); }               // 获取分配器

                // 返回迭代器
                iterator        begin()                   noexcept
                { return t.begin(); }

                const_iterator  begin()     const noexcept
                { return t.begin(); }

                iterator        end()             noexcept
                { return t.end(); }

                const_iterator  end()       const noexcept
                { return t.end(); }

                reverse_iterator        rbegin()          noexcept
                { return reverse_iterator(end()); }

                const_reverse_iterator  rbegin()    const noexcept
                { return const_reverse_iterator(end()); }

                reverse_iterator        rend()            noexcept
                { return reverse_iterator(begin()); }

                const_reverse_iterator  rend()      const noexcept
                { return const_reverse_iterator(begin()); }

                const_iterator  cbegin()    const noexcept
                { return begin(); }

                const_iterator  cend()      const noexcept
                { return end(); }

                const_reverse_iterator  crbegin()   const noexcept
                { return rbegin(); }

                const_reverse_iterator  crend()     const noexcept
                { return rend(); }


                bool                    empty()     const noexcept { return t.empty(); }        // 判断容器是否为空
                size_type               size()      const noexcept { return t.size(); }         // 返回容器中元素个数
                size_type               max_size()  const noexcept { return t.max_size(); }     // 返回容器可存储的最大元素个数


                // 插入删除相关

                template <class ...Args>
                    iterator emplace(Args&& ...args)
                    {
                        return t.emplace_multi(mystl::forward<Args>(args)...);             // 在容器末尾构造元素
                    }

                template <class ...Args>
                    iterator emplace_hint(iterator hint, Args&& ...args)
                    {
                        return t.emplace_multi_use_hint(hint, mystl::forward<Args>(args)...);  // 在给定位置之前构造元素
                    }

                iterator insert(const value_type& value)
                {
                    return t.insert_multi(value);                                      // 插入元素
                }

                iterator insert(value_type&& value)
                {
                    return t.insert_multi(mystl::move(value));                          // 移动插入元素
                }

                iterator insert(iterator hint, const value_type& value)
                {
                    return t.insert_multi(hint, value);                                 // 在给定位置插入元素
                }

                iterator insert(iterator hint, value_type&& value)
                {
                    return t.insert_multi(hint, mystl::move(value));                     // 在给定位置插入元素
                }

                template <class InputIter>
                    void insert(InputIter first, InputIter last)
                    {
                        t.insert_multi(first, last);                                         // 插入迭代器范围内的元素
                    }

                void erase(iterator position)             { t.erase(position); }            // 删除指定位置的元素
                size_type   erase(const key_type& key)           { return t.erase_multi(key); } // 删除指定键对应的所有元素
                void        erase(iterator first, iterator last) { t.erase(first, last); }      // 删除指定范围内的元素

                void        clear()                              { t.clear(); }              // 清空容器

                iterator        find(const key_type& key)               { return t.find(key); }    // 查找指定键的元素
                const_iterator  find(const key_type& key)        const  { return t.find(key); }    // 查找指定键的元素（常量版本）

                size_type       count(const key_type& key)       const  { return t.count_unique(key); }   // 统计指定键对应的元素个数

                iterator        lower_bound(const key_type& key)        { return t.lower_bound(key); }    // 返回第一个不小于给定键的位置
                const_iterator  lower_bound(const key_type& key) const  { return t.lower_bound(key); }    // 返回第一个不小于给定键的位置（常量版本）

                iterator        upper_bound(const key_type& key)        { return t.upper_bound(key); }    // 返回第一个大于给定键的位置
                const_iterator  upper_bound(const key_type& key) const  { return t.upper_bound(key); }    // 返回第一个大于给定键的位置（常量版本）

                mystl::pair<iterator, iterator>
                    equal_range(const key_type& key)
                    { return t.equal_range_multi(key); }                        // 返回等于给定键的元素范围

                mystl::pair<const iterator, const iterator>
                    equal_range(const key_type& key) const
                    { return t.equal_range_multi(key); }                        // 返回等于给定键的元素范围（常量版本）

                void swap(multimap<Key, T, Compare>& rhs) noexcept
                { t.swap(rhs.t); }                                           // 交换两个容器中的元素
            public:
                // 运算符重载
                bool operator==(const multimap<Key, T, Compare>& rhs) { return this->t == rhs.t; }
                bool operator<(const multimap<Key, T, Compare>& rhs)  { return this->t < rhs.t; }
                bool operator!=(const multimap<Key, T, Compare>& rhs) { return !(this->t == rhs.t); }
                bool operator>(const multimap<Key, T, Compare>& rhs)  { return rhs.t < this->t; }
                bool operator<=(const multimap<Key, T, Compare>& rhs) { return !(rhs.t < this->t); }
                bool operator>=(const multimap<Key, T, Compare>& rhs) { return !(this->t < rhs.t); }
        };

    template <class Key, class T, class Compare>
        void swap(multimap<Key, T, Compare>& lhs, multimap<Key, T, Compare>& rhs)
        {
            lhs.swap(rhs);  // 调用 multimap 的swap成员函数，交换两个容器中的元素
        }

}

#endif //TINYSTL_MAP_H
