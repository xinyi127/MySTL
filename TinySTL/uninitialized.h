#ifndef MYTINYSTL_UNINITIALIZED_H_
#define MYTINYSTL_UNINITIALIZED_H_

// 这个头文件用于对未初始化空间构造元素，避免了手动调用构造函数的麻烦。通过判断元素类型是否是trivially copy/move assignable，选择不同的实现方式。

#include "algobase.h"
#include "construct.h"
#include "iterator.h"
#include "type_traits.h"
#include "util.h"

namespace mystl {

    /******************************************/
    // uninitialized_copy：将源区间 [first, last) 中的元素复制到目标区间 [result, result + (last - first)) 中
    /********************************************/
    template <class InputIter, class ForwardIter>
        ForwardIter
        unchecked_uninit_copy(InputIter first, InputIter last, ForwardIter result, std::true_type)
        {// 若是 POD 类型，直接使用 copy 函数复制即可
            return mystl::copy(first, last, result);
        }

    template <class InputIter, class ForwardIter>
        ForwardIter
        unchecked_uninit_copy(InputIter first, InputIter last, ForwardIter result, std::false_type)
        {// 非 POD 类型需要调用构造函数构造对象
            auto cur = result;// 指向目标区间的当前位置
            try
            {
                for (; first != last; ++first, ++cur)
                {// 构造对象
                    mystl::construct(&*cur, *first);
                }
            }
            catch (...)
            {
                for (; result != cur; --cur)
                {// 析构已经构造好的对象
                    mystl::destroy(&*cur);
                }
            }
            return cur;
        }

    template <class InputIter, class ForwardIter>
        ForwardIter uninitialized_copy(InputIter first, InputIter last, ForwardIter result)
        {
            return mystl::unchecked_uninit_copy(first, last, result,
                    std::is_trivially_copy_assignable<
                    typename iterator_traits<ForwardIter>::value_type>{});
            // 判断 ForwardIter 类型的元素是否为 POD 类型, 以此来确定返回哪个形式的实现
        }

    /***************************************************/
    // uninitialized_copy_n：将源区间 [first, first + n) 中的元素复制到目标区间 [result, result + n) 中
    /****************************************************/
    template <class InputIter, class Size, class ForwardIter>
        ForwardIter
        unchecked_uninit_copy_n(InputIter first, Size n, ForwardIter result, std::true_type)
        {// 若是 POD 类型，直接使用 copy_n 函数复制即可
            return mystl::copy_n(first, n, result).second;
        }

    template <class InputIter, class Size, class ForwardIter>
        ForwardIter
        unchecked_uninit_copy_n(InputIter first, Size n, ForwardIter result, std::false_type)
        {// 非 POD 类型需要调用构造函数构造对象
            auto cur = result;// 指向目标区间的当前位置
            try
            {
                for (; n > 0; --n, ++cur, ++first)
                {// 构造对象
                    mystl::construct(&*cur, *first);
                }
            }
            catch (...)
            {
                for (; result != cur; --cur)
                {// 析构已经构造好的对象
                    mystl::destroy(&*cur);
                }
            }
            return cur;
        }

    template <class InputIter, class Size, class ForwardIter>
        ForwardIter uninitialized_copy_n(InputIter first, Size n, ForwardIter result)
        {
            return mystl::unchecked_uninit_copy_n(first, n, result,
                    std::is_trivially_copy_assignable<
                    typename iterator_traits<InputIter>::value_type>{});
            // 判断 InputIter 类型的元素是否为 POD 类型, 以此来确定返回哪个形式的实现
        }

    /********************************************************/
    // uninitialized_fill：将目标区间 [first, last) 中的元素全部赋值为 value
    /*********************************************************/
    template <class ForwardIter, class T>
        void
        unchecked_uninit_fill(ForwardIter first, ForwardIter last, const T& value, std::true_type)
        {// 若是 POD 类型，直接使用 fill 函数填充即可
            mystl::fill(first, last, value);
        }

    template <class ForwardIter, class T>
        void
        unchecked_uninit_fill(ForwardIter first, ForwardIter last, const T& value, std::false_type)
        {// 非 POD 类型需要调用构造函数构造对象
            auto cur = first;// 指向目标区间的当前位置
            try
            {
                for (; cur != last; ++cur)
                {// 构造对象
                    mystl::construct(&*cur, value);
                }
            }
            catch (...)
            {
                for (;first != cur; ++first)
                {// 析构已经构造好的对象
                    mystl::destroy(&*first);
                }
            }
        }

    template <class ForwardIter, class T>
        void uninitialized_fill(ForwardIter first, ForwardIter last, const T& value)
        {
            mystl::unchecked_uninit_fill(first, last, value,
                    std::is_trivially_copy_assignable<
                    typename iterator_traits<ForwardIter>::value_type>{});
            // 判断 ForwardIter 类型的元素是否为 POD 类型, 以此来确定返回哪个形式的实现
        }

