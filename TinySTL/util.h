#ifndef MYTINYSTL_UTIL_H_
#define MYTINYSTL_UTIL_H_

// 这个文件包含一些通用工具，包括 move, forward, swap 等函数，以及 pair 等 

#include <cstddef>

#include "type_traits.h"

namespace mystl
{

    // move 接受一个参数，返回该参数的右值引用

    template <class T>
    // typename 用于指示 std::remove_reference<T>::type 是一个类型名称，并且将其用作返回类型的一部分。通常与模板元编程中的类型萃取结合使用。
    // std::remove_reference 只会去除类型的引用修饰符（& 或 &&），而保留常量修饰符（const 或 volatile）。如果需要同时去除引用和常量修饰符，可以使用 std::remove_cvref。
    typename std::remove_reference<T>::type&& move(T&& arg) noexcept // noexcept 指定抛出异常
    {
        // 通过 static_cast 将 arg 转换为右值引用。这个转换确保了返回的结果是一个右值引用，无论原始参数是左值还是右值。
        return static_cast<typename std::remove_reference<T>::type&&>(arg);
    }

    // forward 用于完美转发，基于引用坍缩规则：无论模板参数是什么类型的引用，当且仅当实参类型为右引用时，模板参数才能被推导为右引用类型
    // 注意是“能”，如果模板参数是左引用，即便实参是右引用，还是会被推导为左引用

    template <class T>
    T&& forward(typename std::remove_reference<T>::type& arg) noexcept
    {
        return static_cast<T&&>(arg);
    }

    template <class T>
    T&& forward(typename std::remove_reference<T>::type&& arg) noexcept
    {
        // 对 T 进行判断，如果 T 是一个左值引用类型（如 T = int&），则会触发 static_assert 断言，编译将会失败
        static_assert(!std::is_lvalue_reference<T>::value, "bad forward");
        return static_cast<T&&>(arg);
    }

    // swap

    template <class Tp>
    void swap(Tp& lhs, Tp& rhs) // 交换两个元素
    {
        auto tmp(mystl::move(lhs));
        lhs = mystl::move(rhs);
        rhs = mystl::move(tmp);
    }

    template <class ForwardIter1, class ForwardIter2>
    ForwardIter2 swap_range(ForwardIter1 first1, ForwardIter1 last1, ForwardIter2 first2) // 交换范围
    {
        for (; first1 != last1; ++first1, (void) ++first2) {
            mystl::swap(*first1, *first2);
        }
        return first2;
    }

    template <class Tp, size_t N>
    void swap(Tp(&a)[N], Tp(&b)[N]) // 交换数组
    {
        mystl::swap_range(a, a + N, b);
    }

    // --------------------------------------------------------------------------------------
    // pair

    // 结构体模板 : pair
    // 两个模板参数分别表示两个数据的类型
    // 用 first 和 second 来分别取出第一个数据和第二个数据
    template <class Ty1, class Ty2>
    struct pair
    {
        typedef Ty1    first_type;
        typedef Ty2    second_type;

        first_type first;    // 保存第一个数据
        second_type second;  // 保存第二个数据

        // default constructiable 默认可构造
        template <class Other1 = Ty1, class Other2 = Ty2,
            typename = typename std::enable_if < // typename= 类型模板参数，基于一个布尔条件来选择是否启用某个模板
            std::is_default_constructible<Other1>::value && // 判断类型 Other1 是可默认构造的
            std::is_default_constructible<Other2>::value, void>::type>
            constexpr pair()
            : first(), second()
        {
        }

