#ifndef TINYSTL_VECTOR_H
#define TINYSTL_VECTOR_H

#include <initializer_list>
#include "memory.h"
#include "iterator.h"
#include "util.h"
#include "exceptdef.h"
#include <algorithm>

namespace mystl
{
    // 模板类 vector
    // 模板参数 T 为数据类型
    template <class T>
        class vector
        {
            // 静态断言：确保T不是bool类型，因为vector<bool>在mystl中已被废弃
            static_assert(!std::is_same<bool, T>::value, "vector<bool> is abandoned in mystl");

            public:
            // 定义类型别名
            typedef mystl::allocator<T>                      allocator_type;
            typedef mystl::allocator<T>                      data_allocator;

            typedef typename allocator_type::value_type      value_type;
            typedef typename allocator_type::pointer         pointer;
            typedef typename allocator_type::const_pointer   const_pointer;
            typedef typename allocator_type::reference       reference;
            typedef typename allocator_type::const_reference const_reference;
            typedef typename allocator_type::size_type       size_type;
            typedef typename allocator_type::difference_type difference_type;

            typedef value_type*                              iterator;
            typedef const value_type*                        const_iterator;
            typedef mystl::reverse_iterator<iterator>        reverse_iterator;
            typedef mystl::reverse_iterator<const_iterator>  const_reverse_iterator;

            protected:
            // 成员变量
            T* start;               // 指向数组起始位置的指针
            T* finish;              // 指向数组结束位置的指针（即最后一个元素之后）
            T* end_of_storage;      // 指向可用内存结束位置的指针

            // 辅助函数
            void insert_aux(iterator position, const T& value);  // 在指定位置插入元素（辅助函数）

            // 使用变参模板，支持以任意参数类型构造对象
            template<class ...Args>
                void insert_aux(iterator position, Args&& ...args);

            void deallocate();                               // 释放内存
            void fill_initialize(size_type n, const T& value);     // 初始化，将n个元素置为value
            void try_init() noexcept;                         // 对象初始化
            void space_initialize(size_type size, size_type cap); // 初始化空间，分配内存
            template <class Iter>
                void range_initialize(Iter first, Iter last);       // 通过迭代器范围初始化

            public:
            // 构造, 赋值, 析构函数
            vector();                                           // 默认构造函数
            explicit vector(size_type n);                       // 构造n个元素的vector，并赋初值为T()
            vector(size_type n, const T& value);                 // 构造n个元素的vector，并赋初值为value
            template <class Iter, typename std::enable_if<
                mystl::is_input_iterator<Iter>::value, int>::type = 0>
                vector(Iter first, Iter last);                      // 通过迭代器范围构造vector
            vector(const vector<T>& other);                      // 拷贝构造函数
            vector(vector<T>&& other);                           // 移动构造函数
            vector(std::initializer_list<T> ilist);              // 通过初始化列表构造vector

            // 赋值运算符重载
            vector<T>& operator=(const vector<T>& rhs);          // 拷贝赋值运算符
            vector<T>& operator=(vector<T>&& rhs);               // 移动赋值运算符
            vector<T>& operator=(std::initializer_list<T> ilist);// 初始化列表赋值运算符

            ~vector();                                          // 析构函数

            public:
            // 迭代器相关操作
            iterator                begin()           noexcept;  // 返回首元素的迭代器
            const_iterator          begin()     const noexcept;  // 返回首元素的常量迭代器
            iterator                end()             noexcept;  // 返回尾后元素的迭代器
            const_iterator          end()       const noexcept;  // 返回尾后元素的常量迭代器

            reverse_iterator        rbegin()          noexcept;  // 返回逆向首元素的迭代器
            const_reverse_iterator  rbegin()    const noexcept;  // 返回逆向首元素的常量迭代器
            reverse_iterator        rend()            noexcept;  // 返回逆向尾后元素的迭代器
            const_reverse_iterator  rend()      const noexcept;  // 返回逆向尾后元素的常量迭代器

            const_iterator          cbegin()    const noexcept;  // 返回首元素的常量迭代器
            const_iterator          cend()      const noexcept;  // 返回尾后元素的常量迭代器
            const_reverse_iterator  crbegin()   const noexcept;  // 返回逆向首元素的常量迭代器
            const_reverse_iterator  crend()     const noexcept;  // 返回逆向尾后元素的常量迭代器

