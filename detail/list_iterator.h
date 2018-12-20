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
#ifndef GUARD_NOE_STD_list_iterator_H
#define GUARD_NOE_STD_list_iterator_H

#include <iterator>
#include "list_node.h"

// TODO: fix iterator problem
// TODO: make iterator use pointer to pointer
namespace noe_std
{
    template<class T, class Allocator> class list; // forward declare list
namespace detail
{
    template<class T, class IteratorCategory, class Allocator, class ConnectorPolicy> class list_base; // forward declare list base

    template<class IteratorCategory, class T, bool isConstant>
    class list_iterator_base;

    template<class IteratorCategory, class T>
    class list_iterator_base<IteratorCategory, T, false> : public std::iterator<IteratorCategory, T>
    {
    protected:
        typedef std::iterator<IteratorCategory, T>        base_iterator;
        typedef typename node<IteratorCategory, T>::pointer node_pointer;

//        list_iterator_base() : m_node(0) {}
        list_iterator_base(node_pointer ptr = 0) : m_node(ptr) {}

        node_pointer m_node;
    };

    template<class IteratorCategory, class T>
    class list_iterator_base<IteratorCategory, T, true> : public std::iterator<IteratorCategory, T>
    {
    protected:
        typedef std::iterator<IteratorCategory, T>        base_iterator;
        typedef typename node<IteratorCategory, T>::pointer node_pointer;

//        list_iterator_base() : m_node(0) {}
        list_iterator_base(node_pointer ptr = 0) : m_node(ptr) {}

        mutable node_pointer m_node;
    };

    template<class IteratorCategory, class T> class list_iterator;
    template<class IteratorCategory, class T> class const_list_iterator;

    template<class T>
    class list_iterator<std::forward_iterator_tag, T> : private list_iterator_base<std::forward_iterator_tag, T, false>
    {
    private:
        typedef list_iterator_base<std::forward_iterator_tag, T, false> list_iterator_base_t;
        using typename list_iterator_base_t::node_pointer;
        using typename list_iterator_base_t::base_iterator;

    public:
        using typename base_iterator::value_type;
        using typename base_iterator::difference_type;
        using typename base_iterator::pointer;
        using typename base_iterator::reference;
        using typename base_iterator::iterator_category;

        list_iterator() {}
        explicit list_iterator(node_pointer ptr = 0) : list_iterator_base_t(ptr) {}

        list_iterator& operator++() { list_iterator_base_t::m_node = list_iterator_base_t::m_node->next; return *this; }
        list_iterator operator++(int) { list_iterator old = *this; list_iterator_base_t::m_node = list_iterator_base_t::m_node->next; return old; }

        bool operator==(const list_iterator& rhs) const { return list_iterator_base_t::m_node == rhs.m_node; }
        bool operator!=(const list_iterator& rhs) const { return list_iterator_base_t::m_node != rhs.m_node; }
        value_type& operator*() { return list_iterator_base_t::m_node->value; }
        value_type* operator->() { return &list_iterator_base_t::m_node->value; }

    private:
        node_pointer node() { return list_iterator_base_t::m_node; } // only forward_list have access to this
    };

    template<class T>
    class list_iterator<std::bidirectional_iterator_tag, T> : private list_iterator_base<std::bidirectional_iterator_tag, T, false>
    {
    private:
        typedef list_iterator_base<std::bidirectional_iterator_tag, T, false> list_iterator_base_t;
        using typename list_iterator_base_t::node_pointer;
        using typename list_iterator_base_t::base_iterator;

    public:
        using typename base_iterator::value_type;
        using typename base_iterator::difference_type;
        using typename base_iterator::pointer;
        using typename base_iterator::reference;
        using typename base_iterator::iterator_category;

        list_iterator() {}
        explicit list_iterator(node_pointer ptr) : list_iterator_base_t(ptr) {}

        list_iterator& operator++() { list_iterator_base_t::m_node = list_iterator_base_t::m_node->next; return *this; }
        list_iterator operator++(int) { list_iterator old = *this; list_iterator_base_t::m_node = list_iterator_base_t::m_node->next; return old; }
        list_iterator& operator--() { list_iterator_base_t::m_node = list_iterator_base_t::m_node->prev; return *this; }
        list_iterator operator--(int) { list_iterator old = *this; list_iterator_base_t::m_node = list_iterator_base_t::m_node->prev; return old; }

