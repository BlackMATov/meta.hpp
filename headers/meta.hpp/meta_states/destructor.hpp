/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_states.hpp"

#include "../meta_detail/type_registry.hpp"
#include "../meta_detail/value_utilities/uarg.hpp"
#include "../meta_types/destructor_type.hpp"

namespace meta_hpp::detail
{
    template < class_kind Class >
    void raw_destructor_destroy(type_registry& registry, const uarg& arg) {
        using dt = destructor_traits<Class>;
        using class_type = typename dt::class_type;

        META_HPP_ASSERT(                           //
            arg.can_cast_to<class_type*>(registry) //
            && "an attempt to call a destructor with an incorrect argument type"
        );

        std::unique_ptr<class_type>{arg.cast<class_type*>(registry)}.reset();
    }

    template < class_kind Class >
    void raw_destructor_destroy_at(void* mem) {
        using dt = destructor_traits<Class>;
        using class_type = typename dt::class_type;

        std::destroy_at(static_cast<class_type*>(mem));
    }

    template < class_kind Class >
    uerror raw_destructor_destroy_error(type_registry& registry, const uarg_base& arg) noexcept {
        using dt = destructor_traits<Class>;
        using class_type = typename dt::class_type;

        if ( !arg.can_cast_to<class_type*>(registry) ) {
            return uerror{error_code::argument_type_mismatch};
        }

        return uerror{error_code::no_error};
    }
}

namespace meta_hpp::detail
{
    template < class_kind Class >
    destructor_state::destroy_impl make_destructor_destroy(type_registry& registry) {
        return [&registry](const uarg& arg) { //
            return raw_destructor_destroy<Class>(registry, arg);
        };
    }

    template < class_kind Class >
    destructor_state::destroy_at_impl make_destructor_destroy_at() {
        return &raw_destructor_destroy_at<Class>;
    }

    template < class_kind Class >
    destructor_state::destroy_error_impl make_destructor_destroy_error(type_registry& registry) {
        return [&registry](const uarg_base& arg) { //
            return raw_destructor_destroy_error<Class>(registry, arg);
        };
    }
}

namespace meta_hpp::detail
{
    inline destructor_state::destructor_state(destructor_index nindex, metadata_map nmetadata)
    : index{nindex}
    , metadata{std::move(nmetadata)} {}

    template < class_kind Class >
    destructor_state::state_ptr destructor_state::make(metadata_map metadata) {
        type_registry& registry{type_registry::instance()};

        destructor_state state{
            destructor_index{registry.resolve_by_traits<destructor_traits<Class>>()},
            std::move(metadata),
        };

        state.destroy = make_destructor_destroy<Class>(registry);
        state.destroy_at = make_destructor_destroy_at<Class>();
        state.destroy_error = make_destructor_destroy_error<Class>(registry);

        return std::make_shared<destructor_state>(std::move(state));
    }
}

namespace meta_hpp
{
    inline destructor_type destructor::get_type() const noexcept {
        return state_->index.get_type();
    }

    template < typename Arg >
    void destructor::destroy(Arg&& arg) const {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const uarg varg{registry, META_HPP_FWD(arg)};
        return state_->destroy(varg);
    }

    template < typename Arg >
    uresult destructor::try_destroy(Arg&& arg) const {
        using namespace detail;
        type_registry& registry{type_registry::instance()};

        {
            // doesn't actually move an 'arg', just checks conversion errors
            const uarg_base varg{registry, META_HPP_FWD(arg)};
            if ( const uerror err = state_->destroy_error(varg) ) {
                return err;
            }
        }

        const uarg varg{registry, META_HPP_FWD(arg)};
        state_->destroy(varg);
        return uerror{error_code::no_error};
    }

    inline void destructor::destroy_at(void* mem) const {
        state_->destroy_at(mem);
    }

    inline uresult destructor::try_destroy_at(void* mem) const {
        state_->destroy_at(mem);
        return uerror{error_code::no_error};
    }

    template < typename Arg >
    bool destructor::is_invocable_with() const {
        return !check_invocable_error<Arg>();
    }

    template < typename Arg >
    bool destructor::is_invocable_with(Arg&& arg) const {
        return !check_invocable_error(META_HPP_FWD(arg));
    }

    template < typename Arg >
    uerror destructor::check_invocable_error() const {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const uarg_base varg{registry, type_list<Arg>{}};
        return state_->destroy_error(varg);
    }

    template < typename Arg >
    uerror destructor::check_invocable_error(Arg&& arg) const {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const uarg_base varg{registry, META_HPP_FWD(arg)};
        return state_->destroy_error(varg);
    }
}