            // 容器基本属性
            bool      empty()         const noexcept;            // 判断容器是否为空
            size_type size()     const noexcept;                  // 返回容器中元素的个数
            size_type max_size() const noexcept;                  // 返回容器的最大容量
            size_type capacity() const noexcept;                  // 返回容器的当前容量
            void      reserve(size_type n);                        // 分配内存空间，以便存储n个元素

            // 访问元素相关操作
            reference operator[](size_type n);                     // 重载[]运算符，返回索引为n的元素的引用
            const_reference operator[](size_type n) const;         // 重载[]运算符，返回索引为n的元素的常量引用
            reference at(size_type n);                             // 返回索引为n的元素的引用，并执行边界检查
            const_reference at(size_type n) const;                 // 返回索引为n的元素的常量引用，并执行边界检查
            reference front();                                     // 返回首元素的引用
            const_reference front() const;                         // 返回首元素的常量引用
            reference back();                                      // 返回尾元素的引用
            const_reference back() const;                          // 返回尾元素的常量引用

            // 修改容器相关操作
            void      fill_assign(size_type n, const T& value);     // 将容器中所有元素赋值为value
            template <class IIter>
                void      copy_assign(IIter first, IIter last, input_iterator_tag);// 通过迭代器范围赋值（输入迭代器）
            template <class FIter>
                void      copy_assign(FIter first, FIter last, forward_iterator_tag);// 通过迭代器范围赋值（前向迭代器）
            void assign(size_type n, const T& value);               // 赋值n个元素，所有元素值都为value
            template <class Iter, typename std::enable_if<
                mystl::is_input_iterator<Iter>::value, int>::type = 0>
                void assign(Iter first, Iter last);                     // 通过迭代器范围赋值
            void assign(std::initializer_list<value_type> il);      // 通过初始化列表赋值
            template <class... Args>
                iterator emplace(const_iterator pos, Args&& ...args);   // 在指定位置插入元素
            template <class... Args>
                void emplace_back(Args&& ...args);                      // 在尾部插入元素
            void push_back(const T& value);                          // 在尾部插入元素
            void push_back(T&& value);                               // 在尾部插入元素
            void pop_back();                                        // 删除尾部元素
            template<class Iter>
                void range_insert(const_iterator position, Iter first, Iter last);// 在指定位置插入迭代器范围内的元素
            iterator insert(const_iterator pos, const T& value);      // 在指定位置插入元素
            iterator insert(const_iterator pos, T&& value);           // 在指定位置插入元素
            iterator insert(const_iterator pos);                      // 在指定位置插入默认构造的元素
            void insert(const_iterator pos, size_type n, const T& value);// 在指定位置插入n个值为value的元素
            template <class Iter, typename std::enable_if<
                mystl::is_input_iterator<Iter>::value, int>::type = 0>
                void insert(const_iterator pos, Iter first, Iter last);    // 在指定位置插入迭代器范围内的元素
            iterator erase(const_iterator pos);                        // 删除指定位置的元素
            iterator erase(const_iterator first, const_iterator last); // 删除[first, last)范围内的元素

            public:
            // 辅助功能函数
            void swap(vector<T>& other) noexcept;                 // 交换两个vector容器的内容
            void clear();                                         // 清空容器
            void reverse();                                       // 反转容器中的元素
            void resize(size_type new_size);                      // 调整容器的大小，多余的元素将会被移除或者剔除
            void resize(size_type new_size, const T& value);       // 调整容器的大小，并用value填充新添加的元素
        };

    /****************************************************************/

    template <class T>
        void vector<T>::deallocate()
        {
            // 如果 start 指针不为空，释放内存
            if (start)
                data_allocator::deallocate(start, end_of_storage - start);
        }

    template <class T>
        void vector<T>::try_init() noexcept
        {
            try
            {
                // 分配初始容量为 16 的内存空间
                start = data_allocator::allocate(16);
                finish = start;
                end_of_storage = start + 16;
            }
            catch (...)
            {
                // 分配内存失败，则将指针设置为 nullptr
                start = nullptr;
                finish = nullptr;
                end_of_storage = nullptr;
            }
        }

