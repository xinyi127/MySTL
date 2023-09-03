#ifndef TINYSTL_BASIC_STRING_H
#define TINYSTL_BASIC_STRING_H


// 模板类 basic_string
// 用于表示字符串类型

#include <iostream>
#include "memory.h"
#include "functional.h"
#include "exceptdef.h"

namespace mystl
{
    template <class CharType>
    struct char_traits
    {
        typedef CharType char_type;

        // 计算字符串的长度
        static size_t length(const char_type* str)
        {
            size_t len = 0;
            for (; *str != char_type(0); ++str)
                ++len;
            return len;
        }

        // 比较两个字符串的前 n 个字符
        static int compare(const char_type* s1, const char_type* s2, size_t n)
        {
            for (; n != 0; --n, ++s1, ++s2)
            {
                if (*s1 < *s2)
                    return -1;
                if (*s2 < *s1)
                    return 1;
            }
            return 0;
        }

        // 将源字符串复制到目标字符串
        static char_type* copy(char_type* dst, const char_type* src, size_t n)
        {
            MYSTL_DEBUG(src + n <= dst || dst + n <= src);
            char_type* r = dst;
            for (; n > 0; --n, ++dst, ++src)
                *dst = *src;
            return r;
        }

        // 将源字符串移动到目标字符串
        static char_type* move(char_type* dst, const char_type* src, size_t n)
        {
            char_type* r = dst;
            if (dst < src)
            {
                for (; n > 0; --n, ++dst, ++src)
                    *dst = *src;
            }
            else if (dst > src)
            {
                dst += n,
                src += n;
                for (; n > 0; n--)
                    *(--dst) = *(--src);
            }
            return r;
        }

        // 将目标字符串填充为指定字符
        static char_type* fill(char_type* dst, char_type ch, size_t count)
        {
            char_type* r = dst;
            for (; count > 0; --count, ++dst)
                *dst = ch;
            return r;
        }
    };
}


#endif //TINYSTL_BASIC_STRING_H
