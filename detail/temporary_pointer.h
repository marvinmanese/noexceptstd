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
#ifndef GUARD_NOE_STD_detail_temporary_pointer_H
#define GUARD_NOE_STD_detail_temporary_pointer_H

#include <memory>

namespace noe_std
{
namespace detail
{
#if __cplusplus >= 201103L
    template<class T> using temporary_pointer = std::unique_ptr<T>;
#else
    template<class T>
    class temporary_pointer
    {
    public:
        typedef T* pointer;
        typedef const T* const_pointer;

        explicit temporary_pointer(const_pointer ptr = 0) : m_ptr(ptr) {}
        ~temporary_pointer();

    private:
        temporary_pointer(const temporary_pointer& rhs);
        temporary_pointer& operator=(const temporary_pointer& rhs);
    public:

        const_pointer get() const { return m_ptr; }
        pointer release();

    private:
        pointer m_ptr;
    };

    template<class T>
    temporary_pointer<T>::pointer temporary_pointer<T>::release()
    {
        pointer t = m_ptr;
        m_ptr = 0;
        return t;
    }

    template<class T>
    temporary_pointer<T>::~temporary_pointer()
    {
        if(m_ptr)
            delete m_ptr;
    };
#endif // __cplusplus >= 201103L
}
}

#endif // GUARD_NOE_STD_detail_temporary_pointer_H
