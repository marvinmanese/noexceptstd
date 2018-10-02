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
#ifndef GUARD_NOE_STD_vector_H
#define GUARD_NOE_STD_vector_H

#include <algorithm>
#include <cstring>
#include <iterator>
#include <utility>
#include "allocator.h"
#include "macro.h"

/// TODO:
/// - Implement insert
/// - Implement emplace
/// - More bug checking
namespace noe_std
{
namespace detail
{
    template<class AllocatorT>
    struct vector_allocator_holder
    {
        typedef AllocatorT                                  allocator_type;
#if __cplusplus >= 201103L
        typedef std::allocator_traits<allocator_type>       alloc_traits_t;
        typedef typename alloc_traits_t::value_type         value_type;
        typedef typename alloc_traits_t::size_type          size_type;
        typedef typename alloc_traits_t::difference_type    difference_type;
        typedef value_type&                                 reference;
        typedef const value_type&                           const_reference;
        typedef typename alloc_traits_t::void_pointer       void_pointer;
        typedef typename alloc_traits_t::const_void_pointer const_void_pointer;
        typedef typename alloc_traits_t::pointer            pointer;
        typedef typename alloc_traits_t::const_pointer      const_pointer;
#else
        typedef typename allocator_type::value_type         value_type;
        typedef typename allocator_type::size_type          size_type;
        typedef typename allocator_type::difference_type    difference_type;
        typedef value_type&                                 reference;
        typedef const value_type&                           const_reference;
        typedef typename void*                              void_pointer;
        typedef typename const void*                        const_void_pointer;
        typedef typename allocator_type::pointer            pointer;
        typedef typename allocator_type::const_pointer      const_pointer;
#endif // __cplusplus >= 201103L
        vector_allocator_holder() {}
#if __cplusplus >= 201103L
        vector_allocator_holder(const vector_allocator_holder& rhs) = delete;
        vector_allocator_holder& operator=(const vector_allocator_holder& rhs) = delete;
        vector_allocator_holder(vector_allocator_holder&& rhs) = default;
        vector_allocator_holder& operator=(vector_allocator_holder&& rhs) = default;
#else
    private:
        vector_allocator_holder(const vector_allocator_holder& rhs);
        vector_allocator_holder& operator=(const vector_allocator_holder& rhs);
    public:
#endif // __cplusplus >= 201103L
        vector_allocator_holder(const allocator_type& alloc) : m_member(alloc) {}
        vector_allocator_holder(const allocator_type& alloc, size_type n) : m_member(alloc, n) {}
#if __cplusplus >= 201103L
        vector_allocator_holder(allocator_type&& alloc) : m_member(std::move(alloc)) {}
        vector_allocator_holder(allocator_type&& alloc, size_type n) : m_member(std::move(alloc), n) {}
#endif // __cplusplus >= 201103L

        struct vector_allocator_impl : public allocator_type
        {
            vector_allocator_impl() : m_capacity(0), m_size(0), m_data(0) {}
#if __cplusplus >= 201103L
            vector_allocator_impl(const vector_allocator_impl& rhs) = delete;
            vector_allocator_impl& operator=(const vector_allocator_impl& rhs) = delete;
            vector_allocator_impl(vector_allocator_impl&& rhs) = default;
            vector_allocator_impl& operator=(vector_allocator_impl&& rhs) = default;
#else
        private:
            vector_allocator_impl(const vector_allocator_impl& rhs);
            vector_allocator_impl& operator=(const vector_allocator_impl& rhs);
        public:
#endif // __cplusplus >= 201103L
            ~vector_allocator_impl()
            {
                if(m_data) {
                    while(m_size)
                        allocator_type::destroy(m_data + (--m_size));
                    allocator_type::deallocate(m_data, m_capacity);
                }
            }

            vector_allocator_impl(const allocator_type& alloc) : allocator_type(alloc), m_capacity(0), m_size(0), m_data(0) {}
            vector_allocator_impl(const allocator_type& alloc, size_type n) : allocator_type(alloc), m_capacity(n), m_size(0), m_data(n > 0 ? allocator_type::allocate(n) : 0) {}
#if __cplusplus >= 201103L
            vector_allocator_impl(allocator_type&& alloc) : allocator_type(std::move(alloc)), m_capacity(0), m_size(0), m_data(0) {}
            vector_allocator_impl(allocator_type&& alloc, size_type n) : allocator_type(std::move(alloc)), m_capacity(n), m_size(0), m_data(n > 0 ? allocator_type::allocate(n) : 0) {}
#endif // __cplusplus >= 201103L

