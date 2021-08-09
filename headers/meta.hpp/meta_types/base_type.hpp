/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "_types_fwd.hpp"

namespace meta_hpp
{
    class base_type final : public type_base {
    public:
        base_type() = default;

        base_type(base_type&&) = default;
        base_type& operator=(base_type&&) = default;

        base_type(const base_type&) = default;
        base_type& operator=(const base_type&) = default;

        template < typename Base, typename Derived >
        explicit base_type(typename_arg_t<Base>, typename_arg_t<Derived>);

        any_type base_class_type() const noexcept;
        any_type derived_class_type() const noexcept;
    private:
        struct state;
        std::shared_ptr<state> state_;
    };
}

namespace meta_hpp::detail
{
    template < typename Base, typename Derived >
    struct base_traits {
        static_assert(std::is_base_of_v<Base, Derived>);

        using base_class_type = Base;
        using derived_class_type = Derived;

        static any_type make_base_class_type() {
            return type_db::get<base_class_type>();
        }

        static any_type make_derived_class_type() {
            return type_db::get<derived_class_type>();
        }
    };
}

namespace meta_hpp
{
    struct base_type::state final {
        const any_type base_class_type;
        const any_type derived_class_type;
    };

    template < typename Base, typename Derived >
    base_type::base_type(typename_arg_t<Base>, typename_arg_t<Derived>)
    : type_base{typename_arg<struct base_type_tag, Base, Derived>}
    , state_{std::make_shared<state>(state{
        detail::base_traits<Base, Derived>::make_base_class_type(),
        detail::base_traits<Base, Derived>::make_derived_class_type(),
    })} {
        static_assert(std::is_base_of_v<Base, Derived>);
    }

    inline any_type base_type::base_class_type() const noexcept {
        return state_->base_class_type;
    }

    inline any_type base_type::derived_class_type() const noexcept {
        return state_->derived_class_type;
    }
}
