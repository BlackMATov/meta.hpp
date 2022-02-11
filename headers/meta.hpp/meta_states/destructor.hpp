/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_states.hpp"

#include "../meta_types/destructor_type.hpp"
#include "../meta_detail/value_utilities/uarg.hpp"

namespace meta_hpp::detail
{
    template < class_kind Class >
    void raw_destructor_invoke(const uarg& ptr) {
        using dt = destructor_traits<Class>;
        using class_type = typename dt::class_type;

        if ( !ptr.can_cast_to<class_type*>() ) {
            throw_exception_with("an attempt to call a destructor with an incorrect argument type");
        }

        class_type* raw_ptr = ptr.cast<class_type*>();
        if ( raw_ptr ) {
            std::unique_ptr<class_type>{raw_ptr}.reset();
        }
    }

    template < class_kind Class >
    bool raw_destructor_is_invocable_with(const uarg_base& ptr) {
        using dt = destructor_traits<Class>;
        using class_type = typename dt::class_type;

        return ptr.can_cast_to<class_type*>();
    }
}

namespace meta_hpp::detail
{
    template < class_kind Class >
    destructor_state::invoke_impl make_destructor_invoke() {
        return &raw_destructor_invoke<Class>;
    }

    template < class_kind Class >
    destructor_state::is_invocable_with_impl make_destructor_is_invocable_with() {
        return &raw_destructor_is_invocable_with<Class>;
    }
}

namespace meta_hpp::detail
{
    template < class_kind Class >
    destructor_state_ptr destructor_state::make(metadata_map metadata) {
        return std::make_shared<destructor_state>(destructor_state{
            .index{destructor_index::make<Class>()},
            .metadata{std::move(metadata)},
            .invoke{make_destructor_invoke<Class>()},
            .is_invocable_with{make_destructor_is_invocable_with<Class>()},
        });
    }
}

namespace meta_hpp
{
    inline destructor::destructor(detail::destructor_state_ptr state) noexcept
    : state_{std::move(state)} {}

    inline destructor& destructor::operator=(detail::destructor_state_ptr state) noexcept {
        state_ = std::move(state);
        return *this;
    }

    inline bool destructor::is_valid() const noexcept {
        return !!state_;
    }

    inline destructor::operator bool() const noexcept {
        return is_valid();
    }

    inline const destructor_index& destructor::get_index() const noexcept {
        return state_->index;
    }

    inline const metadata_map& destructor::get_metadata() const noexcept {
        return state_->metadata;
    }

    inline const destructor_type& destructor::get_type() const noexcept {
        return state_->index.get_type();
    }

    template < typename Arg >
    void destructor::invoke(Arg&& ptr) const {
        using namespace detail;
        const uarg varg{std::forward<Arg>(ptr)};
        state_->invoke(varg);
    }

    template < typename Arg >
    void destructor::operator()(Arg&& ptr) const {
        invoke(std::forward<Arg>(ptr));
    }

    template < typename Arg >
    bool destructor::is_invocable_with() const noexcept {
        using namespace detail;
        const uarg_base varg{type_list<Arg>{}};
        return state_->is_invocable_with(varg);
    }

    template < typename Arg >
    bool destructor::is_invocable_with(Arg&& ptr) const noexcept {
        using namespace detail;
        const uarg_base varg{std::forward<Arg>(ptr)};
        return state_->is_invocable_with(varg);
    }
}
