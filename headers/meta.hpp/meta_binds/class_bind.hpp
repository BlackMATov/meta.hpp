/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_binds.hpp"
#include "../meta_registry.hpp"

namespace meta_hpp
{
    template < detail::class_kind Class >
    class_bind<Class>::class_bind(metadata_map metadata)
    : type_bind_base{resolve_type<Class>(), std::move(metadata)} {
        if constexpr ( std::is_destructible_v<Class> ) {
            destructor_();
        }
    }

    template < detail::class_kind Class >
    template < typename... Args, typename... Opts >
        requires detail::class_bind_constructor_kind<Class, Args...>
    class_bind<Class>& class_bind<Class>::constructor_(Opts&&... opts) {
        using opts_t = detail::type_list<std::remove_cvref_t<Opts>...>;
        using policy_t = detail::type_list_first_of_t<constructor_policy::is_family, constructor_policy::as_object_t, opts_t>;

        static_assert( //
            detail::type_list_count_of_v<constructor_policy::is_family, opts_t> <= 1,
            "constructor policy may be specified only once"
        );

        // there is no 'use after move' here because `from opts` takes only relevant opts
        metadata_bind::values_t metadata = metadata_bind::from_opts(META_HPP_FWD(opts)...);
        arguments_bind::values_t arguments = arguments_bind::from_opts(META_HPP_FWD(opts)...);

        auto state = detail::constructor_state::make<policy_t, Class, Args...>(std::move(metadata));

        META_HPP_ASSERT(                                //
            arguments.size() <= state->arguments.size() //
            && "provided argument names don't match constructor argument count"
        );

        for ( std::size_t i{}, e{std::min(arguments.size(), state->arguments.size())}; i < e; ++i ) {
            argument& arg = state->arguments[i];
            detail::state_access(arg)->name = std::move(arguments[i].get_name());
            detail::state_access(arg)->metadata = std::move(arguments[i].get_metadata());
        }

        detail::insert_or_assign(get_data().constructors, constructor{std::move(state)});
        return *this;
    }

    template < detail::class_kind Class >
    template < typename... Opts >
        requires detail::class_bind_destructor_kind<Class>
    class_bind<Class>& class_bind<Class>::destructor_(Opts&&... opts) {
        metadata_bind::values_t metadata = metadata_bind::from_opts(META_HPP_FWD(opts)...);
        auto state = detail::destructor_state::make<Class>(std::move(metadata));
        detail::insert_or_assign(get_data().destructors, destructor{std::move(state)});
        return *this;
    }

    template < detail::class_kind Class >
    template < detail::function_pointer_kind Function, typename... Opts >
    class_bind<Class>& class_bind<Class>::function_(std::string name, Function function_ptr, Opts&&... opts) {
        using opts_t = detail::type_list<std::remove_cvref_t<Opts>...>;
        using policy_t = detail::type_list_first_of_t<function_policy::is_family, function_policy::as_copy_t, opts_t>;

        static_assert( //
            detail::type_list_count_of_v<function_policy::is_family, opts_t> <= 1,
            "function policy may be specified only once"
        );

        // there is no 'use after move' here because `from opts` takes only relevant opts
        metadata_bind::values_t metadata = metadata_bind::from_opts(META_HPP_FWD(opts)...);
        arguments_bind::values_t arguments = arguments_bind::from_opts(META_HPP_FWD(opts)...);

        auto state = detail::function_state::make<policy_t>(std::move(name), function_ptr, std::move(metadata));

        META_HPP_ASSERT(                                //
            arguments.size() <= state->arguments.size() //
            && "provided argument names don't match function argument count"
        );

        for ( std::size_t i{}, e{std::min(arguments.size(), state->arguments.size())}; i < e; ++i ) {
            argument& arg = state->arguments[i];
            detail::state_access(arg)->name = std::move(arguments[i].get_name());
            detail::state_access(arg)->metadata = std::move(arguments[i].get_metadata());
        }

        detail::insert_or_assign(get_data().functions, function{std::move(state)});
        return *this;
    }

    template < detail::class_kind Class >
    template < detail::class_bind_member_kind<Class> Member, typename... Opts >
    class_bind<Class>& class_bind<Class>::member_(std::string name, Member member_ptr, Opts&&... opts) {
        using opts_t = detail::type_list<std::remove_cvref_t<Opts>...>;
        using policy_t = detail::type_list_first_of_t<member_policy::is_family, member_policy::as_copy_t, opts_t>;

        static_assert( //
            detail::type_list_count_of_v<member_policy::is_family, opts_t> <= 1,
            "member policy may be specified only once"
        );

        metadata_bind::values_t metadata = metadata_bind::from_opts(META_HPP_FWD(opts)...);
        auto state = detail::member_state::make<policy_t>(std::move(name), member_ptr, std::move(metadata));
        detail::insert_or_assign(get_data().members, member{std::move(state)});
        return *this;
    }

    template < detail::class_kind Class >
    template < detail::class_bind_method_kind<Class> Method, typename... Opts >
    class_bind<Class>& class_bind<Class>::method_(std::string name, Method method_ptr, Opts&&... opts) {
        using opts_t = detail::type_list<std::remove_cvref_t<Opts>...>;
        using policy_t = detail::type_list_first_of_t<method_policy::is_family, method_policy::as_copy_t, opts_t>;

        static_assert( //
            detail::type_list_count_of_v<method_policy::is_family, opts_t> <= 1,
            "method policy may be specified only once"
        );

        // there is no 'use after move' here because `from opts` takes only relevant opts
        metadata_bind::values_t metadata = metadata_bind::from_opts(META_HPP_FWD(opts)...);
        arguments_bind::values_t arguments = arguments_bind::from_opts(META_HPP_FWD(opts)...);

        auto state = detail::method_state::make<policy_t>(std::move(name), method_ptr, std::move(metadata));

        META_HPP_ASSERT(                                //
            arguments.size() <= state->arguments.size() //
            && "provided argument names don't match method argument count"
        );

        for ( std::size_t i{}, e{std::min(arguments.size(), state->arguments.size())}; i < e; ++i ) {
            argument& arg = state->arguments[i];
            detail::state_access(arg)->name = std::move(arguments[i].get_name());
            detail::state_access(arg)->metadata = std::move(arguments[i].get_metadata());
        }

        detail::insert_or_assign(get_data().methods, method{std::move(state)});
        return *this;
    }

    template < detail::class_kind Class >
    template < typename Type >
    class_bind<Class>& class_bind<Class>::typedef_(std::string name) {
        get_data().typedefs.insert_or_assign(std::move(name), resolve_type<Type>());
        return *this;
    }

    template < detail::class_kind Class >
    template < detail::pointer_kind Pointer, typename... Opts >
    class_bind<Class>& class_bind<Class>::variable_(std::string name, Pointer variable_ptr, Opts&&... opts) {
        using opts_t = detail::type_list<std::remove_cvref_t<Opts>...>;
        using policy_t = detail::type_list_first_of_t<variable_policy::is_family, variable_policy::as_copy_t, opts_t>;

        static_assert( //
            detail::type_list_count_of_v<variable_policy::is_family, opts_t> <= 1,
            "variable policy may be specified only once"
        );

        metadata_bind::values_t metadata = metadata_bind::from_opts(META_HPP_FWD(opts)...);
        auto state = detail::variable_state::make<policy_t>(std::move(name), variable_ptr, std::move(metadata));
        detail::insert_or_assign(get_data().variables, variable{std::move(state)});
        return *this;
    }
}