    /*************************************************/
    // uninitialized_fill_n：从 first 位置开始，填充 n 个元素值 value，返回填充结束的位置
    /***************************************************/
    template <class ForwardIter, class Size, class T>
        ForwardIter
        unchecked_uninit_fill_n(ForwardIter first, Size n, const T& value, std::true_type)
        {// 若是 POD 类型，直接使用 fill_n 函数填充即可
            return mystl::fill_n(first, n, value);
        }

    template <class ForwardIter, class Size, class T>
        ForwardIter
        unchecked_uninit_fill_n(ForwardIter first, Size n, const T& value, std::false_type)
        {// 非 POD 类型需要调用构造函数构造对象
            auto cur = first;// 指向目标区间的当前位置
            try
            {
                for (; n > 0; --n, ++cur)
                {// 构造对象
                    mystl::construct(&*cur, value);
                }
            }
            catch (...)
            {
                for (; first != cur; ++first)
                {// 析构已经构造好的对象
                    mystl::destroy(&*first);
                }
                throw;
            }
            return cur;
        }

    template <class ForwardIter, class Size, class T>
        ForwardIter uninitialized_fill_n(ForwardIter first, Size n, const T& value)
        {
            return mystl::unchecked_uninit_fill_n(first, n, value,
                    std::is_trivially_copy_assignable<
                    typename iterator_traits<ForwardIter>::value_type>{});
            // 判断 ForwardIter 类型的元素是否为 POD 类型, 以此来确定返回哪个形式的实现
        }

    /*******************************/
    // uninitialized_move：将源区间 [first, last) 中的元素移动到目标区间 [result, result + (last - first)) 中
    /***************************************/
    template <class InputIter, class ForwardIter>
        ForwardIter
        unchecked_uninit_move(InputIter first, InputIter last, ForwardIter result, std::true_type)
        {// 若是 POD 类型，直接使用 move 函数移动即可
            return mystl::move(first, last, result);
        }

    template <class InputIter, class ForwardIter>
        ForwardIter
        unchecked_uninit_move(InputIter first, InputIter last, ForwardIter result, std::false_type)
        {// 非 POD 类型需要调用构造函数构造对象
            ForwardIter cur = result;// 指向目标区间的当前位置
            try
            {
                for (; first != last; ++first, ++cur)
                {// 构造对象
                    mystl::construct(&*cur, mystl::move(*first));
                }
            }
            catch (...)
            {
                mystl::destroy(result, cur);
            }
            return cur;
        }

    template <class InputIter, class ForwardIter>
        ForwardIter uninitialized_move(InputIter first, InputIter last, ForwardIter result)
        {
            return mystl::unchecked_uninit_move(first, last, result,
                    std::is_trivially_move_assignable<
                    typename iterator_traits<InputIter>::value_type>{});
            // 判断 InputIter 类型的元素是否为 POD 类型, 以此来确定返回哪个形式的实现
        }

    /***************************************/
    // uninitialized_move_n：将源区间 [first, first + n) 中的元素移动到目标区间 [result, result + n) 中
    /********************************************/
    template <class InputIter, class Size, class ForwardIter>
        ForwardIter
        unchecked_uninit_move_n(InputIter first, Size n, ForwardIter result, std::true_type)
        {// 若是 POD 类型，直接使用 move_n 函数移动即可
            return mystl::move(first, first + n, result);
        }

    template <class InputIter, class Size, class ForwardIter>
        ForwardIter
        unchecked_uninit_move_n(InputIter first, Size n, ForwardIter result, std::false_type)
        {// 非 POD 类型需要调用构造函数构造对象
            auto cur = result;// 指向目标区间的当前位置
            try
            {
                for (; n > 0; --n, ++first, ++cur)
                {// 构造对象
                    mystl::construct(&*cur, mystl::move(*first));
                }
            }
            catch (...)
            {
                for (; result != cur; ++result)
                {// 析构已经构造好的对象
                    mystl::destroy(&*result);
                }
                throw;
            }
            return cur;
        }

    template <class InputIter, class Size, class ForwardIter>
        ForwardIter uninitialized_move_n(InputIter first, Size n, ForwardIter result)
        {
            return mystl::unchecked_uninit_move_n(first, n, result,
                    std::is_trivially_move_assignable<
                    typename iterator_traits<InputIter>::value_type>{});
            // 判断 InputIter 类型的元素是否为 POD 类型, 以此来确定返回哪个形式的实现
        }

} // namespace mystl
#endif // !MYTINYSTL_UNINITIALIZED_H_
