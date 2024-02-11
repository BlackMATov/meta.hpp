/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_registry.hpp"
#include "../meta_types.hpp"

#include "../meta_states/constructor.hpp"
#include "../meta_states/function.hpp"
#include "../meta_states/member.hpp"
#include "../meta_states/method.hpp"
#include "../meta_states/variable.hpp"

#include "../meta_detail/type_sharing.hpp"
#include "../meta_detail/type_traits/class_traits.hpp"

namespace meta_hpp::detail::class_type_data_impl
{
    struct new_base_info_t final {
        class_list base_classes;
        class_type_data::deep_upcasts_t deep_upcasts;
    };

    template < class_kind Class, class_kind Target >
    void add_upcast_info(new_base_info_t& info) {
        const class_type_data::upcast_func_t::upcast_t class_to_target = []() {
            if constexpr ( requires { static_cast<Target*>(std::declval<Class*>()); } ) {
                return +[](void* from) -> void* { //
                    return static_cast<Target*>(static_cast<Class*>(from));
                };
            } else {
                return nullptr;
            }
        }();

        info.deep_upcasts.push_back(class_type_data::upcast_func_t{
            .target{resolve_type<Target>().get_id()},
            .upcast{class_to_target},
        });

        if constexpr ( check_base_info_enabled<Target> ) {
            [&info]<typename... TargetBases>(type_list<TargetBases...>) {
                (add_upcast_info<Class, TargetBases>(info), ...);
            }(get_meta_base_info<Target>{});
        }
    }

    template < class_kind Class >
    void fill_upcast_info(new_base_info_t& info) {
        if constexpr ( check_base_info_enabled<Class> ) {
            [&info]<typename... ClassBases>(type_list<ClassBases...>) {
                (info.base_classes.push_back(resolve_type<ClassBases>()), ...);
                (add_upcast_info<Class, ClassBases>(info), ...);
            }(get_meta_base_info<Class>{});
        }
    }

    template < class_kind Class >
    any_type_list make_argument_types() {
        using ct = class_traits<Class>;
        using ct_argument_types = typename ct::argument_types;

        return []<std::size_t... Is>(std::index_sequence<Is...>) {
            any_type_list argument_types;
            argument_types.reserve(type_list_arity_v<ct_argument_types>);

            [[maybe_unused]] const auto make_argument_type = []<std::size_t I>(index_constant<I>) {
                return resolve_type<type_list_at_t<I, ct_argument_types>>();
            };

            (argument_types.emplace_back(make_argument_type(index_constant<Is>{})), ...);
            return argument_types;
        }(std::make_index_sequence<type_list_arity_v<ct_argument_types>>());
    }

    template < class_kind Class >
    uvalue_list make_argument_values() {
        using ct = class_traits<Class>;
        using ct_argument_values = decltype(ct::argument_values);

        return []<std::size_t... Is>(std::index_sequence<Is...>) {
            uvalue_list argument_values;
            argument_values.reserve(std::tuple_size_v<ct_argument_values>);

            [[maybe_unused]] const auto make_argument_value = []<std::size_t I>(index_constant<I>) {
                return overloaded{
                    [](decltype(std::ignore)) { return uvalue{}; },
                    [](auto&& value) { return uvalue{META_HPP_FWD(value)}; },
                }(std::get<I>(ct::argument_values));
            };

            (argument_values.emplace_back(make_argument_value(index_constant<Is>{})), ...);
            return argument_values;
        }(std::make_index_sequence<std::tuple_size_v<ct_argument_values>>());
    }
}

namespace meta_hpp::detail
{
    template < class_kind Class >
    class_type_data::class_type_data(class_traits<Class>)
    : type_data_base{type_kind::class_, shared_traits_hash<class_traits<Class>>{}(this)}
    , flags{class_traits<Class>::make_flags()}
    , size{class_traits<Class>::size}
    , align{class_traits<Class>::align}
    , argument_types(class_type_data_impl::make_argument_types<Class>())
    , argument_values(class_type_data_impl::make_argument_values<Class>()) {
        class_type_data_impl::new_base_info_t new_base_info;
        class_type_data_impl::fill_upcast_info<Class>(new_base_info);
        base_classes.swap(new_base_info.base_classes);
        deep_upcasts.swap(new_base_info.deep_upcasts);
    }
}

