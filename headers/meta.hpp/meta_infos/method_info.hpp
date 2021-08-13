/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "_infos_fwd.hpp"

#include "data_info.hpp"

namespace meta_hpp
{
    class method_info final {
    public:
        method_info() = default;

        void merge(const method_info& other);
        explicit operator bool() const noexcept;

        const std::string& name() const noexcept;
        const method_type& type() const noexcept;

        template < typename Instance, typename... Args >
        std::optional<value> invoke(Instance&& instance, Args&&... args) const;

        template < typename Inst, typename... Args >
        bool is_invocable_with() const noexcept;
    public:
        template < typename F >
        void visit(F&& f) const;

        template < typename F >
        void each_data(F&& f) const;

        data_info get_data_by_name(std::string_view name) const noexcept;
    private:
        template < typename Method > friend class method_;

        template < typename Method >
        explicit method_info(std::string name, Method method);
    private:
        struct state;
        std::shared_ptr<state> state_;
    };
}

namespace meta_hpp::detail
{
    using method_invoke = std::function<std::optional<value>(const inst&, const arg*, std::size_t)>;

    template < typename Method, std::size_t... Is >
    std::optional<value> raw_method_invoke_impl(
        Method method,
        const inst& inst,
        const arg* args,
        std::index_sequence<Is...>)
    {
        using mt = method_pointer_traits<Method>;
        using return_type = typename mt::return_type;
        using qualified_type = typename mt::qualified_type;
        using argument_types = typename mt::argument_types;

        if ( !inst.can_cast_to<qualified_type>() ) {
            throw std::logic_error("an attempt to call a method with an incorrect instance type");
        }

        if ( !(... && (args + Is)->can_cast_to<std::tuple_element_t<Is, argument_types>>()) ) {
            throw std::logic_error("an attempt to call a method with an incorrect argument types");
        }

        if constexpr ( std::is_void_v<return_type> ) {
            std::invoke(method,
                inst.cast<qualified_type>(),
                (args + Is)->cast<std::tuple_element_t<Is, argument_types>>()...);
            return std::nullopt;
        } else {
            return_type return_value{std::invoke(method,
                inst.cast<qualified_type>(),
                (args + Is)->cast<std::tuple_element_t<Is, argument_types>>()...)};
            return value{std::forward<return_type>(return_value)};
        }
    }

    template < typename Method >
    std::optional<value> raw_method_invoke(
        Method method,
        const inst& inst,
        const arg* args,
        std::size_t arg_count)
    {
        using mt = method_pointer_traits<Method>;

        if ( arg_count != mt::arity ) {
            throw std::logic_error("an attempt to call a method with an incorrect arity");
        }

        return raw_method_invoke_impl<Method>(
            method,
            inst,
            args,
            std::make_index_sequence<mt::arity>());
    }

    template < typename Method >
    method_invoke make_method_invoke(Method method) {
        using namespace std::placeholders;
        return std::bind(&raw_method_invoke<Method>, method, _1, _2, _3);
    }
}

namespace meta_hpp::detail
{
    using method_is_invocable_with = std::function<bool(const inst_base&, const arg_base*, std::size_t)>;

    template < typename Method, std::size_t... Is >
    bool raw_method_is_invocable_with_impl(
        const inst_base& inst_base,
        const arg_base* arg_bases,
        std::index_sequence<Is...>)
    {
        using mt = method_pointer_traits<Method>;
        using qualified_type = typename mt::qualified_type;
        using argument_types = typename mt::argument_types;

        return inst_base.can_cast_to<qualified_type>()
            && (... && (arg_bases + Is)->can_cast_to<std::tuple_element_t<Is, argument_types>>() );
    }

    template < typename Method >
    bool raw_method_is_invocable_with(
        const inst_base& inst_base,
        const arg_base* arg_bases,
        std::size_t arg_count)
    {
        using mt = method_pointer_traits<Method>;

        if ( arg_count != mt::arity ) {
            return false;
        }

        return raw_method_is_invocable_with_impl<Method>(
            inst_base,
            arg_bases,
            std::make_index_sequence<mt::arity>());
    }

    template < typename Method >
    method_is_invocable_with make_method_is_invocable_with() {
        using namespace std::placeholders;
        return std::bind(&raw_method_is_invocable_with<Method>, _1, _2, _3);
    }
}

namespace meta_hpp
{
    struct method_info::state final {
        std::string name;
        method_type type;
        data_info_map datas;
        detail::method_invoke invoke;
        detail::method_is_invocable_with is_invocable_with;
    };
}

namespace meta_hpp
{
    inline void method_info::merge(const method_info& other) {
        (void)other;
        ///TODO: implme
    }

    inline method_info::operator bool() const noexcept {
        return !!state_;
    }

    inline const std::string& method_info::name() const noexcept {
        return state_->name;
    }

    inline const method_type& method_info::type() const noexcept {
        return state_->type;
    }

    template < typename Instance, typename... Args >
    std::optional<value> method_info::invoke(Instance&& instance, Args&&... args) const {
        std::array<arg, sizeof...(Args)> vargs{arg{std::forward<Args>(args)}...};
        return state_->invoke(inst{std::forward<Instance>(instance)}, vargs.data(), vargs.size());
    }

    template < typename Inst, typename... Args >
    bool method_info::is_invocable_with() const noexcept {
        std::array<arg_base, sizeof...(Args)> arg_bases{arg_base{typename_arg<Args>}...};
        return state_->is_invocable_with(inst_base{typename_arg<Inst>}, arg_bases.data(), arg_bases.size());
    }
}

namespace meta_hpp
{
    template < typename F >
    void method_info::visit(F&& f) const {
        each_data(f);
    }

    template < typename F >
    void method_info::each_data(F&& f) const {
        for ( auto&& name_info : state_->datas ) {
            std::invoke(f, name_info.second);
        }
    }

    inline data_info method_info::get_data_by_name(std::string_view name) const noexcept {
        return detail::find_or_default(state_->datas, name);
    }
}

namespace meta_hpp
{
    template < typename Method >
    method_info::method_info(std::string name, Method method)
    : state_{std::make_shared<state>(state{
        std::move(name),
        type_db::get<Method>().template as<method_type>(),
        {},
        detail::make_method_invoke<Method>(method),
        detail::make_method_is_invocable_with<Method>(),
    })} {}
}
