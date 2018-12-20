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
#ifndef GUARD_NOE_STD_detail_list_node_H
#define GUARD_NOE_STD_detail_list_node_H

#include <iterator>
#include <type_traits>
#include "../type_traits.h"

namespace noe_std
{
namespace detail
{
    template<class IteratorCategory, class T> class node;
    template<class IteratorCategory, class T> struct node_impl;

    template<class T>
    struct node_impl<std::forward_iterator_tag, T>
    {
        typedef node<std::forward_iterator_tag, T>* node_pointer;

        node_impl() : next(0) {}

        node_pointer next;
    };

    template<class T>
    struct node_impl<std::bidirectional_iterator_tag, T>
    {
        typedef node<std::bidirectional_iterator_tag, T>* node_pointer;

        node_impl() : next(0), prev(0) {}

        node_pointer next;
        node_pointer prev;
    };
//    template<class T>
//    struct node_base<std::forward_iterator_tag, T>
//    {
//        typedef node<std::forward_iterator_tag, T>* node_pointer;

//        node_base(const T& v) : value(v), next(0) {}
//#if __cplusplus >= 201103L
//        node_base(T&& v) : value(std::move(v)), next(0) {}
//#endif // __cplusplus >= 201103L

//        T               value;
//        node_pointer    next;
//    };

//    template<class T>
//    struct node_base<std::bidirectional_iterator_tag, T>
//    {
//        typedef node<std::bidirectional_iterator_tag, T>* node_pointer;

//        node_base(const T& v) : value(v), next(0), prev(0) {}
//#if __cplusplus >= 201103L
//        node_base(T&& v) : value(std::move(v)), next(0), prev(0) {}
//        template<class... Args,
//                 class = typename std::enable_if<enable_if_parameter_pack_and_compressor<!std::is_same<T, Args>::value...>::value>::type>
//        node_base(Args&&... args) : value(std::forward<Args>(args)...), next(0), prev(0) {}
//#endif // __cplusplus >= 201103L

//        T               value;
//        node_pointer    next;
//        node_pointer    prev;
//    };
    template<class IteratorCategory, class T>
    struct node_base : public node_impl<IteratorCategory, T>
    {
        node_base(const T& v) : value(v) {}
#if __cplusplus >= 201103L
        node_base(T&& v) : value(std::move(v)) {}
        template<class... Args,
                 class = typename std::enable_if<enable_if_parameter_pack_and_compressor<!std::is_same<T, Args>::value...>::value>::type>
        node_base(Args&&... args) : value(std::forward<Args>(args)...) {}
#endif // __cplusplus >= 201103L

        T value;
    };

    template<class IteratorCategory, class T>
    struct node : public node_base<IteratorCategory, T>
    {
        typedef IteratorCategory iterator_category;
        typedef node_base<IteratorCategory, T> node_base_t;
        typedef typename node_base<IteratorCategory, T>::node_pointer pointer;

        node(const T& v) : node_base_t(v) {}
#if __cplusplus >= 201103L
        node(T&& v) : node_base_t(std::move(v)) {}
        template<class... Args,
                 class = typename std::enable_if<enable_if_parameter_pack_and_compressor<!std::is_same<T, Args>::value...>::value>::type>
        node(Args&&... args) : node_base_t(std::forward<Args>(args)...) {}
#endif // __cplusplus >= 201103L
    };
}
}
namespace std
{
    template<class T>
    inline void swap(typename noe_std::detail::node<std::bidirectional_iterator_tag, T>::pointer& n1, typename noe_std::detail::node<std::bidirectional_iterator_tag, T>::pointer& n2)
    {
//        swap(n1->next, n2->next);
//        swap(n1->prev, n2->prev);
        swap(n1->value, n2->value);
    }

    template<class T>
    inline void swap(typename noe_std::detail::node<std::forward_iterator_tag, T>::pointer& n1, typename noe_std::detail::node<std::forward_iterator_tag, T>::pointer& n2)
    {
//        swap(n1->next, n2->next);
        swap(n1->value, n2->value);
    }
}

#endif // GUARD_NOE_STD_detail_list_node_H
