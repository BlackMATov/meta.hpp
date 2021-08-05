/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "_types_fwd.hpp"

namespace meta_hpp
{
    class reference_type final : public base_type {
    public:
        reference_type() = default;

        reference_type(reference_type&&) = default;
        reference_type& operator=(reference_type&&) = default;

        reference_type(const reference_type&) = default;
        reference_type& operator=(const reference_type&) = default;

        template < typename T >
        explicit reference_type(typename_arg_t<T>);

        any_type data_type() const noexcept;
        bool is_lvalue() const noexcept;
        bool is_rvalue() const noexcept;
    private:
        struct state;
        std::shared_ptr<state> state_;
    };
}

namespace meta_hpp
{
    struct reference_type::state final {
        const any_type data_type;
        const bool is_lvalue;
        const bool is_rvalue;
    };
}

namespace meta_hpp::detail
{
    template < typename T >
    struct reference_traits;

    template < typename T >
    struct reference_traits<T&> {
        using data_type = T;
        static constexpr bool is_lvalue{true};
        static constexpr bool is_rvalue{false};
    };

    template < typename T >
    struct reference_traits<T&&> {
        using data_type = T;
        static constexpr bool is_lvalue{false};
        static constexpr bool is_rvalue{true};
    };
}

namespace meta_hpp
{
    template < typename T >
    inline reference_type::reference_type(typename_arg_t<T>)
    : base_type{typename_arg<T>}
    , state_{std::make_shared<state>(state{
        type_db::get<typename detail::reference_traits<T>::data_type>(),
        detail::reference_traits<T>::is_lvalue,
        detail::reference_traits<T>::is_rvalue,
    })} {
        static_assert(std::is_reference_v<T>);
    }

    inline any_type reference_type::data_type() const noexcept {
        return state_->data_type;
    }

    inline bool reference_type::is_lvalue() const noexcept {
        return state_->is_lvalue;
    }

    inline bool reference_type::is_rvalue() const noexcept {
        return state_->is_rvalue;
    }
}
