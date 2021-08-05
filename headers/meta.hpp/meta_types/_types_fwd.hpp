/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_fwd.hpp"

namespace meta_hpp
{
    class type_id final {
    public:
        type_id() = default;

        type_id(type_id&&) = default;
        type_id& operator=(type_id&&) = default;

        type_id(const type_id&) = default;
        type_id& operator=(const type_id&) = default;

        template < typename T >
        explicit type_id(typename_arg_t<T>) noexcept
        : id_{type_to_id<T>()} {}

        explicit operator bool() const noexcept {
            return !!id_;
        }

        std::size_t hash() const noexcept {
            return std::hash<underlying_type>{}(id_);
        }

        friend bool operator<(type_id l, type_id r) noexcept {
            return l.id_ < r.id_;
        }

        friend bool operator==(type_id l, type_id r) noexcept {
            return l.id_ == r.id_;
        }

        friend bool operator!=(type_id l, type_id r) noexcept {
            return l.id_ != r.id_;
        }
    private:
        using underlying_type = std::size_t;
        underlying_type id_{0u};
    private:
        static underlying_type next() noexcept {
            static std::atomic<underlying_type> id{};
            return ++id;
        }

        template < typename T >
        static underlying_type type_to_id() noexcept {
            static const underlying_type id{next()};
            return id;
        }
    };
}

namespace std
{
    template <>
    struct hash<meta_hpp::type_id> {
        size_t operator()(meta_hpp::type_id id) const noexcept {
            return id.hash();
        }
    };
}

namespace meta_hpp
{
    class base_type {
    public:
        template < typename... Ts >
        struct tag {};
    public:
        base_type() = default;
        virtual ~base_type() = default;

        base_type(base_type&&) = default;
        base_type& operator=(base_type&&) = default;

        base_type(const base_type&) = default;
        base_type& operator=(const base_type&) = default;

        template < typename... Ts >
        explicit base_type(typename_arg_t<Ts...>)
        : id_{typename_arg<tag<Ts...>>} {}

        type_id id() const noexcept {
            return id_;
        }

        explicit operator bool() const noexcept {
            return !!id_;
        }
    private:
        type_id id_;
    };
}

namespace meta_hpp
{
    class any_type final {
    public:
        any_type() = default;

        any_type(any_type&&) = default;
        any_type& operator=(any_type&&) = default;

        any_type(const any_type&) = default;
        any_type& operator=(const any_type&) = default;

        template < typename T, std::enable_if_t<std::is_base_of_v<base_type, T>, int> = 0 >
        any_type(const T& type)
        : id_{type.id()}
        , type_{std::addressof(type)} {}

        type_id id() const noexcept {
            return id_;
        }

        explicit operator bool() const noexcept {
            return !!id_;
        }

        template < typename T >
        std::enable_if_t<std::is_base_of_v<base_type, T>, T> as() const noexcept {
            using Tptr = std::add_pointer_t<std::add_const_t<T>>;
            return is<T>() ? *std::get<Tptr>(type_) : T{};
        }

        template < typename T >
        std::enable_if_t<std::is_base_of_v<base_type, T>, bool> is() const noexcept {
            using Tptr = std::add_pointer_t<std::add_const_t<T>>;
            return std::holds_alternative<Tptr>(type_);
        }
    private:
        type_id id_;
        std::variant<
            const arithmetic_type*,
            const array_type*,
            const class_type*,
            const ctor_type*,
            const enum_type*,
            const function_type*,
            const member_type*,
            const method_type*,
            const pointer_type*,
            const reference_type*,
            const void_type*> type_;
    };
}

namespace meta_hpp
{
    class type_db final {
    public:
        type_db() = delete;

        template < typename T >
        static any_type get();

        template < typename Tuple >
        static std::vector<any_type> multi_get();
    };
}