        // implicit constructiable for this type 该类型可隐式构造
        // 返回类型是 int 类型，并且使用了默认参数 = 0。然而，这是一个特殊技巧，实际上返回的并不是 int 类型的值，而是 constexpr 修饰的 pair<Ty1, Ty2> 类型。
        // 这是通过模板参数的 SFINAE 技术实现的。当满足特定条件时，该函数模板会返回一个 int 值（默认为 0），否则该函数模板将无法匹配，并通过 SFINAE 被排除在重载选择之外。
        // 因此，实际调用的是具有返回类型 pair<Ty1, Ty2> 的函数模板。
        // SFINAE: Substitution Failure Is Not An Error 替换错误而不是失败
        template <class U1 = Ty1, class U2 = Ty2,
            typename std::enable_if< // 非类型模板参数，设置默认值为 0，在编译时决定返回类型
            std::is_copy_constructible<U1>::value && // 判断类型 U1 是否可通过拷贝构造函数进行构造
            std::is_copy_constructible<U2>::value &&
            std::is_convertible<const U1&, Ty1>::value && // 判断类型 const U1& 是否可以隐式转换为类型 Ty1 和 Ty2
            std::is_convertible<const U2&, Ty2>::value, int>::type = 0>
            constexpr pair(const Ty1& a, const Ty2& b)
            : first(a), second(b)
        {
        }

        // explicit constructible for this type 该类型的显示构造函数
        template <class U1 = Ty1, class U2 = Ty2,
            typename std::enable_if<
            std::is_copy_constructible<U1>::value &&
            std::is_copy_constructible<U2>::value &&
            (!std::is_convertible<const U1&, Ty1>::value || // U1 类型不能隐式地转换为 Ty1 类型
            !std::is_convertible<const U2&, Ty2>::value), int>::type = 0>
            explicit constexpr pair(const Ty1& a, const Ty2& b) // 带有显式 explicit 修饰符的构造函数
            : first(a), second(b)
        {
        }

        pair(const pair& rhs) = default;
        pair(pair&& rhs) = default;

        // implicit constructiable for other type 其它类型可隐式构造
        template <class Other1, class Other2,
            typename std::enable_if<
            std::is_constructible<Ty1, Other1>::value && // 是否可以从参数类型 Other1 和 Other2 直接构造出 Ty1 和 Ty2 类型的对象
            std::is_constructible<Ty2, Other2>::value &&
            std::is_convertible<Other1&&, Ty1>::value && // 否可以将参数类型 Other1 和 Other2 隐式地转换为 Ty1 和 Ty2 类型
            std::is_convertible<Other2&&, Ty2>::value, int>::type = 0>
            constexpr pair(Other1&& a, Other2&& b)
            : first(mystl::forward<Other1>(a)),
            second(mystl::forward<Other2>(b))
        {
        }

        // explicit constructiable for other type 其它类型的显示可构造
        template <class Other1, class Other2,
            typename std::enable_if<
            std::is_constructible<Ty1, Other1>::value &&
            std::is_constructible<Ty2, Other2>::value &&
            (!std::is_convertible<Other1, Ty1>::value ||
            !std::is_convertible<Other2, Ty2>::value), int>::type = 0>
            explicit constexpr pair(Other1&& a, Other2&& b)
            : first(mystl::forward<Other1>(a)),
            second(mystl::forward<Other2>(b))
        {
        }

        // implicit constructiable for other pair 可隐式构造其它 pair
        template <class Other1, class Other2,
            typename std::enable_if<
             std::is_constructible<Ty1, const Other1&>::value &&
            std::is_constructible<Ty2, const Other2&>::value &&
            std::is_convertible<const Other1&, Ty1>::value &&
            std::is_convertible<const Other2&, Ty2>::value, int>::type = 0>
        constexpr pair(const pair<Other1, Other2>& other)
            : first(other.first), second(other.second)
        {
        }

        // explicit constructiable for other pair 可显示构造其它 pair
        template <class Other1, class Other2,
            typename std::enable_if<
            std::is_constructible<Ty1, const Other1&>::value &&
            std::is_constructible<Ty2, const Other2&>::value &&
            (!std::is_convertible<const Other1&, Ty1>::value ||
            !std::is_convertible<const Other2&, Ty2>::value), int>::type = 0>
        explicit constexpr pair(const pair<Other1, Other2>& other)
            : first(other.first), second(other.second)
        {
        }