    template <class T>
        void vector<T>::space_initialize(size_type size, size_type cap)
        {
            try
            {
                // 分配容量为 cap 的内存空间
                start = data_allocator::allocate(cap);
                finish = start + size;
                end_of_storage = start + cap;
            }
            catch (...)
            {
                // 分配内存失败，则将指针设置为 nullptr，并抛出异常
                start = nullptr;
                finish = nullptr;
                end_of_storage = nullptr;
                throw ;
            }
        }

    template <class T>
        void vector<T>::fill_initialize(size_type n, const T& value)
        {
            // 初始化容量为 n 的空间，并使用 value 填充每个元素
            const size_type init_size = mystl::max(static_cast<size_type>(16), n);
            space_initialize(n, init_size);
            mystl::uninitialized_fill_n(start, n, value);
        }

    template <class T>
        template <class Iter>
        void vector<T>::range_initialize(Iter first, Iter last)
        {
            /* 此处只考虑线性容器的迭代器 */
            // 初始化容量为 last - first 的空间，并使用 [first, last) 范围内的值填充每个元素
            const size_type init_size = mystl::max(static_cast<size_type>(16),
                    static_cast<size_type>(last - first));
            space_initialize(static_cast<size_type>(last - first), init_size);
            mystl::uninitialized_copy(first, last, start);
        }

    template <class T>
        vector<T>&
        vector<T>::operator=(const vector<T>& rhs)
        {
            if (this != &rhs)
            {
                const size_type len = rhs.size();
                if (len > capacity())
                {
                    // 如果 rhs 的大小大于当前容量，则创建临时向量 t，并交换内容
                    vector<T> t(rhs.begin(), rhs.end());
                    swap(t);
                }
                else if (size() >= len)
                {
                    // 如果当前大小大于等于 rhs 的大小，则复制 rhs 的元素到当前向量，销毁多余的元素
                    auto i = mystl::copy(rhs.begin(), rhs.end(), begin());
                    data_allocator::destroy(i, end());
                    finish = start + len;
                }
                else
                {
                    // 如果当前大小小于 rhs 的大小，则先复制当前向量前 size() 个元素，再构造剩余的元素
                    mystl::copy(rhs.begin(), rhs.begin() + size(), start);
                    mystl::uninitialized_copy(rhs.begin() + size(), rhs.end(), finish);
                    end_of_storage = finish = start + len;
                }
            }
            return *this;
        }

    template <class T>
        vector<T>& vector<T>::operator=(vector<T>&& rhs)
        {
            // 移动赋值运算符，将 rhs 的资源交换给当前向量
            clear();
            start = rhs.start;
            finish = rhs.finish;
            end_of_storage = rhs.end_of_storage;
            rhs.start = rhs.finish = rhs.end_of_storage = nullptr;
            return *this;
        }

    // 当原容量小于要求大小时才会重新分配
    template <class T>
        void vector<T>::reserve(size_type n)
        {
            if (capacity() < n)
            {
                THROW_LENGTH_ERROR_IF(n > max_size(),
                        "n can not larger than max_size() in vector<T>::reserve(n)");
                const auto old_size = size();
                // 分配新的内存空间，并将原有元素移动到新空间中
                auto tmp = data_allocator::allocate(n);
                mystl::move((iterator)start, (iterator)finish, (iterator)tmp);
                clear();
                start = tmp;
                finish = tmp + old_size;
                end_of_storage = tmp + n;
            }
        }

    template <class T>
        void vector<T>::fill_assign(size_type n, const T& value)
        {
            if (n > capacity())
            {
                // 如果 n 大于当前容量，则创建临时向量 tmp，并交换内容
                vector<T> tmp(n, value);
                swap(tmp);
            }
            else if (n > size())
            {
                // 如果 n 大于当前大小，则用 value 填充当前向量，再构造剩余的元素
                mystl::fill(begin(), end(), value);
                finish = mystl::uninitialized_fill_n(end(), n - size(), value);
            }
            else
                // 如果 n 小于等于当前大小，则先用 value 填充前 n 个元素，再销毁多余的元素
                erase(mystl::fill_n(begin(), n, value), end());
        }

    template <class T>
        template <class IIter>
        void vector<T>::
        copy_assign(IIter first, IIter last, input_iterator_tag)
        {
            // 输入迭代器版本的复制赋值
            iterator cur = start;
            for (; first != last && cur != finish; ++first, ++cur)
            {
                // 将 [first, last) 范围内的值复制到当前向量中
                *cur = *first;
            }
            if (first == last)
                // 如果 [first, last) 范围内的值已经复制完毕，删除剩余的元素
                erase(const_iterator(cur), const_iterator(finish));
            else
                // 如果当前向量已经遍历完了，但是 [first, last) 范围内还有元素，则插入这些元素
                insert(finish, first, last);
        }

