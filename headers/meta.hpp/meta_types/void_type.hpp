/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "_types_fwd.hpp"

namespace meta_hpp
{
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
        const bool is_const;
    };
}

namespace meta_hpp::detail
{
    template < typename T, typename = void >
    struct void_traits;

    template < typename T >
    struct void_traits<T, std::enable_if_t<std::is_void_v<T>>> {
        using raw_type = std::remove_const_t<T>;
        static constexpr bool is_const{std::is_const_v<T>};
    };
}

namespace meta_hpp
{
    template < typename T >
    inline void_type::void_type(typename_arg_t<T>)
    : base_type{typename_arg<T>}
    , state_{std::make_shared<state>(state{
        std::is_same_v<T, typename detail::void_traits<T>::raw_type>
            ? any_type{*this}
            : type_db::get<typename detail::void_traits<T>::raw_type>(),
        detail::void_traits<T>::is_const,
    })} {
        static_assert(std::is_void_v<T>);
    }

    inline any_type void_type::raw_type() const noexcept {
        return state_->raw_type;
    }

    inline bool void_type::is_const() const noexcept {
        return state_->is_const;
    }
}
