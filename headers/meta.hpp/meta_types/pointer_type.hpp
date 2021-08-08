/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "_types_fwd.hpp"

namespace meta_hpp
{
    enum class pointer_flags : unsigned {
        is_const = 1 << 0,
    };

    class pointer_type final : public type_base {
    public:
        pointer_type() = default;

        pointer_type(pointer_type&&) = default;
        pointer_type& operator=(pointer_type&&) = default;

        pointer_type(const pointer_type&) = default;
        pointer_type& operator=(const pointer_type&) = default;

        template < typename T >
        explicit pointer_type(typename_arg_t<T>);

        any_type data_type() const noexcept;

        bitflags<pointer_flags> flags() const noexcept;
        bool is_const() const noexcept;
    private:
        struct state;
        std::shared_ptr<state> state_;
    };
}

namespace meta_hpp::detail
{
    template < typename T >
    struct pointer_traits;

    template < typename T >
    struct pointer_traits<T*> {
        static any_type make_data_type() {
            using data_type = T;
            return type_db::get<data_type>();
        }

        static bitflags<pointer_flags> make_flags() noexcept {
            return bitflags<pointer_flags>{};
        }
    };

    template < typename T >
    struct pointer_traits<T* const> {
        static any_type make_data_type() {
            using data_type = T;
            return type_db::get<data_type>();
        }

        static bitflags<pointer_flags> make_flags() noexcept {
            return pointer_flags::is_const;
        }
    };
}

namespace meta_hpp
{
    struct pointer_type::state final {
        const any_type data_type;
        const bitflags<pointer_flags> flags;
    };

    template < typename T >
    pointer_type::pointer_type(typename_arg_t<T>)
    : type_base{typename_arg<struct pointer_type_tag, T>}
    , state_{std::make_shared<state>(state{
        detail::pointer_traits<T>::make_data_type(),
        detail::pointer_traits<T>::make_flags(),
    })} {
        static_assert(std::is_pointer_v<T>);
        static_assert(!std::is_function_v<std::remove_pointer_t<T>>);
    }

    inline any_type pointer_type::data_type() const noexcept {
        return state_->data_type;
    }

    inline bitflags<pointer_flags> pointer_type::flags() const noexcept {
        return state_->flags;
    }

    inline bool pointer_type::is_const() const noexcept {
        return state_->flags.has(pointer_flags::is_const);
    }
}
