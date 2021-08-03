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
        type_id() = delete;

        type_id(type_id&&) = default;
        type_id& operator=(type_id&&) = default;

        type_id(const type_id&) = default;
        type_id& operator=(const type_id&) = default;

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
        template < typename T >
        friend type_id make_type_id() noexcept;

        template < typename T >
        explicit type_id(typename_arg_t<T>) noexcept
        : id_{next()} {}

        static underlying_type next() noexcept {
            static std::atomic<underlying_type> id{};
            return ++id;
        }
    };

    template < typename T >
    type_id make_type_id() noexcept {
        static const type_id id = type_id{typename_arg<T>};
        return id;
    }
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
        template < typename... Ts >
        explicit base_type(typename_arg_t<Ts...>)
        : id_{make_type_id<tag<Ts...>>()} {}

        type_id id() const noexcept {
            return id_;
        }
    private:
        type_id id_;
    };
}
