/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../../meta_base.hpp"
#include "../../meta_utilities.hpp"

namespace meta_hpp::detail
{
    template < typename T >
    struct value_deref_traits;

    template < typename T >
    concept has_value_deref_traits = requires(const T& v) {
        { value_deref_traits<T>{}(v) } -> stdex::convertible_to<value>;
    };
}

namespace meta_hpp::detail
{
    template < stdex::copy_constructible T >
    struct value_deref_traits<T*> {
        value operator()(T* v) const {
            return value{*v};
        }
    };

    template < stdex::copy_constructible T >
    struct value_deref_traits<const T*> {
        value operator()(const T* v) const {
            return value{*v};
        }
    };

    template < stdex::copy_constructible T >
    struct value_deref_traits<std::shared_ptr<T>> {
        value operator()(const std::shared_ptr<T>& v) const {
            return value{*v};
        }
    };

    template < stdex::copy_constructible T >
    struct value_deref_traits<std::unique_ptr<T>> {
        value operator()(const std::unique_ptr<T>& v) const {
            return value{*v};
        }
    };
}
