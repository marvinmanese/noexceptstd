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
#ifndef GUARD_NOE_STD_detail_list_base_H
#define GUARD_NOE_STD_detail_list_base_H

#include "list_iterator.h"
#include "list_node.h"
#include "macro.h"
#include "temporary_pointer.h"

namespace noe_std
{
namespace detail
{
    template<class Allocator, class T, class NodeType> void list_inserter_root(Allocator& alloc, const T& t, node<std::forward_iterator_tag, T>** tail);
    template<class Allocator, class T> void list_inserter_next(Allocator& alloc, const T& t, node<std::forward_iterator_tag, T>* tail);
    template<class Allocator, class T> void list_inserter_next(Allocator& alloc, const T& t, node<std::bidirectional_iterator_tag, T>* tail);

    template<class Allocator, class IteratorCategory>
    struct list_base_allocator_holder
    {
        typedef Allocator                                   allocator_type;
#if __cplusplus >= 201103L
        typedef std::allocator_traits<allocator_type>     alloc_traits_t;
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
        typedef node<IteratorCategory, value_type>          node_t;
        typedef typename allocator_type::template rebind<node_t>::other
                                                            node_allocator_t;

        list_base_allocator_holder() {}
        list_base_allocator_holder(const node_allocator_t& alloc) : m_member(alloc) {}
#if __cplusplus >= 201103L
        list_base_allocator_holder(node_allocator_t&& alloc) : m_member(std::move(alloc)) {}
#endif // __cplusplus >= 201103L

        struct list_base_allocator_impl : public node_allocator_t
        {
            list_base_allocator_impl() : m_size(0), m_begin(0), m_end(0) {}
            list_base_allocator_impl(const node_allocator_t& alloc) : node_allocator_t(alloc), m_size(0), m_begin(0), m_end(0) {}
#if __cplusplus >= 201103L
            list_base_allocator_impl(node_allocator_t&& alloc) : node_allocator_t(std::move(alloc)), m_size(0), m_begin(0), m_end(0) {}
#endif // __cplusplus >= 201103L

            ~list_base_allocator_impl()
            {
                clear();
            }

            void swap(list_base_allocator_impl& other)
            {
                std::swap(m_size, other.m_size);
                std::swap(m_begin, other.m_begin);
                std::swap(m_end, other.m_end);
            }

            void clear()
            {
                typename node_t::pointer curr = m_begin;
                while(curr) {
                    typename node_t::pointer t = curr;
                    curr = curr->next;

                    node_allocator_t::destroy(t);
                    node_allocator_t::deallocate(t, 0);
                }

                m_begin = m_end = 0;
                m_size = 0;
            }

            size_type                   m_size;
            typename node_t::pointer    m_begin;
            typename node_t::pointer    m_end;
        } m_member;

        node_allocator_t& allocator() { return m_member; }
        const node_allocator_t& allocator() const { return m_member; }
    };

    template<class T,
             class IteratorCategory,
             class Allocator,
             class ConnectorPolicy>
    class list_base : protected list_base_allocator_holder<Allocator, IteratorCategory>
    {
    private:
        typedef list_base_allocator_holder<Allocator, IteratorCategory> base_t;

    public:
//        typedef T                                           value_type;
//        typedef Allocator                                   allocator_type;
//        typedef std::size_t                               size_type;
//        typedef std::ptrdiff_t                            difference_type;
//        typedef value_type&                                 reference;
//        typedef const reference                             const_reference;
//        typedef typename Allocator::pointer                 pointer;
//        typedef typename Allocator::const_pointer           const_pointer;
        using typename base_t::allocator_type;
        using typename base_t::value_type;
        using typename base_t::size_type;
        using typename base_t::difference_type;
        using typename base_t::reference;
        using typename base_t::const_reference;
        using typename base_t::pointer;
        using typename base_t::const_pointer;
        typedef list_iterator<IteratorCategory, T> iterator;
        typedef const_list_iterator<IteratorCategory, T> const_iterator;

