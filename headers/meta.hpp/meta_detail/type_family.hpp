/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"

#include "type_kinds.hpp"

#include "type_traits/array_traits.hpp"
#include "type_traits/class_traits.hpp"
#include "type_traits/constructor_traits.hpp"
#include "type_traits/destructor_traits.hpp"
#include "type_traits/enum_traits.hpp"
#include "type_traits/function_traits.hpp"
#include "type_traits/member_traits.hpp"
#include "type_traits/method_traits.hpp"
#include "type_traits/number_traits.hpp"
#include "type_traits/pointer_traits.hpp"
#include "type_traits/reference_traits.hpp"

namespace meta_hpp::detail
{
    template <type_family Type>
    struct type_traits;

    template < type_family Type >
    [[nodiscard]] typename type_traits<Type>::data_ptr type_access(const Type& type) {
        return type.data_;
    }
}

namespace meta_hpp::detail
{
    template <>
    struct type_traits<any_type> {
        using data_ptr = type_data_base*;
    };

    template <>
    struct type_traits<array_type> {
        using data_ptr = array_type_data*;
        inline static constexpr type_kind kind{type_kind::array_};
    };

    template <>
    struct type_traits<class_type> {
        using data_ptr = class_type_data*;
        inline static constexpr type_kind kind{type_kind::class_};
    };

    template <>
    struct type_traits<constructor_type> {
        using data_ptr = constructor_type_data*;
        inline static constexpr type_kind kind{type_kind::constructor_};
    };

    template <>
    struct type_traits<destructor_type> {
        using data_ptr = destructor_type_data*;
        inline static constexpr type_kind kind{type_kind::destructor_};
    };

    template <>
    struct type_traits<enum_type> {
        using data_ptr = enum_type_data*;
        inline static constexpr type_kind kind{type_kind::enum_};
    };

    template <>
    struct type_traits<function_type> {
        using data_ptr = function_type_data*;
        inline static constexpr type_kind kind{type_kind::function_};
    };

    template <>
    struct type_traits<member_type> {
        using data_ptr = member_type_data*;
        inline static constexpr type_kind kind{type_kind::member_};
    };

    template <>
    struct type_traits<method_type> {
        using data_ptr = method_type_data*;
        inline static constexpr type_kind kind{type_kind::method_};
    };

    template <>
    struct type_traits<nullptr_type> {
        using data_ptr = nullptr_type_data*;
        inline static constexpr type_kind kind{type_kind::nullptr_};
    };

    template <>
    struct type_traits<number_type> {
        using data_ptr = number_type_data*;
        inline static constexpr type_kind kind{type_kind::number_};
    };

    template <>
    struct type_traits<pointer_type> {
        using data_ptr = pointer_type_data*;
        inline static constexpr type_kind kind{type_kind::pointer_};
    };

    template <>
    struct type_traits<reference_type> {
        using data_ptr = reference_type_data*;
        inline static constexpr type_kind kind{type_kind::reference_};
    };

    template <>
    struct type_traits<void_type> {
        using data_ptr = void_type_data*;
        inline static constexpr type_kind kind{type_kind::void_};
    };
}