            void swap(vector_allocator_impl& other) noe_std_no_except
            {
                std::swap(m_capacity, other.m_capacity);
                std::swap(m_size, other.m_size);
                std::swap(m_data, other.m_data);
            }

            size_type   m_capacity;
            size_type   m_size;
            pointer     m_data;
        } m_member;

        allocator_type& allocator() { return m_member; }
        const allocator_type& allocator() const { return m_member; }
    };
}
    template<class T,
             class AllocatorT = allocator<T>>
    class vector : private detail::vector_allocator_holder<AllocatorT>
    {
    private:
        struct base_vector_iterator : std::iterator<std::random_access_iterator_tag, T>
        {
            typedef std::iterator<std::random_access_iterator_tag, T> base_iterator;
        };

        class vector_iterator : base_vector_iterator
        {
        private:
            typedef typename base_vector_iterator::base_iterator base_iterator;

        public:
            typedef typename base_iterator::value_type          value_type;
            typedef typename base_iterator::difference_type     difference_type;
            typedef typename base_iterator::pointer             pointer;
            typedef typename base_iterator::reference           reference;
            typedef typename base_iterator::iterator_category   iterator_category;

            explicit vector_iterator(pointer ptr = 0) : m_ptr(ptr) {}

            vector_iterator& operator++() { ++m_ptr; return *this; }
            const vector_iterator operator++(int) { vector_iterator old = *this; ++m_ptr; return old; }
            vector_iterator& operator--() { --m_ptr; return *this; }
            const vector_iterator operator--(int) { vector_iterator old = *this; --m_ptr; return old; }
            vector_iterator& operator+=(int n) { m_ptr += n; return *this; }
            vector_iterator& operator-=(int n) { m_ptr -= n; return *this; }

            bool operator==(const vector_iterator& rhs) const { return m_ptr == rhs.m_ptr; }
            bool operator!=(const vector_iterator& rhs) const { return m_ptr != rhs.m_ptr; }
            value_type& operator*() { return *m_ptr; }
            value_type* operator->() { return m_ptr; }

        private:
            pointer m_ptr;
        };

        class const_vector_iterator : base_vector_iterator
        {
        private:
            typedef typename base_vector_iterator::base_iterator base_iterator;

        public:
            typedef typename base_iterator::value_type          value_type;
            typedef typename base_iterator::difference_type     difference_type;
            typedef typename base_iterator::pointer             pointer;
            typedef typename base_iterator::reference           reference;
            typedef typename base_iterator::iterator_category   iterator_category;

//            const_vector_iterator() : m_ptr(0) {}
            explicit const_vector_iterator(pointer ptr = 0) : m_ptr(ptr) {}

            const const_vector_iterator& operator++() const { ++m_ptr; return *this; }
            const const_vector_iterator operator++(int) const { const_vector_iterator old = *this; ++m_ptr; return old; }
            const const_vector_iterator& operator--() const { --m_ptr; return *this; }
            const const_vector_iterator operator--(int) const { const_vector_iterator old = *this; --m_ptr; return old; }
            const const_vector_iterator& operator+=(int n) const { m_ptr += n; return *this; }
            const const_vector_iterator& operator-=(int n) const { m_ptr -= n; return *this; }

            bool operator==(const const_vector_iterator& rhs) const { return m_ptr == rhs.m_ptr; }
            bool operator!=(const const_vector_iterator& rhs) const { return m_ptr != rhs.m_ptr; }
            const value_type& operator*() const { return *m_ptr; }
            const value_type* operator->() const { return m_ptr; }

        private:
            mutable pointer m_ptr;
        };

        typedef detail::vector_allocator_holder<AllocatorT> base_t;

