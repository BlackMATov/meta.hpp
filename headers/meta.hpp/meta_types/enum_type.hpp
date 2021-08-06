/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "_types_fwd.hpp"

namespace meta_hpp
{
    enum class enum_flags : unsigned {
        is_const = 1 << 0,
    };

    ENUM_HPP_OPERATORS_DECL(enum_flags)

    class enum_type final : public base_type {
    public:
        enum_type() = default;

        enum_type(enum_type&&) = default;
        enum_type& operator=(enum_type&&) = default;

        enum_type(const enum_type&) = default;
        enum_type& operator=(const enum_type&) = default;

        template < typename T >
        explicit enum_type(typename_arg_t<T>);

        any_type raw_type() const noexcept;
        any_type underlying_type() const noexcept;

        bitflags<enum_flags> flags() const noexcept;
        bool is_const() const noexcept;
    private:
        struct state;
        std::shared_ptr<state> state_;
    };
}

namespace meta_hpp::detail
{
    template < typename T, typename = void >
    struct enum_traits;

    template < typename T >
    struct enum_traits<T, std::enable_if_t<std::is_enum_v<T>>> {
        static any_type make_raw_type(const enum_type& self) {
            using raw_type = std::remove_const_t<T>;
            return std::is_same_v<T, raw_type>
                ? any_type{self}
                : type_db::get<raw_type>();
        }

        static any_type make_underlying_type() {
            using underlying_type = std::underlying_type_t<T>;
            return type_db::get<underlying_type>();
        }

        static bitflags<enum_flags> make_flags() noexcept {
            bitflags<enum_flags> flags;
            if ( std::is_const_v<T> ) flags.set(enum_flags::is_const);
            return flags;
        }
    };
}

namespace meta_hpp
{
    struct enum_type::state final {
        const any_type raw_type;
        const any_type underlying_type;
        const bitflags<enum_flags> flags;
    };

    template < typename T >
    inline enum_type::enum_type(typename_arg_t<T>)
    : base_type{typename_arg<T>}
    , state_{std::make_shared<state>(state{
        detail::enum_traits<T>::make_raw_type(*this),
        detail::enum_traits<T>::make_underlying_type(),
        detail::enum_traits<T>::make_flags(),
    })} {
        static_assert(std::is_enum_v<T>);
    }

    inline any_type enum_type::raw_type() const noexcept {
        return state_->raw_type;
    }

    inline any_type enum_type::underlying_type() const noexcept {
        return state_->underlying_type;
    }

    inline bitflags<enum_flags> enum_type::flags() const noexcept {
        return state_->flags;
    }

    inline bool enum_type::is_const() const noexcept {
        return state_->flags.has(enum_flags::is_const);
    }
}