    protected:
//        typedef node<IteratorCategory, T>                               node_t;
//        typedef typename allocator_type::template rebind<node_t>::other node_allocator_t;
        using typename base_t::node_t;
        using typename base_t::node_allocator_t;

    protected:
        list_base() /*: m_size(0), m_begin(0), m_end(0)*/ {}
        list_base(const list_base& rhs);
        list_base& operator=(const list_base& rhs);
        ~list_base();

#if __cplusplus >= 201103L
        list_base(list_base&& rhs);
        list_base& operator=(list_base&& rhs);
#endif // __cplusplus >= 201103L

    public:
        iterator insert(iterator pos, const T& v);
        bool insert(iterator pos, size_type count, const T& v);
        template<class InputIt> bool insert(iterator pos, InputIt first, InputIt last);

#if __cplusplus >= 201103L
        void erase(iterator it);
        void erase(iterator first, iterator last);
#endif // __cplusplus >= 201103L
        bool push_back(const T& t);
#if __cplusplus >= 201103L
        bool push_back(T&& t);
        template<class... Args> bool emplace_back(Args&&... args);
#endif // __cplusplus >= 201103L
        bool push_front(const T& t);
#if __cplusplus >= 201103L
        bool push_front(T&& t);
        template<class... Args> bool emplace_front(Args&&... args);
#endif // __cplusplus >= 201103L

        reference operator[](size_type n);
        const_reference operator[](size_type n) const;
        reference front() { return base_t::m_member.m_begin->value; }
        const_reference front() const { return base_t::m_member.m_begin->value; }
        reference back() { return base_t::m_member.m_end->value; }
        const_reference back() const { return base_t::m_member.m_end->value; }

        iterator begin() noe_std_no_except { return iterator(base_t::m_member.m_begin); }
        const_iterator begin() const noe_std_no_except { return const_iterator(base_t::m_member.m_begin); }
        iterator end() noe_std_no_except { return iterator(0); }
        const_iterator end() const noe_std_no_except { return const_iterator(0); }

        bool empty() const noe_std_no_except { return (size() > 0); }
        size_type size() const noe_std_no_except { return base_t::m_member.m_size; }
        size_type max_size() const noe_std_no_except;

        void clear();
        void swap(list_base& other);

//    protected:
//        size_type                    m_size;
//        typename node_t::pointer     m_begin;
//        typename node_t::pointer     m_end;
    };

    template<class Allocator, class T, class NodeType>
    void list_inserter_root(Allocator& alloc, const T& t, NodeType** tail)
    {
        temporary_pointer<NodeType> temp(alloc.allocate(1));
        alloc.construct(temp.get(), t);

//        *tail = alloc.allocate(1);
//        alloc.construct(*tail, t);
        *tail = temp.release();
    }

    template<class Allocator, class T> void list_inserter_next(Allocator& alloc, const T& t, node<std::forward_iterator_tag, T>* tail)
    {
        temporary_pointer<node<std::forward_iterator_tag, T>> temp(alloc.allocate(1));
        alloc.construct(temp.get(), t);

//        tail->next = alloc.allocate(1);
//        alloc.construct(tail->next, t);
        tail->next = temp.release();
    }

    template<class Allocator, class T> void list_inserter_next(Allocator& alloc, const T& t, node<std::bidirectional_iterator_tag, T>* tail)
    {
        temporary_pointer<node<std::bidirectional_iterator_tag, T>> temp(alloc.allocate(1));
        alloc.construct(temp.get(), t);

//        tail->next = alloc.allocate(1);
//        alloc.construct(tail->next, t);
//        tail->next->prev = tail;
        tail->next = temp.release();
        tail->next->prev = tail;
    }

    template<class T, class IteratorCategory, class Allocator, class ConnectorPolicy>
    list_base<T, IteratorCategory, Allocator, ConnectorPolicy>::list_base(const list_base& rhs) :
        base_t(node_allocator_t())
//        m_size(rhs.m_size),
//        m_begin(0),
//        m_end(0)
    {
        const_iterator it = rhs.begin(), it_end = rhs.end();
        if(it != it_end) {
            list_inserter_root(base_t::allocator(), *it, &base_t::m_member.m_begin);
            std::advance(it, 1);
            base_t::m_member.m_end = base_t::m_member.m_begin;
            while(it != it_end) {
                list_inserter_next(base_t::allocator(), *it, base_t::m_member.m_end);
                std::advance(it, 1);
                base_t::m_member.m_end = base_t::m_member.m_end->next;
            }
        }
    }

