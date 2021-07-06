/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include <any>
#include <array>
#include <atomic>
#include <cassert>
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
#include <variant>

#define META_HPP_AUTO_T(V) decltype(V), V

namespace meta_hpp
{
    template < typename... Ts >
    struct overloaded : Ts... {
        using Ts::operator()...;
    };

    template < typename... Ts >
    overloaded(Ts...) -> overloaded<Ts...>;
}

namespace meta_hpp
{
    struct family_id {
        using underlying_type = std::size_t;
        underlying_type id{};

        friend bool operator<(family_id l, family_id r) noexcept {
            return l.id < r.id;
        }

        friend bool operator==(family_id l, family_id r) noexcept {
            return l.id == r.id;
        }

        friend bool operator!=(family_id l, family_id r) noexcept {
            return l.id != r.id;
        }
    };

    namespace family_id_detail
    {
        struct family_id_sequence {
            static family_id next() noexcept {
                static std::atomic<family_id::underlying_type> id{};
                return { ++id };
            }
        };

        template < typename T >
        struct type_to_family_id {
            static family_id id() noexcept {
                static const family_id type_family_id = family_id_sequence::next();
                assert(type_family_id.id > 0u && "family_id overflow");
                return type_family_id;
            }
        };
    }

    template < typename T >
    family_id get_family_id() noexcept {
        return family_id_detail::type_to_family_id<T>::id();
    }
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
    template < typename TArg, TArg Arg >
    struct auto_arg_t {
    };

    template < typename Arg >
    struct typename_arg_t {
    };

    template < typename TArg, TArg Arg >
    inline auto_arg_t<TArg, Arg> auto_arg;

    template < typename Arg >
    inline typename_arg_t<Arg> typename_arg;
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

namespace meta_hpp::detail
{
    template < typename Field >
    struct field_traits;

    template < typename T, typename I >
    struct field_traits<T I::*> {
        static constexpr bool is_const = false;
        using value_type = T;
        using instance_type = I;
    };

    template < typename T, typename I >
    struct field_traits<const T I::*>
         : field_traits<T I::*> {
        static constexpr bool is_const = true;
    };
}

namespace meta_hpp::detail
{
    template < typename Function >
    struct function_traits;

    template < typename R, typename... Args >
    struct function_traits<R(Args...)> {
        static constexpr bool is_noexcept = false;
        static constexpr std::size_t arity = sizeof...(Args);
        using return_type = R;
        using argument_types = std::tuple<Args...>;
    };

    template < typename R, typename... Args >
    struct function_traits<R(*)(Args...)>
         : function_traits<R(Args...)> {};

    template < typename R, typename... Args >
    struct function_traits<R(&)(Args...)>
         : function_traits<R(Args...)> {};

    template < typename R, typename... Args >
    struct function_traits<R(*)(Args...) noexcept>
         : function_traits<R(*)(Args...)> {
        static constexpr bool is_noexcept = true;
    };

    template < typename R, typename... Args >
    struct function_traits<R(&)(Args...) noexcept>
         : function_traits<R(&)(Args...)> {
        static constexpr bool is_noexcept = true;
    };
}

namespace meta_hpp::detail
{
    template < typename Method >
    struct method_traits;

    template < typename R, typename I, typename... Args >
    struct method_traits<R(I::*)(Args...)>
         : function_traits<R(Args...)> {
        static constexpr bool is_const = false;
        using instance_type = I;
    };

    template < typename R, typename I, typename... Args >
    struct method_traits<R(I::*)(Args...) const>
         : method_traits<R(I::*)(Args...)> {
        static constexpr bool is_const = true;
    };

    template < typename R, typename I, typename... Args >
    struct method_traits<R(I::*)(Args...) noexcept>
         : method_traits<R(I::*)(Args...)> {
        static constexpr bool is_noexcept = true;
    };

    template < typename R, typename I, typename... Args >
    struct method_traits<R(I::*)(Args...) const noexcept>
         : method_traits<R(I::*)(Args...)> {
        static constexpr bool is_const = true;
        static constexpr bool is_noexcept = true;
    };
}

namespace meta_hpp::detail
{
    template < typename Variable >
    struct variable_traits;

    template < typename T >
    struct variable_traits<T*> {
        static constexpr bool is_const = false;
        using value_type = T;
    };

    template < typename T >
    struct variable_traits<const T*>
         : variable_traits<T*> {
        static constexpr bool is_const = true;
    };
}
