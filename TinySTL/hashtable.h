#ifndef TINYSTL_HASHTABLE_H
#define TINYSTL_HASHTABLE_H

// 这个头文件包含了一个模板类 hashtable
// hashtable: 哈希表, 使用拉链法处理冲突
// https://blog.csdn.net/jhgjhg225/article/details/124545266?spm=1001.2014.3001.5501

#include <initializer_list>

#include "algo.h"
#include "functional.h"
#include "memory.h"
#include "vector.h"
#include "util.h"
#include "exceptdef.h"

namespace mystl
{

    // hashtable 节点定义
    template <class T>
        struct hashtable_node
        {
            hashtable_node* next;  // 指向下一个节点的指针
            T value;               // 节点存储的值

            hashtable_node() = default;
            hashtable_node(const T& n) :next(nullptr), value(n)
            {
            }

            hashtable_node(const hashtable_node<T>& node)
                : next(node.next), value(node.value)
            {
            }

            hashtable_node(hashtable_node&& node)
                : next(node.next), value(mystl::move(node.value))
            {
                node.next = nullptr;
            }
        };

    template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
        class hashtable;

    template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
        struct hashtable_iterator;

    template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
        struct hashtable_const_iterator;

    template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
        struct hashtable_iterator
        {
            typedef hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>                  hashtable;
            typedef hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>         iterator;
            typedef hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>   const_iterator;
            typedef hashtable_node<Value>                                                 node;

            typedef forward_iterator_tag                                                  iterator_tag;
            typedef Value                                                                 value_type;
            typedef ptrdiff_t                                                             difference_type;
            typedef size_t                                                                size_type;
            typedef Value&                                                                reference;
            typedef Value*                                                                pointer;

            node* cur;             // 当前迭代器指向的节点
            hashtable* ht;         // 迭代器所属的hashtable对象

            hashtable_iterator() = default;
            hashtable_iterator(node* n, hashtable* tab)
                : cur(n), ht(tab)
            {
            }

            hashtable_iterator(const iterator& other)
                : cur(other.cur), ht(other.ht)
            {
            }

            hashtable_iterator(const const_iterator& other)
                : cur(other.cur), ht(other.ht)
            {
            }

            iterator& operator=(const iterator& rhs)
            {
                if (this != &rhs)
                {
                    cur = rhs.cur;    // 将当前节点指针指向rhs迭代器的节点
                    ht  = rhs.ht;     // 将所属hashtable对象指针指向rhs迭代器所属的hashtable对象
                }
                return *this;
            }

            iterator& operator=(const const_iterator& rhs)
            {
                if (this != &rhs)
                {
                    cur = rhs.cur;    // 将当前节点指针指向rhs迭代器的节点
                    ht  = rhs.ht;     // 将所属hashtable对象指针指向rhs迭代器所属的hashtable对象
                }
            }

            reference operator*()   const { return cur->value; }  // 返回当前节点存储的值的引用
            pointer   operator->()  const { return &(operator*()); }  // 返回当前节点存储的值的指针

            iterator& operator++();  // 前置自增运算符重载
            iterator& operator++(int);  // 后置自增运算符重载

            bool operator==(const iterator& other) const { return cur == other.cur; }  // 判断是否相等
            bool operator!=(const iterator& other) const { return cur != other.cur; }  // 判断是否不相等
        };

    template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
        struct hashtable_const_iterator
        {
            typedef hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>                  hashtable;
            typedef hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>         iterator;
            typedef hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>   const_iterator;
            typedef hashtable_node<Value>                                                 node;

            typedef forward_iterator_tag                                                  iterator_tag;
            typedef Value                                                                 value_type;
            typedef ptrdiff_t                                                             difference_type;
            typedef size_t                                                                size_type;
            typedef const Value&                                                          reference;
            typedef const Value*                                                          pointer;

            const node* cur;             // 当前迭代器指向的节点
            const hashtable* ht;         // 迭代器所属的hashtable对象

            hashtable_const_iterator() = default;
            hashtable_const_iterator(const node* n, const hashtable* tab)
                : cur(n), ht(tab)
            {
            }

            hashtable_const_iterator(const iterator& other)
                : cur(other.cur), ht(other.ht)
            {
            }

            hashtable_const_iterator(const const_iterator& other)
                : cur(other.cur), ht(other.ht)
            {
            }

            const_iterator& operator=(const const_iterator& rhs)
            {
                if (this != &rhs)
                {
                    cur = rhs.cur;    // 将当前节点指针指向rhs迭代器的节点
                    ht  = rhs.ht;     // 将所属hashtable对象指针指向rhs迭代器所属的hashtable对象
                }
            }

