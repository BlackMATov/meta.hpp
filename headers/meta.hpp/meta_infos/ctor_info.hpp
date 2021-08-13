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
    class ctor_info final {
    public:
        ctor_info() = default;

        void merge(const ctor_info& other);
        explicit operator bool() const noexcept;

        const ctor_type& type() const noexcept;

        template < typename... Args >
        value invoke(Args&&... args) const;

        template < typename... Args >
        bool is_invocable_with() const noexcept;
    public:
        template < typename F >
        void visit(F&& f) const;

        template < typename F >
        void each_data(F&& f) const;

        data_info get_data_by_name(std::string_view name) const noexcept;
    private:
        template < typename... Args > friend class ctor_;

        template < typename Class, typename... Args >
        explicit ctor_info(typename_arg_t<Class>, typename_arg_t<Args...>);
    private:
        struct state;
        std::shared_ptr<state> state_;
    };
}

namespace meta_hpp::detail
{
    using ctor_invoke = std::function<value(const arg*, std::size_t)>;

    template < typename Class, typename... Args, std::size_t... Is >
    value raw_ctor_invoke_impl(
        const arg* args,
        std::index_sequence<Is...>)
    {
        using ct = ctor_traits<Class, Args...>;
        using class_type = typename ct::class_type;
        using argument_types = typename ct::argument_types;

        if ( !(... && (args + Is)->can_cast_to<std::tuple_element_t<Is, argument_types>>()) ) {
            throw std::logic_error("an attempt to call a ctor with an incorrect argument types");
        }

        class_type class_value{(args + Is)->cast<std::tuple_element_t<Is, argument_types>>()...};
        return value{std::forward<class_type>(class_value)};
    }

    template < typename Class, typename... Args >
    value raw_ctor_invoke(
        const arg* args,
        std::size_t arg_count)
    {
        using ct = ctor_traits<Class, Args...>;

        if ( arg_count != ct::arity ) {
            throw std::logic_error("an attempt to call a ctor with an incorrect arity");
        }

        return raw_ctor_invoke_impl<Class, Args...>(
            args,
            std::make_index_sequence<ct::arity>());
    }

    template < typename Class, typename... Args >
    ctor_invoke make_ctor_invoke() {
        using namespace std::placeholders;
        return std::bind(&raw_ctor_invoke<Class, Args...>, _1, _2);
    }
}

namespace meta_hpp::detail
{
    using ctor_is_invocable_with = std::function<bool(const arg_base*, std::size_t)>;

    template < typename Class, typename... Args, std::size_t... Is >
    bool raw_ctor_is_invocable_with_impl(
        const arg_base* arg_bases,
        std::index_sequence<Is...>)
    {
        using ct = ctor_traits<Class, Args...>;
        using argument_types = typename ct::argument_types;
        return (... && (arg_bases + Is)->can_cast_to<std::tuple_element_t<Is, argument_types>>() );
    }

    template < typename Class, typename... Args >
    bool raw_ctor_is_invocable_with(
        const arg_base* arg_bases,
        std::size_t arg_count)
    {
        using ct = ctor_traits<Class, Args...>;

        if ( arg_count != ct::arity ) {
            return false;
        }

        return raw_ctor_is_invocable_with_impl<Class, Args...>(
            arg_bases,
            std::make_index_sequence<ct::arity>());
    }

    template < typename Class, typename... Args >
    ctor_is_invocable_with make_ctor_is_invocable_with() {
        using namespace std::placeholders;
        return std::bind(&raw_ctor_is_invocable_with<Class, Args...>, _1, _2);
    }
}

namespace meta_hpp
{
    struct ctor_info::state final {
        ctor_type type;
        data_info_map datas;
        detail::ctor_invoke invoke;
        detail::ctor_is_invocable_with is_invocable_with;
    };
}

namespace meta_hpp
{
    inline void ctor_info::merge(const ctor_info& other) {
        (void)other;
        ///TODO: implme
    }

    inline ctor_info::operator bool() const noexcept {
        return !!state_;
    }

    inline const ctor_type& ctor_info::type() const noexcept {
        return state_->type;
    }

    template < typename... Args >
    value ctor_info::invoke(Args&&... args) const {
        std::array<arg, sizeof...(Args)> vargs{arg{std::forward<Args>(args)}...};
        return state_->invoke(vargs.data(), vargs.size());
    }

    template < typename... Args >
    bool ctor_info::is_invocable_with() const noexcept {
        std::array<arg_base, sizeof...(Args)> arg_bases{arg_base{typename_arg<Args>}...};
        return state_->is_invocable_with(arg_bases.data(), arg_bases.size());
    }
}

namespace meta_hpp
{
    template < typename F >
    void ctor_info::visit(F&& f) const {
        each_data(f);
    }

    template < typename F >
    void ctor_info::each_data(F&& f) const {
        for ( auto&& name_info : state_->datas ) {
            std::invoke(f, name_info.second);
        }
    }

    inline data_info ctor_info::get_data_by_name(std::string_view name) const noexcept {
        return detail::find_or_default(state_->datas, name);
    }
}

namespace meta_hpp
{
    template < typename Class, typename... Args >
    ctor_info::ctor_info(typename_arg_t<Class>, typename_arg_t<Args...>)
    : state_{std::make_shared<state>(state{
        ctor_type{typename_arg<Class>, typename_arg<Args...>},
        {},
        detail::make_ctor_invoke<Class, Args...>(),
        detail::make_ctor_is_invocable_with<Class, Args...>(),
    })} {}
}
