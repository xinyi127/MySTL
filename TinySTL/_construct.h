#ifndef MYTINYSTL_CONSTRUCT_H_
#define MYTINYSTL_CONSTRUCT_H_

// 这个头文件包含两个函数 construct，destroy
// construct : 负责对象的构造
// destroy   : 负责对象的析构

#include <new>  // 使用 new 运算符
#include "type_traits.h"  // 使用类型特征
#include "iterator.h"  // 使用迭代器

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4100)  // 忽略未使用的参数警告
#endif // _MSC_VER

namespace mystl
{

    // construct 构造对象

    // 默认构造对象
    template <class Ty>
        void construct(Ty* ptr)
        {
            ::new ((void*)ptr) Ty();
        }

    // 使用指定的值初始化对象
    template <class Ty1, class Ty2>
        void construct(Ty1* ptr, const Ty2& value)
        {
            ::new ((void*)ptr) Ty1(value);
        }

    // 使用参数列表初始化对象
    template <class Ty, class... Args>
        void construct(Ty* ptr, Args&&... args)
        {
            ::new ((void*)ptr) Ty(mystl::forward<Args>(args)...);
        }

    // destroy 将对象析构

    // 对于类型为 TriviallyDestructible 的对象，不做任何操作
    template <class Ty>
        void destroy_one(Ty*, std::true_type) {}

    // 对于非 TriviallyDestructible 的对象，调用其析构函数进行析构
    template <class Ty>
        void destroy_one(Ty* pointer, std::false_type)
        {
            if (pointer != nullptr)
            {
                pointer->~Ty();
            }
        }

    // 对于一组类型为 TriviallyDestructible 的对象范围，不做任何操作
    template <class ForwardIter>
        void destroy_cat(ForwardIter , ForwardIter , std::true_type) {}

    // 对于一组非 TriviallyDestructible 的对象范围，逐个调用其析构函数进行析构
    template <class ForwardIter>
        void destroy_cat(ForwardIter first, ForwardIter last, std::false_type)
        {
            for (; first != last; ++first)
                destroy(&*first);
        }

    // 析构单个对象
    template <class Ty>
        void destroy(Ty* pointer)
        {
            destroy_one(pointer, std::is_trivially_destructible<Ty>{});
        }

    // 析构一组对象
    template <class ForwardIter>
        void destroy(ForwardIter first, ForwardIter last)
        {
            destroy_cat(first, last, std::is_trivially_destructible<
                    typename iterator_traits<ForwardIter>::value_type>{});
        }

} // namespace mystl

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

#endif // !MYTINYSTL_CONSTRUCT_H_