            const_iterator& operator=(const iterator& rhs)
            {
                if (this != &rhs)
                {
                    cur = rhs.cur;    // 将当前节点指针指向rhs迭代器的节点
                    ht  = rhs.ht;     // 将所属hashtable对象指针指向rhs迭代器所属的hashtable对象
                }
            }

            reference operator*()   const { return cur->value; }  // 返回当前节点存储的值的引用
            pointer   operator->()  const { return &(operator*()); }  // 返回当前节点存储的值的指针

            const_iterator& operator++();  // 前置自增运算符重载
            const_iterator& operator++(int);  // 后置自增运算符重载

            bool operator==(const const_iterator& rhs) const { return cur == rhs.cur; }  // 判断是否相等
            bool operator!=(const const_iterator& rhs) const { return cur != rhs.cur; }  // 判断是否不相等
        };



    // bucket 大小
#if (_MSC_VER && WIN64) || ((__GNUC__ || __clang__) && __SIZEOF_POINTER__ == 8)
#define SYSTEM_64 1
#else
#define SYSTEM_32 1
#endif

#ifdef SYSTEM_64
#define PRIME_NUM 99
    // 1. start with p = 101
    // 2. p = next_prime(p * 1.7)
    // 3. if p < (2 << 63), go to step 2, otherwise, go to step 4
    // 4. end with p = prev_prime(2 << 63 - 1)
    static constexpr size_t stl_prime_list[] =
    {
        101ull, 173ull, 263ull, 397ull, 599ull, 907ull, 1361ull, 2053ull, 3083ull,
        4637ull, 6959ull, 10453ull, 15683ull, 23531ull, 35311ull, 52967ull, 79451ull,
        119179ull, 178781ull, 268189ull, 402299ull, 603457ull, 905189ull, 1357787ull,
        2036687ull, 3055043ull, 4582577ull, 6873871ull, 10310819ull, 15466229ull,
        23199347ull, 34799021ull, 52198537ull, 78297827ull, 117446801ull, 176170229ull,
        264255353ull, 396383041ull, 594574583ull, 891861923ull, 1337792887ull,
        2006689337ull, 3010034021ull, 4515051137ull, 6772576709ull, 10158865069ull,
        15238297621ull, 22857446471ull, 34286169707ull, 51429254599ull, 77143881917ull,
        115715822899ull, 173573734363ull, 260360601547ull, 390540902329ull,
        585811353559ull, 878717030339ull, 1318075545511ull, 1977113318311ull,
        2965669977497ull, 4448504966249ull, 6672757449409ull, 10009136174239ull,
        15013704261371ull, 22520556392057ull, 33780834588157ull, 50671251882247ull,
        76006877823377ull, 114010316735089ull, 171015475102649ull, 256523212653977ull,
        384784818980971ull, 577177228471507ull, 865765842707309ull, 1298648764060979ull,
        1947973146091477ull, 2921959719137273ull, 4382939578705967ull, 6574409368058969ull,
        9861614052088471ull, 14792421078132871ull, 22188631617199337ull, 33282947425799017ull,
        49924421138698549ull, 74886631708047827ull, 112329947562071807ull, 168494921343107851ull,
        252742382014661767ull, 379113573021992729ull, 568670359532989111ull, 853005539299483657ull,
        1279508308949225477ull, 1919262463423838231ull, 2878893695135757317ull, 4318340542703636011ull,
        6477510814055453699ull, 9716266221083181299ull, 14574399331624771603ull, 18446744073709551557ull
    };

#else
#define PRIME_NUM 44
    // 1. start with p = 101
    // 2. p = next_prime(p * 1.7)
    // 3. if p < (2 << 31), go to step 2, otherwise, go to step 4
    // 4. end with p = prev_prime(2 << 31 - 1)
    static constexpr size_t stl_prime_list[] =
    {
        101u, 173u, 263u, 397u, 599u, 907u, 1361u, 2053u, 3083u, 4637u, 6959u,
        10453u, 15683u, 23531u, 35311u, 52967u, 79451u, 119179u, 178781u, 268189u,
        402299u, 603457u, 905189u, 1357787u, 2036687u, 3055043u, 4582577u, 6873871u,
        10310819u, 15466229u, 23199347u, 34799021u, 52198537u, 78297827u, 117446801u,
        176170229u, 264255353u, 396383041u, 594574583u, 891861923u, 1337792887u,
        2006689337u, 3010034021u, 4294967291u,
    };
#endif

    // 找到 >= n 的第一个质数
    inline size_t stl_next_prime(size_t n)
    {
        const size_t* first = stl_prime_list;
        const size_t* last  = stl_prime_list + PRIME_NUM;
        const size_t* pos   = mystl::lower_bound(first, last, n);
        return pos == last ? *(last - 1) : *pos;
    }

