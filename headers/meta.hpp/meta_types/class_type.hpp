/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_types.hpp"

#include "../meta_states/constructor.hpp"
#include "../meta_states/function.hpp"
#include "../meta_states/member.hpp"
#include "../meta_states/method.hpp"
#include "../meta_states/variable.hpp"

#include "../meta_detail/type_registry.hpp"
#include "../meta_detail/type_traits/class_traits.hpp"

namespace meta_hpp::detail
{
    template < class_kind Class >
    struct class_tag {};

    template < class_kind Class >
    // NOLINTNEXTLINE(readability-named-parameter)
    class_type_data::class_type_data(type_list<Class>)
    : type_data_base{type_id{type_list<class_tag<Class>>{}}, type_kind::class_}
    , flags{class_traits<Class>::make_flags()}
    , size{class_traits<Class>::size}
    , argument_types{resolve_types(typename class_traits<Class>::argument_types{})} {}
}

namespace meta_hpp
{
    inline class_type::class_type(detail::class_type_data_ptr data)
    : data_{std::move(data)} {}

    inline bool class_type::is_valid() const noexcept {
        return !!data_;
    }

    inline class_type::operator bool() const noexcept {
        return is_valid();
    }

    inline type_id class_type::get_id() const noexcept {
        return data_->id;
    }

    inline class_bitflags class_type::get_flags() const noexcept {
        return data_->flags;
    }

    inline const metadata_map& class_type::get_metadata() const noexcept {
        return data_->metadata;
    }

    inline std::size_t class_type::get_size() const noexcept {
        return data_->size;
    }

    inline std::size_t class_type::get_arity() const noexcept {
        return data_->argument_types.size();
    }

    inline any_type class_type::get_argument_type(std::size_t position) const noexcept {
        return position < data_->argument_types.size() ? data_->argument_types[position] : any_type{};
    }

    inline const std::vector<any_type>& class_type::get_argument_types() const noexcept {
        return data_->argument_types;
    }

    inline const class_map& class_type::get_classes() const noexcept {
        return data_->classes;
    }

    inline const class_set& class_type::get_bases() const noexcept {
        return data_->bases;
    }

    inline const constructor_map& class_type::get_ctors() const noexcept {
        return data_->constructors;
    }

    inline const destructor_map& class_type::get_dtors() const noexcept {
        return data_->destructors;
    }

    inline const enum_map& class_type::get_enums() const noexcept {
        return data_->enums;
    }

    inline const function_map& class_type::get_functions() const noexcept {
        return data_->functions;
    }

    inline const member_map& class_type::get_members() const noexcept {
        return data_->members;
    }

    inline const method_map& class_type::get_methods() const noexcept {
        return data_->methods;
    }

    inline const variable_map& class_type::get_variables() const noexcept {
        return data_->variables;
    }

    template < typename... Args >
    uvalue class_type::create(Args&&... args) const {
        for ( auto&& ctor : data_->constructors ) {
            if ( ctor.second.is_invocable_with(std::forward<Args>(args)...) ) {
                return ctor.second.invoke(std::forward<Args>(args)...);
            }
        }
        return uvalue{};
    }

    template < typename... Args >
    uvalue class_type::operator()(Args&&... args) const {
        return create(std::forward<Args>(args)...);
    }

    template < typename Arg >
    bool class_type::destroy(Arg&& ptr) const {
        for ( auto&& dtor : data_->destructors ) {
            if ( dtor.second.is_invocable_with(std::forward<Arg>(ptr)) ) {
                dtor.second.invoke(std::forward<Arg>(ptr));
                return true;
            }
        }
        return false;
    }

    template < detail::class_kind Derived >
    bool class_type::is_base_of() const noexcept {
        return is_base_of(detail::resolve_type<Derived>());
    }

    inline bool class_type::is_base_of(const class_type& derived) const noexcept {
        if ( !is_valid() || !derived.is_valid() ) {
            return false;
        }

        if ( derived.data_->bases.contains(*this) ) {
            return true;
        }

        for ( auto&& derived_base : derived.data_->bases ) {
            if ( is_base_of(derived_base) ) {
                return true;
            }
        }

        return false;
    }

    template < detail::class_kind Base >
    bool class_type::is_derived_from() const noexcept {
        return is_derived_from(detail::resolve_type<Base>());
    }

    inline bool class_type::is_derived_from(const class_type& base) const noexcept {
        if ( !is_valid() || !base.is_valid() ) {
            return false;
        }

        if ( data_->bases.contains(base) ) {
            return true;
        }

        for ( auto&& self_base : data_->bases ) {
            if ( self_base.is_derived_from(base) ) {
                return true;
            }
        }

        return false;
    }

    inline class_type class_type::get_class(std::string_view name) const noexcept {
        if ( auto iter = data_->classes.find(name); iter != data_->classes.end() ) {
            return iter->second;
        }
        return class_type{};
    }

