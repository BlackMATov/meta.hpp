/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../../meta_base.hpp"
#include "../../meta_uvalue.hpp"

namespace meta_hpp::detail
{
    template < typename T >
    struct deref_traits;

    template < typename T >
    concept has_deref_traits //
        = requires(const T& v) { deref_traits<T>{}(v); };
}

namespace meta_hpp::detail
{
    template < typename T >
        requires std::is_copy_constructible_v<T>
    struct deref_traits<T*> {
        uvalue operator()(T* v) const {
            return v != nullptr ? uvalue{*v} : uvalue{};
        }
    };

    template < typename T >
        requires std::is_copy_constructible_v<T>
    struct deref_traits<std::shared_ptr<T>> {
        uvalue operator()(const std::shared_ptr<T>& v) const {
            return v != nullptr ? uvalue{*v} : uvalue{};
        }
    };

    template < typename T, typename Deleter >
        requires std::is_copy_constructible_v<T>
    struct deref_traits<std::unique_ptr<T, Deleter>> {
        uvalue operator()(const std::unique_ptr<T, Deleter>& v) const {
            return v != nullptr ? uvalue{*v} : uvalue{};
        }
    };
}
