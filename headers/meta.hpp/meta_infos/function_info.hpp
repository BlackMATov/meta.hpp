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
    class function_info final {
    public:
        function_info() = default;

        void merge(const function_info& other);
        explicit operator bool() const noexcept;

        const std::string& name() const noexcept;
        const function_type& type() const noexcept;

        template < typename... Args >
        std::optional<value> invoke(Args&&... args) const;

        template < typename... Args >
        bool is_invocable_with() const noexcept;
    public:
        template < typename F >
        void visit(F&& f) const;

        template < typename F >
        void each_data(F&& f) const;

        data_info get_data_by_name(std::string_view name) const noexcept;
    private:
        template < typename Function > friend class function_;

        template < typename Function >
        explicit function_info(std::string name, Function function);
    private:
        struct state;
        std::shared_ptr<state> state_;
    };
}

namespace meta_hpp::detail
{
    using function_invoke = std::function<std::optional<value>(const arg*, std::size_t)>;

    template < typename Function, std::size_t... Is >
    std::optional<value> raw_function_invoke_impl(
        Function function,
        const arg* args,
        std::index_sequence<Is...>)
    {
        using ft = function_pointer_traits<Function>;
        using return_type = typename ft::return_type;
        using argument_types = typename ft::argument_types;

        if ( !(... && (args + Is)->can_cast_to<std::tuple_element_t<Is, argument_types>>()) ) {
            throw std::logic_error("an attempt to call a function with an incorrect argument types");
        }

        if constexpr ( std::is_void_v<return_type> ) {
            std::invoke(function,
                (args + Is)->cast<std::tuple_element_t<Is, argument_types>>()...);
            return std::nullopt;
        } else {
            return_type return_value{std::invoke(function,
                (args + Is)->cast<std::tuple_element_t<Is, argument_types>>()...)};
            return value{std::forward<return_type>(return_value)};
        }
    }

    template < typename Function >
    std::optional<value> raw_function_invoke(
        Function function,
        const arg* args,
        std::size_t arg_count)
    {
        using ft = function_pointer_traits<Function>;

        if ( arg_count != ft::arity ) {
            throw std::logic_error("an attempt to call a function with an incorrect arity");
        }

        return raw_function_invoke_impl<Function>(
            function,
            args,
            std::make_index_sequence<ft::arity>());
    }

    template < typename Function >
    function_invoke make_function_invoke(Function function) {
        using namespace std::placeholders;
        return std::bind(&raw_function_invoke<Function>, function, _1, _2);
    }
}

namespace meta_hpp::detail
{
    using function_is_invocable_with = std::function<bool(const arg_base*, std::size_t)>;

    template < typename Function, std::size_t... Is >
    bool raw_function_is_invocable_with_impl(
        const arg_base* arg_bases,
        std::index_sequence<Is...>)
    {
        using ft = function_pointer_traits<Function>;
        using argument_types = typename ft::argument_types;
        return (... && (arg_bases + Is)->can_cast_to<std::tuple_element_t<Is, argument_types>>() );
    }

    template < typename Function >
    bool raw_function_is_invocable_with(
        const arg_base* arg_bases,
        std::size_t arg_count)
    {
        using ft = function_pointer_traits<Function>;

        if ( arg_count != ft::arity ) {
            return false;
        }

        return raw_function_is_invocable_with_impl<Function>(
            arg_bases,
            std::make_index_sequence<ft::arity>());
    }

    template < typename Function >
    function_is_invocable_with make_function_is_invocable_with() {
        using namespace std::placeholders;
        return std::bind(&raw_function_is_invocable_with<Function>, _1, _2);
    }
}

namespace meta_hpp
{
    struct function_info::state final {
        std::string name;
        function_type type;
        data_info_map datas;
        detail::function_invoke invoke;
        detail::function_is_invocable_with is_invocable_with;
    };
}

namespace meta_hpp
{
    inline void function_info::merge(const function_info& other) {
        (void)other;
        ///TODO: implme
    }

    inline function_info::operator bool() const noexcept {
        return !!state_;
    }

    inline const std::string& function_info::name() const noexcept {
        return state_->name;
    }

    inline const function_type& function_info::type() const noexcept {
        return state_->type;
    }

    template < typename... Args >
    std::optional<value> function_info::invoke(Args&&... args) const {
        if constexpr ( sizeof...(Args) > 0 ) {
            std::array<arg, sizeof...(Args)> vargs{arg{std::forward<Args>(args)}...};
            return state_->invoke(vargs.data(), vargs.size());
        } else {
            return state_->invoke(nullptr, 0);
        }
    }

    template < typename... Args >
    bool function_info::is_invocable_with() const noexcept {
        if constexpr ( sizeof...(Args) > 0 ) {
            std::array<arg_base, sizeof...(Args)> arg_bases{arg_base{typename_arg<Args>}...};
            return state_->is_invocable_with(arg_bases.data(), arg_bases.size());
        } else {
            return state_->is_invocable_with(nullptr, 0);
        }
    }
}

namespace meta_hpp
{
    template < typename F >
    void function_info::visit(F&& f) const {
        each_data(f);
    }

    template < typename F >
    void function_info::each_data(F&& f) const {
        for ( auto&& name_info : state_->datas ) {
            std::invoke(f, name_info.second);
        }
    }

    inline data_info function_info::get_data_by_name(std::string_view name) const noexcept {
        return detail::find_or_default(state_->datas, name);
    }
}

namespace meta_hpp
{
    template < typename Function >
    function_info::function_info(std::string name, Function function)
    : state_{std::make_shared<state>(state{
        std::move(name),
        type_db::get<Function>().template as<function_type>(),
        {},
        detail::make_function_invoke<Function>(function),
        detail::make_function_is_invocable_with<Function>(),
    })} {}
}