namespace meta_hpp::detail
{
    inline void* class_type_data::upcast_func_t::apply(void* ptr) const noexcept {
        return upcast(ptr);
    }

    inline const void* class_type_data::upcast_func_t::apply(const void* ptr) const noexcept {
        // NOLINTNEXTLINE(*-const-cast)
        return apply(const_cast<void*>(ptr));
    }
}

namespace meta_hpp
{
    inline class_bitflags class_type::get_flags() const noexcept {
        return data_->flags;
    }

    inline std::size_t class_type::get_size() const noexcept {
        return data_->size;
    }

    inline std::size_t class_type::get_align() const noexcept {
        return data_->align;
    }

    inline std::size_t class_type::get_arity() const noexcept {
        return data_->argument_types.size();
    }

    inline any_type class_type::get_argument_type(std::size_t position) const noexcept {
        return position < data_->argument_types.size() ? data_->argument_types[position] : any_type{};
    }

    inline const uvalue& class_type::get_argument_value(std::size_t position) const noexcept {
        if ( position < data_->argument_values.size() ) {
            return data_->argument_values[position];
        }
        static const uvalue empty_value;
        return empty_value;
    }

    inline const any_type_list& class_type::get_argument_types() const noexcept {
        return data_->argument_types;
    }

    inline const uvalue_list& class_type::get_argument_values() const noexcept {
        return data_->argument_values;
    }

    inline const class_list& class_type::get_base_classes() const noexcept {
        return data_->base_classes;
    }

    inline const constructor_list& class_type::get_constructors() const noexcept {
        return data_->constructors;
    }

    inline const function_list& class_type::get_functions() const noexcept {
        return data_->functions;
    }

    inline const member_list& class_type::get_members() const noexcept {
        return data_->members;
    }

    inline const method_list& class_type::get_methods() const noexcept {
        return data_->methods;
    }

    inline const typedef_map& class_type::get_typedefs() const noexcept {
        return data_->typedefs;
    }

    inline const variable_list& class_type::get_variables() const noexcept {
        return data_->variables;
    }

    template < typename... Args >
    uvalue class_type::create(Args&&... args) const {
        for ( const constructor& constructor : data_->constructors ) {
            if ( constructor.is_invocable_with(META_HPP_FWD(args)...) ) {
                // there is no 'use after move' here because
                // 'is_invocable_with' doesn't actually move 'args'
                return constructor.create(META_HPP_FWD(args)...);
            }
        }
        return uvalue{};
    }

    template < typename... Args >
    uvalue class_type::create_at(void* mem, Args&&... args) const {
        for ( const constructor& constructor : data_->constructors ) {
            if ( constructor.is_invocable_with(META_HPP_FWD(args)...) ) {
                // there is no 'use after move' here because
                // 'is_invocable_with' doesn't actually move 'args'
                return constructor.create_at(mem, META_HPP_FWD(args)...);
            }
        }
        return uvalue{};
    }

    template < typename Arg >
    bool class_type::destroy(Arg&& arg) const {
        if ( const destructor& destructor = get_destructor() ) {
            if ( destructor.is_invocable_with(META_HPP_FWD(arg)) ) {
                // there is no 'use after move' here because
                // 'is_invocable_with' doesn't actually move an 'arg'
                destructor.destroy(META_HPP_FWD(arg));
                return true;
            }
        }
        return false;
    }

    inline bool class_type::destroy_at(void* mem) const {
        if ( const destructor& destructor = get_destructor() ) {
            destructor.destroy_at(mem);
            return true;
        }
        return false;
    }

    template < class_kind Derived >
    bool class_type::is_base_of() const {
        return is_base_of(resolve_type<Derived>());
    }

