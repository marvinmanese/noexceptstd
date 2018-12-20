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
#ifndef GUARD_NOE_STD_type_traits_H
#define GUARD_NOE_STD_type_traits_H

namespace noe_std
{
/// C++11 type traits
#if __cplusplus >= 201103L
    /// Parameter pack AND compressor for boolean parameter packs
    /// that can be used on parameter packs for enable_if
    template<bool... Booleans>
    struct enable_if_parameter_pack_and_compressor;

    template<bool... Booleans>
    struct enable_if_parameter_pack_and_compressor<true, Booleans...>
    {
        enum : bool { value = enable_if_parameter_pack_and_compressor<Booleans...>::value };
    };

    template<bool... Booleans>
    struct enable_if_parameter_pack_and_compressor<false, Booleans...>
    {
        enum : bool { value = false };
    };

    template<>
    struct enable_if_parameter_pack_and_compressor<>
    {
        enum : bool { value = true };
    };

    /// Parameter pack OR compressor for boolean parameter packs
    /// that can be used on parameter packs for enable_if
    template<bool... Booleans>
    struct enable_if_parameter_pack_or_compressor;

    template<bool... Booleans>
    struct enable_if_parameter_pack_or_compressor<true, Booleans...>
    {
        enum : bool { value = true };
    };

    template<bool... Booleans>
    struct enable_if_parameter_pack_or_compressor<false, Booleans...>
    {
        enum : bool { value = enable_if_parameter_pack_or_compressor<Booleans...>::value };
    };

    template<>
    struct enable_if_parameter_pack_or_compressor<>
    {
        enum : bool { value = false };
    };
#endif // __cplusplus >= 201103L

/// Non-C++11 type traits
}

#endif // GUARD_NOE_STD_type_traits_H