    public:
        typedef typename base_t::allocator_type     allocator_type;
        typedef typename base_t::value_type         value_type;
        typedef typename base_t::size_type          size_type;
        typedef typename base_t::difference_type    difference_type;
        typedef typename base_t::reference          reference;
        typedef typename base_t::const_reference    const_reference;
        typedef typename base_t::pointer            pointer;
        typedef typename base_t::const_pointer      const_pointer;
        typedef vector_iterator                     iterator;
        typedef const_vector_iterator               const_iterator;

        vector() /*: m_capacity(0), m_size(0), m_data(0)*/ {}
        vector(const vector& rhs);
        vector& operator=(const vector& rhs);
//        ~vector(); // moved work to base class vector_allocator_impl

#if __cplusplus >= 201103L
        vector(vector&& rhs);
        vector& operator=(vector&& rhs);
#endif // __cplusplus >= 201103L

        template<class InputIt> vector(InputIt first, InputIt last, const allocator_type& alloc = allocator_type());
        explicit vector(size_type count, const T& t = T(), const allocator_type& alloc = allocator_type());

        reference operator[](size_type n) { return this->m_member.m_data[n]; }
        const_reference operator[](size_type n) const { return this->m_member.m_data[n]; }
        reference front() { return this->m_member.m_data[0]; }
        const_reference front() const { return this->m_member.m_data[0]; }
        reference back() { return this->m_member.m_data[this->m_member.m_size - 1]; }
        const_reference back() const { return this->m_member.m_data[this->m_member.m_size - 1]; }

        iterator begin() { return iterator(this->m_member.m_data); }
        const_iterator begin() const { return const_iterator(this->m_member.m_data); }
        const_iterator cbegin() const { return const_iterator(this->m_member.m_data); }
        iterator end() { return iterator(this->m_member.m_data + this->m_member.m_size); }
        const_iterator end() const { return const_iterator(this->m_member.m_data + this->m_member.m_size); }
        const_iterator cend() const { return const_iterator(this->m_member.m_data + this->m_member.m_size); }

        bool empty() const noe_std_no_except { return (size() > 0); }
        size_type size() const noe_std_no_except { return this->m_member.m_size; }
        size_type max_size() const noe_std_no_except { return this->allocator().max_size(); }
        void reserve(size_type n);
        size_type capacity() const noe_std_no_except { return this->m_member.m_capacity; }
        void shrink_to_fit();

        void clear();
//        iterator insert(const_iterator pos, const_reference v);
//#if __cplusplus >= 201103L
//        iterator insert(const_iterator pos, value_type&& v);
//#endif // __cplusplus >= 201103L
//        iterator insert(const_iterator pos, size_type count, const_reference v);
//        template<class InputIt> iterator insert(const_iterator pos, InputIt first, InputIt last);
//#if __cplusplus >= 201103L
//        iterator insert(const_iterator pos, std::initializer_list<T> init_list);
//        template<class... Args> void emplace(iterator pos, Args&&... args);
//#endif // __cplusplus >= 201103L
        void erase(iterator it);
        bool push_back(const_reference v);
#if __cplusplus >= 201103L
        bool push_back(value_type&& v);
        template<class... Args> bool emplace_back(Args&&... args);
        template<class... Args> std::pair<bool, iterator> emplace_back(Args&&... args);
#endif // __cplusplus >= 201103L
        void pop_back();
        void resize(size_type n);
        void resize(size_type n, const_reference v);
        void swap(vector& other) noe_std_no_except;

    private:
        enum : size_type { DEFAULT_NEW_SIZE = 10 };

        template<class U> friend bool operator==(const vector<U>& lhs, const vector<U>& rhs) noe_std_no_except;
        template<class U> friend bool operator!=(const vector<U>& lhs, const vector<U>& rhs) noe_std_no_except;
        template<class U> friend bool operator<(const vector<U>& lhs, const vector<U>& rhs) noe_std_no_except;
        template<class U> friend bool operator<=(const vector<U>& lhs, const vector<U>& rhs) noe_std_no_except;
        template<class U> friend bool operator>(const vector<U>& lhs, const vector<U>& rhs) noe_std_no_except;
        template<class U> friend bool operator>=(const vector<U>& lhs, const vector<U>& rhs) noe_std_no_except;
        template<class U> friend void std::swap(noe_std::vector<U>& v1, noe_std::vector<U>& v2) noe_std_no_except;

