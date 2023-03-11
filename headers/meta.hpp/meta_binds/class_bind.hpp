/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_binds.hpp"
#include "../meta_registry.hpp"

namespace meta_hpp::detail::class_bind_impl
{
    using base_upcasts_t = class_type_data::base_upcasts_t;
    using deep_upcasts_t = class_type_data::deep_upcasts_t;

    using upcast_func_t = class_type_data::upcast_func_t;
    using upcast_func_list_t = class_type_data::upcast_func_list_t;

    using new_bases_db_t = std::vector<std::pair<class_type, upcast_func_t>>;
    using deep_upcasts_db_t = std::map<class_type, deep_upcasts_t, std::less<>>;
    using derived_classes_db_t = std::map<class_type, class_list, std::less<>>;

    template < class_kind Class, class_kind Base >
        requires std::is_base_of_v<Base, Class>
    void update_new_bases_db( //
        new_bases_db_t& new_bases_db
    ) {
        const class_type& new_base = resolve_type<Base>();

        for ( auto&& [db_base, _] : new_bases_db ) {
            if ( db_base == new_base ) {
                return;
            }
        }

        new_bases_db.emplace_back( //
            new_base,
            upcast_func_t{std::in_place_type<Class>, std::in_place_type<Base>}
        );
    }

    inline void update_deep_upcasts_db( //
        const class_type& derived_class,
        const class_type& new_base_class,
        upcast_func_list_t&& derived_to_new_base,
        deep_upcasts_db_t& deep_upcasts_db
    ) {
        const class_type_data& derived_class_data = *type_access(derived_class);
        const class_type_data& new_base_class_data = *type_access(new_base_class);

        const auto [deep_upcasts_db_iter, _] = deep_upcasts_db.try_emplace(derived_class, derived_class_data.deep_upcasts);
        deep_upcasts_t& derived_deep_upcasts = deep_upcasts_db_iter->second;

        const auto add_derived_deep_upcast = [&derived_deep_upcasts](const class_type& deep_class, upcast_func_list_t&& upcasts) {
            auto&& [position, emplaced] = derived_deep_upcasts.try_emplace(deep_class, std::move(upcasts));
            if ( !emplaced ) {
                position->second.is_ambiguous = is_disjoint(position->second.vbases, upcasts.vbases);
            }
        };

        for ( auto&& [new_deep_class, new_base_to_deep] : new_base_class_data.deep_upcasts ) {
            upcast_func_list_t derived_to_new_deep = derived_to_new_base + new_base_to_deep;
            add_derived_deep_upcast(new_deep_class, std::move(derived_to_new_deep));
        }

        for ( const class_type& subderived_class : derived_class_data.derived_classes ) {
            const class_type_data& subderived_data = *type_access(subderived_class);
            upcast_func_t subderived_to_derived = subderived_data.base_upcasts.at(derived_class);
            upcast_func_list_t subderived_to_new_base = subderived_to_derived + derived_to_new_base;
            update_deep_upcasts_db(subderived_class, new_base_class, std::move(subderived_to_new_base), deep_upcasts_db);
        }

        add_derived_deep_upcast(new_base_class, std::move(derived_to_new_base));
    }

    inline void updata_derived_classes_db( //
        const class_type& self_class,
        const class_type& new_base_class,
        derived_classes_db_t& derived_classes_db
    ) {
        const class_type_data& base_class_data = *type_access(new_base_class);
        class_list new_derived_classes{base_class_data.derived_classes};
        insert_or_assign(new_derived_classes, self_class);
        derived_classes_db.emplace(new_base_class, std::move(new_derived_classes));
    }
}

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
        using namespace detail;
        using namespace detail::class_bind_impl;

        new_bases_db_t new_bases_db;
        (update_new_bases_db<Class, Bases>(new_bases_db), ...);

        deep_upcasts_db_t deep_upcasts_db;
        derived_classes_db_t derived_classes_db;

        class_list new_base_classes{get_data().base_classes};
        base_upcasts_t new_base_upcasts{get_data().base_upcasts};

        for ( auto&& [new_base_class, self_to_new_base] : new_bases_db ) {
            if ( std::find(new_base_classes.begin(), new_base_classes.end(), new_base_class) != new_base_classes.end() ) {
                continue;
            }

            update_deep_upcasts_db(*this, new_base_class, self_to_new_base, deep_upcasts_db);
            updata_derived_classes_db(*this, new_base_class, derived_classes_db);

            new_base_classes.emplace_back(new_base_class);
            new_base_upcasts.emplace(new_base_class, self_to_new_base);
        }

        get_data().base_classes.swap(new_base_classes);
        get_data().base_upcasts.swap(new_base_upcasts);

        for ( auto&& [derived_class, new_deep_upcasts] : deep_upcasts_db ) {
            class_type_data& derived_data = *type_access(derived_class);
            derived_data.deep_upcasts.swap(new_deep_upcasts);
        }

        for ( auto&& [base_class, new_derived_classes] : derived_classes_db ) {
            class_type_data& base_data = *type_access(base_class);
            base_data.derived_classes.swap(new_derived_classes);
        }

        return *this;
    }

    //
    // constructor_
    //

    template < detail::class_kind Class >
    template < typename... Args, constructor_policy_family Policy >
    class_bind<Class>& class_bind<Class>::constructor_(Policy policy)
        requires detail::class_bind_constructor_kind<Class, Args...>
    {
        return constructor_<Args...>({}, policy);
    }

    template < detail::class_kind Class >
    template < typename... Args, constructor_policy_family Policy >
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
    template < detail::function_pointer_kind Function, function_policy_family Policy >
    class_bind<Class>& class_bind<Class>::function_(std::string name, Function function_ptr, Policy policy) {
        return function_(std::move(name), function_ptr, {}, policy);
    }

    template < detail::class_kind Class >
    template < detail::function_pointer_kind Function, function_policy_family Policy >
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
    template < detail::function_pointer_kind Function, function_policy_family Policy >
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
    template < detail::member_pointer_kind Member, member_policy_family Policy >
        requires detail::class_bind_member_kind<Class, Member>
    class_bind<Class>& class_bind<Class>::member_(std::string name, Member member_ptr, Policy policy) {
        return member_(std::move(name), member_ptr, {}, policy);
    }

    template < detail::class_kind Class >
    template < detail::member_pointer_kind Member, member_policy_family Policy >
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
    template < detail::method_pointer_kind Method, method_policy_family Policy >
        requires detail::class_bind_method_kind<Class, Method>
    class_bind<Class>& class_bind<Class>::method_(std::string name, Method method_ptr, Policy policy) {
        return method_(std::move(name), method_ptr, {}, policy);
    }

    template < detail::class_kind Class >
    template < detail::method_pointer_kind Method, method_policy_family Policy >
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
    template < detail::method_pointer_kind Method, method_policy_family Policy >
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
    template < detail::pointer_kind Pointer, variable_policy_family Policy >
    class_bind<Class>& class_bind<Class>::variable_(std::string name, Pointer variable_ptr, Policy policy) {
        return variable_(std::move(name), variable_ptr, {}, policy);
    }

    template < detail::class_kind Class >
    template < detail::pointer_kind Pointer, variable_policy_family Policy >
    class_bind<Class>& class_bind<Class>::variable_(std::string name, Pointer variable_ptr, variable_opts opts, Policy) {
        auto state = detail::variable_state::make<Policy>(std::move(name), variable_ptr, std::move(opts.metadata));
        detail::insert_or_assign(get_data().variables, variable{std::move(state)});
        return *this;
    }
}
