/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_types.hpp"

#include "../meta_traits/class_traits.hpp"

namespace meta_hpp::detail
{
    template < class_kind Class >
    struct class_tag {};

    template < class_kind Class >
    class_type_data_ptr class_type_data::get_static() {
        static class_type_data_ptr data = std::make_shared<class_type_data>(type_list<Class>{});
        return data;
    }

    template < class_kind Class >
    class_type_data::class_type_data(type_list<Class>)
    : type_data_base{type_id{type_list<class_tag<Class>>{}}, type_kind::class_}
    , flags{class_traits<Class>::make_flags()}
    , size{class_traits<Class>::size}
    , argument_types{class_traits<Class>::make_argument_types()} {}
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

    inline bitflags<class_flags> class_type::get_flags() const noexcept {
        return data_->flags;
    }

    inline std::size_t class_type::get_size() const noexcept {
        return data_->size;
    }

    inline std::size_t class_type::get_arity() const noexcept {
        return data_->argument_types.size();
    }

    inline any_type class_type::get_argument_type(std::size_t index) const noexcept {
        return index < data_->argument_types.size() ? data_->argument_types[index] : any_type{};
    }

    inline const std::vector<any_type>& class_type::get_argument_types() const noexcept {
        return data_->argument_types;
    }

    inline const ctor_map& class_type::get_ctors() const noexcept {
        return data_->ctors;
    }

    inline const class_set& class_type::get_bases() const noexcept {
        return data_->bases;
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
    std::optional<value> class_type::create(Args&&... args) const {
        for ( auto&& ctor : data_->ctors ) {
            if ( ctor.second.is_invocable_with<Args...>() ) {
                return ctor.second.invoke(std::forward<Args>(args)...);
            }
        }
        return std::nullopt;
    }

    template < typename... Args >
    std::optional<value> class_type::operator()(Args&&... args) const {
        return create(std::forward<Args>(args)...);
    }

    template < detail::class_kind Derived >
    bool class_type::is_base_of() const noexcept {
        return is_base_of(resolve_type<Derived>());
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
        return is_derived_from(resolve_type<Base>());
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

    inline function class_type::get_function(std::string_view name) const noexcept {
        for ( auto&& [index, function] : data_->functions ) {
            if ( index.name == name ) {
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
            if ( index.name == name ) {
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
            if ( index.name == name ) {
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
            if ( index.name == name ) {
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
    // get_ctor_with
    //

    template < typename... Args >
    ctor class_type::get_ctor_with() const noexcept {
        return get_ctor_with({resolve_type<Args>()...});
    }

    inline ctor class_type::get_ctor_with(std::vector<any_type> args) const noexcept {
        for ( auto&& [index, ctor] : data_->ctors ) {
            if ( ctor.get_type().get_arity() != args.size() ) {
                continue;
            }

            const std::vector<any_type>& ctor_args = ctor.get_type().get_argument_types();
            if ( std::equal(args.begin(), args.end(), ctor_args.begin(), ctor_args.end()) ) {
                return ctor;
            }
        }

        return ctor{};
    }

    inline ctor class_type::get_ctor_with(std::initializer_list<any_type> args) const noexcept {
        for ( auto&& [index, ctor] : data_->ctors ) {
            if ( ctor.get_type().get_arity() != args.size() ) {
                continue;
            }

            const std::vector<any_type>& ctor_args = ctor.get_type().get_argument_types();
            if ( std::equal(args.begin(), args.end(), ctor_args.begin(), ctor_args.end()) ) {
                return ctor;
            }
        }

        return ctor{};
    }

    //
    // get_function_with
    //

    template < typename... Args >
    function class_type::get_function_with(std::string_view name) const noexcept {
        return get_function_with(name, {resolve_type<Args>()...});
    }

    inline function class_type::get_function_with(std::string_view name, std::vector<any_type> args) const noexcept {
        for ( auto&& [index, function] : data_->functions ) {
            if ( index.name != name ) {
                continue;
            }

            if ( function.get_type().get_arity() != args.size() ) {
                continue;
            }

            const std::vector<any_type>& function_args = function.get_type().get_argument_types();
            if ( std::equal(args.begin(), args.end(), function_args.begin(), function_args.end()) ) {
                return function;
            }
        }

        for ( auto&& base : data_->bases ) {
            if ( function function = base.get_function_with(name, args); function ) {
                return function;
            }
        }

        return function{};
    }

    inline function class_type::get_function_with(std::string_view name, std::initializer_list<any_type> args) const noexcept {
        for ( auto&& [index, function] : data_->functions ) {
            if ( index.name != name ) {
                continue;
            }

            if ( function.get_type().get_arity() != args.size() ) {
                continue;
            }

            const std::vector<any_type>& function_args = function.get_type().get_argument_types();
            if ( std::equal(args.begin(), args.end(), function_args.begin(), function_args.end()) ) {
                return function;
            }
        }

        for ( auto&& base : data_->bases ) {
            if ( function function = base.get_function_with(name, args); function ) {
                return function;
            }
        }

        return function{};
    }

    //
    // get_method_with
    //

    template < typename... Args >
    method class_type::get_method_with(std::string_view name) const noexcept {
        return get_method_with(name, {resolve_type<Args>()...});
    }

    inline method class_type::get_method_with(std::string_view name, std::vector<any_type> args) const noexcept {
        for ( auto&& [index, method] : data_->methods ) {
            if ( index.name != name ) {
                continue;
            }

            if ( method.get_type().get_arity() != args.size() ) {
                continue;
            }

            const std::vector<any_type>& method_args = method.get_type().get_argument_types();
            if ( std::equal(args.begin(), args.end(), method_args.begin(), method_args.end()) ) {
                return method;
            }
        }

        for ( auto&& base : data_->bases ) {
            if ( method method = base.get_method_with(name, args); method ) {
                return method;
            }
        }

        return method{};
    }

    inline method class_type::get_method_with(std::string_view name, std::initializer_list<any_type> args) const noexcept {
        for ( auto&& [index, method] : data_->methods ) {
            if ( index.name != name ) {
                continue;
            }

            if ( method.get_type().get_arity() != args.size() ) {
                continue;
            }

            const std::vector<any_type>& method_args = method.get_type().get_argument_types();
            if ( std::equal(args.begin(), args.end(), method_args.begin(), method_args.end()) ) {
                return method;
            }
        }

        for ( auto&& base : data_->bases ) {
            if ( method method = base.get_method_with(name, args); method ) {
                return method;
            }
        }

        return method{};
    }
}
