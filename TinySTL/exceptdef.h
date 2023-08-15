#ifndef MYTINYSTL_EXCEPTDEF_H_
#define MYTINYSTL_EXCEPTDEF_H_

// 这个头文件定义了一些异常处理的宏

#include <stdexcept>
#include <cassert>

namespace mystl
{

    // 用于进行调试断言。当 expr 表达式为假时，将触发断言错误并终止程序运行。
#define MYSTL_DEBUG(expr) \
    assert(expr)  

    // 用于在满足条件 expr 时抛出 std::length_error 异常，并传递异常信息 what。
#define THROW_LENGTH_ERROR_IF(expr, what) \
    if ((expr)) throw std::length_error(what)

    // 用于在满足条件 expr 时抛出 std::out_of_range 异常，并传递异常信息 what。
#define THROW_OUT_OF_RANGE_IF(expr, what) \
    if ((expr)) throw std::out_of_range(what)

    // 用于在满足条件 expr 时抛出 std::runtime_error 异常，并传递异常信息 what。
#define THROW_RUNTIME_ERROR_IF(expr, what) \
    if ((expr)) throw std::runtime_error(what)

} // namepsace mystl

#endif // !MYTINYSTL_EXCEPTDEF_H_