    template <class T>
        template <class FIter>
        void vector<T>::
        copy_assign(FIter first, FIter last, forward_iterator_tag)
        {
            // 前向迭代器版本的复制赋值
            auto len = mystl::distance(first, last);
            if (len > capacity())
            {
                // 如果 len 大于当前容量，则创建临时向量 tmp，并交换内容
                vector tmp(first, last);
                swap(tmp);
            }
            else if (size() >= len)
            {
                // 如果当前大小大于等于 len，则直接复制 [first, last) 范围内的值到当前向量，销毁多余的元素
                iterator new_finish = mystl::copy(first, last, start);
                data_allocator::destroy(new_finish, finish);
                finish = new_finish;
            }
            else
            {
                // 如果当前大小小于 len，则先复制 [first, mid) 范围内的值到当前向量，再构造剩余的元素
                auto mid = first;
                mystl::advance(mid, size());
                mystl::copy(first, mid, start);
                iterator new_finish = mystl::uninitialized_copy(mid, last, finish);
                finish = new_finish;
            }
        }

    // 在 pos 位置处就地构造元素, 避免额外的复制或者移动开销
    template <class T>
        template <class ...Args>
        typename vector<T>::iterator
        vector<T>::emplace(const_iterator pos, Args&& ...args)
        {
            MYSTL_DEBUG(pos >= begin() && pos <= end());
            iterator xpos = const_cast<iterator>(pos);
            const size_type n = xpos - start;
            if (finish != end_of_storage && xpos == end())
            {
                // 如果插入位置在末尾，并且还有剩余空间，则直接构造元素
                data_allocator::construct(end(), mystl::forward<Args>(args)...);
                ++finish;
            }
            else
                // 否则调用 insert_aux() 函数插入元素
                insert_aux(xpos, mystl::forward<Args>(args)...);
            return begin() + n;
        }

    template <class T>
        template <class ...Args>
        void vector<T>::emplace_back(Args&& ...args)
        {
            if (finish < end_of_storage)
            {
                // 如果还有剩余空间，则直接在末尾构造元素
                data_allocator::construct(end(), forward<Args>(args)...);
                ++finish;
            }
            else
                // 否则调用 insert_aux() 函数，在末尾插入元素
                insert_aux(end(), forward<Args>(args)...);
        }

    // insert

    template <class T>
        template <class ...Args>
        void vector<T>::insert_aux(iterator position, Args&& ...args)
        {
            // 如果容器还有剩余空间
            if (finish != end_of_storage)
            {
                // 在尾部创建一个临时对象
                data_allocator::construct(finish, *(finish - 1));
                ++finish;
                // 将 position 到 finish-2 之间的元素往后移动一位
                copy_backward(position, finish - 2, finish - 1);
                // 在 position 处插入一个新元素
                *position = value_type(mystl::forward<Args>(args)...);
            }
            else
            {
                // 如果容器空间不够了，需要重新分配内存
                auto old_size = size();
                auto len = (old_size != 0 ? 2 * old_size : 1);
                iterator new_start = data_allocator::allocate(len);
                iterator new_finish = new_start;
                try
                {
                    // 将 start 到 position 的元素复制到新内存中
                    new_finish = mystl::uninitialized_copy(start, position, new_start);
                    // 在新位置处构造一个新元素
                    data_allocator::construct(new_finish,  mystl::forward<Args>(args)...);
                    ++new_finish;
                    // 将 position 到 finish 的元素复制到新内存中
                    new_finish = mystl::uninitialized_copy(position, finish, new_finish);
                }
                catch (...)
                {
                    // 如果出现异常，销毁已构造的对象并释放内存
                    data_allocator::destroy(new_start, new_finish);
                    data_allocator::deallocate(new_start, len);
                    throw ;
                }
                // 清空原内存，并更新容器的指针和大小信息
                clear();
                start = new_start;
                finish = new_finish;
                end_of_storage = new_start + len;
            }
        }