    inline bool class_type::is_base_of(const class_type& derived) const noexcept {
        if ( !is_valid() || !derived.is_valid() ) {
            return false;
        }

        for ( const auto& upcast : derived.data_->deep_upcasts ) {
            if ( upcast.target == *this ) {
                return true;
            }
        }

        return false;
    }

    template < class_kind Derived >
    bool class_type::is_direct_base_of() const {
        return is_direct_base_of(resolve_type<Derived>());
    }

    inline bool class_type::is_direct_base_of(const class_type& derived) const noexcept {
        if ( !is_valid() || !derived.is_valid() ) {
            return false;
        }

        for ( const class_type& base_class : derived.data_->base_classes ) {
            if ( base_class == *this ) {
                return true;
            }
        }

        return false;
    }

    template < class_kind Base >
    bool class_type::is_derived_from() const {
        return is_derived_from(resolve_type<Base>());
    }

    inline bool class_type::is_derived_from(const class_type& base) const noexcept {
        return base.is_base_of(*this);
    }

    template < class_kind Base >
    bool class_type::is_direct_derived_from() const {
        return is_direct_derived_from(resolve_type<Base>());
    }

    inline bool class_type::is_direct_derived_from(const class_type& base) const noexcept {
        return base.is_direct_base_of(*this);
    }

    inline function class_type::get_function(std::string_view name, bool recursively) const noexcept {
        for ( const function& function : data_->functions ) {
            if ( function.get_name() == name ) {
                return function;
            }
        }

        if ( recursively ) {
            for ( auto iter{data_->base_classes.rbegin()}, end{data_->base_classes.rend()}; iter != end; ++iter ) {
                if ( const function& function = iter->get_function(name, recursively) ) {
                    return function;
                }
            }
        }

        return function{};
    }

    inline member class_type::get_member(std::string_view name, bool recursively) const noexcept {
        for ( const member& member : data_->members ) {
            if ( member.get_name() == name ) {
                return member;
            }
        }

        if ( recursively ) {
            for ( auto iter{data_->base_classes.rbegin()}, end{data_->base_classes.rend()}; iter != end; ++iter ) {
                if ( const member& member = iter->get_member(name, recursively) ) {
                    return member;
                }
            }
        }

        return member{};
    }

    inline method class_type::get_method(std::string_view name, bool recursively) const noexcept {
        for ( const method& method : data_->methods ) {
            if ( method.get_name() == name ) {
                return method;
            }
        }

        if ( recursively ) {
            for ( auto iter{data_->base_classes.rbegin()}, end{data_->base_classes.rend()}; iter != end; ++iter ) {
                if ( const method& method = iter->get_method(name, recursively) ) {
                    return method;
                }
            }
        }

        return method{};
    }

    inline any_type class_type::get_typedef(std::string_view name, bool recursively) const noexcept {
        if ( auto iter{data_->typedefs.find(name)}; iter != data_->typedefs.end() ) {
            return iter->second;
        }

        if ( recursively ) {
            for ( auto iter{data_->base_classes.rbegin()}, end{data_->base_classes.rend()}; iter != end; ++iter ) {
                if ( const any_type& type = iter->get_typedef(name, recursively) ) {
                    return type;
                }
            }
        }

        return any_type{};
    }

    inline variable class_type::get_variable(std::string_view name, bool recursively) const noexcept {
        for ( const variable& variable : data_->variables ) {
            if ( variable.get_name() == name ) {
                return variable;
            }
        }

        if ( recursively ) {
            for ( auto iter{data_->base_classes.rbegin()}, end{data_->base_classes.rend()}; iter != end; ++iter ) {
                if ( const variable& variable = iter->get_variable(name, recursively) ) {
                    return variable;
                }
            }
        }

        return variable{};
    }

    //
    // get_constructor_with
    //

    template < typename... Args >
    constructor class_type::get_constructor_with() const {
        detail::type_registry& registry{detail::type_registry::instance()};
        return get_constructor_with({registry.resolve_by_type<Args>()...});
    }

