/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2025, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../../meta_base.hpp"
#include "../../meta_uvalue.hpp"

namespace meta_hpp::detail
{
    template < typename T >
    struct size_traits;

    template < typename T >
    concept has_size_traits //
        = requires(const T& v) { size_traits<std::remove_cv_t<T>>{}(v); };
}

namespace meta_hpp::detail
{
    template < typename T, std::size_t Size >
    struct size_traits<std::array<T, Size>> {
        std::size_t operator()(const std::array<T, Size>& v) const {
            using std::size;
            return size(v);
        }
    };

    template < typename T, typename Traits, typename Allocator >
    struct size_traits<std::basic_string<T, Traits, Allocator>> {
        std::size_t operator()(const std::basic_string<T, Traits, Allocator>& v) const {
            using std::size;
            return size(v);
        }
    };

    template < typename T, typename Traits >
    struct size_traits<std::basic_string_view<T, Traits>> {
        std::size_t operator()(const std::basic_string_view<T, Traits>& v) const {
            using std::size;
            return size(v);
        }
    };

    template < typename T, std::size_t Extent >
    struct size_traits<std::span<T, Extent>> {
        std::size_t operator()(const std::span<T, Extent>& v) const {
            using std::size;
            return size(v);
        }
    };

    template < typename T, typename Allocator >
    struct size_traits<std::vector<T, Allocator>> {
        std::size_t operator()(const std::vector<T, Allocator>& v) const {
            using std::size;
            return size(v);
        }
    };
}

#define META_HPP_DECLARE_SIZE_TRAITS_FOR(T) \
    namespace meta_hpp::detail \
    { \
        template <> \
        struct size_traits<T> { \
            std::size_t operator()(const T& v) const { \
                using std::size; \
                return size(v); \
            } \
        }; \
    }