    template<class T, class IteratorCategory, class Allocator, class ConnectorPolicy>
    list_base<T, IteratorCategory, Allocator, ConnectorPolicy>& list_base<T, IteratorCategory, Allocator, ConnectorPolicy>::operator=(const list_base& rhs)
    {
//        if(this != &rhs) {
            list_base list_(rhs);
            swap(list_);
//        }

        return *this;
    }

    template<class T, class IteratorCategory, class Allocator, class ConnectorPolicy>
    list_base<T, IteratorCategory, Allocator, ConnectorPolicy>::~list_base()
    {
        clear();
    }

    template<class T, class IteratorCategory, class Allocator, class ConnectorPolicy>
    typename list_base<T, IteratorCategory, Allocator, ConnectorPolicy>::iterator list_base<T, IteratorCategory, Allocator, ConnectorPolicy>::insert(iterator pos, const T& v)
    {
        node_allocator_t& alloc = this->allocator();
        typename node_t::pointer pos_ptr = pos.node();
        typename node_t::pointer pos_ptr_next = pos_ptr->next;

        detail::temporary_pointer<node_t> new_node(alloc.allocate(1));
        typename node_t::pointer new_node_ptr = new_node.get();
        if(!new_node_ptr)
            return this->end();
        alloc.construct(new_node_ptr, v);

        new_node_ptr = new_node.release();
        ConnectorPolicy::template connect<typename node_t::pointer>(pos_ptr, new_node_ptr);
        ConnectorPolicy::template connect<typename node_t::pointer>(new_node_ptr, pos_ptr_next);
        ++this->m_member.m_size;

        return iterator(new_node_ptr);
    }

    template<class T, class IteratorCategory, class Allocator, class ConnectorPolicy>
    bool list_base<T, IteratorCategory, Allocator, ConnectorPolicy>::insert(iterator pos, size_type count, const T& v)
    {
        node_allocator_t& alloc = this->allocator();
        typename node_t::pointer pos_ptr = pos.node();
        typename node_t::pointer pos_ptr_next = pos_ptr->next;

        while(count--) {
            detail::temporary_pointer<node_t> new_node(alloc.allocate(1));
            typename node_t::pointer new_node_ptr = new_node.get();
            if(!new_node_ptr)
                return false;
            alloc.construct(new_node_ptr, v);

            new_node_ptr = new_node.release();
            ConnectorPolicy::template connect<typename node_t::pointer>(pos_ptr, new_node_ptr);
            ConnectorPolicy::template connect<typename node_t::pointer>(new_node_ptr, pos_ptr_next);
            ++this->m_member.m_size;
        }

        return true;
    }

    template<class T, class IteratorCategory, class Allocator, class ConnectorPolicy>
    template<class InputIt>
    bool list_base<T, IteratorCategory, Allocator, ConnectorPolicy>::insert(iterator pos, InputIt first, InputIt last)
    {
        node_allocator_t& alloc = this->allocator();
        typename node_t::pointer pos_ptr = pos.node();
        typename node_t::pointer pos_ptr_next = pos_ptr->next;

        while(first != last) {
            detail::temporary_pointer<node_t> new_node(alloc.allocate(1));
            typename node_t::pointer new_node_ptr = new_node.get();
            if(!new_node_ptr)
                return false;
            alloc.construct(new_node_ptr, *first++);

            new_node_ptr = new_node.release();
            ConnectorPolicy::template connect<typename node_t::pointer>(pos_ptr, new_node_ptr);
            ConnectorPolicy::template connect<typename node_t::pointer>(new_node_ptr, pos_ptr_next);
            ++this->m_member.m_size;
        }

        return true;
    }
#if __cplusplus >= 201103L
    template<class T, class IteratorCategory, class Allocator, class ConnectorPolicy>
    void list_base<T, IteratorCategory, Allocator, ConnectorPolicy>::erase(iterator it)
    {
        typename node_t::pointer node = it.node(); // get iterator node
        if(node) {
            typename node_t::pointer prev = ConnectorPolicy::template get_prev_node<typename node_t::pointer>(base_t::m_member.m_begin, node);
            ConnectorPolicy::template disconnect<typename node_t::pointer>(prev, node, node->next);
            ConnectorPolicy::template disconnect_ends<typename node_t::pointer>(&base_t::m_member.m_begin, &base_t::m_member.m_end, node);
//            if(base_t::m_member.m_begin == node) {
//                base_t::m_member.m_begin = node->next;
//            }
//            if(base_t::m_member.m_end == node) {
//                base_t::m_member.m_end = node->prev;
//            }
            base_t::allocator().destroy(node);
            base_t::allocator().deallocate(node, 1);
            --base_t::m_member.m_size;
        }
    }

