/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
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

#include "../meta_detail/type_traits/class_traits.hpp"

namespace meta_hpp::detail
{
    template < class_kind Class >
    struct class_tag {};

    template < class_kind Class >
    class_type_data::class_type_data(type_list<Class>)
    : type_data_base{type_id{type_list<class_tag<Class>>{}}, type_kind::class_}
    , flags{class_traits<Class>::make_flags()}
    , size{class_traits<Class>::size}
    , align{class_traits<Class>::align}
    , argument_types{resolve_types(typename class_traits<Class>::argument_types{})} {}
}

namespace meta_hpp::detail
{
    template < typename Derived, typename Base >
        requires std::is_base_of_v<Base, Derived>
    inline class_type_data::upcast_func_t::upcast_func_t(std::in_place_type_t<Derived>, std::in_place_type_t<Base>)
    : upcast{[](void* from) -> void* { return static_cast<Base*>(static_cast<Derived*>(from)); }}
    , is_virtual_upcast{is_virtual_base_of_v<Base, Derived>}
    , target_class{resolve_type<Base>()} {}

    inline void* class_type_data::upcast_func_t::apply(void* ptr) const noexcept {
        return upcast(ptr);
    }

    inline const void* class_type_data::upcast_func_t::apply(const void* ptr) const noexcept {
        // NOLINTNEXTLINE(*-const-cast)
        return apply(const_cast<void*>(ptr));
    }
}

namespace meta_hpp::detail
{
    inline class_type_data::upcast_func_list_t::upcast_func_list_t(const upcast_func_t& _upcast)
    : upcasts{_upcast} {
        for ( const upcast_func_t& upcast : upcasts ) {
            if ( upcast.is_virtual_upcast ) {
                vbases.emplace(upcast.target_class);
            }
        }
    }

    inline class_type_data::upcast_func_list_t::upcast_func_list_t(class_set _vbases, std::vector<upcast_func_t> _upcasts)
    : vbases{std::move(_vbases)}
    , upcasts{std::move(_upcasts)} {}

    inline void* class_type_data::upcast_func_list_t::apply(void* ptr) const noexcept {
        for ( const upcast_func_t& upcast : upcasts ) {
            ptr = upcast.apply(ptr);
        }
        return ptr;
    }

    inline const void* class_type_data::upcast_func_list_t::apply(const void* ptr) const noexcept {
        // NOLINTNEXTLINE(*-const-cast)
        return apply(const_cast<void*>(ptr));
    }

