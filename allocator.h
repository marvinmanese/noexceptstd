/**
 * NoException Standard Library Container Implementation
 * by Marvin Manese 2017
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 **/
#ifndef GUARD_NOE_STD_allocator_H
#define GUARD_NOE_STD_allocator_H

#include <cstdint>
#include <limits>
#include <new>
#include <utility>
#include "macro.h"

namespace noe_std
{
namespace detail
{
    template<class T>
    struct allocator_max_size
    {
        enum : std::size_t { value = std::numeric_limits<std::size_t>::max() / sizeof(T) };
    };
}
    template<class T>
    class allocator
    {
    public:
        typedef T                   value_type;
        typedef value_type*         pointer;
        typedef const pointer       const_pointer;
        typedef value_type&         reference;
        typedef const reference     const_reference;
        typedef std::size_t       size_type;
        typedef std::ptrdiff_t    difference_type;

        template<class U>
        struct rebind
        {
            typedef allocator<U> other;
        };

        allocator() noe_std_no_except {}
        ~allocator() noe_std_no_except {}

        pointer allocate(size_type n) noe_std_no_except;
        void deallocate(pointer p, size_type n) noe_std_no_except;
#if __cplusplus >= 201103L
        constexpr size_type max_size() const noexcept;
        template<class... Args> void construct(pointer p, Args&&... args);
#else
        size_type max_size() const;
        static void construct(pointer p, const_reference t);
#endif // __cplusplus >= 201103L
        void destroy(pointer p);
    };

    template<class T>
    typename allocator<T>::pointer allocator<T>::allocate(size_type n) noe_std_no_except
    {
        return static_cast<pointer>(::operator new(n * sizeof(T), std::nothrow));
    }

    template<class T>
    void allocator<T>::deallocate(pointer p, size_type) noe_std_no_except
    {
        ::operator delete(p);
    }
#if __cplusplus >= 201103L
    template<class T>
    constexpr typename allocator<T>::size_type allocator<T>::max_size() const noexcept
    {
        return detail::allocator_max_size<T>::value;
    }

    template<class T>
    template<class... Args>
    void allocator<T>::construct(pointer p, Args&&... args)
    {
        new(p) T(std::forward<Args>(args)...);
    }
#else
    template<class T>
    typename allocator<T>::size_type allocator<T>::max_size() const
    {
        return detail::allocator_max_size<T>::value;
    }

    template<class T>
    void allocator<T>::construct(pointer p, const_reference t)
    {
        new(p) T(t);
    }
#endif // __cplusplus >= 201103L
    template<class T>
    void allocator<T>::destroy(pointer p)
    {
        p->~T();
    }
}

#endif // GUARD_NOE_STD_allocator_H
