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
#include <vector>

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
    class family_id {
    public:
        family_id(const family_id&) = default;
        family_id& operator=(const family_id&) = default;

        friend bool operator<(family_id l, family_id r) noexcept {
            return l.id_ < r.id_;
        }

        friend bool operator==(family_id l, family_id r) noexcept {
            return l.id_ == r.id_;
        }

        friend bool operator!=(family_id l, family_id r) noexcept {
            return l.id_ != r.id_;
        }
    private:
        using underlying_type = std::size_t;
        underlying_type id_{};
    private:
        template < typename T >
        friend family_id get_family_id() noexcept;

        family_id() noexcept
        : id_{next()} {}

        static underlying_type next() noexcept {
            static std::atomic<underlying_type> id{};
            return ++id;
        }
    };

    template < typename T >
    family_id get_family_id() noexcept {
        static const family_id id = family_id{};
        return id;
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
    template < typename Arg >
    struct typename_arg_t {
    };

    template < typename... Args >
    struct typename_args_t {
    };

    template < typename Arg >
    inline typename_arg_t<Arg> typename_arg;

    template < typename... Args >
    inline typename_args_t<Args...> typename_args;
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
    template < typename Class, typename... Args >
    struct ctor_traits {
        static constexpr std::size_t arity = sizeof...(Args);
        using return_type = std::decay_t<Class>;
        using argument_types = std::tuple<std::decay_t<Args>...>;
    };
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
        using return_type = std::decay_t<R>;
        using argument_types = std::tuple<std::decay_t<Args>...>;
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