        bool check_capacity();
        bool grow();
        bool grow(size_type new_capacity);
        void clear_data(pointer data, size_type size);
    };

    template<class T, class AllocatorT>
    vector<T, AllocatorT>::vector(const vector& rhs) :
        base_t(AllocatorT(), rhs.m_member.m_size)
    {
//        if(this->m_member.m_data) // allocation success / rhs has data?
//            std::copy(rhs.m_member.m_data, rhs.m_member.m_data + rhs.m_member.m_size, this->m_member.m_data);
//        if(rhs.m_member.m_data) { // capacity check already does the check
            pointer data = this->m_member.m_data;
            for(const_iterator it = rhs.cbegin(), it_end = rhs.cend(); it != it_end; ++it) {
                this->allocator().construct((data + this->m_member.m_size), *(it)); // invoke copy constructor
                ++this->m_member.m_size; // increment size here for exception safety
            }
//        }
    }

    template<class T, class AllocatorT>
    vector<T, AllocatorT>& vector<T, AllocatorT>::operator=(const vector& rhs)
    {
        vector other(rhs);
        swap(other);
        return *this;
    }

//    template<class T, class Allocator>
//    vector<T, Allocator>::~vector()
//    {
//        clear();
//        this->allocator().deallocate(this->m_member.m_data, this->m_member.m_capacity);
//    }
#if __cplusplus >= 201103L
    template<class T, class AllocatorT>
    vector<T, AllocatorT>::vector(vector&& rhs) :
        base_t(std::move(rhs))
    {
        rhs.m_member.m_capacity = 0;
        rhs.m_member.m_size = 0;
        rhs.m_member.m_data = 0; // transfer ownership
    }

    template<class T, class AllocatorT>
    vector<T, AllocatorT>& vector<T, AllocatorT>::operator=(vector&& rhs)
    {
//        if(this != &rhs) {
//            // clear old
//            clear();
//            this->allocator().deallocate(this->m_member.m_data, this->m_member.m_capacity);

//            // move new
//            base_t::operator=(std::move(rhs));
//            rhs.m_member.m_capacity = 0;
//            rhs.m_member.m_size = 0;
//            rhs.m_member.m_data = 0; // transfer ownership
//        }
        swap(rhs);
        return *this;
    }
#endif // __cplusplus >= 201103L
    template<class T, class AllocatorT>
    template<class InputIt>
    vector<T, AllocatorT>::vector(InputIt first, InputIt last, const allocator_type& alloc) :
        base_t(alloc, std::distance(first, last))
    {
        if(this->m_member.m_data) { // allocation success / there is data to be copied?
//            std::copy(first, last, this->m_member.m_data);
            pointer data = this->m_member.m_data;
            for(InputIt it = first; it != last; ++it) {
                this->allocator().construct((data + this->m_member.m_size), (*it)); // invoke copy constructor
                ++this->m_member.m_size; // increment size here for exception safety
            }
        }
    }

    template<class T, class AllocatorT>
    vector<T, AllocatorT>::vector(size_type count, const T& t, const allocator_type& alloc) :
        base_t(alloc, count)
    {
        if(this->m_member.m_data) { // allocation success / count > 0?
            pointer data = this->m_member.m_data;
//            for(size_type i = 0; i < count; ++i) {
            while(this->m_member.m_size < count) {
                this->allocator().construct((data + this->m_member.m_size), t);
                ++this->m_member.m_size;
            }
        }
    }

    template<class T, class AllocatorT>
    void vector<T, AllocatorT>::reserve(size_type n)
    {
        if(n > this->m_member.m_capacity) {
            grow(n);
        }
    }

    template<class T, class AllocatorT>
    void vector<T, AllocatorT>::shrink_to_fit()
    {
//        size_type size = this->m_member.m_size;
//        size_type capacity = this->m_member.m_capacity;

//        if(size < capacity) {
//            if(size > 0)
//                grow(size);
//            else { // size == 0
//                this->allocator().deallocate(this->m_member.m_data, capacity);
//                this->m_member.m_capacity = 0;
//                this->m_member.m_data = 0;
//            }
//        }
        vector<T> temp(*this);
        swap(temp);
    }