    template <class T>
        void vector<T>::insert_aux(iterator position, const T& value)
        {
            // 如果容器还有剩余空间
            if (finish != end_of_storage)
            {
                // 在尾部创建一个临时对象
                data_allocator::construct(finish, *(finish - 1));
                ++finish;
                // 复制 position 到 finish-2 之间的元素往后移动一位
                copy_backward(position, finish - 2, finish - 1);
                // 在 position 处插入一个新元素
                *position = value;
            }
            else
            {
                // 如果容器空间不够了，需要重新分配内存
                auto old_size = size();
                auto len = (old_size != 0 ? 2 * old_size : 1);
                iterator new_start = data_allocator::allocate(len);
                iterator new_finish = new_start;
                try
                {
                    // 将 start 到 position 的元素复制到新内存中
                    new_finish = mystl::uninitialized_copy(start, position, new_start);
                    // 在新位置处构造一个新元素
                    data_allocator::construct(new_finish, value);
                    ++new_finish;
                    // 将 position 到 finish 的元素复制到新内存中
                    new_finish = mystl::uninitialized_copy(position, finish, new_finish);
                }
                catch (...)
                {
                    // 如果出现异常，销毁已构造的对象并释放内存
                    data_allocator::destroy(new_start, new_finish);
                    data_allocator::deallocate(new_start, len);
                    throw ;
                }
                // 清空原内存，并更新容器的指针和大小信息
                clear();
                start = new_start;
                finish = new_finish;
                end_of_storage = new_start + len;
            }
        }


    template <class T>
        void vector<T>::insert(const_iterator pos, size_type n, const T& value)
        {
            if (n != 0)
            {
                // 如果容器后方剩余空间足够容纳新增元素
                if (static_cast<size_type>(end_of_storage - finish) >= n)
                {
                    T value_copy = value;
                    auto elems_after = finish - pos;
                    iterator old_finish = finish;
                    if (elems_after > n)
                    {
                        // 将插入点之后的元素往后移动 n 个位置
                        mystl::uninitialized_copy(finish - n, finish, finish);
                        finish += n;
                        // 将插入点之前的元素往后移动 n 个位置
                        mystl::copy_backward((iterator)pos, (iterator)(old_finish - n), old_finish);
                        // 在插入点处填充 n 个新元素
                        mystl::fill((iterator)pos, (iterator)pos + n, value_copy);
                    }
                    else
                    {
                        // 在插入点之后增加 n-elems_after 个新元素
                        mystl::uninitialized_fill_n(finish, n - elems_after, value_copy);
                        finish += n - elems_after;
                        // 将插入点之前的元素复制到新增位置
                        mystl::uninitialized_copy((iterator)pos, old_finish, finish);
                        finish += elems_after;
                        // 在插入点处填充 elems_after 个新元素
                        mystl::fill((iterator)pos, old_finish, value_copy);
                    }
                }
                else
                {
                    // 如果容器后方剩余空间不足，则重新分配内存
                    auto old_size = size();
                    auto len = old_size + mystl::max(old_size, n);
                    iterator new_start = data_allocator::allocate(len);
                    iterator new_finish = new_start;
                    try
                    {
                        // 将 start 到 pos 的元素复制到新内存中
                        new_finish = mystl::uninitialized_copy((iterator)start, (iterator)pos, (iterator)new_start);
                        // 填充 n 个新元素
                        new_finish = mystl::uninitialized_fill_n(new_finish, n, value);
                        // 将 pos 到 finish 的元素复制到新内存中
                        new_finish = mystl::uninitialized_copy((iterator)pos, (iterator)finish, (iterator)new_finish);
                    }
                    catch (...)
                    {
                        // 如果出现异常，销毁已构造的对象并释放内存
                        data_allocator::destroy(new_start, new_finish);
                        data_allocator::deallocate(new_start, len);
                        throw ;
                    }
                    // 清空原内存，并更新容器的指针和大小信息
                    clear();
                    start = new_start;
                    finish = new_finish;
                    end_of_storage = new_start + len;
                }
            }
        }