    template<class T, class IteratorCategory, class Allocator, class ConnectorPolicy>
    void list_base<T, IteratorCategory, Allocator, ConnectorPolicy>::erase(iterator first, iterator last)
    {
        typename node_t::pointer node_first = first.node();
        typename node_t::pointer node_last = last.node();
        typename node_t::pointer node_first_prev = ConnectorPolicy::template get_prev_node<typename node_t::pointer>(this->m_member.m_begin, node_first);
        typename node_t::pointer node_last_next = node_last ? node_last->next : node_last;

        // set the begin and end pointers
        if(!node_first_prev) {
            this->m_member.m_begin = node_last_next;
        } else {
            this->m_member.m_begin = node_first_prev;
        }
        if(!node_last_next) {
            this->m_member.m_end = node_first_prev;
        } else {
            this->m_member.m_end = node_last_next;
        }

        // connect the prev to the next
        ConnectorPolicy::template connect<typename node_t::pointer>(node_first_prev, node_last_next);

        // deallocate from first to last
        if(node_last)
            node_last->next = 0;
        typename node_t::pointer curr = node_first;
        node_allocator_t& alloc = this->allocator();
        while(curr) {
            typename node_t::pointer temp = curr;

            alloc.destroy(temp);
            alloc.deallocate(temp, 1);
            curr = curr->next;
            --this->m_member.m_size;
        }
//        }
    }
#endif // __cplusplus >= 201103L
    template<class T, class IteratorCategory, class Allocator, class ConnectorPolicy>
    bool list_base<T, IteratorCategory, Allocator, ConnectorPolicy>::push_back(const T& t)
    {
        node_allocator_t& alloc = base_t::allocator();

        detail::temporary_pointer<node_t> new_node(alloc.allocate(1));
        typename node_t::pointer new_node_ptr = new_node.get();
        if(!new_node_ptr)
            return false;
        alloc.construct(new_node_ptr, t);

        // if at this point, would no longer throw exception
        new_node_ptr = new_node.release();
        if(!base_t::m_member.m_begin)
            base_t::m_member.m_begin = base_t::m_member.m_end = new_node_ptr;
        else {
            ConnectorPolicy::template connect<typename node_t::pointer>(base_t::m_member.m_end, new_node_ptr);
            base_t::m_member.m_end = new_node_ptr;
        }
        ++base_t::m_member.m_size;

        return true;
    }
#if __cplusplus >= 201103L
    template<class T, class IteratorCategory, class Allocator, class ConnectorPolicy>
    bool list_base<T, IteratorCategory, Allocator, ConnectorPolicy>::push_back(T&& t)
    {
//        node_allocator_t& alloc = base_t::allocator();

//        typename node_t::pointer new_node = alloc.allocate(1);
//        if(!new_node)
//            return false;
//        alloc.construct(new_node, std::move(t));
//        if(!base_t::m_member.m_begin) {
//            base_t::m_member.m_begin = base_t::m_member.m_end = new_node;
//        } else {
//            connect(base_t::m_member.m_end, new_node);
//            base_t::m_member.m_end = new_node;
//        }
//        ++base_t::m_member.m_size;
//        return true;
        node_allocator_t& alloc = base_t::allocator();

        detail::temporary_pointer<node_t> new_node(alloc.allocate(1));
        typename node_t::pointer new_node_ptr = new_node.get();
        if(!new_node_ptr)
            return false;
        alloc.construct(new_node_ptr, std::move(t));

        // if at this point, would no longer throw exception
        new_node_ptr = new_node.release();
        if(!base_t::m_member.m_begin)
            base_t::m_member.m_begin = base_t::m_member.m_end = new_node_ptr;
        else {
            ConnectorPolicy::template connect<typename node_t::pointer>(base_t::m_member.m_end, new_node_ptr);
            base_t::m_member.m_end = new_node_ptr;
        }
        ++base_t::m_member.m_size;

        return true;
    }

