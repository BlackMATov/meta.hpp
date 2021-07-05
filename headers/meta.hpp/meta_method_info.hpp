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
    template < auto Method, std::size_t... Is >
    std::optional<value> invoke(instance instance, value* args, std::index_sequence<Is...>) {
        using mt = detail::method_traits<decltype(Method)>;
        using return_type = typename mt::return_type;
        using instance_type = typename mt::instance_type;
        using argument_types = typename mt::argument_types;

        instance_type* typed_instance{instance.try_cast<instance_type>()};
        if ( !typed_instance ) {
            throw std::logic_error("an attempt to call a method with incorrect instance type");
        }

        auto typed_arguments = std::make_tuple(
            (args + Is)->try_cast<std::tuple_element_t<Is, argument_types>>()...);

        if ( !(std::get<Is>(typed_arguments) && ...) ) {
            throw std::logic_error("an attempt to call a method with incorrect argument types");
        }

        if constexpr ( std::is_void_v<return_type> ) {
            std::invoke(Method, *typed_instance, std::move(*std::get<Is>(typed_arguments))...);
            return std::nullopt;
        } else {
            return_type return_value{std::invoke(Method, *typed_instance, std::move(*std::get<Is>(typed_arguments))...)};
            return value{std::move(return_value)};
        }
    }

    template < auto Method >
    std::optional<value> invoke(instance instance, value* args, std::size_t arg_count) {
        using mt = detail::method_traits<decltype(Method)>;

        if ( arg_count != mt::arity ) {
            throw std::logic_error("an attempt to call a method with an incorrect arity");
        }

        return invoke<Method>(instance, args, std::make_index_sequence<mt::arity>());
    }

    template < auto Method, std::size_t... Is >
    std::optional<value> cinvoke([[maybe_unused]] cinstance instance, value* args, std::index_sequence<Is...>) {
        using mt = detail::method_traits<decltype(Method)>;
        using return_type = typename mt::return_type;
        using instance_type = typename mt::instance_type;
        using argument_types = typename mt::argument_types;

        if constexpr ( mt::is_const ) {
            const instance_type* typed_instance{instance.try_cast<instance_type>()};
            if ( !typed_instance ) {
                throw std::logic_error("an attempt to call a method with incorrect instance type");
            }

            auto typed_arguments = std::make_tuple(
                (args + Is)->try_cast<std::tuple_element_t<Is, argument_types>>()...);

            if ( !(std::get<Is>(typed_arguments) && ...) ) {
                throw std::logic_error("an attempt to call a method with incorrect argument types");
            }

            if constexpr ( std::is_void_v<return_type> ) {
                std::invoke(Method, *typed_instance, std::move(*std::get<Is>(typed_arguments))...);
                return std::nullopt;
            } else {
                return_type return_value{std::invoke(Method, *typed_instance, std::move(*std::get<Is>(typed_arguments))...)};
                return value{std::move(return_value)};
            }
        } else {
            throw std::logic_error("an attempt to call a non-constant method by constant instance");
        }
    }

    template < auto Method >
    std::optional<value> cinvoke(cinstance instance, value* args, std::size_t arg_count) {
        using mt = detail::method_traits<decltype(Method)>;

        if ( arg_count != mt::arity ) {
            throw std::logic_error("an attempt to call a method with a different arity");
        }

        return cinvoke<Method>(instance, args, std::make_index_sequence<mt::arity>());
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
        const family_id& fid() const noexcept {
            return fid_;
        }

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
            if ( fid() != other.fid() ) {
                throw std::logic_error("method_info::merge failed");
            }
            detail::merge_with(datas_, other.datas_, &data_info::merge);
        }
    private:
        template < auto Method >
        friend class method_;

        template < auto Method >
        method_info(detail::auto_arg_t<Method>, std::string id)
        : fid_{get_value_family_id<Method>()}
        , id_{std::move(id)}
        , invoke_{&method_detail::invoke<Method>}
        , cinvoke_{&method_detail::cinvoke<Method>} {}
    private:
        family_id fid_;
        std::string id_;
        std::optional<value>(*invoke_)(instance, value*, std::size_t);
        std::optional<value>(*cinvoke_)(cinstance, value*, std::size_t);
        std::map<std::string, data_info, std::less<>> datas_;
    };
}
