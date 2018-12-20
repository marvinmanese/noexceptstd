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
#ifndef GUARD_NOE_STD_list_H
#define GUARD_NOE_STD_list_H

#include <algorithm>
#include <iterator>
#include "detail/list_base.h"
#include "detail/temporary_pointer.h"
#include "allocator.h"

// TODO: do move operators
// TODO: change resize to use erase(first, last) operation
// TODO: do reverse iterator for reverse();
// TODO: implement sort
namespace noe_std
{
namespace
{
    struct bidirectional_node_connector_policy
    {
        template<class NodePtr>
        static void connect(NodePtr tail, NodePtr node)
        {
            if(tail)
                tail->next = node;
            if(node)
                node->prev = tail;
        }

        template<class NodePtr>
        static void disconnect(NodePtr left_node, NodePtr mid_node, NodePtr right_node)
        {
            if(left_node)
                left_node->next = right_node;
            if(right_node)
                right_node->prev = left_node;
        }

        template<class NodePtr>
        static void disconnect_ends(NodePtr* begin, NodePtr* end, NodePtr node)
        {
            if(*begin == node)
                *begin = node->next;
            if(*end == node)
                *end = node->prev;

            // disconnect the mid node
            node->next = node->prev = 0;
        }

        template<class NodePtr>
        static NodePtr get_prev_node(NodePtr /*begin*/, NodePtr node)
        {
            return node->prev;
        }
    };
}
    template<class T,
             class Allocator = allocator<T>>
    class list : public detail::list_base<T, std::bidirectional_iterator_tag, Allocator, bidirectional_node_connector_policy>
    {
        typedef detail::list_base<T, std::bidirectional_iterator_tag, Allocator, bidirectional_node_connector_policy> base_t;
        typedef typename base_t::node_allocator_t   node_allocator_t;
        typedef typename base_t::node_t             node_t;

    public:
        // Inherit typedefs
        typedef typename base_t::value_type         value_type;
        typedef typename base_t::allocator_type     allocator_type;
        typedef typename base_t::size_type          size_type;
        typedef typename base_t::difference_type    difference_type;
        typedef typename base_t::reference          reference;
        typedef typename base_t::const_reference    const_reference;
        typedef typename base_t::pointer            pointer;
        typedef typename base_t::const_pointer      const_pointer;
        typedef typename base_t::iterator           iterator;
        typedef typename base_t::const_iterator     const_iterator;

        list() {}
        list(const list& rhs);
        list& operator=(const list& rhs);
        ~list() {}

        void pop_back();
        void pop_front();

#if __cplusplus >= 201103L
        void resize(size_type n);
        void resize(size_type n, const value_type& v);
#else
        void resize(size_type n, T v = T());
#endif // __cplusplus >= 201103L

        void swap(list& l);

        void merge(const list& l);
        void splice(const list& l);
        template<class Pred> void removeremove_if(const Pred& pred);
#if __cplusplus >= 201103L
        template<class Pred> void removeremove_if(Pred&& pred);
#endif // __cplusplus >= 201103L
        void reverse();
        void unique();
        void sort();

    private:
        template<class U> friend bool operator==(const noe_std::list<U>& lhs, const noe_std::list<U>& rhs);
        template<class U> friend bool operator!=(const noe_std::list<U>& lhs, const noe_std::list<U>& rhs);
        template<class U> friend bool operator<(const noe_std::list<U>& lhs, const noe_std::list<U>& rhs);
        template<class U> friend bool operator<=(const noe_std::list<U>& lhs, const noe_std::list<U>& rhs);
        template<class U> friend bool operator>(const noe_std::list<U>& lhs, const noe_std::list<U>& rhs);
        template<class U> friend bool operator>=(const noe_std::list<U>& lhs, const noe_std::list<U>& rhs);
        template<class U> friend void std::swap(noe_std::list<U>& l1, noe_std::list<U>& l2);
    };

    template<class T, class Allocator>
    list<T, Allocator>::list(const list& rhs) :
        base_t(rhs)
    {
    }