    template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
        class hashtable
        {
            public:
                typedef Key                                                 key_type;               // 键类型
                typedef Value                                               value_type;             // 值类型
                typedef HashFcn                                             hasher;                 // 哈希函数类型
                typedef EqualKey                                            key_equal;              // 判断键相等的函数类型

                typedef hashtable_node<Value>                               node;                   // 哈希表节点类型
                typedef mystl::allocator<Value>                             allocator_type;         // 分配器类型
                typedef mystl::allocator<Value>                             data_allocator;         // 数据分配器类型
                typedef mystl::allocator<node>                              node_allocator;         // 节点分配器类型

                typedef typename allocator_type::pointer                    pointer;                // 指针类型
                typedef typename allocator_type::const_pointer              const_pointer;          // 常量指针类型
                typedef typename allocator_type::reference                  reference;              // 引用类型
                typedef typename allocator_type::const_reference            const_reference;        // 常量引用类型
                typedef typename allocator_type::size_type                  size_type;              // 大小类型
                typedef typename allocator_type::difference_type            difference_type;        // 差异类型

                typedef mystl::hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>   iterator;  // 迭代器类型
                typedef mystl::hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>   const_iterator;  // 常量迭代器类型

                friend struct mystl::hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>;
                friend struct mystl::hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>;

                hasher      hash_funct()        const { return hash; }              // 获取哈希函数
                key_equal   key_eq()            const { return equals; }            // 获取键相等判断函数
                allocator_type get_allocator()  const { return allocator_type(); }  // 获取分配器

            private:
                typedef mystl::vector<node*>                                bucket_type;            // 存储桶类型

                bucket_type buckets;                                                    // 存储桶数组
                size_type   num_elements;                                               // 元素数量
                hasher      hash;                                                        // 哈希函数对象
                key_equal   equals;                                                      // 判断键相等的函数对象
                ExtractKey  get_key;                                                     // 提取键的函数对象

            public:
                explicit hashtable(size_type bucket_count,
                        const HashFcn& hf = HashFcn(),
                        const EqualKey& eql = EqualKey())
                    : hash(hf), equals(eql), get_key(ExtractKey()), num_elements(0)
                {
                    initialize_buckets(bucket_count);                                    // 初始化存储桶
                }

                template <class Iter, typename std::enable_if<
                    mystl::is_input_iterator<Iter>::value, int>::type = 0>
                    hashtable(Iter first, Iter last,
                            size_type bucket_count,
                            const HashFcn& hf = HashFcn(),
                            const EqualKey& eql = EqualKey())
                    : get_key(ExtractKey()), hash(hf), equals(eql), num_elements(mystl::distance(first, last))
                    {
                        initialize_buckets(mystl::max(bucket_count, static_cast<size_type>(mystl::distance(first, last))));
                    }

                hashtable(const hashtable& other)
                    : hash(other.hash), equals(other.equals), get_key(ExtractKey()), num_elements(0)
                {
                    copy_from(other);
                }

                hashtable(hashtable&& other) noexcept
                    : get_key(other.get_key),
                    hash(other.hash),
                    equals(other.equals),
                    num_elements(other.num_elements)
                    {
                        buckets = mystl::move(other.buckets);
                        other.num_elements = 0;
                    }

                hashtable& operator=(const hashtable& rhs)
                {
                    if (this != &rhs)
                    {
                        clear();
                        hash = rhs.hash;
                        equals = rhs.equals;
                        get_key = rhs.get_key;
                        copy_from(rhs);
                    }
                    return *this;
                }

                hashtable& operator=(hashtable&& rhs) noexcept
                {
                    if (this != &rhs)
                    {
                        clear();
                        hash = rhs.hash;
                        equals = rhs.equals;
                        get_key = rhs.get_key;
                        num_elements = rhs.num_elements;
                        buckets = mystl::move(rhs.buckets);
                        rhs.num_elements = 0;
                    }
                    return *this;
                }

                ~hashtable() { clear(); }

            public:
                iterator begin() noexcept
                {
                    for (size_type n = 0; n < buckets.size(); ++n)
                        if (buckets[n])
                            return iterator(buckets[n], this);
                    return end();
                }
                const_iterator begin() const noexcept
                {
                    for (size_type n = 0; n < buckets.size(); ++n)
                        if (buckets[n])
                            return const_iterator(buckets[n], this);
                    return end();
                }

                iterator end()                noexcept { return iterator(0, this); }
                const_iterator end()    const noexcept { return const_iterator(0, this); }

                const_iterator cbegin() const noexcept { return begin(); }
                const_iterator cend()   const noexcept { return end(); }

