/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "_types_fwd.hpp"

namespace meta_hpp
{
    class member_type final : public type_base {
    public:
        member_type() = default;

        member_type(member_type&&) = default;
        member_type& operator=(member_type&&) = default;

        member_type(const member_type&) = default;
        member_type& operator=(const member_type&) = default;

        template < typename T >
        explicit member_type(typename_arg_t<T>);

        any_type class_type() const noexcept;
        any_type value_type() const noexcept;
    private:
        struct state;
        std::shared_ptr<state> state_;
    };
}

namespace meta_hpp
{
    struct member_type::state final {
        const any_type class_type;
        const any_type value_type;
    };
}

namespace meta_hpp::detail
{
    template < typename T >
    struct member_pointer_traits;

    template < typename V, typename C >
    struct member_pointer_traits<V C::*> {
        static any_type make_class_type() {
            using class_type = C;
            return type_db::get<class_type>();
        }

        static any_type make_value_type() {
            using value_type = V;
            return type_db::get<value_type>();
        }
    };
}

namespace meta_hpp
{
    template < typename T >
    inline member_type::member_type(typename_arg_t<T>)
    : type_base{typename_arg<T>}
    , state_{std::make_shared<state>(state{
        detail::member_pointer_traits<T>::make_class_type(),
        detail::member_pointer_traits<T>::make_value_type(),
    })} {
        static_assert(std::is_member_object_pointer_v<T>);
    }

    inline any_type member_type::class_type() const noexcept {
        return state_->class_type;
    }

    inline any_type member_type::value_type() const noexcept {
        return state_->value_type;
    }
}