        // implicit constructiable for other pair 对其他 pair 隐式可构造
        template <class Other1, class Other2,
            typename std::enable_if<
            std::is_constructible<Ty1, Other1>::value &&
            std::is_constructible<Ty2, Other2>::value &&
            std::is_convertible<Other1, Ty1>::value &&
            std::is_convertible<Other2, Ty2>::value, int>::type = 0>
            constexpr pair(pair<Other1, Other2>&& other)
            : first(mystl::forward<Other1>(other.first)),
            second(mystl::forward<Other2>(other.second))
        {
        }

         // explicit constructiable for other pair 对其他 pair 显示可构造
        template <class Other1, class Other2,
            typename std::enable_if<
            std::is_constructible<Ty1, Other1>::value &&
            std::is_constructible<Ty2, Other2>::value &&
            (!std::is_convertible<Other1, Ty1>::value ||
            !std::is_convertible<Other2, Ty2>::value), int>::type = 0>
            explicit constexpr pair(pair<Other1, Other2>&& other)
            : first(mystl::forward<Other1>(other.first)),
            second(mystl::forward<Other2>(other.second))
        {
        }

        // copy assign for this pair 复制赋值
        pair& operator=(const pair& rhs)
        {
            if (this != &rhs)
            {
                first = rhs.first;
                second = rhs.second;
            }
            return *this;
        }

        // move assign for this pair 移动赋值
        pair& operator=(pair&& rhs)
        {
            if (this != &rhs)
            {
                first = mystl::move(rhs.first);
                second = mystl::move(rhs.second);
            }
            return *this;
        }

        // copy assign for other pair
        template <class Other1, class Other2>
        pair& operator=(const pair<Other1, Other2>& other)
        {
            first = other.first;
            second = other.second;
            return *this;
        }

        // move assign for other pair
        template <class Other1, class Other2>
        pair& operator=(pair<Other1, Other2>&& other)
        {
            first = mystl::forward<Other1>(other.first);
            second = mystl::forward<Other2>(other.second);
            return *this;
        }

        ~pair() = default;

        void swap(pair& other)
        {
            if (this != &other)
            {
                mystl::swap(first, other.first);
                mystl::swap(second, other.second);
            }
        }

    };
    // ----------------------------------------------------------------
    // 重载比较操作符 
    template <class Ty1, class Ty2>
    bool operator==(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs)
    {
        return lhs.first == rhs.first && lhs.second == rhs.second;
    }

    template <class Ty1, class Ty2>
    bool operator<(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs)
    {
         return lhs.first < rhs.first || (lhs.first == rhs.first && lhs.second < rhs.second);
    }

    template <class Ty1, class Ty2>
    bool operator!=(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs)
    {
        return !(lhs == rhs);
    }

    template <class Ty1, class Ty2>
    bool operator>(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs)
    {
        return rhs < lhs;
    }

    template <class Ty1, class Ty2>
    bool operator<=(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs)
    {
         return !(rhs < lhs);
    }

    template <class Ty1, class Ty2>
    bool operator>=(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs)
    {
        return !(lhs < rhs);
    }

    // 重载 mystl 的 swap
    template <class Ty1, class Ty2>
    void swap(pair<Ty1, Ty2>& lhs, pair<Ty1, Ty2>& rhs)
    {
        lhs.swap(rhs);
    }

    // 全局函数，让两个数据成为一个 pair，也就是 make_pair
    template <class Ty1, class Ty2>
    pair<Ty1, Ty2> make_pair(Ty1&& first, Ty2&& second)
    {
        return pair<Ty1, Ty2>(mystl::forward<Ty1>(first), mystl::forward<Ty2>(second));
    }

}

#endif // !MYTINYSTL_UTIL_H_
