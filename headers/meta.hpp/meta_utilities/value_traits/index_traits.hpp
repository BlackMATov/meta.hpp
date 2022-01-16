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
    struct value_index_traits;

    template < typename T >
    concept has_value_index_traits = requires(const T& v, std::size_t i) {
        { value_index_traits<T>{}(v, i) } -> stdex::convertible_to<value>;
    };
}

namespace meta_hpp::detail
{
    template < stdex::copy_constructible T >
    struct value_index_traits<T*> {
        value operator()(T* v, std::size_t i) const {
            return value{*(v + i)};
        }
    };

    template < stdex::copy_constructible T >
    struct value_index_traits<const T*> {
        value operator()(const T* v, std::size_t i) const {
            return value{*(v + i)};
        }
    };

    template < stdex::copy_constructible T, std::size_t Size >
    struct value_index_traits<std::array<T, Size>> {
        value operator()(const std::array<T, Size>& v, std::size_t i) const {
            return value{v[i]};
        }
    };

    template < stdex::copy_constructible T, std::size_t Extent >
    struct value_index_traits<std::span<T, Extent>> {
        value operator()(const std::span<T, Extent>& v, std::size_t i) const {
            return value{v[i]};
        }
    };

    template < stdex::copy_constructible T, typename Traits, typename Allocator >
    struct value_index_traits<std::basic_string<T, Traits, Allocator>> {
        value operator()(const std::basic_string<T, Traits, Allocator>& v, std::size_t i) const {
            return value{v[i]};
        }
    };

    template < stdex::copy_constructible T, typename Allocator >
    struct value_index_traits<std::vector<T, Allocator>> {
        value operator()(const std::vector<T, Allocator>& v, std::size_t i) {
            return value{v[i]};
        }
    };
}
