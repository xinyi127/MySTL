fndef TINYSTL_STACK_H
#define TINYSTL_STACK_H

#include "list.h"  // 包含底层容器list的头文件

namespace mystl
{

    template <class T, class Container = mystl::list<T>>  // 模板类，T为元素类型，Container为底层容器类型，默认为mystl::list<T>
    class stack
    {
    public:
        typedef typename Container::value_type      value_type;        // 元素类型
        typedef typename Container::size_type       size_type;         // 大小类型
        typedef typename Container::reference       reference;         // 引用类型
        typedef typename Container::const_reference const_reference;   // 常量引用类型

    protected:
        Container c;  // 底层容器对象

    public:

        stack() = default;  // 使用默认构造函数

        explicit stack(size_type n)  // 构造指定大小的栈
            : c(n) {}

        stack(size_type n, const value_type& value)  // 使用指定值构造指定大小的栈
            : c(n, value)
        {
        }

        template <class Iter>
        stack(Iter first, Iter last)  // 使用迭代器范围构造栈
            : c(first, last)
        {
        }

        stack(std::initializer_list<T> ilist)  // 使用初始化列表构造栈
            : c(ilist.begin(), ilist.end())
        {
        }

        stack(const Container& c_)  // 使用已有容器构造栈
            : c(c_)
        {
        }

        stack(Container&& c_) noexcept(std::is_nothrow_constructible<Container>::value)  // 使用已有容器构造栈（移动语义）
            : c(mystl::move(c_))
        {
        }

        stack(const stack& rhs)  // 拷贝构造函数
            : c(rhs.c)
        {
        }

        stack(stack&& rhs) noexcept(std::is_nothrow_constructible<Container>::value)  // 移动构造函数
            : c(mystl::move(rhs.c))
        {
        }

        stack& operator=(const stack& rhs)  // 拷贝赋值运算符
        {
            c = rhs.c;
            return *this;
        }

        stack& operator=(const stack&& rhs)  // 移动赋值运算符
        {
            c = mystl::move(rhs.c);
            return *this;
        }

        stack& operator=(std::initializer_list<T> ilist)  // 赋值运算符重载
        {
            c = ilist;
            return *this;
        }

        bool empty() const { return c.empty(); }  // 判断栈是否为空
        size_type size() const { return c.size(); }  // 返回栈的大小
        reference top() { return c.back(); }  // 返回栈顶元素的引用
        const_reference top() const { return c.back(); }  // 返回栈顶元素的常量引用
        void push(const value_type& value) { c.push_back(value); }  // 入栈操作，将元素加入到栈顶
        void pop() { c.pop_back(); }  // 出栈操作，将栈顶元素删除
        void swap(stack& rhs) noexcept(noexcept(mystl::swap(c, rhs.c)))  // 交换两个栈的内容
        {
            mystl::swap(c, rhs.c);
        }
        void clear() { c.clear(); }  // 清空栈中的元素

        bool operator==(const stack<T, Container>& x)  // 比较运算符==重载，判断两个栈是否相等
        {
            return c == x.c;
        }

        bool operator<(const stack<T, Container>& x)  // 比较运算符<重载，判断一个栈是否小于另一个栈
        {
            return c < x.c;
        }


    };

} // namespace mystl


#endif //TINYSTL_STACK_H