    template<class T, class AllocatorT>
    void vector<T, AllocatorT>::clear()
    {
        clear_data(this->m_member.m_data, this->m_member.m_size);
        this->m_member.m_size = 0;
    }

    template<class T, class AllocatorT>
    void vector<T, AllocatorT>::erase(iterator it)
    {
        // if(primite_type<T>::value)
        //  move others
        //  --m_size;
        // else
        //  move others
        //  call type destructor
        //  --m_size

        pointer pte = &(*it);
        difference_type diff = (this->m_member.m_data + this->m_member.m_size) - pte;
        if(diff > 1) {
            std::copy(pte + 1, pte + diff, pte);
            this->allocator().destroy(pte + (diff - 1));
        } else {
            this->allocator().destroy(pte);
        }
        this->m_member.m_size;
    }

    template<class T, class AllocatorT>
    bool vector<T, AllocatorT>::push_back(const_reference v)
    {
        if(!check_capacity())
            return false;
        this->allocator().construct((this->m_member.m_data + this->m_member.m_size), v);
        ++this->m_member.m_size;

        return true;
    }
#if __cplusplus >= 201103L
    template<class T, class AllocatorT>
    bool vector<T, AllocatorT>::push_back(value_type&& v)
    {
        if(!check_capacity())
            return false;
        this->allocator().construct((this->m_member.m_data + this->m_member.m_size), std::move(v));
        ++this->m_member.m_size;

        return true;
    }

    template<class T, class AllocatorT>
    template<class... Args>
    bool vector<T, AllocatorT>::emplace_back(Args&&... args)
    {
        if(!check_capacity())
            return false;
        // if construction fails, due to exception, m_size will be m_size + 1, which could be not strong
        // exception safe
//        this->allocator().construct((this->m_member.m_data + (this->m_member.m_size++)), std::forward<Args>(args)...);
        this->allocator().construct((this->m_member.m_data + this->m_member.m_size), std::forward<Args>(args)...);
        ++this->m_member.m_size; // if exception was thrown before this, (ie. constructor), size won't change
        return false;
    }

    template<class T, class AllocatorT>
    template<class... Args>
    std::pair<bool, typename vector<T, AllocatorT>::iterator> vector<T, AllocatorT>::emplace_back(Args&&... args)
    {
        if(!check_capacity())
            return std::make_pair(false, iterator(0));
        pointer pos = (this->m_member.m_data + this->m_member.m_size);
        this->allocator().construct(pos, std::forward<Args>(args)...);
        ++this->m_member.m_size; // if exception was thrown before this, size would not increase
        return std::make_pair(true, iterator(pos));
    }
#endif // __cplusplus >= 201103L
    template<class T, class AllocatorT>
    void vector<T, AllocatorT>::pop_back()
    {
        // destructor should not throw exception
        // so exception safe
        this->allocator().destroy(this->m_member.m_data + (--this->m_member.m_size));
    }

    template<class T, class AllocatorT>
    inline void vector<T, AllocatorT>::resize(size_type n)
    {
        resize(n, T());
    }

    template<class T, class AllocatorT>
    void vector<T, AllocatorT>::resize(size_type n, const_reference value)
    {
//        size_type capacity = this->m_member.m_capacity;
//        size_type size = this->m_member.m_size;

//        if(n > size) {
//            if(n > capacity) {
//                if(!grow(n))
//                    return;
//            }
//            size_type diff = n - size;
//            while(diff--)
//                this->allocator().construct((this->m_member.m_data + (this->m_member.m_size++)), value);
//        } else if(n < size) {
//            size_type diff = size - n;
//            while(diff--)
//                this->allocator().destroy((this->m_member.m_data + (this->m_member.m_size--)));
//        }
        size_type capacity = this->m_member.m_capacity;
        size_type size = this->m_member.m_size;

        if(n > size) {
            if(n > capacity) {
                if(!grow(n))
                    return;
            }
            size_type diff = n - size;
            while(diff--) {
                this->allocator().construct((this->m_member.m_data + this->m_member.m_size), value);
                ++this->m_member.m_size;
            }
        } else if(n < size) {
            size_type diff = size - n;
            while(diff--) {
                this->allocator().destroy((this->m_member.m_data + this->m_member.m_size));
                --this->m_member.m_size;
            }
        }
    }