    template<class T, class IteratorCategory, class Allocator, class ConnectorPolicy>
    template<class... Args>
    bool list_base<T, IteratorCategory, Allocator, ConnectorPolicy>::emplace_back(Args&&... args)
    {
//        node_allocator_t& alloc = base_t::allocator();

//        typename node_t::pointer new_node = alloc.allocate(1);
//        if(!new_node)
//            return false;
//        alloc.construct(new_node, std::forward<Args>(args)...);
//        if(!base_t::m_member.m_begin) {
//            base_t::m_member.m_begin = base_t::m_member.m_end = new_node;
//        } else {
//            connect(base_t::m_member.m_end, new_node);
//            base_t::m_member.m_end = new_node;
//        }
//        ++base_t::m_member.m_size;
//        return true;
        node_allocator_t& alloc = base_t::allocator();

        detail::temporary_pointer<node_t> new_node(alloc.allocate(1));
        typename node_t::pointer new_node_ptr = new_node.get();
        if(!new_node_ptr)
            return false;
        alloc.construct(new_node_ptr, std::forward<Args>(args)...);

        // if at this point, would no longer throw exception
        new_node_ptr = new_node.release();
        if(!base_t::m_member.m_begin)
            base_t::m_member.m_begin = base_t::m_member.m_end = new_node_ptr;
        else {
            ConnectorPolicy::template connect<typename node_t::pointer>(base_t::m_member.m_end, new_node_ptr);
            base_t::m_member.m_end = new_node_ptr;
        }
        ++base_t::m_member.m_size;

        return true;
    }
#endif // __cplusplus >= 201103L
    template<class T, class IteratorCategory, class Allocator, class ConnectorPolicy>
    bool list_base<T, IteratorCategory, Allocator, ConnectorPolicy>::push_front(const T& t)
    {
        node_allocator_t& alloc = base_t::alocator();

        detail::temporary_pointer<node_t> new_node(alloc.allocate(1));
        typename node_t::pointer new_node_ptr = new_node.get();
        if(!new_node_ptr)
            return false;
        alloc.construct(new_node_ptr, t);

        new_node_ptr = new_node.release();
        if(!base_t::m_member.m_begin)
            base_t::m_member.m_begin = base_t::m_member.m_end = new_node_ptr;
        else {
            ConnectorPolicy::template connect<typename node_t::pointer>(new_node_ptr, base_t::m_member.m_begin);
            base_t::m_member.m_begin = new_node_ptr;
        }
        ++base_t::m_member.m_size;

        return true;
    }
#if __cplusplus >= 201103L
    template<class T, class IteratorCategory, class Allocator, class ConnectorPolicy>
    bool list_base<T, IteratorCategory, Allocator, ConnectorPolicy>::push_front(T&& t)
    {
        node_allocator_t& alloc = base_t::alocator();

        detail::temporary_pointer<node_t> new_node(alloc.allocate(1));
        typename node_t::pointer new_node_ptr = new_node.get();
        if(!new_node_ptr)
            return false;
        alloc.construct(new_node_ptr, std::move(t));

        new_node_ptr = new_node.release();
        if(!base_t::m_member.m_begin)
            base_t::m_member.m_begin = base_t::m_member.m_end = new_node_ptr;
        else {
            ConnectorPolicy::template connect<typename node_t::pointer>(new_node_ptr, base_t::m_member.m_begin);
            base_t::m_member.m_begin = new_node_ptr;
        }
        ++base_t::m_member.m_size;

        return true;
    }

