/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include <any>
#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <map>
#include <optional>
#include <stdexcept>
#include <string_view>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

namespace meta_hpp
{
    class value;

    class class_info;
    class data_info;
    class field_info;
    class function_info;
    class method_info;
    class namespace_info;
    class variable_info;

    template < typename Class > class class_;
    class data_;
    template < auto Field > class field_;
    template < auto Function > class function_;
    template < auto Method > class method_;
    class namespace_;
    template < auto Variable > class variable_;
}

namespace meta_hpp
{
    template < typename Signature >
    constexpr auto select(Signature* f) noexcept {
        return f;
    }

    template < typename Signature, typename Base >
    constexpr auto select(Signature Base::*f) noexcept {
        return f;
    }

    template < typename R, typename Base, typename... Args >
    constexpr auto select_const(R (Base::*f)(Args...) const) noexcept {
        return f;
    }

    template < typename R, typename Base, typename... Args >
    constexpr auto select_non_const(R(Base::*f)(Args...)) noexcept {
        return f;
    }
}

namespace meta_hpp::detail
{
    template < typename K, typename V, typename C, typename K2 >
    std::optional<V> find_opt(const std::map<K, V, C>& src, K2&& key) {
        if ( auto iter = src.find(key); iter != src.end() ) {
            return iter->second;
        }
        return std::nullopt;
    }

    template < typename K, typename V, typename C, typename K2, typename V2, typename F >
    void merge_with(std::map<K, V, C>& dst, K2&& key, V2&& value, F&& f) {
        if ( auto iter = dst.find(key); iter != dst.end() ) {
            std::invoke(std::forward<F>(f), iter->second, std::forward<V2>(value));
        } else {
            dst.emplace(std::forward<K2>(key), std::forward<V2>(value));
        }
    }

    template < typename K, typename V, typename C, typename F >
    void merge_with(std::map<K, V, C>& dst, const std::map<K, V, C>& src, F&& f) {
        for ( auto [key, value] : src ) {
            merge_with(dst, key, value, f);
        }
    }
}