    inline class_type_data::upcast_func_list_t operator+( //
        const class_type_data::upcast_func_list_t& l,
        const class_type_data::upcast_func_list_t& r
    ) {
        class_set new_vbases;
        new_vbases.insert(l.vbases.begin(), l.vbases.end());
        new_vbases.insert(r.vbases.begin(), r.vbases.end());

        std::vector<class_type_data::upcast_func_t> new_upcasts;
        new_upcasts.reserve(l.upcasts.size() + r.upcasts.size());
        new_upcasts.insert(new_upcasts.end(), l.upcasts.begin(), l.upcasts.end());
        new_upcasts.insert(new_upcasts.end(), r.upcasts.begin(), r.upcasts.end());

        return class_type_data::upcast_func_list_t{std::move(new_vbases), std::move(new_upcasts)};
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

    inline const any_type_list& class_type::get_argument_types() const noexcept {
        return data_->argument_types;
    }

    inline const class_set& class_type::get_base_classes() const noexcept {
        return data_->base_classes;
    }

    inline const class_set& class_type::get_derived_classes() const noexcept {
        return data_->derived_classes;
    }

    inline const constructor_set& class_type::get_constructors() const noexcept {
        return data_->constructors;
    }

    inline const destructor_set& class_type::get_destructors() const noexcept {
        return data_->destructors;
    }

    inline const function_set& class_type::get_functions() const noexcept {
        return data_->functions;
    }

    inline const member_set& class_type::get_members() const noexcept {
        return data_->members;
    }

    inline const method_set& class_type::get_methods() const noexcept {
        return data_->methods;
    }

    inline const typedef_map& class_type::get_typedefs() const noexcept {
        return data_->typedefs;
    }

    inline const variable_set& class_type::get_variables() const noexcept {
        return data_->variables;
    }

    template < typename... Args >
    uvalue class_type::create(Args&&... args) const {
        for ( const constructor& ctor : data_->constructors ) {
            if ( ctor.is_invocable_with(std::forward<Args>(args)...) ) {
                return ctor.create(std::forward<Args>(args)...);
            }
        }
        return uvalue{};
    }

    template < typename... Args >
    uvalue class_type::create_at(void* mem, Args&&... args) const {
        for ( const constructor& ctor : data_->constructors ) {
            if ( ctor.is_invocable_with(std::forward<Args>(args)...) ) {
                return ctor.create_at(mem, std::forward<Args>(args)...);
            }
        }
        return uvalue{};
    }

    template < typename Arg >
    bool class_type::destroy(Arg&& arg) const {
        if ( const destructor& dtor = get_destructor() ) {
            if ( dtor.is_invocable_with(std::forward<Arg>(arg)) ) {
                dtor.destroy(std::forward<Arg>(arg));
                return true;
            }
        }
        return false;
    }

    inline bool class_type::destroy_at(void* mem) const {
        if ( const destructor& dtor = get_destructor() ) {
            dtor.destroy_at(mem);
            return true;
        }
        return false;
    }

    template < detail::class_kind Derived >
    bool class_type::is_base_of() const noexcept {
        return is_base_of(resolve_type<Derived>());
    }

    inline bool class_type::is_base_of(const class_type& derived) const noexcept {
        return is_valid() && derived.is_valid() && derived.data_->deep_upcasts.contains(*this);
    }

    template < detail::class_kind Derived >
    bool class_type::is_direct_base_of() const noexcept {
        return is_direct_base_of(resolve_type<Derived>());
    }

    inline bool class_type::is_direct_base_of(const class_type& derived) const noexcept {
        return is_valid() && derived.is_valid() && derived.data_->base_upcasts.contains(*this);
    }

    template < detail::class_kind Derived >
    bool class_type::is_virtual_base_of() const noexcept {
        return is_virtual_base_of(resolve_type<Derived>());
    }

    inline bool class_type::is_virtual_base_of(const class_type& derived) const noexcept {
        if ( !is_valid() || !derived.is_valid() ) {
            return false;
        }

        const detail::class_type_data& derived_data = *derived.data_;
        const auto upcasts_range = derived_data.deep_upcasts.equal_range(*this);

        if ( upcasts_range.first == upcasts_range.second ) {
            return false;
        }

        const class_set& first_vbases = upcasts_range.first->second.vbases;
        return std::any_of(first_vbases.begin(), first_vbases.end(), [&upcasts_range](const class_type& vbase) {
            return std::all_of(std::next(upcasts_range.first), upcasts_range.second, [&vbase](auto&& upcasts_p) {
                return upcasts_p.second.vbases.contains(vbase);
            });
        });
    }

    template < detail::class_kind Base >
    bool class_type::is_derived_from() const noexcept {
        return is_derived_from(resolve_type<Base>());
    }

    inline bool class_type::is_derived_from(const class_type& base) const noexcept {
        return base.is_base_of(*this);
    }

    template < detail::class_kind Base >
    bool class_type::is_direct_derived_from() const noexcept {
        return is_direct_derived_from(resolve_type<Base>());
    }

    inline bool class_type::is_direct_derived_from(const class_type& base) const noexcept {
        return base.is_direct_base_of(*this);
    }

    template < detail::class_kind Base >
    bool class_type::is_virtual_derived_from() const noexcept {
        return is_virtual_derived_from(resolve_type<Base>());
    }

    inline bool class_type::is_virtual_derived_from(const class_type& base) const noexcept {
        return base.is_virtual_base_of(*this);
    }

    inline function class_type::get_function(std::string_view name) const noexcept {
        for ( const function& function : data_->functions ) {
            if ( function.get_name() == name ) {
                return function;
            }
        }

        for ( const class_type& base : data_->base_classes ) {
            if ( const function& function = base.get_function(name) ) {
                return function;
            }
        }

        return function{};
    }

    inline member class_type::get_member(std::string_view name) const noexcept {
        for ( const member& member : data_->members ) {
            if ( member.get_name() == name ) {
                return member;
            }
        }

        for ( const class_type& base : data_->base_classes ) {
            if ( const member& member = base.get_member(name) ) {
                return member;
            }
        }

        return member{};
    }

    inline method class_type::get_method(std::string_view name) const noexcept {
        for ( const method& method : data_->methods ) {
            if ( method.get_name() == name ) {
                return method;
            }
        }

        for ( const class_type& base : data_->base_classes ) {
            if ( const method& method = base.get_method(name) ) {
                return method;
            }
        }

        return method{};
    }

    inline any_type class_type::get_typedef(std::string_view name) const noexcept {
        if ( auto iter{data_->typedefs.find(name)}; iter != data_->typedefs.end() ) {
            return iter->second;
        }

        for ( const class_type& base : data_->base_classes ) {
            if ( const any_type& type = base.get_typedef(name) ) {
                return type;
            }
        }

        return any_type{};
    }

    inline variable class_type::get_variable(std::string_view name) const noexcept {
        for ( const variable& variable : data_->variables ) {
            if ( variable.get_name() == name ) {
                return variable;
            }
        }

        for ( const class_type& base : data_->base_classes ) {
            if ( const variable& variable = base.get_variable(name) ) {
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
        detail::type_registry& registry{detail::type_registry::instance()};
        return get_constructor_with({registry.resolve_type<Args>()...});
    }

    template < typename Iter >
    constructor class_type::get_constructor_with(Iter first, Iter last) const noexcept {
        for ( const constructor& ctor : data_->constructors ) {
            const any_type_list& args = ctor.get_type().get_argument_types();
            if ( std::equal(first, last, args.begin(), args.end()) ) {
                return ctor;
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
    function class_type::get_function_with(std::string_view name) const noexcept {
        detail::type_registry& registry{detail::type_registry::instance()};
        return get_function_with(name, {registry.resolve_type<Args>()...});
    }

    template < typename Iter >
    function class_type::get_function_with(std::string_view name, Iter first, Iter last) const noexcept {
        for ( const function& function : data_->functions ) {
            if ( function.get_name() != name ) {
                continue;
            }

            const any_type_list& args = function.get_type().get_argument_types();
            if ( std::equal(first, last, args.begin(), args.end()) ) {
                return function;
            }
        }

        for ( const class_type& base : data_->base_classes ) {
            if ( const function& function = base.get_function_with(name, first, last) ) {
                return function;
            }
        }

        return function{};
    }

    inline function class_type::get_function_with(std::string_view name, std::span<const any_type> args) const noexcept {
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
        detail::type_registry& registry{detail::type_registry::instance()};
        return get_method_with(name, {registry.resolve_type<Args>()...});
    }

    template < typename Iter >
    method class_type::get_method_with(std::string_view name, Iter first, Iter last) const noexcept {
        for ( const method& method : data_->methods ) {
            if ( method.get_name() != name ) {
                continue;
            }

            const any_type_list& args = method.get_type().get_argument_types();
            if ( std::equal(first, last, args.begin(), args.end()) ) {
                return method;
            }
        }

        for ( const class_type& base : data_->base_classes ) {
            if ( const method& method = base.get_method_with(name, first, last) ) {
                return method;
            }
        }

        return method{};
    }

    inline method class_type::get_method_with(std::string_view name, std::span<const any_type> args) const noexcept {
        return get_method_with(name, args.begin(), args.end());
    }

    inline method class_type::get_method_with(std::string_view name, std::initializer_list<any_type> args) const noexcept {
        return get_method_with(name, args.begin(), args.end());
    }
}
