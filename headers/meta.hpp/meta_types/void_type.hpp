/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "_types_fwd.hpp"

namespace meta_hpp
{
    enum class void_flags : unsigned {
        is_const = 1 << 0,
    };

    ENUM_HPP_OPERATORS_DECL(void_flags)

    class void_type final : public base_type {
    public:
        void_type() = default;

        void_type(void_type&&) = default;
        void_type& operator=(void_type&&) = default;

        void_type(const void_type&) = default;
        void_type& operator=(const void_type&) = default;

        template < typename T >
        explicit void_type(typename_arg_t<T>);

        any_type raw_type() const noexcept;

        bitflags<void_flags> flags() const noexcept;
        bool is_const() const noexcept;
    private:
        struct state;
        std::shared_ptr<state> state_;
    };
}

namespace meta_hpp
{
    struct void_type::state final {
        const any_type raw_type;
        const bitflags<void_flags> flags;
    };
}

namespace meta_hpp::detail
{
    template < typename T, typename = void >
    struct void_traits;

    template < typename T >
    struct void_traits<T, std::enable_if_t<std::is_void_v<T>>> {
        static any_type make_raw_type(const void_type& self) {
            using raw_type = std::remove_const_t<T>;
            return std::is_same_v<T, raw_type>
                ? any_type{self}
                : type_db::get<raw_type>();
        }

        static bitflags<void_flags> make_flags() noexcept {
            bitflags<void_flags> flags;
            if ( std::is_const_v<T> ) flags.set(void_flags::is_const);
            return flags;
        }
    };
}

namespace meta_hpp
{
    template < typename T >
    inline void_type::void_type(typename_arg_t<T>)
    : base_type{typename_arg<T>}
    , state_{std::make_shared<state>(state{
        detail::void_traits<T>::make_raw_type(*this),
        detail::void_traits<T>::make_flags(),
    })} {
        static_assert(std::is_void_v<T>);
    }

    inline any_type void_type::raw_type() const noexcept {
        return state_->raw_type;
    }

    inline bitflags<void_flags> void_type::flags() const noexcept {
        return state_->flags;
    }

    inline bool void_type::is_const() const noexcept {
        return state_->flags.has(void_flags::is_const);
    }
}
