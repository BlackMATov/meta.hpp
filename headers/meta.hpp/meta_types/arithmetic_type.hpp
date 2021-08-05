/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "_types_fwd.hpp"

namespace meta_hpp
{
    class arithmetic_type final : public base_type {
    public:
        arithmetic_type() = default;

        arithmetic_type(arithmetic_type&&) = default;
        arithmetic_type& operator=(arithmetic_type&&) = default;

        arithmetic_type(const arithmetic_type&) = default;
        arithmetic_type& operator=(const arithmetic_type&) = default;

        template < typename T >
        explicit arithmetic_type(typename_arg_t<T>);

        any_type raw_type() const noexcept;
        std::size_t size() const noexcept;
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

namespace meta_hpp
{
    struct arithmetic_type::state final {
        const any_type raw_type;
        const std::size_t size;
        const bool is_const;
        const bool is_signed;
        const bool is_unsigned;
        const bool is_integral;
        const bool is_floating_point;
    };
}

namespace meta_hpp::detail
{
    template < typename T, typename = void >
    struct arithmetic_traits;

    template < typename T >
    struct arithmetic_traits<T, std::enable_if_t<std::is_arithmetic_v<T>>> {
        using raw_type = std::remove_const_t<T>;
        static constexpr std::size_t size{sizeof(T)};
        static constexpr bool is_const{std::is_const_v<T>};
        static constexpr bool is_signed{std::is_signed_v<T>};
        static constexpr bool is_unsigned{std::is_unsigned_v<T>};
        static constexpr bool is_integral{std::is_integral_v<T>};
        static constexpr bool is_floating_point{std::is_floating_point_v<T>};
    };
}

namespace meta_hpp
{
    template < typename T >
    inline arithmetic_type::arithmetic_type(typename_arg_t<T>)
    : base_type{typename_arg<T>}
    , state_{std::make_shared<state>(state{
        std::is_same_v<T, typename detail::arithmetic_traits<T>::raw_type>
            ? any_type{*this}
            : type_db::get<typename detail::arithmetic_traits<T>::raw_type>(),
        detail::arithmetic_traits<T>::size,
        detail::arithmetic_traits<T>::is_const,
        detail::arithmetic_traits<T>::is_signed,
        detail::arithmetic_traits<T>::is_unsigned,
        detail::arithmetic_traits<T>::is_integral,
        detail::arithmetic_traits<T>::is_floating_point,
    })} {
        static_assert(std::is_arithmetic_v<T>);
    }

    inline any_type arithmetic_type::raw_type() const noexcept {
        return state_->raw_type;
    }

    inline std::size_t arithmetic_type::size() const noexcept {
        return state_->size;
    }

    inline bool arithmetic_type::is_const() const noexcept {
        return state_->is_const;
    }

    inline bool arithmetic_type::is_signed() const noexcept {
        return state_->is_signed;
    }

    inline bool arithmetic_type::is_unsigned() const noexcept {
        return state_->is_unsigned;
    }

    inline bool arithmetic_type::is_integral() const noexcept {
        return state_->is_integral;
    }

    inline bool arithmetic_type::is_floating_point() const noexcept {
        return state_->is_floating_point;
    }
}