    inline enum_type class_type::get_enum(std::string_view name) const noexcept {
        if ( auto iter = data_->enums.find(name); iter != data_->enums.end() ) {
            return iter->second;
        }
        return enum_type{};
    }

    inline function class_type::get_function(std::string_view name) const noexcept {
        for ( auto&& [index, function] : data_->functions ) {
            if ( index.get_name() == name ) {
                return function;
            }
        }

        for ( auto&& base : data_->bases ) {
            if ( function function = base.get_function(name); function ) {
                return function;
            }
        }

        return function{};
    }

    inline member class_type::get_member(std::string_view name) const noexcept {
        for ( auto&& [index, member] : data_->members ) {
            if ( index.get_name() == name ) {
                return member;
            }
        }

        for ( auto&& base : data_->bases ) {
            if ( member member = base.get_member(name); member ) {
                return member;
            }
        }

        return member{};
    }

    inline method class_type::get_method(std::string_view name) const noexcept {
        for ( auto&& [index, method] : data_->methods ) {
            if ( index.get_name() == name ) {
                return method;
            }
        }

        for ( auto&& base : data_->bases ) {
            if ( method method = base.get_method(name); method ) {
                return method;
            }
        }

        return method{};
    }

    inline variable class_type::get_variable(std::string_view name) const noexcept {
        for ( auto&& [index, variable] : data_->variables ) {
            if ( index.get_name() == name ) {
                return variable;
            }
        }

        for ( auto&& base : data_->bases ) {
            if ( variable variable = base.get_variable(name); variable ) {
                return variable;
            }
        }

        return variable{};
    }

    //
    // get_constructor_with
    //

    template < typename... Args >
    constructor class_type::get_constructor_with() const noexcept {
        return get_constructor_with({detail::resolve_type<Args>()...});
    }

    template < typename Iter >
    constructor class_type::get_constructor_with(Iter first, Iter last) const noexcept {
        for ( auto&& [index, ctor] : data_->constructors ) {
            const std::vector<any_type>& args = ctor.get_type().get_argument_types();
            if ( std::equal(first, last, args.begin(), args.end()) ) {
                return ctor;
            }
        }
        return constructor{};
    }

    inline constructor class_type::get_constructor_with(const std::vector<any_type>& args) const noexcept {
        return get_constructor_with(args.begin(), args.end());
    }

    inline constructor class_type::get_constructor_with(std::initializer_list<any_type> args) const noexcept {
        return get_constructor_with(args.begin(), args.end());
    }

    //
    // get_function_with
    //

    template < typename... Args >
    function class_type::get_function_with(std::string_view name) const noexcept {
        return get_function_with(name, {detail::resolve_type<Args>()...});
    }

    template < typename Iter >
    function class_type::get_function_with(std::string_view name, Iter first, Iter last) const noexcept {
        for ( auto&& [index, function] : data_->functions ) {
            if ( index.get_name() != name ) {
                continue;
            }

            const std::vector<any_type>& args = function.get_type().get_argument_types();
            if ( std::equal(first, last, args.begin(), args.end()) ) {
                return function;
            }
        }

        for ( auto&& base : data_->bases ) {
            if ( function function = base.get_function_with(name, first, last); function ) {
                return function;
            }
        }

        return function{};
    }

    inline function class_type::get_function_with(std::string_view name, const std::vector<any_type>& args) const noexcept {
        return get_function_with(name, args.begin(), args.end());
    }

    inline function class_type::get_function_with(std::string_view name, std::initializer_list<any_type> args) const noexcept {
        return get_function_with(name, args.begin(), args.end());
    }

    //
    // get_method_with
    //

    template < typename... Args >
    method class_type::get_method_with(std::string_view name) const noexcept {
        return get_method_with(name, {detail::resolve_type<Args>()...});
    }

    template < typename Iter >
    method class_type::get_method_with(std::string_view name, Iter first, Iter last) const noexcept {
        for ( auto&& [index, method] : data_->methods ) {
            if ( index.get_name() != name ) {
                continue;
            }

            const std::vector<any_type>& args = method.get_type().get_argument_types();
            if ( std::equal(first, last, args.begin(), args.end()) ) {
                return method;
            }
        }

        for ( auto&& base : data_->bases ) {
            if ( method method = base.get_method_with(name, first, last); method ) {
                return method;
            }
        }

        return method{};
    }

    inline method class_type::get_method_with(std::string_view name, const std::vector<any_type>& args) const noexcept {
        return get_method_with(name, args.begin(), args.end());
    }

    inline method class_type::get_method_with(std::string_view name, std::initializer_list<any_type> args) const noexcept {
        return get_method_with(name, args.begin(), args.end());
    }
}