    template<class T, class IteratorCategory, class Allocator, class ConnectorPolicy>
    template<class... Args>
    bool list_base<T, IteratorCategory, Allocator, ConnectorPolicy>::emplace_front(Args&&... args)
    {
        node_allocator_t& alloc = base_t::alocator();

        detail::temporary_pointer<node_t> new_node(alloc.allocate(1));
        typename node_t::pointer new_node_ptr = new_node.get();
        if(!new_node_ptr)
            return false;
        alloc.construct(new_node_ptr, std::forward<Args>(args)...);

        new_node_ptr = new_node.release();
        if(!base_t::m_member.m_begin)
            base_t::m_member.m_begin = base_t::m_member.m_end = new_node_ptr;
        else {
            ConnectorPolicy::template connect<typename node_t::pointer>(new_node_ptr, base_t::m_member.m_begin);
            base_t::m_member.m_begin = new_node_ptr;
        }
        ++base_t::m_member.m_size;

        return true;
    }
#endif // __cplusplus >= 201103L
    template<class T, class IteratorCategory, class Allocator, class ConnectorPolicy>
    typename list_base<T, IteratorCategory, Allocator, ConnectorPolicy>::reference list_base<T, IteratorCategory, Allocator, ConnectorPolicy>::operator[](size_type n)
    {
        iterator it(base_t::m_member.m_begin);
        while(n--) {
            std::advance(it, 1);
//            ++it;
        }
        return (*it);
    }

    template<class T, class IteratorCategory, class Allocator, class ConnectorPolicy>
    typename list_base<T, IteratorCategory, Allocator, ConnectorPolicy>::const_reference list_base<T, IteratorCategory, Allocator, ConnectorPolicy>::operator[](size_type n) const
    {
        const_iterator it(base_t::m_member.m_begin);
        while(n--) {
            std::advance(it, 1);
//            ++it;
        }
        return (*it);
    }

//    template<class T, class IteratorCategory, class Allocator, class ConnectorPolicy>
//    typename list_base<T, IteratorCategory, Allocator, ConnectorPolicy>::iterator list_base<T, IteratorCategory, Allocator, ConnectorPolicy>::end()
//    {
//        return iterator(0);
//    }

//    template<class T, class IteratorCategory, class Allocator, class ConnectorPolicy>
//    typename list_base<T, IteratorCategory, Allocator, ConnectorPolicy>::const_iterator list_base<T, IteratorCategory, Allocator, ConnectorPolicy>::end() const
//    {
//        return const_iterator(0);
//    }

    template<class T, class IteratorCategory, class Allocator, class ConnectorPolicy>
    typename list_base<T, IteratorCategory, Allocator, ConnectorPolicy>::size_type list_base<T, IteratorCategory, Allocator, ConnectorPolicy>::max_size() const noe_std_no_except
    {
        return base_t::allocator().max_size();
    }

    template<class T, class IteratorCategory, class Allocator, class ConnectorPolicy>
    void list_base<T, IteratorCategory, Allocator, ConnectorPolicy>::clear()
    {
//        typename node_t::pointer curr = base_t::m_member.m_begin;
//        while(curr) {
//            typename node_t::pointer t = curr;
//            curr = curr->next;
//            base_t::allocator().destroy(t);
//            base_t::allocator().deallocate(t, 0);
//        }
//        base_t::m_member.m_begin = base_t::m_member.m_end = 0;
//        base_t::m_member.m_size = 0;
        base_t::m_member.clear();
    }

    template<class T, class IteratorCategory, class Allocator, class ConnectorPolicy>
    inline void list_base<T, IteratorCategory, Allocator, ConnectorPolicy>::swap(list_base& other)
    {
        base_t::m_member.swap(other.m_member);
    }
}
}

#endif // GUARD_NOE_STD_detail_list_base_H
