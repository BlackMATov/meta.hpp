/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_fwd.hpp"

#include "meta_value.hpp"

#include "meta_data_info.hpp"

namespace meta_hpp::function_detail
{
    template < typename FunctionType, std::size_t... Is >
    std::optional<value> raw_invoke_impl(
        FunctionType function,
        value* args,
        std::index_sequence<Is...>)
    {
        using ft = detail::function_traits<FunctionType>;
        using return_type = typename ft::return_type;
        using argument_types = typename ft::argument_types;

        [[maybe_unused]] auto typed_arguments = std::make_tuple(
            (args + Is)->try_cast<std::tuple_element_t<Is, argument_types>>()...);

        if ( !(std::get<Is>(typed_arguments) && ...) ) {
            throw std::logic_error("an attempt to call a function with incorrect argument types");
        }

        if constexpr ( std::is_void_v<return_type> ) {
            std::invoke(function, std::move(*std::get<Is>(typed_arguments))...);
            return std::nullopt;
        } else {
            return_type return_value{std::invoke(function, std::move(*std::get<Is>(typed_arguments))...)};
            return value{std::move(return_value)};
        }
    }

    template < typename FunctionType >
    std::optional<value> raw_invoke(
        FunctionType function,
        value* args,
        std::size_t arg_count)
    {
        using ft = detail::function_traits<FunctionType>;

        if ( arg_count != ft::arity ) {
            throw std::logic_error("an attempt to call a function with an incorrect arity");
        }

        return raw_invoke_impl<FunctionType>(function, args, std::make_index_sequence<ft::arity>());
    }

    using function_invoke = std::function<std::optional<value>(value*, std::size_t)>;

    template < typename FunctionType >
    function_invoke make_invoke(FunctionType function) {
        using namespace std::placeholders;
        return std::bind(&raw_invoke<FunctionType>, function, _1, _2);
    }
}

namespace meta_hpp
{
    class function_info {
    public:
        function_info() = delete;

        function_info(function_info&&) = default;
        function_info(const function_info&) = default;

        function_info& operator=(function_info&&) = default;
        function_info& operator=(const function_info&) = default;
    public:
        const std::string& id() const noexcept {
            return id_;
        }

        template < typename... Args >
        std::optional<value> invoke(Args&&... args) const {
            if constexpr ( sizeof...(Args) > 0u ) {
                std::array<value, sizeof...(Args)> vargs{std::forward<Args>(args)...};
                return invoke_(vargs.data(), vargs.size());
            } else {
                return invoke_(nullptr, 0u);
            }
        }

        template < typename F >
        void each_data(F&& f) const {
            for ( auto&& id_info : datas_ ) {
                std::invoke(f, id_info.second);
            }
        }

        template < typename F >
        void visit(F&& f) const {
            each_data(f);
        }

        std::optional<data_info> get_data(std::string_view id) const {
            return detail::find_opt(datas_, id);
        }

        void merge(const function_info& other) {
            if ( id() != other.id() ) {
                throw std::logic_error("function_info::merge failed");
            }
            detail::merge_with(datas_, other.datas_, &data_info::merge);
        }
    private:
        template < typename FunctionType >
        friend class function_;

        template < typename FunctionType >
        function_info(std::string id, FunctionType function)
        : id_{std::move(id)}
        , invoke_{function_detail::make_invoke(function)} {}
    private:
        std::string id_;
        function_detail::function_invoke invoke_;
        std::map<std::string, data_info, std::less<>> datas_;
    };
}
