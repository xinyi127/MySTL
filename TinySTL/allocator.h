#ifndef MYTINYSTL_ALLOCATOR_H_
#define MYTINYSTL_ALLOCATOR_H_

// 这个头文件包含一个模板类 allocator，用于管理内存的分配、释放，对象的构造、析构

#include "construct.h"  // 包含构造、析构相关的函数
#include "util.h"       // 包含一些工具函数

namespace mystl
{

    // allocator类模板，管理内存的分配、释放，对象的构造、析构
    template <class T>
        class allocator
        {
            public:
                typedef T            value_type;          // 数据类型
                typedef T*           pointer;             // 指针类型
                typedef const T*     const_pointer;       // 常量指针类型
                typedef T&           reference;           // 引用类型
                typedef const T&     const_reference;     // 常量引用类型
                typedef size_t       size_type;           // 大小类型
                typedef ptrdiff_t    difference_type;     // 指针差值类型

            public:
                // 内存分配（allocate）和释放（deallocate）相关函数
                static T*   allocate();                // 分配一个T类型的对象
                static T*   allocate(size_type n);     // 分配n个T类型的对象

                static void deallocate(T* ptr);        // 释放ptr指向的T类型对象的内存空间
                static void deallocate(T* ptr, size_type n);  // 释放由ptr指向的一串T类型的对象的内存空间

                // 对象构造（construct）相关函数
                static void construct(T* ptr);                 // 构造一个T类型的对象
                static void construct(T* ptr, const T& value); // 构造一个T类型的对象，并用value值来初始化
                static void construct(T* ptr, T&& value);      // 构造一个T类型的对象，并用右值value来初始化

                // 对象构造（construct，可变参数版本）相关函数
                template <class... Args>
                    static void construct(T* ptr, Args&& ...args); // 构造一个T类型的对象，使用args参数列表来初始化

                // 对象析构（destroy）相关函数
                static void destroy(T* ptr);             // 销毁ptr指向的T类型的对象
                static void destroy(T* first, T* last); // 销毁[first,last)范围内的一串T类型的对象
        };

    // allocator类模板成员函数的实现

    // 内存分配相关函数实现
    template <class T>
        T* allocator<T>::allocate()
        {
            return static_cast<T*>(::operator new(sizeof(T)));
        }

    template <class T>
        T* allocator<T>::allocate(size_type n)
        {
            if (n == 0)
                return nullptr;
            return static_cast<T*>(::operator new(n * sizeof(T)));
        }

    // 内存释放相关函数实现
    template <class T>
        void allocator<T>::deallocate(T* ptr)
        {
            if (ptr == nullptr)
                return;
            ::operator delete(ptr);
        }

    template <class T>
        void allocator<T>::deallocate(T* ptr, size_type /*size*/)
        {
            if (ptr == nullptr)
                return;
            ::operator delete(ptr);
        }

    // 对象构造相关函数实现
    template <class T>
        void allocator<T>::construct(T* ptr)
        {
            mystl::construct(ptr);
        }

    template <class T>
        void allocator<T>::construct(T* ptr, const T& value)
        {
            mystl::construct(ptr, value);
        }

    template <class T>
        void allocator<T>::construct(T* ptr, T&& value)
        {
            mystl::construct(ptr, mystl::move(value));
        }

    // 可变参数版本的对象构造相关函数实现
    template <class T>
        template <class ...Args>
        void allocator<T>::construct(T* ptr, Args&& ...args)
        {
            mystl::construct(ptr, mystl::forward<Args>(args)...);
        }

    // 对象析构相关函数实现
    template <class T>
        void allocator<T>::destroy(T* ptr)
        {
            mystl::destroy(ptr);
        }

    template <class T>
        void allocator<T>::destroy(T* first, T* last)
        {
            mystl::destroy(first, last);
        }

} // namespace mystl

#endif // !MYTINYSTL_ALLOCATOR_H_
