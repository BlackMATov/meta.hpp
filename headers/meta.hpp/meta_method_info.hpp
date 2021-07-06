/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_fwd.hpp"

#include "meta_instance.hpp"
#include "meta_value.hpp"

#include "meta_data_info.hpp"

namespace meta_hpp::method_detail
{
    template < typename MethodType, std::size_t... Is >
    std::optional<value> raw_invoke_impl(
        MethodType method,
        instance instance,
        value* args,
        std::index_sequence<Is...>)
    {
        using mt = detail::method_traits<MethodType>;
        using return_type = typename mt::return_type;
        using instance_type = typename mt::instance_type;
        using argument_types = typename mt::argument_types;

        instance_type* typed_instance{instance.try_cast<instance_type>()};
        if ( !typed_instance ) {
            throw std::logic_error("an attempt to call a method with incorrect instance type");
        }

        [[maybe_unused]] auto typed_arguments = std::make_tuple(
            (args + Is)->try_cast<std::tuple_element_t<Is, argument_types>>()...);

        if ( !(std::get<Is>(typed_arguments) && ...) ) {
            throw std::logic_error("an attempt to call a method with incorrect argument types");
        }

        if constexpr ( std::is_void_v<return_type> ) {
            std::invoke(method, *typed_instance, std::move(*std::get<Is>(typed_arguments))...);
            return std::nullopt;
        } else {
            return_type return_value{std::invoke(method, *typed_instance, std::move(*std::get<Is>(typed_arguments))...)};
            return value{std::move(return_value)};
        }
    }

    template < typename MethodType >
    std::optional<value> raw_invoke(
        MethodType method,
        instance instance,
        value* args,
        std::size_t arg_count)
    {
        using mt = detail::method_traits<MethodType>;

        if ( arg_count != mt::arity ) {
            throw std::logic_error("an attempt to call a method with an incorrect arity");
        }

        return raw_invoke_impl<MethodType>(method, instance, args, std::make_index_sequence<mt::arity>());
    }

    template < typename MethodType, std::size_t... Is >
    std::optional<value> raw_cinvoke_impl(
        [[maybe_unused]] MethodType method,
        [[maybe_unused]] cinstance instance,
        [[maybe_unused]] value* args,
        std::index_sequence<Is...>)
    {
        using mt = detail::method_traits<MethodType>;
        using return_type = typename mt::return_type;
        using instance_type = typename mt::instance_type;
        using argument_types = typename mt::argument_types;

        if constexpr ( mt::is_const ) {
            const instance_type* typed_instance{instance.try_cast<instance_type>()};
            if ( !typed_instance ) {
                throw std::logic_error("an attempt to call a method with incorrect instance type");
            }

            [[maybe_unused]] auto typed_arguments = std::make_tuple(
                (args + Is)->try_cast<std::tuple_element_t<Is, argument_types>>()...);

            if ( !(std::get<Is>(typed_arguments) && ...) ) {
                throw std::logic_error("an attempt to call a method with incorrect argument types");
            }

            if constexpr ( std::is_void_v<return_type> ) {
                std::invoke(method, *typed_instance, std::move(*std::get<Is>(typed_arguments))...);
                return std::nullopt;
            } else {
                return_type return_value{std::invoke(method, *typed_instance, std::move(*std::get<Is>(typed_arguments))...)};
                return value{std::move(return_value)};
            }
        } else {
            throw std::logic_error("an attempt to call a non-constant method by constant instance");
        }
    }

    template < typename MethodType >
    std::optional<value> raw_cinvoke(
        MethodType method,
        cinstance instance,
        value* args,
        std::size_t arg_count)
    {
        using mt = detail::method_traits<MethodType>;

        if ( arg_count != mt::arity ) {
            throw std::logic_error("an attempt to call a method with a different arity");
        }

        return raw_cinvoke_impl<MethodType>(method, instance, args, std::make_index_sequence<mt::arity>());
    }

    using method_invoke = std::function<std::optional<value>(instance, value*, std::size_t)>;
    using method_cinvoke = std::function<std::optional<value>(cinstance, value*, std::size_t)>;

    template < typename MethodType >
    method_invoke make_invoke(MethodType method) {
        using namespace std::placeholders;
        return std::bind(&raw_invoke<MethodType>, method, _1, _2, _3);
    }

    template < typename MethodType >
    method_cinvoke make_cinvoke(MethodType method) {
        using namespace std::placeholders;
        return std::bind(&raw_cinvoke<MethodType>, method, _1, _2, _3);
    }
}

namespace meta_hpp
{
    class method_info {
    public:
        method_info() = delete;

        method_info(method_info&&) = default;
        method_info(const method_info&) = default;

        method_info& operator=(method_info&&) = default;
        method_info& operator=(const method_info&) = default;
    public:
        const std::string& id() const noexcept {
            return id_;
        }

        template < typename T, typename... Args >
        std::optional<value> invoke(T& inst, Args&&... args) const {
            if constexpr ( sizeof...(Args) > 0u ) {
                std::array<value, sizeof...(Args)> vargs{std::forward<Args>(args)...};
                return invoke_(inst, vargs.data(), vargs.size());
            } else {
                return invoke_(inst, nullptr, 0u);
            }
        }

        template < typename T, typename... Args >
        std::optional<value> invoke(const T& inst, Args&&... args) const {
            if constexpr ( sizeof...(Args) > 0u ) {
                std::array<value, sizeof...(Args)> vargs{std::forward<Args>(args)...};
                return cinvoke_(inst, vargs.data(), vargs.size());
            } else {
                return cinvoke_(inst, nullptr, 0u);
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

        void merge(const method_info& other) {
            if ( id() != other.id() ) {
                throw std::logic_error("method_info::merge failed");
            }
            detail::merge_with(datas_, other.datas_, &data_info::merge);
        }
    private:
        template < typename MethodType >
        friend class method_;

        template < typename MethodType >
        method_info(std::string id, MethodType method_ptr)
        : id_{std::move(id)}
        , invoke_{method_detail::make_invoke(method_ptr)}
        , cinvoke_{method_detail::make_cinvoke(method_ptr)} {}
    private:
        std::string id_;
        method_detail::method_invoke invoke_;
        method_detail::method_cinvoke cinvoke_;
        std::map<std::string, data_info, std::less<>> datas_;
    };
}