            public:
                bool      empty()                 const noexcept { return num_elements == 0; }  // 判断是否为空
                size_type size()                  const noexcept { return num_elements; }       // 返回元素数量
                size_type max_size()              const noexcept { return static_cast<size_type>(-1); }  // 返回最大容量大小
                size_type bucket_count()          const noexcept { return buckets.size(); }     // 返回存储桶数量
                size_type max_bucket_count()      const noexcept
                { return stl_prime_list[PRIME_NUM - 1]; }                                          // 返回最大存储桶数量

                void swap(hashtable& rhs)
                {
                    mystl::swap(hash, rhs.hash);
                    mystl::swap(equals, rhs.equals);
                    mystl::swap(get_key, rhs.get_key);
                    buckets.swap(rhs.buckets);
                    mystl::swap(num_elements, rhs.num_elements);
                }

                size_type elems_in_bucket(size_type bucket) const
                {
                    size_type result = 0;
                    for (auto cur = buckets[bucket]; cur != nullptr; cur = cur->next)
                        ++result;
                    return result;
                }

                template <class ...Args>
                    iterator emplace_multi(Args&& ...args);   // 在容器中插入多个元素

                template <class ...Args>
                    mystl::pair<iterator, bool> emplace_unique(Args&& ...args);   // 在容器中插入一个元素

                mystl::pair<iterator, bool> insert_unique(value_type&& value)
                { return emplace_unique(mystl::move(value));}

                iterator insert_multi(value_type&& value)
                { return emplace_multi(mystl::move(value)); }

                mystl::pair<iterator, bool> insert_unique(const value_type& value)
                {
                    resize(num_elements + 1);             // 扩容
                    return insert_unique_noresize(value);  // 在不需要扩容的情况下插入元素
                }

                iterator insert_multi(const value_type& value)
                {
                    resize(num_elements + 1);                  // 扩容
                    return insert_multi_noresize(value);        // 在不需要扩容的情况下插入元素
                }

                mystl::pair<iterator, bool> insert_unique_noresize(const value_type& value);  // 在容器中插入一个元素，不进行扩容

                iterator insert_multi_noresize(const value_type& value);          // 在容器中插入一个元素，不进行扩容

                template <class InputIter>
                    void insert_unique(InputIter first, InputIter last)               // 在容器中插入输入迭代器范围内的元素
                    {
                        insert_unique(first, last, mystl::iterator_category(first));
                    }

                template <class InputIter>
                    void insert_multi(InputIter first, InputIter last)                // 在容器中插入输入迭代器范围内的元素
                    {
                        insert_multi(first, last, mystl::iterator_category(first));
                    }

                template <class InputIter>
                    void insert_unique(InputIter first, InputIter last,
                            input_iterator_tag)                           // 在容器中插入输入迭代器范围内的元素（逐个插入）
                    {
                        for (; first != last; ++first)
                            insert_unique(*first);
                    }

                template <class InputIter>
                    void insert_multi(InputIter first, InputIter last,
                            input_iterator_tag)                            // 在容器中插入输入迭代器范围内的元素（逐个插入）
                    {
                        for (; first != last; ++first)
                            insert_multi(*first);
                    }

                template <class ForwardIterator>
                    void insert_unique(ForwardIterator first, ForwardIterator last,
                            forward_iterator_tag)                         // 在容器中插入前向迭代器范围内的元素（批量插入）
                    {
                        size_type n = mystl::distance(first, last);
                        resize(num_elements + n);                                    // 扩容
                        for (; n > 0; --n, ++first)
                            insert_unique_noresize(*first);                          // 在不需要扩容的情况下插入元素
                    }

                template <class ForwardIterator>
                    void insert_multi(ForwardIterator first, ForwardIterator last,
                            forward_iterator_tag)                           // 在容器中插入前向迭代器范围内的元素（批量插入）
                    {
                        size_type n = mystl::distance(first, last);
                        resize(num_elements + n);                                    // 扩容
                        for (; n > 0; --n, ++first)
                            insert_multi_noresize(*first);                          // 在不需要扩容的情况下插入元素
                    }

                reference find_or_insert(const value_type& value);               // 查找或插入元素

                iterator find(const key_type& key)                                // 查找指定键的元素
                {
                    auto n = bkt_num_key(key);
                    node* first;
                    for ( first = buckets[n];                                         // 在桶中寻找键相等的节点
                            first && !equals(get_key(first->value), key);
                            first = first->next)
                    {}
                    return iterator(first, this);
                }

                const_iterator find(const key_type& key) const                     // 查找指定键的元素
                {
                    auto n = bkt_num_key(key);
                    node* first;
                    for ( first = buckets[n];                                         // 在桶中寻找键相等的节点
                            first && !equals(get_key(first->val), key);
                            first = first->next)
                    {}
                    return const_iterator(first, this);
                }

