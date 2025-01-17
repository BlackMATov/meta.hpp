/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2025, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../../meta_base.hpp"
#include "../../meta_uvalue.hpp"

#include "copy_traits.hpp"

namespace meta_hpp::detail
{
    template < typename T >
    struct index_traits;

    template < typename T >
    concept has_index_traits //
        = requires(const T& v, std::size_t i) { index_traits<std::remove_cv_t<T>>{}(v, i); };
}

namespace meta_hpp::detail
{
    template < typename T >
        requires has_copy_traits<T> && (!std::is_function_v<T>)
    struct index_traits<T*> {
        uvalue operator()(T* v, std::size_t i) const {
            // NOLINTNEXTLINE(*-pointer-arithmetic)
            return v != nullptr ? uvalue{v[i]} : uvalue{};
        }
    };

    template < typename T, std::size_t Size >
        requires has_copy_traits<T>
    struct index_traits<std::array<T, Size>> {
        uvalue operator()(const std::array<T, Size>& v, std::size_t i) const {
            return i < v.size() ? uvalue{v[i]} : uvalue{};
        }
    };

    template < typename T, typename Traits, typename Allocator >
        requires has_copy_traits<T>
    struct index_traits<std::basic_string<T, Traits, Allocator>> {
        uvalue operator()(const std::basic_string<T, Traits, Allocator>& v, std::size_t i) const {
            return i < v.size() ? uvalue{v[i]} : uvalue{};
        }
    };

    template < typename T, typename Traits >
        requires has_copy_traits<T>
    struct index_traits<std::basic_string_view<T, Traits>> {
        uvalue operator()(const std::basic_string_view<T, Traits>& v, std::size_t i) const {
            return i < v.size() ? uvalue{v[i]} : uvalue{};
        }
    };

    template < typename T, std::size_t Extent >
        requires has_copy_traits<T>
    struct index_traits<std::span<T, Extent>> {
        uvalue operator()(const std::span<T, Extent>& v, std::size_t i) const {
            return i < v.size() ? uvalue{v[i]} : uvalue{};
        }
    };

    template < typename T, typename Allocator >
        requires has_copy_traits<T>
    struct index_traits<std::vector<T, Allocator>> {
        uvalue operator()(const std::vector<T, Allocator>& v, std::size_t i) const {
            return i < v.size() ? uvalue{v[i]} : uvalue{};
        }
    };
}

#define META_HPP_DECLARE_INDEX_TRAITS_FOR(T) \
    namespace meta_hpp::detail \
    { \
        template <> \
        struct index_traits<T> { \
            uvalue operator()(const T& v, std::size_t i) const { \
                return uvalue{v[i]}; \
            } \
        }; \
    }
