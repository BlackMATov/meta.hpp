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
    class_type_data::class_type_data(type_list<Class>)
    : type_data_base{type_kind::class_}
    , flags{class_traits<Class>::make_flags()}
    , size{class_traits<Class>::size}
    , align{class_traits<Class>::align}
    , argument_types{resolve_types(typename class_traits<Class>::argument_types{})} {
        if constexpr ( check_base_info_enabled<Class> ) {
            using meta_base_info = typename Class::meta_base_info;

            [this]<std::size_t... Is>(std::index_sequence<Is...>) {
                struct {
                    class_list base_classes;
                    base_upcasts_t base_upcasts;
                    deep_upcasts_t deep_upcasts;
                } new_base_data;

                [[maybe_unused]] const auto add_base_class = [&new_base_data]<class_kind Base>() {
                    const class_type& base_class = resolve_type<Base>();
                    const class_type_data& base_data = *type_access(base_class);

                    upcast_func_t self_to_base{std::in_place_type<Class>, std::in_place_type<Base>};
                    upcast_func_list_t self_to_base_list{self_to_base};

                    new_base_data.base_classes.emplace_back(base_class);
                    new_base_data.base_upcasts.emplace(base_class, self_to_base);

                    for ( const auto& [deep_class, base_to_deep] : base_data.deep_upcasts ) {
                        upcast_func_list_t self_to_deep = self_to_base_list + base_to_deep;

                        const auto& [position, emplaced] = new_base_data.deep_upcasts.try_emplace(
                            deep_class, std::move(self_to_deep)
                        );

                        if ( !emplaced ) {
                            position->second.is_ambiguous = is_disjoint(position->second.vbases, self_to_deep.vbases);
                        }
                    }

                    new_base_data.deep_upcasts.emplace(base_class, std::move(self_to_base_list));
                };
                (add_base_class.template operator()<type_list_at_t<Is, meta_base_info>>(), ...);

                base_classes.swap(new_base_data.base_classes);
                base_upcasts.swap(new_base_data.base_upcasts);
                deep_upcasts.swap(new_base_data.deep_upcasts);
            }(std::make_index_sequence<type_list_arity_v<meta_base_info>>());
        }
    }
}

namespace meta_hpp::detail
{
    template < typename Derived, typename Base >
        requires std::is_base_of_v<Base, Derived>
    inline class_type_data::upcast_func_t::upcast_func_t(std::in_place_type_t<Derived>, std::in_place_type_t<Base>)
    : upcast{[](void* from) -> void* { return static_cast<Base*>(static_cast<Derived*>(from)); }}
    , target{resolve_type<Base>()}
    , is_virtual{is_virtual_base_of_v<Base, Derived>} {}

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
    inline class_type_data::upcast_func_list_t::upcast_func_list_t(const upcast_func_t& in_upcast)
    : upcasts{in_upcast} {
        for ( const upcast_func_t& upcast : upcasts ) {
            if ( upcast.is_virtual ) {
                vbases.emplace(upcast.target);
            }
        }
    }

    inline class_type_data::upcast_func_list_t::upcast_func_list_t(upcasts_t in_upcasts, vbases_t in_vbases)
    : upcasts{std::move(in_upcasts)}
    , vbases{std::move(in_vbases)} {}

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
        class_type_data::upcast_func_list_t::upcasts_t new_upcasts;
        new_upcasts.reserve(l.upcasts.size() + r.upcasts.size());
        new_upcasts.insert(new_upcasts.end(), l.upcasts.begin(), l.upcasts.end());
        new_upcasts.insert(new_upcasts.end(), r.upcasts.begin(), r.upcasts.end());

        class_type_data::upcast_func_list_t::vbases_t new_vbases;
        new_vbases.insert(l.vbases.begin(), l.vbases.end());
        new_vbases.insert(r.vbases.begin(), r.vbases.end());

        class_type_data::upcast_func_list_t result{std::move(new_upcasts), std::move(new_vbases)};
        result.is_ambiguous = l.is_ambiguous || r.is_ambiguous;
        return result;
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

    inline const class_list& class_type::get_base_classes() const noexcept {
        return data_->base_classes;
    }

    inline const constructor_list& class_type::get_constructors() const noexcept {
        return data_->constructors;
    }

    inline const destructor_list& class_type::get_destructors() const noexcept {
        return data_->destructors;
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
        for ( const constructor& ctor : data_->constructors ) {
            if ( ctor.is_invocable_with(META_HPP_FWD(args)...) ) {
                // there is no 'use after move' here because
                // 'is_invocable_with' doesn't actually move 'args'
                return ctor.create(META_HPP_FWD(args)...);
            }
        }
        return uvalue{};
    }

    template < typename... Args >
    uvalue class_type::create_at(void* mem, Args&&... args) const {
        for ( const constructor& ctor : data_->constructors ) {
            if ( ctor.is_invocable_with(META_HPP_FWD(args)...) ) {
                // there is no 'use after move' here because
                // 'is_invocable_with' doesn't actually move 'args'
                return ctor.create_at(mem, META_HPP_FWD(args)...);
            }
        }
        return uvalue{};
    }

    template < typename Arg >
    bool class_type::destroy(Arg&& arg) const {
        if ( const destructor& dtor = get_destructor() ) {
            if ( dtor.is_invocable_with(META_HPP_FWD(arg)) ) {
                // there is no 'use after move' here because
                // 'is_invocable_with' doesn't actually move an 'arg'
                dtor.destroy(META_HPP_FWD(arg));
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

        using deep_upcasts_t = detail::class_type_data::deep_upcasts_t;
        const deep_upcasts_t& deep_upcasts = derived.data_->deep_upcasts;

        if ( auto iter{deep_upcasts.find(*this)}; iter != deep_upcasts.end() ) {
            return !iter->second.is_ambiguous && !iter->second.vbases.empty();
        }

        return false;
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
    function class_type::get_function_with( //
        std::string_view name,
        bool recursively
    ) const noexcept {
        detail::type_registry& registry{detail::type_registry::instance()};
        return get_function_with(name, {registry.resolve_type<Args>()...}, recursively);
    }

    template < typename Iter >
    function class_type::get_function_with( //
        std::string_view name,
        Iter first,
        Iter last,
        bool recursively
    ) const noexcept {
        for ( const function& function : data_->functions ) {
            if ( function.get_name() != name ) {
                continue;
            }

            const any_type_list& args = function.get_type().get_argument_types();
            if ( std::equal(first, last, args.begin(), args.end()) ) {
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
    ) const noexcept {
        detail::type_registry& registry{detail::type_registry::instance()};
        return get_method_with(name, {registry.resolve_type<Args>()...}, recursively);
    }

    template < typename Iter >
    method class_type::get_method_with( //
        std::string_view name,
        Iter first,
        Iter last,
        bool recursively
    ) const noexcept {
        for ( const method& method : data_->methods ) {
            if ( method.get_name() != name ) {
                continue;
            }

            const any_type_list& args = method.get_type().get_argument_types();
            if ( std::equal(first, last, args.begin(), args.end()) ) {
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