                // 返回等于key的元素个数
                size_type count(const key_type& key) const
                {
                    const size_type n = bkt_num_key(key);  // 计算关键字key所对应的哈希桶索引
                    size_type result = 0;

                    // 遍历桶链表，统计等于key的元素个数
                    for (const node* cur = buckets[n]; cur != nullptr; cur = cur->next)
                        if (equals(get_key(cur->value), key))   // 判断当前节点的键值是否等于key
                            ++result;
                    return result;  // 返回等于key的元素个数
                }

                // 返回一对迭代器，表示所有等于key的元素的范围
                mystl::pair<iterator, iterator> equal_range(const key_type& key);

                // 返回一对迭代器，表示所有等于key的元素的范围（常量版本）
                mystl::pair<const_iterator, const_iterator> equal_range(const key_type& key) const;

                // 删除所有等于给定key的元素，并返回删除的元素数量
                size_type erase(const key_type& key);

                // 删除迭代器指向的元素
                void erase(const iterator& it);

                // 删除[first, last)范围内的元素
                void erase(iterator first, iterator last);

                // 删除迭代器指向的常量元素
                void erase(const const_iterator& it);

                // 删除[first, last)范围内的常量元素
                void erase(const_iterator first, const_iterator last);

                // 重新分配哈希表的大小
                void resize(size_type num_elements_hint);

                // 清空哈希表
                void clear();

            private:
                // 计算下一个合适的大小
                size_type next_size(size_type n) const
                { return stl_next_prime(n); }

                // 初始化桶链表的大小
                void initialize_buckets(size_type n)
                {
                    const size_type n_buckets = next_size(n);
                    try
                    {
                        buckets.reserve(n_buckets);  // 分配桶的内存
                        buckets.assign(n_buckets, nullptr);  // 初始化桶为空指针
                    }
                    catch (...) { throw ; }
                    num_elements = 0;  // 元素数量归零
                }

                // 计算关键字key所对应的哈希桶索引
                size_type bkt_num_key(const key_type& key) const
                {
                    return bkt_num_key(key, buckets.size());
                }

                // 计算关键字key所对应的哈希桶索引
                size_type bkt_num_key(const key_type& key, size_t n) const
                {
                    return hash(key) % n;  // 使用哈希函数计算关键字key的哈希值，并对桶的数量取模得到索引
                }

                // 计算给定值value所对应的哈希桶索引
                size_type bkt_num(const value_type& value) const
                {
                    return bkt_num_key(get_key(value));  // 调用提取键值的函数，计算给定值的哈希桶索引
                }

                // 计算给定值value所对应的哈希桶索引
                size_type bkt_num(const value_type& value, size_t n) const
                {
                    return bkt_num_key(get_key(value), n);  // 调用提取键值的函数，计算给定值的哈希桶索引
                }

                // 创建并构造新节点
                node* new_node(const value_type& value)
                {
                    auto n = node_allocator::allocate(1);  // 分配节点的内存空间
                    try
                    {
                        data_allocator::construct(&(n->value), value);  // 构造节点的值
                        n->next = nullptr;
                    }
                    catch (...)
                    {
                        node_allocator::deallocate(n);  // 析构节点并释放内存
                        throw ;
                    }
                    return n;  // 返回新节点的指针
                }

                // 创建并构造新节点（支持可变参数）
                template <class ...Args>
                    node* new_node(Args&& ...args)
                    {
                        auto n = node_allocator::allocate(1);  // 分配节点的内存空间
                        try
                        {
                            data_allocator::construct(&(n->value), mystl::forward<Args>(args)...);  // 构造节点的值
                            n->next = nullptr;
                        }
                        catch (...)
                        {
                            node_allocator::deallocate(n);  // 析构节点并释放内存
                            throw ;
                        }
                        return n;  // 返回新节点的指针
                    }

                // 销毁并释放节点的内存空间
                void delete_node(node* n)
                {
                    data_allocator::destroy(&(n->value));  // 销毁节点的值
                    node_allocator::deallocate(n);  // 释放节点的内存空间
                    n = nullptr;
                }

                // 删除给定桶中[first, last)范围内的元素
                void erase_bucket(const size_type n, node* first, node* last);

                // 删除给定桶中的所有元素
                void erase_bucket(const size_type n, node* last);

                // 从另一个哈希表复制元素
                void copy_from(const hashtable& ht);

