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
    struct copy_traits;

    template < typename T >
    concept has_copy_traits //
        = requires(const T& v) { copy_traits<std::remove_cv_t<T>>{}(v); };
}

namespace meta_hpp::detail
{
    template < typename T >
        requires(!std::is_class_v<T>) && requires(T v) { uvalue{v}; }
    struct copy_traits<T> {
        uvalue operator()(T v) const {
            return uvalue{v};
        }
    };

    template < typename T, std::size_t Size >
        requires has_copy_traits<T>
    struct copy_traits<std::array<T, Size>> {
        using value_t = std::array<T, Size>;

        uvalue operator()(const value_t& v) const {
            return uvalue{v};
        }
    };

    template < typename T, typename Traits, typename Allocator >
        requires has_copy_traits<T>
    struct copy_traits<std::basic_string<T, Traits, Allocator>> {
        using value_t = std::basic_string<T, Traits, Allocator>;

        uvalue operator()(const value_t& v) const {
            return uvalue{v};
        }
    };

    template < typename T, typename Traits >
        requires has_copy_traits<T>
    struct copy_traits<std::basic_string_view<T, Traits>> {
        using value_t = std::basic_string_view<T, Traits>;

        uvalue operator()(const value_t& v) const {
            return uvalue{v};
        }
    };

    template < typename T, typename Allocator >
        requires has_copy_traits<T>
    struct copy_traits<std::vector<T, Allocator>> {
        using value_t = std::vector<T, Allocator>;

        uvalue operator()(const value_t& v) const {
            return uvalue{v};
        }
    };

    template < typename T >
    struct copy_traits<std::shared_ptr<T>> {
        using value_t = std::shared_ptr<T>;

        uvalue operator()(const value_t& v) const {
            return uvalue{v};
        }
    };

    template < typename T >
    struct copy_traits<std::reference_wrapper<T>> {
        using value_t = std::reference_wrapper<T>;

        uvalue operator()(const value_t& v) const {
            return uvalue{v};
        }
    };

    template < typename... Ts >
        requires(... && has_copy_traits<Ts>)
    struct copy_traits<std::tuple<Ts...>> {
        using value_t = std::tuple<Ts...>;

        uvalue operator()(const value_t& v) const {
            return uvalue{v};
        }
    };
}

#define META_HPP_DECLARE_COPY_TRAITS_FOR(T) \
    namespace meta_hpp::detail \
    { \
        template <> \
        struct copy_traits<T> { \
            uvalue operator()(const T& v) const { \
                return uvalue{v}; \
            } \
        }; \
    }
