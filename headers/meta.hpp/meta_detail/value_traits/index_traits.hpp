/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../../meta_base.hpp"
#include "../../meta_uvalue.hpp"

namespace meta_hpp::detail
{
    template < typename T >
    struct index_traits;

    template < typename T >
    concept has_index_traits = requires(const T& v, std::size_t i) {
        { index_traits<T>{}(v, i) } -> stdex::convertible_to<uvalue>;
    };
}

namespace meta_hpp::detail
{
    template < stdex::copy_constructible T >
    struct index_traits<T*> {
        uvalue operator()(T* v, std::size_t i) const {
            // NOLINTNEXTLINE(*-pointer-arithmetic)
            return uvalue{v[i]};
        }
    };

    template < stdex::copy_constructible T >
    struct index_traits<const T*> {
        uvalue operator()(const T* v, std::size_t i) const {
            // NOLINTNEXTLINE(*-pointer-arithmetic)
            return uvalue{v[i]};
        }
    };

    template < stdex::copy_constructible T, std::size_t Size >
    struct index_traits<std::array<T, Size>> {
        uvalue operator()(const std::array<T, Size>& v, std::size_t i) const {
            return uvalue{v[i]};
        }
    };

    template < stdex::copy_constructible T, std::size_t Extent >
    struct index_traits<std::span<T, Extent>> {
        uvalue operator()(const std::span<T, Extent>& v, std::size_t i) const {
            return uvalue{v[i]};
        }
    };

    template < stdex::copy_constructible T, typename Traits, typename Allocator >
    struct index_traits<std::basic_string<T, Traits, Allocator>> {
        uvalue operator()(const std::basic_string<T, Traits, Allocator>& v, std::size_t i) const {
            return uvalue{v[i]};
        }
    };

    template < stdex::copy_constructible T, typename Allocator >
    struct index_traits<std::vector<T, Allocator>> {
        uvalue operator()(const std::vector<T, Allocator>& v, std::size_t i) {
            return uvalue{v[i]};
        }
    };
}