    template<class T, class AllocatorT>
    void vector<T, AllocatorT>::swap(vector& other) noe_std_no_except
    {
        this->m_member.swap(other.m_member);
    }

    template<class T, class AllocatorT>
    inline bool vector<T, AllocatorT>::check_capacity()
    {
        if(this->m_member.m_size >= this->m_member.m_capacity)
            return grow();
        return true;
    }

    template<class T, class AllocatorT>
    inline bool vector<T, AllocatorT>::grow()
    {
        const size_type default_new_size = DEFAULT_NEW_SIZE;
        size_type new_capacity = std::max(this->m_member.m_capacity * 2, default_new_size);
        return grow(new_capacity);
    }

    template<class T, class AllocatorT>
    bool vector<T, AllocatorT>::grow(size_type new_capacity)
    {
        size_type size = this->m_member.m_size;
        pointer old_data = this->m_member.m_data;

        typename detail::vector_allocator_holder<AllocatorT>::vector_allocator_impl tmp(this->allocator(), new_capacity);
        if(!tmp.m_data)
            return false;

        if(size > 0) {
//            for(size_type i = 0; i < size; ++i) {
            while(tmp.m_size < size) {
                size_type tmp_size = tmp.m_size;
                this->allocator().construct((tmp.m_data + tmp_size), *(old_data + tmp_size));
                ++tmp.m_size;
            }
//            }
        }
        tmp.swap(this->m_member);
        return true;

//        size_type size = this->m_member.m_size;
//        pointer old_data = this->m_member.m_data;
//        this->m_member.m_data = this->allocator().allocate(new_capacity);
//        if(!this->m_member.m_data) {
//            this->m_member.m_data = old_data;
//            return false;
//        }
//        if(size > 0)
//            std::copy(old_data, old_data + size, this->m_member.m_data);
//        this->m_member.m_capacity = new_capacity;

//        clear_data(old_data, size);
//        this->allocator().deallocate(old_data, size);
//        return true;
    }

    template<class T, class AllocatorT>
    void vector<T, AllocatorT>::clear_data(pointer data, size_type size)
    {
        if(size == 0)
            return;

        // if(primite_type<T>::value)
        //  return;
        // else
        //  call type destructors
//        for(size_type i = 0; i < size - 1; ++i)
//            this->allocator().destroy(data + i);

        // destroy top most
        while(size)
            this->allocator().destroy(data + (--size));
    }

    template<class T>
    bool operator==(const vector<T>& lhs, const vector<T>& rhs) noe_std_no_except
    {
        if(lhs.size() != rhs.size())
            return false;

        typedef typename vector<T>::const_iterator const_iterator;
        for(const_iterator it = lhs.cbegin(), it_end = lhs.cend(), it2 = rhs.cbegin(); it != it_end; ++it, ++it2) { // it != it_end should be enough, since we checked if sizes don't match
            if((*it) != (*it2))
                return false;
        }
        return true;
    }

    template<class T>
    inline bool operator!=(const vector<T>& lhs, const vector<T>& rhs) noe_std_no_except
    {
        return !(lhs == rhs);
    }

    template<class T>
    inline bool operator<(const vector<T>& lhs, const vector<T>& rhs) noe_std_no_except
    {
        return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), std::less<T>());
    }

    template<class T>
    inline bool operator<=(const vector<T>& lhs, const vector<T>& rhs) noe_std_no_except
    {
        return (lhs < rhs || lhs == rhs);
    }

    template<class T>
    inline bool operator>(const vector<T>& lhs, const vector<T>& rhs) noe_std_no_except
    {
        return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), std::greater<T>());
    }

    template<class T>
    inline bool operator>=(const vector<T>& lhs, const vector<T>& rhs) noe_std_no_except
    {
        return (lhs > rhs || lhs == rhs);
    }
}
namespace std
{
    template<class T>
    inline void swap(noe_std::vector<T>& v1, noe_std::vector<T>& v2) noe_std_no_except
    {
        v1.swap(v2);
    }
}

#endif // GUARD_NOE_STD_vector_H