        bool operator==(const list_iterator& rhs) const { return list_iterator_base_t::m_node == rhs.m_node; }
        bool operator!=(const list_iterator& rhs) const { return list_iterator_base_t::m_node != rhs.m_node; }
        value_type& operator*() { return list_iterator_base_t::m_node->value; }
        value_type* operator->() { return &list_iterator_base_t::m_node->value; }

    private:
        template<class T2, class Allocator2> friend class noe_std::list;
        template<class T2, class IteratorCategory2, class Allocator2, class ConnectorPolicy2> friend class noe_std::detail::list_base;

        node_pointer node() { return list_iterator_base_t::m_node; } // only list have access to this
    };

    template<class T>
    class const_list_iterator<std::forward_iterator_tag, T> : private list_iterator_base<std::forward_iterator_tag, T, true>
    {
    private:
        typedef list_iterator_base<std::forward_iterator_tag, T, true> list_iterator_base_t;
        using typename list_iterator_base_t::node_pointer;
        using typename list_iterator_base_t::base_iterator;

    public:
        using typename base_iterator::value_type;
        using typename base_iterator::difference_type;
        using typename base_iterator::pointer;
        using typename base_iterator::reference;
        using typename base_iterator::iterator_category;

        const_list_iterator() {}
        explicit const_list_iterator(node_pointer ptr) : list_iterator_base_t(ptr) {}

        const const_list_iterator& operator++() const { list_iterator_base_t::m_node = list_iterator_base_t::m_node->next; return *this; }
        const_list_iterator operator++(int) const { const_list_iterator old = *this; list_iterator_base_t::m_node = list_iterator_base_t::m_node->next; return old; }

        bool operator==(const const_list_iterator& rhs) const { return list_iterator_base_t::m_node == rhs.m_node; }
        bool operator!=(const const_list_iterator& rhs) const { return list_iterator_base_t::m_node != rhs.m_node; }
        const value_type& operator*() const { return list_iterator_base_t::m_node->value; }
        const value_type* operator->() const { return &list_iterator_base_t::m_node->value; }
//        node_pointer node() const { return list_iterator_base_type::m_node; }
    };

    template<class T>
    class const_list_iterator<std::bidirectional_iterator_tag, T> : private list_iterator_base<std::bidirectional_iterator_tag, T, true>
    {
    private:
        typedef list_iterator_base<std::bidirectional_iterator_tag, T, true> list_iterator_base_t;
        using typename list_iterator_base_t::node_pointer;
        using typename list_iterator_base_t::base_iterator;

    public:
        using typename base_iterator::value_type;
        using typename base_iterator::difference_type;
        using typename base_iterator::pointer;
        using typename base_iterator::reference;
        using typename base_iterator::iterator_category;

        const_list_iterator() {}
        explicit const_list_iterator(node_pointer ptr) : list_iterator_base_t(ptr) {}

        const const_list_iterator& operator++() const { list_iterator_base_t::m_node = list_iterator_base_t::m_node->next; return *this; }
        const_list_iterator operator++(int) const { const_list_iterator old = *this; list_iterator_base_t::m_node = list_iterator_base_t::m_node->next; return old; }
        const const_list_iterator& operator--() const { list_iterator_base_t::m_node = list_iterator_base_t::m_node->prev; return *this; }
        const_list_iterator operator--(int) const { const_list_iterator old = *this; list_iterator_base_t::m_node = list_iterator_base_t::m_node->prev; return old; }

        bool operator==(const const_list_iterator& rhs) const { return list_iterator_base_t::m_node == rhs.m_node; }
        bool operator!=(const const_list_iterator& rhs) const { return list_iterator_base_t::m_node != rhs.m_node; }
        const value_type& operator*() const { return list_iterator_base_t::m_node->value; }
        const value_type* operator->() const { return &list_iterator_base_t::m_node->value; }
//        node_pointer node() const { return list_iterator_base_type::m_node; }
    };
}
}

#endif // GUARD_NOE_STD_list_iterator_H
