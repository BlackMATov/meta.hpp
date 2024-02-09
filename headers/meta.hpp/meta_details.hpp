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
    using detail::array_bitflags;
    using detail::array_flags;

    using detail::class_bitflags;
    using detail::class_flags;

    using detail::constructor_bitflags;
    using detail::constructor_flags;

    using detail::destructor_bitflags;
    using detail::destructor_flags;

    using detail::enum_bitflags;
    using detail::enum_flags;

    using detail::function_bitflags;
    using detail::function_flags;

    using detail::member_bitflags;
    using detail::member_flags;

    using detail::method_bitflags;
    using detail::method_flags;

    using detail::number_bitflags;
    using detail::number_flags;

    using detail::pointer_bitflags;
    using detail::pointer_flags;

    using detail::reference_bitflags;
    using detail::reference_flags;
}

namespace meta_hpp
{
    using detail::type_kind;

    using detail::array_kind;
    using detail::class_kind;
    using detail::enum_kind;
    using detail::function_kind;
    using detail::function_pointer_kind;
    using detail::member_pointer_kind;
    using detail::method_pointer_kind;
    using detail::nullptr_kind;
    using detail::number_kind;
    using detail::pointer_kind;
    using detail::reference_kind;
    using detail::void_kind;

    using detail::any_pointer_family;
    using detail::not_any_pointer_family;

    using detail::class_constructor_kind;
    using detail::class_destructor_kind;
    using detail::class_member_pointer_kind;
    using detail::class_method_pointer_kind;
}
