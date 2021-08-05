/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "_types_fwd.hpp"

namespace meta_hpp
{
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
        bool is_const() const noexcept;
    private:
        struct state;
        std::shared_ptr<state> state_;
    };
}

namespace meta_hpp
{
    struct enum_type::state final {
        const any_type raw_type;
        const any_type underlying_type;
        const bool is_const;
    };
}

namespace meta_hpp::detail
{
    template < typename T, typename = void >
    struct enum_traits;

    template < typename T >
    struct enum_traits<T, std::enable_if_t<std::is_enum_v<T>>> {
        using raw_type = std::remove_const_t<T>;
        using underlying_type = std::underlying_type_t<T>;
        static constexpr bool is_const{std::is_const_v<T>};
    };
}

namespace meta_hpp
{
    template < typename T >
    inline enum_type::enum_type(typename_arg_t<T>)
    : base_type{typename_arg<T>}
    , state_{std::make_shared<state>(state{
        std::is_same_v<T, typename detail::enum_traits<T>::raw_type>
            ? any_type{*this}
            : type_db::get<typename detail::enum_traits<T>::raw_type>(),
        type_db::get<typename detail::enum_traits<T>::underlying_type>(),
        detail::enum_traits<T>::is_const,
    })} {
        static_assert(std::is_enum_v<T>);
    }

    inline any_type enum_type::raw_type() const noexcept {
        return state_->raw_type;
    }

    inline any_type enum_type::underlying_type() const noexcept {
        return state_->underlying_type;
    }

    inline bool enum_type::is_const() const noexcept {
        return state_->is_const;
    }
}
