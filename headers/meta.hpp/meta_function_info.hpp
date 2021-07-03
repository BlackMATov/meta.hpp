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
    template < typename Function >
    struct function_traits;

    template < typename R, typename... Args >
    struct function_traits<R(*)(Args...)> {
        static constexpr std::size_t arity = sizeof...(Args);
        using return_type = R;
        using argument_types = std::tuple<Args...>;
    };

    template < typename R, typename... Args >
    struct function_traits<R(*)(Args...) noexcept>
         : function_traits<R(*)(Args...)> {};

    template < auto Function, std::size_t... Is >
    std::optional<value> invoke(value* args, std::index_sequence<Is...>) {
        using ft = function_traits<decltype(Function)>;
        using return_type = typename ft::return_type;
        using argument_types = typename ft::argument_types;

        [[maybe_unused]] auto typed_arguments = std::make_tuple(
            (args + Is)->try_cast<std::tuple_element_t<Is, argument_types>>()...);

        if ( !(std::get<Is>(typed_arguments) && ...) ) {
            throw std::logic_error("an attempt to call a function with incorrect argument types");
        }

        if constexpr ( std::is_void_v<return_type> ) {
            std::invoke(Function,
                *std::get<Is>(typed_arguments)...);
            return std::nullopt;
        } else {
            return_type return_value = std::invoke(Function,
                *std::get<Is>(typed_arguments)...);
            return value{std::move(return_value)};
        }
    }

    template < auto Function >
    std::optional<value> invoke(value* args, std::size_t arg_count) {
        using ft = function_traits<decltype(Function)>;

        if ( arg_count != ft::arity ) {
            throw std::logic_error("an attempt to call a function with an incorrect arity");
        }

        return invoke<Function>(args, std::make_index_sequence<ft::arity>());
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
        const family_id& fid() const noexcept {
            return fid_;
        }

        const std::string& id() const noexcept {
            return id_;
        }

        template < typename... Args >
        std::optional<value> invoke(Args&&... args) const {
            std::array<value, sizeof...(Args)> vargs{{std::forward<Args>(args)...}};
            return invoke_(vargs.data(), vargs.size());
        }

        template < typename F >
        void each_data(F&& f) const {
            for ( auto&& id_info : datas_ ) {
                std::invoke(f, id_info.second);
            }
        }

        std::optional<data_info> get_data(std::string_view id) const {
            return detail::find_opt(datas_, id);
        }

        void merge(const function_info& other) {
            if ( fid() != other.fid() ) {
                throw std::logic_error("function_info::merge failed");
            }
            detail::merge_with(datas_, other.datas_, &data_info::merge);
        }
    private:
        template < auto Function >
        friend class function_;

        template < typename FunctionType, FunctionType Function >
        function_info(detail::auto_arg_t<Function>, std::string id)
        : fid_{get_family_id<FunctionType>()}
        , id_{std::move(id)}
        , invoke_{&function_detail::invoke<Function>} {}
    private:
        family_id fid_;
        std::string id_;
        std::optional<value>(*invoke_)(value*, std::size_t);
        std::map<std::string, data_info, std::less<>> datas_;
    };
}
