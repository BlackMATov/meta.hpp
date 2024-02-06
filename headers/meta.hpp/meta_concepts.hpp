/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_base.hpp"

#include "meta_detail/type_family.hpp"

namespace meta_hpp
{
    using detail::array_kind;
    using detail::class_kind;
    using detail::enum_kind;
    using detail::function_kind;
    using detail::function_pointer_kind;
    using detail::member_pointer_kind;
    using detail::method_pointer_kind;
    using detail::non_pointer_kind;
    using detail::nullptr_kind;
    using detail::number_kind;
    using detail::pointer_kind;
    using detail::reference_kind;
    using detail::void_kind;
}

namespace meta_hpp
{
    template < typename Class, typename... Args >
    concept class_constructor_kind                 //
        = class_kind<Class>                        //
       && std::is_constructible_v<Class, Args...>; //

    template < typename Class >
    concept class_destructor_kind                             //
        = class_kind<Class> && std::is_destructible_v<Class>; //

    template < typename Member, typename Class >
    concept class_member_kind                                                        //
        = member_pointer_kind<Member> && class_kind<Class>                           //
       && std::is_same_v<Class, typename detail::member_traits<Member>::class_type>; //

    template < typename Method, typename Class >
    concept class_method_kind                                                        //
        = method_pointer_kind<Method> && class_kind<Class>                           //
       && std::is_same_v<Class, typename detail::method_traits<Method>::class_type>; //
}
