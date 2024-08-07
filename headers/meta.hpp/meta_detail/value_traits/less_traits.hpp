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
    struct less_traits;

    template < typename T >
    concept has_less_traits //
        = requires(const T& v) { less_traits<T>{}(v, v); };
}

namespace meta_hpp::detail
{
    template < typename T >
        requires requires(const T& v) {
                     { v < v } -> std::convertible_to<bool>;
                 } && (!class_kind<T> || type_list_arity_v<typename class_traits<T>::argument_types> == 0)
    struct less_traits<T> {
        bool operator()(const T& l, const T& r) const {
            META_HPP_DETAIL_IGNORE_COMPARISON_WARNINGS_PUSH()
            return l < r;
            META_HPP_DETAIL_IGNORE_COMPARISON_WARNINGS_POP()
        }
    };

    template < typename T, std::size_t Size >
        requires has_less_traits<T>
    struct less_traits<std::array<T, Size>> {
        using value_t = std::array<T, Size>;

        bool operator()(const value_t& l, const value_t& r) const {
            return l < r;
        }
    };

    template < typename T, typename Traits, typename Allocator >
        requires has_less_traits<T>
    struct less_traits<std::basic_string<T, Traits, Allocator>> {
        using value_t = std::basic_string<T, Traits, Allocator>;

        bool operator()(const value_t& l, const value_t& r) const {
            return l < r;
        }
    };

    template < typename T, typename Traits >
        requires has_less_traits<T>
    struct less_traits<std::basic_string_view<T, Traits>> {
        using value_t = std::basic_string_view<T, Traits>;

        bool operator()(const value_t& l, const value_t& r) const {
            return l < r;
        }
    };

    template < typename T, typename Allocator >
        requires has_less_traits<T>
    struct less_traits<std::vector<T, Allocator>> {
        using value_t = std::vector<T, Allocator>;

        bool operator()(const value_t& l, const value_t& r) const {
            return l < r;
        }
    };

    template < typename T >
    struct less_traits<std::shared_ptr<T>> {
        using value_t = std::shared_ptr<T>;

        bool operator()(const value_t& l, const value_t& r) const {
            return l < r;
        }
    };

    template < typename T, typename Deleter >
    struct less_traits<std::unique_ptr<T, Deleter>> {
        using value_t = std::unique_ptr<T, Deleter>;

        bool operator()(const value_t& l, const value_t& r) const {
            return l < r;
        }
    };

    template < typename T >
        requires has_less_traits<T>
    struct less_traits<std::reference_wrapper<T>> {
        using value_t = std::reference_wrapper<T>;

        bool operator()(const value_t& l, const value_t& r) const {
            return l.get() < r.get();
        }
    };

    template < typename... Ts >
        requires(... && has_less_traits<Ts>)
    struct less_traits<std::tuple<Ts...>> {
        using value_t = std::tuple<Ts...>;

        bool operator()(const value_t& l, const value_t& r) const {
            return l < r;
        }
    };
}

#define META_HPP_DECLARE_LESS_TRAITS_FOR(T) \
    namespace meta_hpp::detail \
    { \
        template <> \
        struct less_traits<T> { \
            bool operator()(const T& l, const T& r) const { \
                return l < r; \
            } \
        }; \
    }
