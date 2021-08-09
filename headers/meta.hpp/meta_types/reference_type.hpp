/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "_types_fwd.hpp"

namespace meta_hpp
{
    enum class reference_flags : unsigned {
        is_lvalue = 1 << 0,
        is_rvalue = 1 << 1,
    };

    ENUM_HPP_OPERATORS_DECL(reference_flags)

    class reference_type final : public type_base {
    public:
        reference_type() = default;

        reference_type(reference_type&&) = default;
        reference_type& operator=(reference_type&&) = default;

        reference_type(const reference_type&) = default;
        reference_type& operator=(const reference_type&) = default;

        template < typename T >
        explicit reference_type(typename_arg_t<T>);

        any_type data_type() const noexcept;

        bitflags<reference_flags> flags() const noexcept;
        bool is_lvalue() const noexcept;
        bool is_rvalue() const noexcept;
    private:
        struct state;
        std::shared_ptr<state> state_;
    };
}

namespace meta_hpp::detail
{
    template < typename T >
    struct reference_traits {
        static_assert(std::is_reference_v<T>);
        using data_type = std::remove_reference_t<T>;

        static any_type make_data_type() {
            return type_db::get<data_type>();
        }

        static bitflags<reference_flags> make_flags() noexcept {
            bitflags<reference_flags> flags;
            if ( std::is_lvalue_reference_v<T> ) flags.set(reference_flags::is_lvalue);
            if ( std::is_rvalue_reference_v<T> ) flags.set(reference_flags::is_rvalue);
            return flags;
        }
    };
}

namespace meta_hpp
{
    struct reference_type::state final {
        const any_type data_type;
        const bitflags<reference_flags> flags;
    };

    template < typename T >
    reference_type::reference_type(typename_arg_t<T>)
    : type_base{typename_arg<struct reference_type_tag, T>}
    , state_{std::make_shared<state>(state{
        detail::reference_traits<T>::make_data_type(),
        detail::reference_traits<T>::make_flags(),
    })} {
        static_assert(std::is_reference_v<T>);
    }

    inline any_type reference_type::data_type() const noexcept {
        return state_->data_type;
    }

    inline bitflags<reference_flags> reference_type::flags() const noexcept {
        return state_->flags;
    }

    inline bool reference_type::is_lvalue() const noexcept {
        return state_->flags.has(reference_flags::is_lvalue);
    }

    inline bool reference_type::is_rvalue() const noexcept {
        return state_->flags.has(reference_flags::is_rvalue);
    }
}
