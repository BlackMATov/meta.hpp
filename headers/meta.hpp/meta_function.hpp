/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_fwd.hpp"

#include "meta_value.hpp"

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
    value invoke(value* args, std::index_sequence<Is...>) {
        using ft = function_traits<decltype(Function)>;
        using return_type = typename ft::return_type;
        using argument_types = typename ft::argument_types;

        auto typed_arguments = std::make_tuple(
            (args + Is)->try_cast<std::tuple_element_t<Is, argument_types>>()...);

        if ( !(std::get<Is>(typed_arguments) && ...) ) {
            throw std::logic_error("an attempt to call a function with incorrect argument types");
        }

        if constexpr ( std::is_void_v<return_type> ) {
            std::invoke(Function,
                *std::get<Is>(typed_arguments)...);
            return value{};
        } else {
            return_type return_value = std::invoke(Function,
                *std::get<Is>(typed_arguments)...);
            return value{std::move(return_value)};
        }
    }

    template < auto Function >
    value invoke(value* args, std::size_t arg_count) {
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

        function_info(std::string id)
        : id_(std::move(id)) {}

        const std::string& id() const noexcept {
            return id_;
        }
    public:
        template < typename... Args >
        value invoke(Args&&... args) const {
            std::array<value, sizeof...(Args)> vargs{{std::forward<Args>(args)...}};
            return invoke_(vargs.data(), vargs.size());
        }
    private:
        friend class class_info;
        friend class namespace_info;

        template < typename Class > friend class class_;
        friend class namespace_;

        template < auto Function > friend class function_;
    private:
        void merge_with_(const function_info& other) {
            (void)other;
        }
    private:
        std::string id_;
        value(*invoke_)(value*, std::size_t);
    };
}

namespace meta_hpp
{
    template < auto Function >
    class function_ {
    public:
        static_assert(std::is_function_v<std::remove_pointer_t<decltype(Function)>>);

        explicit function_(std::string id)
        : info_(std::move(id)) {
            info_.invoke_ = &function_detail::invoke<Function>;
        }

        const function_info& info() const noexcept {
            return info_;
        }
    private:
        function_info info_;
    };
}