            public:
                // 比较两个哈希表是否相等
                bool operator==(const hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>& rhs)
                {
                    typedef typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::node node;
                    if (this->buckets.size() != rhs.buckets.size())
                        return false;
                    for (int n = 0; n < rhs.buckets.size(); ++n)
                    {
                        node* cur1 = buckets[n];
                        node* cur2 = rhs.buckets[n];
                        for (; cur1 && cur2 && cur1->val == cur2->val;
                                cur1 = cur1->next, cur2 = cur2->next)
                        {}
                        if (cur1 || cur2)
                            return false;
                    }
                    return true;
                }
        };

        template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
        hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>&
        hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>::operator++()
        {
            const node* old = cur;
            cur = cur->next;

            // 若当前元素的下一个元素为nullptr，说明当前桶的元素已经遍历完
            // 在哈希表中寻找下一个非空桶的第一个元素
            if (cur == nullptr)
            {
                auto bucket = ht->bkt_num(old->value);
                while (cur == nullptr && ++bucket < ht->buckets.size())
                    cur = ht->buckets[bucket];
            }
            return *this;
        }

    template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
        hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>&
        hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>::operator++(int)
        {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

    template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
        hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>&
        hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>::operator++()
        {
            const node* old = cur;
            cur = cur->next;

            // 若当前元素的下一个元素为nullptr，说明当前桶的元素已经遍历完
            // 在哈希表中寻找下一个非空桶的第一个元素
            if (cur == nullptr)
            {
                auto bucket = ht->bkt_num(old->value);
                while (cur == nullptr && ++bucket < ht->buckets.size())
                    cur = ht->buckets[bucket];
            }
            return *this;
        }

    template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
        hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>&
        hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>::operator++(int)
        {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

    template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
        void swap(hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>& lhs,
                hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>& rhs)
        {
            lhs.swap(rhs);
        }

    template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
        template <class ...Args>
        mystl::pair<typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::iterator, bool>
        hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::emplace_unique(Args&& ...args)
        {
            auto tmp = new_node(mystl::forward<Args>(args)...);
            resize(num_elements + 1);
            const size_type n = bkt_num(tmp->value);
            auto first = buckets[n];

            // 遍历当前桶，判断插入的元素是否已存在
            for (auto cur = first; cur; cur = cur->next)
                if (equals(get_key(cur->value), get_key(tmp->value)))
                {
                    delete_node(tmp);
                    return mystl::make_pair(iterator(cur, this), false);
                }

            // 将新节点插入到当前桶的开头
            tmp->next = first;
            buckets[n] = tmp;
            ++num_elements;
            return mystl::make_pair(iterator(tmp, this), true);
        }

    template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
        template <class ...Args>
        typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::iterator
        hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::emplace_multi(Args&& ...args)
        {
            auto np = new_node(mystl::forward<Args>(args)...);
            resize(num_elements + 1);

            const size_type n = bkt_num(np->value);
            auto first = buckets[n];

            // 遍历当前桶，找到匹配的元素后，在其后面插入新节点
            for (auto cur = first; cur; cur = cur->next)
                if (equals(get_key(cur->value), get_key(np->value)))
                {
                    node* tmp = new_node(np->value);
                    tmp->next = cur->next;
                    cur->next = tmp;
                    ++num_elements;
                    return iterator(tmp, this);
                }
            // 若没有匹配的元素，则将新节点插入到当前桶的开头
            np->next = first;
            buckets[n] = np;
            ++num_elements;
            return iterator(np, this);
        }


    template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
        mystl::pair<typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::iterator, bool>
        hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::insert_unique_noresize(const value_type& value)
        {
            const size_type n = bkt_num(value);
            auto first = buckets[n];

            // 遍历当前桶，判断插入的元素是否已存在
            for (auto cur = first; cur; cur = cur->next)
                if (equals(get_key(cur->value), get_key(value)))
                    return mystl::make_pair(iterator(cur, this), false);

            // 将新节点插入到当前桶的开头
            node* tmp = new_node(value);
            tmp->next = first;
            buckets[n] = tmp;
            ++num_elements;
            return mystl::make_pair(iterator(tmp, this), true);
        }

    template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
        typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::iterator
        hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::insert_multi_noresize(const value_type& value)
        {
            const size_type n = bkt_num(value);
            auto first = buckets[n];

            // 遍历当前桶，找到匹配的元素后，在其后面插入新节点
            for (auto cur = first; cur; cur = cur->next)
                if (equals(get_key(cur->value), get_key(value)))
                {
                    node* tmp = new_node(value);
                    tmp->next = cur->next;
                    cur->next = tmp;
                    ++num_elements;
                    return iterator(tmp, this);
                }
            // 若没有匹配的元素，则将新节点插入到当前桶的开头
            node* tmp = new_node(value);
            tmp->next = first;
            buckets[n] = tmp;
            ++num_elements;
            return iterator(tmp, this);
        }

    template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
        typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::reference
        hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::find_or_insert(const value_type& value)
        {
            resize(num_elements + 1);
            size_type n = bkt_num(value);
            node* first = buckets[n];

            // 遍历当前桶，判断插入的元素是否已存在
            for (node* cur = first; cur; cur = cur->next)
                if (equals(get_key(cur->value), get_key(value)))
                    return cur->val;

            // 将新节点插入到当前桶的开头
            node* tmp = new_node(value);
            tmp->next = first;
            buckets[n] = tmp;
            ++num_elements;
            return tmp->value;
        }

    template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
        mystl::pair<typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::iterator,
        typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::iterator>
            hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::equal_range(const key_type& key)
            {
                const size_type n = bkt_num_key(key);

                // 在哈希表中寻找与键匹配的元素范围
                for (auto first = buckets[n]; first; first = first->next)
                {
                    if (equals(get_key(first->value), key))
                    {
                        for (auto cur = first->next; cur; cur = cur->next)
                            if (!equals(get_key(cur->value), key))
                                return mystl::make_pair(iterator(first, this), iterator(cur, this));
                        for (auto m = n  + 1; m < buckets.size(); ++m)
                            if (buckets[m] != nullptr)
                                return mystl::make_pair(iterator(first, this), iterator(buckets[m], this));
                        return mystl::make_pair(iterator(first, this), end());
                    }
                }
                return mystl::make_pair(end(), end());
            }

        template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
        mystl::pair<typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::const_iterator,
        typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::const_iterator>
            hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::equal_range(const key_type& key) const
            {
                const size_type n = bkt_num(key); // 计算 key 所在的桶索引

                for (auto first = buckets[n]; first; first = first->next) // 遍历桶链表
                {
                    if (equals(get_key(first->value), key)) // 如果找到了匹配的键值对
                    {
                        for (auto cur = first->next; cur; cur = cur->next) // 继续向后搜索，直到遇到非匹配的键值对
                            if (!equals(get_key(cur->value), key))
                                return mystl::make_pair(const_iterator(first, this), const_iterator(cur, this)); // 返回迭代器表示找到的范围
                        for (auto m = n  + 1; m < buckets.size(); ++m) // 在后续桶中查找非空桶
                            if (buckets[m] != nullptr)
                                return mystl::make_pair(const_iterator(first, this), const_iterator(buckets[m], this)); // 返回迭代器表示找到的范围
                        return mystl::make_pair(const_iterator(first, this), end()); // 没有找到非匹配的键值对，返回以 first 为起始的整个桶链表范围
                    }
                }
                return mystl::make_pair(end(), end()); // 没有找到匹配的键值对，返回空范围
            }

    template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
        typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::size_type
        hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::erase(const key_type& key)
        {
            const size_type n = bkt_num_key(key); // 计算 key 所在的桶索引
            auto first = buckets[n];
            size_type erased = 0;
            if (first != nullptr)
            {
                node* cur = first;
                node* next = cur->next;
                while (next) // 遍历桶链表
                {
                    if (equals(get_key(next->value), key)) // 如果找到了匹配的键值对
                    {
                        cur->next = next->next; // 删除节点
                        delete_node(next);
                        next = cur->next;
                        ++erased;
                        --num_elements;
                    }
                    else
                    {
                        cur = next;
                        next = cur->next;
                    }
                }
                if (equals(get_key(first->value), key))
                {
                    buckets[n] = first->next; // 删除节点
                    delete_node(first);
                    ++erased;
                    --num_elements;
                }
            }
            return erased; // 返回删除的节点数目
        }

    template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
        void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::erase(const iterator& it)
        {
            node* const p = it.cur;
            if (p != nullptr) // 非空节点
            {
                const size_type n = bkt_num(p->value); // 计算节点所在的桶索引
                auto cur = buckets[n];
                if (cur == p) // p 是桶链表的第一个节点
                {
                    buckets[n] = cur->next; // 删除节点
                    delete_node(cur);
                    --num_elements;
                }
                else // p 不是桶链表的第一个节点
                {
                    auto next = cur->next;
                    while (next)
                    {
                        if (next == p) // 找到节点 p
                        {
                            cur->next = next->next; // 删除节点
                            delete_node(next);
                            break;
                        }
                        else
                        {
                            cur = next;
                            next = cur->next;
                        }
                    }
                }
            }
        }

    template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
        inline void
        hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::
        erase(const const_iterator& it)
        {
            erase(iterator(const_cast<node*>(it.cur),
                        const_cast<hashtable*>(it.ht))); // 调用重载的 erase 函数
        }

    template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
        void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::
        erase(iterator first, iterator last)
        {
            auto f_bucket = first.cur ? bkt_num(first.cur->value) : buckets.size(); // first 所在桶的索引
            auto l_bucket = last.cur ? bkt_num(last.cur->value) : buckets.size(); // last 所在桶的索引

            if (first.cur == last.cur)
                return ;
            else if (f_bucket == l_bucket) // first 和 last 在同一个桶中
                erase_bucket(f_bucket, first.cur, last.cur); // 删除桶链表中的节点
            else
            {
                erase_bucket(f_bucket, first.cur, 0); // 删除第一个桶链表中的节点
                for (auto n = f_bucket + 1; n < l_bucket; ++n) // 删除中间桶链表所有的节点
                    erase_bucket(n, 0);
                if (l_bucket != buckets.size())
                    erase_bucket(l_bucket, last.cur); // 删除最后一个桶链表中的节点
            }
        }

    template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
        void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::
        erase(const_iterator first, const_iterator last)
        {
            erase(iterator(const_cast<node*>(first.cur),
                        const_cast<hashtable*>(first.ht)), // 调用重载的 erase 函数
                    iterator(const_cast<node*>(last.cur),
                        const_cast<hashtable*>(last.ht)));
        }

    template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
        void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::
        resize(size_type num_elements_hint)
        {
            const size_type old_n = buckets.size();
            if (num_elements_hint > old_n) // 如果需要重新调整大小
            {
                const size_type n = next_size(num_elements_hint); // 计算新的桶数量
                if (n > old_n)
                {
                    mystl::vector<node*> tmp(n, nullptr); // 创建新的桶
                    try
                    {
                        for (auto bucket = 0; bucket < old_n; ++bucket) // 将节点重新分配到新的桶中
                        {
                            node* first = buckets[bucket];
                            while (first)
                            {
                                auto new_bucket = bkt_num(first->value, n); // 计算节点应该放入的桶索引
                                buckets[bucket] = first->next;
                                first->next = tmp[new_bucket];
                                tmp[new_bucket] = first;
                                first = buckets[bucket];
                            }
                        }
                        buckets.swap(tmp); // 交换新旧桶
                    }
                    catch (...)
                    {
                        for (auto bucket = 0; bucket < tmp.size(); ++bucket)
                        {
                            while (tmp[bucket])
                            {
                                node* next = tmp[bucket]->next;
                                delete_node(tmp[bucket]);
                                tmp[bucket] = next;
                            }
                        }
                        throw ;
                    }
                }
            }
        }

    template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
        void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::
        erase_bucket(const size_type n, node* first, node* last)
        {
            auto cur = buckets[n];
            if (cur == first) // first 是桶链表的第一个节点
                erase_bucket(n, last); // 删除首尾之间的节点
            else
            {
                node* next = nullptr;
                for (next = cur->next; next != first; cur = next, next = cur->next)
                {}
                while (next != nullptr)
                {
                    cur->next = next->next; // 删除节点
                    delete_node(next);
                    next = cur->next;
                    --num_elements;
                }
            }
        }

    template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
        void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::
        erase_bucket(const size_type n, node* last)
        {
            auto cur = buckets[n];
            while (cur != last) // 删除首尾之间的节点
            {
                node* tmp = cur->next;
                delete_node(cur);
                cur = tmp;
                buckets[n] = cur;
                --num_elements;
            }
        }

    template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
        void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::
        clear()
        {
            for (auto i = 0; i < buckets.size(); ++i) // 遍历桶
            {
                auto cur = buckets[i];
                while (cur != nullptr) // 删除桶链表中的所有节点
                {
                    node* tmp = cur->next;
                    delete_node(cur);
                    cur = tmp;
                }
                buckets[i] = nullptr;
            }
            num_elements = 0; // 将元素个数设置为0
        }

    template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
        void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::
        copy_from(const hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>& ht)
        {
            buckets.clear();
            buckets.reserve(ht.buckets.size()); // 重新分配桶的空间
            buckets.insert(buckets.end(), ht.buckets.size(), nullptr); // 将桶指针设置为空
            try
            {
                for (auto i = 0; i < ht.buckets.size(); ++i) // 复制每个桶链表的节点
                {
                    const node* cur = ht.buckets[i];
                    if (cur != nullptr)
                    {
                        node* copy = new_node(cur->value);
                        buckets[i] = copy;
                        for (auto next = cur->next; next != nullptr; cur = next, next = cur->next)
                        {
                            copy->next = new_node(next->value);
                            copy = copy->next;
                        }
                    }
                }
                num_elements = ht.num_elements; // 复制元素个数
            }
            catch (...)
            {
                clear();
            }
        }

};
#endif //TINYSTL_HASHTABLE_H
