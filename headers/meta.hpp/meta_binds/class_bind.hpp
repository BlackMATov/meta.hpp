/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
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

    //
    // base_
    //

    template < detail::class_kind Class >
    template < detail::class_kind... Bases >
        requires(... && detail::class_bind_base_kind<Class, Bases>)
    class_bind<Class>& class_bind<Class>::base_() {
        const auto register_base{[this]<detail::class_kind Base>(std::in_place_type_t<Base>) {
            const class_type& base_type = resolve_type<Base>();

            auto&& [position, emplaced] = get_data().bases.emplace(base_type);
            if ( !emplaced ) {
                return;
            }

            META_HPP_TRY {
                get_data().bases_info.emplace( //
                    base_type,
                    detail::class_type_data::base_info{
                        .upcast{[](void* derived) -> void* { return static_cast<Base*>(static_cast<Class*>(derived)); }}}
                );
            }
            META_HPP_CATCH(...) {
                get_data().bases.erase(position);
                META_HPP_RETHROW();
            }
        }};

        (register_base(std::in_place_type<Bases>), ...);
        return *this;
    }

    //
    // constructor_
    //

    template < detail::class_kind Class >
    template < typename... Args, constructor_policy_kind Policy >
    class_bind<Class>& class_bind<Class>::constructor_(Policy policy)
        requires detail::class_bind_constructor_kind<Class, Args...>
    {
        return constructor_<Args...>({}, policy);
    }

    template < detail::class_kind Class >
    template < typename... Args, constructor_policy_kind Policy >
    class_bind<Class>& class_bind<Class>::constructor_(constructor_opts opts, Policy)
        requires detail::class_bind_constructor_kind<Class, Args...>
    {
        auto state = detail::constructor_state::make<Policy, Class, Args...>(std::move(opts.metadata));

        META_HPP_ASSERT(                                     //
            opts.arguments.size() <= state->arguments.size() //
            && "provided argument names don't match constructor argument count"
        );

        for ( std::size_t i{}, e{std::min(opts.arguments.size(), state->arguments.size())}; i < e; ++i ) {
            argument& arg = state->arguments[i];
            detail::state_access(arg)->name = std::move(opts.arguments[i].name);
            detail::state_access(arg)->metadata = std::move(opts.arguments[i].metadata);
        }

        detail::insert_or_assign(get_data().constructors, constructor{std::move(state)});
        return *this;
    }

    //
    // destructor_
    //

    template < detail::class_kind Class >
    class_bind<Class>& class_bind<Class>::destructor_()
        requires detail::class_bind_destructor_kind<Class>
    {
        return destructor_({});
    }

    template < detail::class_kind Class >
    class_bind<Class>& class_bind<Class>::destructor_(destructor_opts opts)
        requires detail::class_bind_destructor_kind<Class>
    {
        auto state = detail::destructor_state::make<Class>(std::move(opts.metadata));
        detail::insert_or_assign(get_data().destructors, destructor{std::move(state)});
        return *this;
    }

    //
    // function_
    //

    template < detail::class_kind Class >
    template < detail::function_pointer_kind Function, function_policy_kind Policy >
    class_bind<Class>& class_bind<Class>::function_(std::string name, Function function_ptr, Policy policy) {
        return function_(std::move(name), function_ptr, {}, policy);
    }

    template < detail::class_kind Class >
    template < detail::function_pointer_kind Function, function_policy_kind Policy >
    class_bind<Class>& class_bind<Class>::function_(std::string name, Function function_ptr, function_opts opts, Policy) {
        auto state = detail::function_state::make<Policy>(std::move(name), function_ptr, std::move(opts.metadata));

        META_HPP_ASSERT(                                     //
            opts.arguments.size() <= state->arguments.size() //
            && "provided arguments don't match function argument count"
        );

        for ( std::size_t i{}, e{std::min(opts.arguments.size(), state->arguments.size())}; i < e; ++i ) {
            argument& arg = state->arguments[i];
            detail::state_access(arg)->name = std::move(opts.arguments[i].name);
            detail::state_access(arg)->metadata = std::move(opts.arguments[i].metadata);
        }

        detail::insert_or_assign(get_data().functions, function{std::move(state)});
        return *this;
    }

    template < detail::class_kind Class >
    template < detail::function_pointer_kind Function, function_policy_kind Policy >
    class_bind<Class>& class_bind<Class>::function_(std::string name, Function function_ptr, string_ilist arguments, Policy) {
        auto state = detail::function_state::make<Policy>(std::move(name), function_ptr, {});

        META_HPP_ASSERT(
            arguments.size() <= state->arguments.size() //
            && "provided argument names don't match function argument count"
        );

        for ( std::size_t i{}, e{std::min(arguments.size(), state->arguments.size())}; i < e; ++i ) {
            argument& arg = state->arguments[i];
            // NOLINTNEXTLINE(*-pointer-arithmetic)
            detail::state_access(arg)->name = std::data(arguments)[i];
        }

        detail::insert_or_assign(get_data().functions, function{std::move(state)});
        return *this;
    }

    //
    // member_
    //

    template < detail::class_kind Class >
    template < detail::member_pointer_kind Member, member_policy_kind Policy >
        requires detail::class_bind_member_kind<Class, Member>
    class_bind<Class>& class_bind<Class>::member_(std::string name, Member member_ptr, Policy policy) {
        return member_(std::move(name), member_ptr, {}, policy);
    }

    template < detail::class_kind Class >
    template < detail::member_pointer_kind Member, member_policy_kind Policy >
        requires detail::class_bind_member_kind<Class, Member>
    class_bind<Class>& class_bind<Class>::member_(std::string name, Member member_ptr, member_opts opts, Policy) {
        auto state = detail::member_state::make<Policy>(std::move(name), member_ptr, std::move(opts.metadata));
        detail::insert_or_assign(get_data().members, member{std::move(state)});
        return *this;
    }

    //
    // method_
    //

    template < detail::class_kind Class >
    template < detail::method_pointer_kind Method, method_policy_kind Policy >
        requires detail::class_bind_method_kind<Class, Method>
    class_bind<Class>& class_bind<Class>::method_(std::string name, Method method_ptr, Policy policy) {
        return method_(std::move(name), method_ptr, {}, policy);
    }

    template < detail::class_kind Class >
    template < detail::method_pointer_kind Method, method_policy_kind Policy >
        requires detail::class_bind_method_kind<Class, Method>
    class_bind<Class>& class_bind<Class>::method_(std::string name, Method method_ptr, method_opts opts, Policy) {
        auto state = detail::method_state::make<Policy>(std::move(name), method_ptr, std::move(opts.metadata));

        META_HPP_ASSERT(                                     //
            opts.arguments.size() <= state->arguments.size() //
            && "provided arguments don't match method argument count"
        );

        for ( std::size_t i{}, e{std::min(opts.arguments.size(), state->arguments.size())}; i < e; ++i ) {
            argument& arg = state->arguments[i];
            detail::state_access(arg)->name = std::move(opts.arguments[i].name);
            detail::state_access(arg)->metadata = std::move(opts.arguments[i].metadata);
        }

        detail::insert_or_assign(get_data().methods, method{std::move(state)});
        return *this;
    }

    template < detail::class_kind Class >
    template < detail::method_pointer_kind Method, method_policy_kind Policy >
        requires detail::class_bind_method_kind<Class, Method>
    class_bind<Class>& class_bind<Class>::method_(std::string name, Method method_ptr, string_ilist arguments, Policy) {
        auto state = detail::method_state::make<Policy>(std::move(name), method_ptr, {});

        META_HPP_ASSERT(                                //
            arguments.size() <= state->arguments.size() //
            && "provided argument names don't match method argument count"
        );

        for ( std::size_t i{}, e{std::min(arguments.size(), state->arguments.size())}; i < e; ++i ) {
            argument& arg = state->arguments[i];
            // NOLINTNEXTLINE(*-pointer-arithmetic)
            detail::state_access(arg)->name = std::data(arguments)[i];
        }

        detail::insert_or_assign(get_data().methods, method{std::move(state)});
        return *this;
    }

    //
    // typedef_
    //

    template < detail::class_kind Class >
    template < typename Type >
    class_bind<Class>& class_bind<Class>::typedef_(std::string name) {
        get_data().typedefs.insert_or_assign(std::move(name), resolve_type<Type>());
        return *this;
    }

    //
    // variable_
    //

    template < detail::class_kind Class >
    template < detail::pointer_kind Pointer, variable_policy_kind Policy >
    class_bind<Class>& class_bind<Class>::variable_(std::string name, Pointer variable_ptr, Policy policy) {
        return variable_(std::move(name), variable_ptr, {}, policy);
    }

    template < detail::class_kind Class >
    template < detail::pointer_kind Pointer, variable_policy_kind Policy >
    class_bind<Class>& class_bind<Class>::variable_(std::string name, Pointer variable_ptr, variable_opts opts, Policy) {
        auto state = detail::variable_state::make<Policy>(std::move(name), variable_ptr, std::move(opts.metadata));
        detail::insert_or_assign(get_data().variables, variable{std::move(state)});
        return *this;
    }
}
