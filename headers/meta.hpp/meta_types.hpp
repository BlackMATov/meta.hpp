/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_fwd.hpp"

#include "meta_types/arithmetic_type.hpp"
#include "meta_types/array_type.hpp"
#include "meta_types/base_type.hpp"
#include "meta_types/class_type.hpp"
#include "meta_types/ctor_type.hpp"
#include "meta_types/enum_type.hpp"
#include "meta_types/function_type.hpp"
#include "meta_types/member_type.hpp"
#include "meta_types/method_type.hpp"
#include "meta_types/pointer_type.hpp"
#include "meta_types/reference_type.hpp"
#include "meta_types/void_type.hpp"

namespace meta_hpp
{
    namespace detail
    {
        template < typename Tuple >
        struct multi_get_impl;

        template < typename... Ts >
        struct multi_get_impl<std::tuple<Ts...>> {
            static std::vector<any_type> get() {
                return { type_db::get<Ts>()... };
            }
        };

        template < typename T >
        auto make_any_type() {
            if constexpr ( std::is_arithmetic_v<T> ) {
                return arithmetic_type{typename_arg<T>};
            }
            if constexpr ( std::is_array_v<T> ) {
                return array_type{typename_arg<T>};
            }
            if constexpr ( std::is_class_v<T> ) {
                return class_type{typename_arg<T>};
            }
            if constexpr ( std::is_enum_v<T> ) {
                return enum_type{typename_arg<T>};
            }
            if constexpr ( std::is_pointer_v<T> && std::is_function_v<std::remove_pointer_t<T>> ) {
                return function_type{typename_arg<T>};
            }
            if constexpr ( std::is_member_object_pointer_v<T> ) {
                return member_type{typename_arg<T>};
            }
            if constexpr ( std::is_member_function_pointer_v<T> ) {
                return method_type{typename_arg<T>};
            }
            if constexpr ( std::is_pointer_v<T> && !std::is_function_v<std::remove_pointer_t<T>> ) {
                return pointer_type{typename_arg<T>};
            }
            if constexpr ( std::is_reference_v<T> ) {
                return reference_type{typename_arg<T>};
            }
            if constexpr ( std::is_void_v<T> ) {
                return void_type{typename_arg<T>};
            }
        }
    }

    template < typename T >
    any_type type_db::get() {
        static const auto raw_type = detail::make_any_type<T>();
        return any_type{raw_type};
    }

    template < typename Tuple >
    std::vector<any_type> type_db::multi_get() {
        return detail::multi_get_impl<Tuple>::get();
    }
}
