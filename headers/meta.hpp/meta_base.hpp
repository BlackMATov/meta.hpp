/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_base/base.hpp"
#include "meta_base/bitflags.hpp"
#include "meta_base/cv_traits.hpp"
#include "meta_base/cvref_traits.hpp"
#include "meta_base/exceptions.hpp"
#include "meta_base/fixed_function.hpp"
#include "meta_base/fnv1a_hash.hpp"
#include "meta_base/hash_composer.hpp"
#include "meta_base/inline_vector.hpp"
#include "meta_base/insert_or_assign.hpp"
#include "meta_base/is_in_place_type.hpp"
#include "meta_base/memory_buffer.hpp"
#include "meta_base/noncopyable.hpp"
#include "meta_base/nonesuch.hpp"
#include "meta_base/overloaded.hpp"
#include "meta_base/select_overload.hpp"
#include "meta_base/to_underlying.hpp"
#include "meta_base/type_list.hpp"

namespace meta_hpp
{
    using detail::error_code;
    using detail::exception;
    using detail::get_error_code_message;

    using detail::memory_buffer;
    using detail::overloaded;

    using detail::select_const;
    using detail::select_non_const;
    using detail::select_overload;
}

namespace meta_hpp
{
    class uerror;
    class uresult;

    class uvalue;

    namespace detail
    {
        class uarg_base;
        class uarg;

        class uinst_base;
        class uinst;
    }

    template < typename T >
    concept uvalue_family                       //
        = std::is_same_v<T, uerror>             //
       || std::is_same_v<T, uresult>            //
       || std::is_same_v<T, uvalue>             //
       || std::is_same_v<T, detail::uarg_base>  //
       || std::is_same_v<T, detail::uarg>       //
       || std::is_same_v<T, detail::uinst_base> //
       || std::is_same_v<T, detail::uinst>;     //
}

namespace meta_hpp
{
    class argument;
    class constructor;
    class destructor;
    class evalue;
    class function;
    class member;
    class method;
    class scope;
    class variable;

    template < typename T >
    concept state_family                 //
        = std::is_same_v<T, argument>    //
       || std::is_same_v<T, constructor> //
       || std::is_same_v<T, destructor>  //
       || std::is_same_v<T, evalue>      //
       || std::is_same_v<T, function>    //
       || std::is_same_v<T, member>      //
       || std::is_same_v<T, method>      //
       || std::is_same_v<T, scope>       //
       || std::is_same_v<T, variable>;   //
}

namespace meta_hpp
{
    class any_type;
    class array_type;
    class class_type;
    class constructor_type;
    class destructor_type;
    class enum_type;
    class function_type;
    class member_type;
    class method_type;
    class nullptr_type;
    class number_type;
    class pointer_type;
    class reference_type;
    class void_type;

    template < typename T >
    concept type_family                       //
        = std::is_same_v<T, any_type>         //
       || std::is_same_v<T, array_type>       //
       || std::is_same_v<T, class_type>       //
       || std::is_same_v<T, constructor_type> //
       || std::is_same_v<T, destructor_type>  //
       || std::is_same_v<T, enum_type>        //
       || std::is_same_v<T, function_type>    //
       || std::is_same_v<T, member_type>      //
       || std::is_same_v<T, method_type>      //
       || std::is_same_v<T, nullptr_type>     //
       || std::is_same_v<T, number_type>      //
       || std::is_same_v<T, pointer_type>     //
       || std::is_same_v<T, reference_type>   //
       || std::is_same_v<T, void_type>;       //
}

namespace meta_hpp
{
    class argument_index;
    class constructor_index;
    class destructor_index;
    class evalue_index;
    class function_index;
    class member_index;
    class method_index;
    class scope_index;
    class variable_index;

    template < typename T >
    concept index_family                       //
        = std::is_same_v<T, argument_index>    //
       || std::is_same_v<T, constructor_index> //
       || std::is_same_v<T, destructor_index>  //
       || std::is_same_v<T, evalue_index>      //
       || std::is_same_v<T, function_index>    //
       || std::is_same_v<T, member_index>      //
       || std::is_same_v<T, method_index>      //
       || std::is_same_v<T, scope_index>       //
       || std::is_same_v<T, variable_index>;   //
}

namespace meta_hpp
{
    using metadata_map = std::map<std::string, uvalue, std::less<>>;
    using typedef_map = std::map<std::string, any_type, std::less<>>;
    using uvalue_list = std::vector<uvalue>;

    using any_type_list = std::vector<any_type>;
    using class_list = std::vector<class_type>;
    using enum_list = std::vector<enum_type>;

    using argument_list = std::vector<argument>;
    using constructor_list = std::vector<constructor>;
    using destructor_list = std::vector<destructor>;
    using evalue_list = std::vector<evalue>;
    using function_list = std::vector<function>;
    using member_list = std::vector<member>;
    using method_list = std::vector<method>;
    using variable_list = std::vector<variable>;
}
