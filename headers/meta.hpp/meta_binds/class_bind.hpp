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
    template < class_kind Class >
    class_bind<Class>::class_bind(metadata_map metadata)
    : type_bind_base{resolve_type<Class>(), std::move(metadata)} {
        if constexpr ( std::is_destructible_v<Class> ) {
            destructor_();
        }
    }

    template < class_kind Class >
    template < typename... Args, typename... Opts >
        requires class_constructor_kind<Class, Args...>
    class_bind<Class>& class_bind<Class>::constructor_(Opts&&... opts) {
        using namespace detail;

        using opts_t = type_list<std::remove_cvref_t<Opts>...>;
        using policy_t = type_list_first_of_t<is_constructor_policy, constructor_policy::as_object_t, opts_t>;

        static_assert( //
            type_list_count_of_v<is_constructor_policy, opts_t> <= 1,
            "constructor policy may be specified only once"
        );

        // there is no 'use after move' here because `from opts` takes only relevant opts
        metadata_bind::values_t metadata = metadata_bind::from_opts(META_HPP_FWD(opts)...);
        arguments_bind::values_t arguments = arguments_bind::from_opts(META_HPP_FWD(opts)...);

        auto state = constructor_state::make<policy_t, Class, Args...>(std::move(metadata));

        META_HPP_ASSERT(                                //
            arguments.size() <= state->arguments.size() //
            && "provided argument names don't match constructor argument count"
        );

        using std::min; // prevents windows.h min/max issues
        for ( std::size_t i{}, e{min(arguments.size(), state->arguments.size())}; i < e; ++i ) {
            argument& arg = state->arguments[i];
            state_access(arg)->name = std::move(arguments[i].get_name());
            state_access(arg)->metadata = std::move(arguments[i].get_metadata());
        }

        insert_or_assign(get_data().constructors, constructor{std::move(state)});
        return *this;
    }

    template < class_kind Class >
    template < typename... Opts >
        requires class_destructor_kind<Class>
    class_bind<Class>& class_bind<Class>::destructor_(Opts&&... opts) {
        using namespace detail;
        metadata_bind::values_t metadata = metadata_bind::from_opts(META_HPP_FWD(opts)...);
        auto state = destructor_state::make<Class>(std::move(metadata));
        insert_or_assign(get_data().destructors, destructor{std::move(state)});
        return *this;
    }

    template < class_kind Class >
    template < function_pointer_kind Function, typename... Opts >
    class_bind<Class>& class_bind<Class>::function_(std::string name, Function function_ptr, Opts&&... opts) {
        using namespace detail;

        using opts_t = type_list<std::remove_cvref_t<Opts>...>;
        using policy_t = type_list_first_of_t<is_function_policy, function_policy::as_copy_t, opts_t>;

        static_assert( //
            type_list_count_of_v<is_function_policy, opts_t> <= 1,
            "function policy may be specified only once"
        );

        // there is no 'use after move' here because `from opts` takes only relevant opts
        metadata_bind::values_t metadata = metadata_bind::from_opts(META_HPP_FWD(opts)...);
        arguments_bind::values_t arguments = arguments_bind::from_opts(META_HPP_FWD(opts)...);

        auto state = function_state::make<policy_t>(std::move(name), function_ptr, std::move(metadata));

        META_HPP_ASSERT(                                //
            arguments.size() <= state->arguments.size() //
            && "provided argument names don't match function argument count"
        );

        using std::min; // prevents windows.h min/max issues
        for ( std::size_t i{}, e{min(arguments.size(), state->arguments.size())}; i < e; ++i ) {
            argument& arg = state->arguments[i];
            state_access(arg)->name = std::move(arguments[i].get_name());
            state_access(arg)->metadata = std::move(arguments[i].get_metadata());
        }

        insert_or_assign(get_data().functions, function{std::move(state)});
        return *this;
    }

    template < class_kind Class >
    template < class_member_pointer_kind<Class> Member, typename... Opts >
    class_bind<Class>& class_bind<Class>::member_(std::string name, Member member_ptr, Opts&&... opts) {
        using namespace detail;

        using opts_t = type_list<std::remove_cvref_t<Opts>...>;
        using policy_t = type_list_first_of_t<is_member_policy, member_policy::as_copy_t, opts_t>;

        static_assert( //
            type_list_count_of_v<is_member_policy, opts_t> <= 1,
            "member policy may be specified only once"
        );

        metadata_bind::values_t metadata = metadata_bind::from_opts(META_HPP_FWD(opts)...);
        auto state = member_state::make<policy_t>(std::move(name), member_ptr, std::move(metadata));
        insert_or_assign(get_data().members, member{std::move(state)});
        return *this;
    }

    template < class_kind Class >
    template < class_method_pointer_kind<Class> Method, typename... Opts >
    class_bind<Class>& class_bind<Class>::method_(std::string name, Method method_ptr, Opts&&... opts) {
        using namespace detail;

        using opts_t = type_list<std::remove_cvref_t<Opts>...>;
        using policy_t = type_list_first_of_t<is_method_policy, method_policy::as_copy_t, opts_t>;

        static_assert( //
            type_list_count_of_v<is_method_policy, opts_t> <= 1,
            "method policy may be specified only once"
        );

        // there is no 'use after move' here because `from opts` takes only relevant opts
        metadata_bind::values_t metadata = metadata_bind::from_opts(META_HPP_FWD(opts)...);
        arguments_bind::values_t arguments = arguments_bind::from_opts(META_HPP_FWD(opts)...);

        auto state = method_state::make<policy_t>(std::move(name), method_ptr, std::move(metadata));

        META_HPP_ASSERT(                                //
            arguments.size() <= state->arguments.size() //
            && "provided argument names don't match method argument count"
        );

        using std::min; // prevents windows.h min/max issues
        for ( std::size_t i{}, e{min(arguments.size(), state->arguments.size())}; i < e; ++i ) {
            argument& arg = state->arguments[i];
            state_access(arg)->name = std::move(arguments[i].get_name());
            state_access(arg)->metadata = std::move(arguments[i].get_metadata());
        }

        insert_or_assign(get_data().methods, method{std::move(state)});
        return *this;
    }

    template < class_kind Class >
    template < typename Type >
    class_bind<Class>& class_bind<Class>::typedef_(std::string name) {
        get_data().typedefs.insert_or_assign(std::move(name), resolve_type<Type>());
        return *this;
    }

    template < class_kind Class >
    template < pointer_kind Pointer, typename... Opts >
    class_bind<Class>& class_bind<Class>::variable_(std::string name, Pointer variable_ptr, Opts&&... opts) {
        using namespace detail;

        using opts_t = type_list<std::remove_cvref_t<Opts>...>;
        using policy_t = type_list_first_of_t<is_variable_policy, variable_policy::as_copy_t, opts_t>;

        static_assert( //
            type_list_count_of_v<is_variable_policy, opts_t> <= 1,
            "variable policy may be specified only once"
        );

        metadata_bind::values_t metadata = metadata_bind::from_opts(META_HPP_FWD(opts)...);
        auto state = variable_state::make<policy_t>(std::move(name), variable_ptr, std::move(metadata));
        insert_or_assign(get_data().variables, variable{std::move(state)});
        return *this;
    }
}