    template < typename Iter >
    constructor class_type::get_constructor_with(Iter first, Iter last) const {
        for ( const constructor& constructor : data_->constructors ) {
            const constructor_type& constructor_type = constructor.get_type();
            const any_type_list& constructor_args = constructor_type.get_argument_types();

            if ( std::equal(first, last, constructor_args.begin(), constructor_args.end()) ) {
                return constructor;
            }
        }
        return constructor{};
    }

    inline constructor class_type::get_constructor_with(std::span<const any_type> args) const noexcept {
        return get_constructor_with(args.begin(), args.end());
    }

    inline constructor class_type::get_constructor_with(std::initializer_list<any_type> args) const noexcept {
        return get_constructor_with(args.begin(), args.end());
    }

    //
    // get_destructor
    //

    inline destructor class_type::get_destructor() const noexcept {
        if ( data_->destructors.empty() ) {
            return destructor{};
        }
        return *data_->destructors.begin();
    }

    //
    // get_function_with
    //

    template < typename... Args >
    function class_type::get_function_with( //
        std::string_view name,
        bool recursively
    ) const {
        detail::type_registry& registry{detail::type_registry::instance()};
        return get_function_with(name, {registry.resolve_by_type<Args>()...}, recursively);
    }

    template < typename Iter >
    function class_type::get_function_with( //
        std::string_view name,
        Iter first,
        Iter last,
        bool recursively
    ) const {
        for ( const function& function : data_->functions ) {
            if ( function.get_name() != name ) {
                continue;
            }

            const function_type& function_type = function.get_type();
            const any_type_list& function_args = function_type.get_argument_types();

            if ( std::equal(first, last, function_args.begin(), function_args.end()) ) {
                return function;
            }
        }

        if ( recursively ) {
            for ( auto iter{data_->base_classes.rbegin()}, end{data_->base_classes.rend()}; iter != end; ++iter ) {
                if ( const function& function = iter->get_function_with(name, first, last, recursively) ) {
                    return function;
                }
            }
        }

        return function{};
    }

    inline function class_type::get_function_with( //
        std::string_view name,
        std::span<const any_type> args,
        bool recursively
    ) const noexcept {
        return get_function_with(name, args.begin(), args.end(), recursively);
    }

    inline function class_type::get_function_with( //
        std::string_view name,
        std::initializer_list<any_type> args,
        bool recursively
    ) const noexcept {
        return get_function_with(name, args.begin(), args.end(), recursively);
    }

    //
    // get_method_with
    //

    template < typename... Args >
    method class_type::get_method_with( //
        std::string_view name,
        bool recursively
    ) const {
        detail::type_registry& registry{detail::type_registry::instance()};
        return get_method_with(name, {registry.resolve_by_type<Args>()...}, recursively);
    }

    template < typename Iter >
    method class_type::get_method_with( //
        std::string_view name,
        Iter first,
        Iter last,
        bool recursively
    ) const {
        for ( const method& method : data_->methods ) {
            if ( method.get_name() != name ) {
                continue;
            }

            const method_type& method_type = method.get_type();
            const any_type_list& method_args = method_type.get_argument_types();

            if ( std::equal(first, last, method_args.begin(), method_args.end()) ) {
                return method;
            }
        }

        if ( recursively ) {
            for ( auto iter{data_->base_classes.rbegin()}, end{data_->base_classes.rend()}; iter != end; ++iter ) {
                if ( const method& method = iter->get_method_with(name, first, last, recursively) ) {
                    return method;
                }
            }
        }

        return method{};
    }

    inline method class_type::get_method_with( //
        std::string_view name,
        std::span<const any_type> args,
        bool recursively
    ) const noexcept {
        return get_method_with(name, args.begin(), args.end(), recursively);
    }

    inline method class_type::get_method_with( //
        std::string_view name,
        std::initializer_list<any_type> args,
        bool recursively
    ) const noexcept {
        return get_method_with(name, args.begin(), args.end(), recursively);
    }
}