    template <class T>
        template <class Iter>
        void vector<T>::range_insert(const_iterator position, Iter first, Iter last)
        {
            if (first != last)
            {
                size_type n = 0;
                // 计算插入元素的个数
                n = mystl::distance(first, last);
                // 如果容器后方剩余空间足够容纳新增元素
                if (static_cast<size_type>(end_of_storage - finish) >= n)
                {
                    auto elems_after = finish - position;
                    iterator old_finish = finish;
                    if (elems_after > n)
                    {
                        // 将插入点之后的元素往后移动 n 个位置
                        mystl::uninitialized_copy(finish - n, finish, finish);
                        finish += n;
                        // 将插入点之前的元素往后移动 n 个位置
                        mystl::copy_backward((iterator)position, (iterator)old_finish - n, old_finish);
                        // 将 [first, last) 范围内的元素复制到插入点
                        mystl::copy(first, last, (iterator)position);
                    }
                    else
                    {
                        auto mid = first;
                        // 计算 mid 迭代器的位置，使得插入点之后的元素能够全部容纳在容器内
                        mystl::advance(mid, elems_after);
                        // 在插入点之后增加 n-elems_after 个新元素
                        mystl::uninitialized_copy(mid, last, finish);
                        finish += n - elems_after;
                        // 将插入点之前的元素复制到新增位置
                        mystl::uninitialized_copy((iterator)position, (iterator)old_finish, (iterator)finish);
                        finish += elems_after;
                        // 将 [first, mid) 范围内的元素复制到插入点
                        mystl::copy(first, mid, (iterator)position);
                    }
                }
                else
                {
                    const size_type old_size = size();
                    const size_type len = old_size + max(old_size, n);
                    iterator new_start = data_allocator::allocate(len);
                    iterator new_finish = new_start;
                    try
                    {
                        // 将 start 到 position 的元素复制到新内存中
                        new_finish = mystl::uninitialized_copy((iterator)start, (iterator)position, (iterator)new_start);
                        // 将 [first, last) 范围内的元素复制到新内存中
                        new_finish = mystl::uninitialized_copy((iterator)first, (iterator)last, (iterator)new_finish);
                        // 将 position 到 finish 的元素复制到新内存中
                        new_finish = mystl::uninitialized_copy((iterator)position, (iterator)finish, (iterator)new_finish);
                    }
                    catch (...)
                    {
                        // 如果出现异常，销毁已构造的对象并释放内存
                        data_allocator::destroy(new_start, new_finish);
                        data_allocator::deallocate(new_start, len);
                        throw ;
                    }
                    // 清空原内存，并更新容器的指针和大小信息
                    clear();
                    start = new_start;
                    finish = new_finish;
                    end_of_storage = new_start + len;
                }
            }
        }

    template <class T>
        typename vector<T>::iterator
        vector<T>::erase(const_iterator pos)
        {
            // 检查迭代器 pos 的有效性
            MYSTL_DEBUG(pos >= begin() && pos < end());
            // 将 pos 转换为普通迭代器
            iterator xpos = start + (pos - start);
            // 将 pos 之后的元素往前移动一位
            mystl::move(xpos + 1, finish, xpos);
            // 销毁最末尾的元素
            data_allocator::destroy(finish - 1);
            --finish;
            return xpos;
        }

    template <class T>
        typename vector<T>::iterator
        vector<T>::erase(const_iterator first, const_iterator last)
        {
            // 检查迭代器 first 和 last 的有效性
            MYSTL_DEBUG(first >= begin() && last <= end() && !(last < first));
            const auto n = first - start;
            iterator r = start + n;
            // 销毁区间 [first, last) 中的元素，并将后方元素前移
            data_allocator::destroy(mystl::move(r + (last - first), finish, r), finish);
            finish = finish - (last - first);
            return begin() + n;
        }

    template <class T>
        void vector<T>::resize(size_type new_size, const T& value)
        {
            if (new_size < size())
                // 如果新大小小于当前大小，删除多余元素
                erase(begin() + new_size, end());
            else
                // 否则在末尾插入新元素
                insert(end(), new_size - size(), value);
        }

    template <class T>
        void vector<T>::swap(vector<T>& other) noexcept
        {
            if (this != &other)
            {
                // 交换指针和大小信息
                mystl::swap(start, other.start);
                mystl::swap(finish, other.finish);
                mystl::swap(end_of_storage, other.end_of_storage);
            }
        }

    template <class T>
        void vector<T>::clear()
        {
            // 销毁所有元素并释放内存
            data_allocator::destroy(begin(), end());
            deallocate();
            start = finish = end_of_storage = nullptr;
        }

} // namespace mystl

#endif //TINYSTL_VECTOR_H