    template<class T, class Allocator>
    list<T, Allocator>& list<T, Allocator>::operator=(const list& rhs)
    {
        list list_(rhs);
        swap(list_);

         return *this;
    }

    template<class T, class Allocator>
    void list<T, Allocator>::pop_back()
    {
        this->erase(iterator(this->m_member.m_end));
    }

    template<class T, class Allocator>
    void list<T, Allocator>::pop_front()
    {
        this->erase(this->begin());
    }
#if __cplusplus >= 201103L
    template<class T, class Allocator>
    void list<T, Allocator>::resize(size_type n)
    {
        size_type list_size = this->m_member.m_size;
        if(n > list_size) {
            size_type diff = n - list_size;

            // add more values
            this->insert(iterator(this->m_member.m_end), diff, T());
        } else if(n < list_size) {
            size_type diff = list_size - n;
            iterator it = iterator(this->m_member.m_end);
            std::prev(it, diff);

            this->erase(it, this->end());
        }
    }

    template<class T, class Allocator>
    void list<T, Allocator>::resize(size_type n, const value_type& v)
    {
        size_type list_size = this->m_member.m_size;
        if(n > list_size) {
            size_type diff = n - list_size;

            // add more values
            this->insert(iterator(this->m_member.m_end), diff, v);
        } else if(n < list_size) {
            size_type diff = list_size - n;
            iterator it = iterator(this->m_member.m_end);
            std::prev(it, diff);

            this->erase(it, this->end());
        }
    }
#else
    template<class T, class Allocator>
    void list<T, Allocator>::resize(size_type n, T v = T())
    {
        size_type list_size = this->m_member.m_size;
        if(n > list_size) {
            size_type diff = n - list_size;

            // add more values
            this->insert(iterator(this->m_member.m_end), diff, v);
        } else if(n < list_size) {
            size_type diff = list_size - n;
            iterator it = iterator(this->m_member.m_end);
            std::prev(it, diff);

            this->erase(it, this->end());
        }
    }
#endif // __cplusplus >= 201103L
    template<class T, class Allocator>
    inline void list<T, Allocator>::swap(list& l)
    {
        base_t::swap(l);
    }

    template<class T, class Allocator>
    void list<T, Allocator>::merge(const list& l)
    {
        this->insert(iterator(this->m_member.m_end), l.begin(), l.end());
    }

    template<class T, class Allocator>
    void list<T, Allocator>::reverse()
    {
//        list temp(this->rbegin(), this->rend());
//        swap(temp);
    }

    template<class T, class Allocator>
    void list<T, Allocator>::sort()
    {
//        std::sort(this->begin(), this->end());
    }

    template<class T>
    bool operator==(const noe_std::list<T>& lhs, const noe_std::list<T>& rhs)
    {
        if(lhs.size() != rhs.size())
            return false;

        typedef typename noe_std::list<T>::const_iterator const_iterator;
        for(const_iterator it = lhs.begin(), it_end = lhs.end(), it2 = rhs.begin(); it != it_end; ++it, ++it2) {
            if((*it) != (*it2))
                return false;
        }

        return true;
    }

    template<class T>
    inline bool operator!=(const noe_std::list<T>& lhs, const noe_std::list<T>& rhs)
    {
        return !(lhs == rhs);
    }

    template<class T>
    inline bool operator<(const noe_std::list<T>& lhs, const noe_std::list<T>& rhs)
    {
        return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), std::less<T>());
    }

    template<class T>
    inline bool operator<=(const noe_std::list<T>& lhs, const noe_std::list<T>& rhs)
    {
        return (lhs < rhs || lhs == rhs);
    }

    template<class T>
    inline bool operator>(const noe_std::list<T>& lhs, const noe_std::list<T>& rhs)
    {
        return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), std::greater<T>());
    }

    template<class T>
    inline bool operator>=(const noe_std::list<T>& lhs, const noe_std::list<T>& rhs)
    {
        return (lhs > rhs || lhs == rhs);
    }
}
namespace std
{
    template<class T>
    inline void swap(noe_std::list<T>& l1, noe_std::list<T>& l2)
    {
        l1.swap(l2);
    }
}

#endif // GUARD_NOE_STD_list_H
