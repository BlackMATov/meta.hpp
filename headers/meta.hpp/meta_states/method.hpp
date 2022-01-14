/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_states.hpp"

#include "../meta_types/method_type.hpp"

namespace meta_hpp::detail
{
    template < method_kind Method >
    std::optional<value> vargs_invoke(Method method, const inst& inst, std::span<const arg> args) {
        using mt = method_traits<Method>;
        using return_type = typename mt::return_type;
        using qualified_type = typename mt::qualified_type;
        using argument_types = typename mt::argument_types;

        if ( args.size() != mt::arity ) {
            throw std::logic_error("an attempt to call a method with an incorrect arity");
        }

        if ( !inst.can_cast_to<qualified_type>() ) {
            throw std::logic_error("an attempt to call a method with an incorrect instance type");
        }

        // NOLINTNEXTLINE(readability-named-parameter)
        return std::invoke([method = std::move(method), &inst, &args]<std::size_t... Is>(std::index_sequence<Is...>){
            if ( !(... && (args.data() + Is)->can_cast_to<type_list_at_t<Is, argument_types>>()) ) {
                throw std::logic_error("an attempt to call a method with incorrect argument types");
            }

            if constexpr ( std::is_void_v<return_type> ) {
                std::invoke(
                    std::move(method),
                    inst.cast<qualified_type>(),
                    (args.data() + Is)->cast<type_list_at_t<Is, argument_types>>()...);
                return std::nullopt;
            } else {
                return_type return_value{std::invoke(
                    std::move(method),
                    inst.cast<qualified_type>(),
                    (args.data() + Is)->cast<type_list_at_t<Is, argument_types>>()...)};
                return value{std::forward<return_type>(return_value)};
            }
        }, std::make_index_sequence<mt::arity>());
    }

    template < method_kind Method >
    bool vargs_is_invocable_with(const inst_base& inst, std::span<const arg_base> args) {
        using mt = method_traits<Method>;
        using qualified_type = typename mt::qualified_type;
        using argument_types = typename mt::argument_types;

        if ( args.size() != mt::arity ) {
            return false;
        }

        if ( !inst.can_cast_to<qualified_type>() ) {
            return false;
        }

        // NOLINTNEXTLINE(readability-named-parameter)
        return std::invoke([&args]<std::size_t... Is>(std::index_sequence<Is...>){
            return (... && (args.data() + Is)->can_cast_to<type_list_at_t<Is, argument_types>>());
        }, std::make_index_sequence<mt::arity>());
    }
}

namespace meta_hpp::detail
{
    template < method_kind Method >
    method_state::invoke_impl make_method_invoke(Method method) {
        using namespace std::placeholders;
        return std::bind(&vargs_invoke<Method>, std::move(method), _1, _2);
    }

    template < method_kind Method >
    method_state::is_invocable_with_impl make_method_is_invocable_with() {
        using namespace std::placeholders;
        return std::bind(&vargs_is_invocable_with<Method>, _1, _2);
    }
}

namespace meta_hpp::detail
{
    template < method_kind Method >
    method_state::method_state(method_index index, Method method)
    : index{std::move(index)}
    , invoke{make_method_invoke(std::move(method))}
    , is_invocable_with{make_method_is_invocable_with<Method>()} {}

    template < method_kind Method >
    method_state_ptr method_state::make(std::string name, Method method) {
        method_index index{method_type_data::get_static<Method>(), std::move(name)};
        return std::make_shared<method_state>(std::move(index), std::move(method));
    }
}

namespace meta_hpp
{
    inline method::method(detail::method_state_ptr state)
    : state_{std::move(state)} {}

    inline bool method::is_valid() const noexcept {
        return !!state_;
    }

    inline method::operator bool() const noexcept {
        return is_valid();
    }

    inline const method_index& method::get_index() const noexcept {
        return state_->index;
    }

    inline const method_type& method::get_type() const noexcept {
        return state_->index.type;
    }

    inline const std::string& method::get_name() const noexcept {
        return state_->index.name;
    }

    template < typename Instance, typename... Args >
    std::optional<value> method::invoke(Instance&& instance, Args&&... args) const {
        using namespace detail;
        const inst vinst{std::forward<Instance>(instance)};
        if constexpr ( sizeof...(Args) > 0 ) {
            const std::array<arg, sizeof...(Args)> vargs{arg{std::forward<Args>(args)}...};
            return state_->invoke(vinst, vargs);
        } else {
            return state_->invoke(vinst, {});
        }
    }

    template < typename Instance, typename... Args >
    std::optional<value> method::operator()(Instance&& instance, Args&&... args) const {
        return invoke(std::forward<Instance>(instance), std::forward<Args>(args)...);
    }

    template < typename Instance, typename... Args >
    bool method::is_invocable_with() const noexcept {
        using namespace detail;
        const inst_base vinst{type_list<Instance>{}};
        if constexpr ( sizeof...(Args) > 0 ) {
            const std::array<arg_base, sizeof...(Args)> vargs{arg_base{type_list<Args>{}}...};
            return state_->is_invocable_with(vinst, vargs);
        } else {
            return state_->is_invocable_with(vinst, {});
        }
    }

    template < typename Instance, typename... Args >
    bool method::is_invocable_with(Instance&& instance, Args&&... args) const noexcept {
        using namespace detail;
        const inst_base vinst{std::forward<Instance>(instance)};
        if constexpr ( sizeof...(Args) > 0 ) {
            const std::array<arg_base, sizeof...(Args)> vargs{arg_base{std::forward<Args>(args)}...};
            return state_->is_invocable_with(vinst, vargs);
        } else {
            return state_->is_invocable_with(vinst, {});
        }
    }
}
