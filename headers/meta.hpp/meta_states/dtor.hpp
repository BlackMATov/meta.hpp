/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_states.hpp"

#include "../meta_types/dtor_type.hpp"
#include "../meta_detail/value_utilities/arg.hpp"

namespace meta_hpp::detail
{
    template < class_kind Class >
    void raw_dtor_invoke(const arg& ptr) {
        using dt = dtor_traits<Class>;
        using class_type = typename dt::class_type;

        if ( !ptr.can_cast_to<class_type*>() ) {
            throw_exception_with("an attempt to call a destructor with an incorrect argument type");
        }

        class_type* raw_ptr = ptr.cast<class_type*>();
        if ( raw_ptr ) {
            raw_ptr->~Class();
        }
    }

    template < class_kind Class >
    bool raw_dtor_is_invocable_with(const arg_base& ptr) {
        using dt = dtor_traits<Class>;
        using class_type = typename dt::class_type;

        return ptr.can_cast_to<class_type*>();
    }
}

namespace meta_hpp::detail
{
    template < class_kind Class >
    dtor_state::invoke_impl make_dtor_invoke() {
        return &raw_dtor_invoke<Class>;
    }

    template < class_kind Class >
    dtor_state::is_invocable_with_impl make_dtor_is_invocable_with() {
        return &raw_dtor_is_invocable_with<Class>;
    }
}

namespace meta_hpp::detail
{
    template < class_kind Class >
    dtor_state_ptr dtor_state::make() {
        return std::make_shared<dtor_state>(dtor_state{
            .index{dtor_index::make<Class>()},
            .invoke{make_dtor_invoke<Class>()},
            .is_invocable_with{make_dtor_is_invocable_with<Class>()},
        });
    }
}

namespace meta_hpp
{
    inline dtor::dtor(detail::dtor_state_ptr state)
    : state_{std::move(state)} {}

    inline bool dtor::is_valid() const noexcept {
        return !!state_;
    }

    inline dtor::operator bool() const noexcept {
        return is_valid();
    }

    inline const dtor_index& dtor::get_index() const noexcept {
        return state_->index;
    }

    inline const dtor_type& dtor::get_type() const noexcept {
        return state_->index.get_type();
    }

    template < typename Arg >
    void dtor::invoke(Arg&& ptr) const {
        using namespace detail;
        const arg varg{std::forward<Arg>(ptr)};
        state_->invoke(varg);
    }

    template < typename Arg >
    void dtor::operator()(Arg&& ptr) const {
        invoke(std::forward<Arg>(ptr));
    }

    template < typename Arg >
    bool dtor::is_invocable_with() const noexcept {
        using namespace detail;
        const arg_base varg{type_list<Arg>{}};
        return state_->is_invocable_with(varg);
    }

    template < typename Arg >
    bool dtor::is_invocable_with(Arg&& ptr) const noexcept {
        using namespace detail;
        const arg_base varg{std::forward<Arg>(ptr)};
        return state_->is_invocable_with(varg);
    }
}
