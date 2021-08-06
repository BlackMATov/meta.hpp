/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "_types_fwd.hpp"

namespace meta_hpp
{
    enum class arithmetic_flags : unsigned {
        is_const = 1 << 0,
        is_signed = 1 << 1,
        is_unsigned = 1 << 2,
        is_integral = 1 << 3,
        is_floating_point = 1 << 4,
    };

    ENUM_HPP_OPERATORS_DECL(arithmetic_flags)

    class arithmetic_type final : public base_type {
    public:
        arithmetic_type() = default;

        arithmetic_type(arithmetic_type&&) = default;
        arithmetic_type& operator=(arithmetic_type&&) = default;

        arithmetic_type(const arithmetic_type&) = default;
        arithmetic_type& operator=(const arithmetic_type&) = default;

        template < typename T >
        explicit arithmetic_type(typename_arg_t<T>);

        std::size_t size() const noexcept;
        any_type raw_type() const noexcept;

        bitflags<arithmetic_flags> flags() const noexcept;
        bool is_const() const noexcept;
        bool is_signed() const noexcept;
        bool is_unsigned() const noexcept;
        bool is_integral() const noexcept;
        bool is_floating_point() const noexcept;
    private:
        struct state;
        std::shared_ptr<state> state_;
    };
}

namespace meta_hpp::detail
{
    template < typename T, typename = void >
    struct arithmetic_traits;

    template < typename T >
    struct arithmetic_traits<T, std::enable_if_t<std::is_arithmetic_v<T>>> {
        static constexpr std::size_t size{sizeof(T)};

        static any_type make_raw_type(const arithmetic_type& self) {
            using raw_type = std::remove_const_t<T>;
            return std::is_same_v<T, raw_type>
                ? any_type{self}
                : type_db::get<raw_type>();
        }

        static bitflags<arithmetic_flags> make_flags() noexcept {
            bitflags<arithmetic_flags> flags;
            if ( std::is_const_v<T> ) flags.set(arithmetic_flags::is_const);
            if ( std::is_signed_v<T> ) flags.set(arithmetic_flags::is_signed);
            if ( std::is_unsigned_v<T> ) flags.set(arithmetic_flags::is_unsigned);
            if ( std::is_integral_v<T> ) flags.set(arithmetic_flags::is_integral);
            if ( std::is_floating_point_v<T> ) flags.set(arithmetic_flags::is_floating_point);
            return flags;
        }
    };
}

namespace meta_hpp
{
    struct arithmetic_type::state final {
        const std::size_t size;
        const any_type raw_type;
        const bitflags<arithmetic_flags> flags;
    };

    template < typename T >
    inline arithmetic_type::arithmetic_type(typename_arg_t<T>)
    : base_type{typename_arg<T>}
    , state_{std::make_shared<state>(state{
        detail::arithmetic_traits<T>::size,
        detail::arithmetic_traits<T>::make_raw_type(*this),
        detail::arithmetic_traits<T>::make_flags(),
    })} {
        static_assert(std::is_arithmetic_v<T>);
    }

    inline std::size_t arithmetic_type::size() const noexcept {
        return state_->size;
    }

    inline any_type arithmetic_type::raw_type() const noexcept {
        return state_->raw_type;
    }

    inline bitflags<arithmetic_flags> arithmetic_type::flags() const noexcept {
        return state_->flags;
    }

    inline bool arithmetic_type::is_const() const noexcept {
        return state_->flags.has(arithmetic_flags::is_const);
    }

    inline bool arithmetic_type::is_signed() const noexcept {
        return state_->flags.has(arithmetic_flags::is_signed);
    }

    inline bool arithmetic_type::is_unsigned() const noexcept {
        return state_->flags.has(arithmetic_flags::is_unsigned);
    }

    inline bool arithmetic_type::is_integral() const noexcept {
        return state_->flags.has(arithmetic_flags::is_integral);
    }

    inline bool arithmetic_type::is_floating_point() const noexcept {
        return state_->flags.has(arithmetic_flags::is_floating_point);
    }
}
