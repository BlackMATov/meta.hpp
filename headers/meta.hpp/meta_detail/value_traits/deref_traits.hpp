/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../../meta_base.hpp"
#include "../../meta_uvalue.hpp"

#include "copy_traits.hpp"

namespace meta_hpp::detail
{
    template < typename T >
    struct deref_traits;

    template < typename T >
    concept has_deref_traits //
        = requires(const T& v) { deref_traits<std::remove_cv_t<T>>{}(v); };
}

namespace meta_hpp::detail
{
    template < typename T >
        requires has_copy_traits<T> && (!std::is_function_v<T>)
    struct deref_traits<T*> {
        uvalue operator()(T* v) const {
            return v != nullptr ? uvalue{*v} : uvalue{};
        }
    };

    template < typename T >
        requires has_copy_traits<T>
    struct deref_traits<std::shared_ptr<T>> {
        using value_t = std::shared_ptr<T>;

        uvalue operator()(const value_t& v) const {
            return v != nullptr ? uvalue{*v} : uvalue{};
        }
    };

    template < typename T, typename Deleter >
        requires has_copy_traits<T>
    struct deref_traits<std::unique_ptr<T, Deleter>> {
        using value_t = std::unique_ptr<T, Deleter>;

        uvalue operator()(const value_t& v) const {
            return v != nullptr ? uvalue{*v} : uvalue{};
        }
    };
}

#define META_HPP_DECLARE_DEREF_TRAITS_FOR(T) \
    namespace meta_hpp::detail \
    { \
        template <> \
        struct deref_traits<T> { \
            uvalue operator()(const T& v) const { \
                return uvalue{*v}; \
            } \
        }; \
    }
