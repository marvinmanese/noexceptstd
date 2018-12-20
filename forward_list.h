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
#ifndef GUARD_NOE_STD_forward_list_H
#define GUARD_NOE_STD_forward_list_H

#include <iterator>
#include "detail/list_base.h"
#include "detail/temporary_pointer.h"
#include "allocator.h"

namespace noe_std
{
namespace
{
    struct forward_node_connector_policy
    {
        template<class NodePtr>
        static void connect(NodePtr tail, NodePtr node)
        {
            tail->next = node;
        }

        template<class NodePtr>
        static void disconnect(NodePtr left_node, NodePtr mid_node, NodePtr right_node)
        {
            if(left_node)
                left_node->next = right_node;
        }

        template<class NodePtr>
        static void disconnect_ends(NodePtr* begin, NodePtr* end, NodePtr node)
        {
            if(*end == node) {
                NodePtr prev_node = get_prev_node(begin, node);
                *end = prev_node;
            }
            if(*begin == node)
                *begin = node->next;

            node->next = 0;
        }

        template<class NodePtr>
        static NodePtr get_prev_node(NodePtr begin, NodePtr node)
        {
            NodePtr curr = begin;
            while(curr->next != node)
                curr = curr->next;
            return curr;
        }
    };
}
    template<class T,
             class Allocator = allocator<T>>
    class forward_list : public detail::list_base<T, std::forward_iterator_tag, Allocator, forward_node_connector_policy>
    {
    private:
        typedef detail::list_base<T, std::forward_iterator_tag, Allocator> base_t;
        typedef typename base_t::node_allocator_t   node_allocator_t;
        typedef typename base_t::node_t             node_t;

    public:
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

        forward_list() {}
        forward_list(const forward_list& rhs);
        forward_list& operator=(const forward_list& rhs);
        ~forward_list() {}
    };
}

#endif // GUARD_NOE_STD_forward_list_H
