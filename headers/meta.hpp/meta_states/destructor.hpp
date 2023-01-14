/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_states.hpp"

#include "../meta_types/destructor_type.hpp"
#include "../meta_detail/value_utilities/uarg.hpp"

namespace meta_hpp::detail
{
    template < class_kind Class >
    bool raw_destructor_destroy(const uarg& arg) {
        using dt = destructor_traits<Class>;
        using class_type = typename dt::class_type;

        if ( !arg.can_cast_to<class_type*>() ) {
            return false;
        }

        std::unique_ptr<class_type>{arg.cast<class_type*>()}.reset();
        return true;
    }

    template < class_kind Class >
    void raw_destructor_destroy_at(void* mem) {
        using dt = destructor_traits<Class>;
        using class_type = typename dt::class_type;

        std::destroy_at(static_cast<class_type*>(mem));
    }
}

namespace meta_hpp::detail
{
    template < class_kind Class >
    destructor_state::destroy_impl make_destructor_destroy() {
        return &raw_destructor_destroy<Class>;
    }

    template < class_kind Class >
    destructor_state::destroy_at_impl make_destructor_destroy_at() {
        return &raw_destructor_destroy_at<Class>;
    }
}

namespace meta_hpp::detail
{
    template < class_kind Class >
    destructor_state_ptr destructor_state::make(metadata_map metadata) {
        return std::make_shared<destructor_state>(destructor_state{
            .index{destructor_index::make<Class>()},
            .metadata{std::move(metadata)},
            .destroy{make_destructor_destroy<Class>()},
            .destroy_at{make_destructor_destroy_at<Class>()},
        });
    }
}

namespace meta_hpp
{
    inline destructor::destructor(state_ptr state) noexcept
    : state_{std::move(state)} {}

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
    bool destructor::destroy(Arg&& arg) const {
        using namespace detail;
        const uarg varg{std::forward<Arg>(arg)};
        return state_->destroy(varg);
    }

    inline void destructor::destroy_at(void* mem) const {
        state_->destroy_at(mem);
    }
}
