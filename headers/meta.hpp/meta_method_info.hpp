/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_fwd.hpp"
#include "meta_value.hpp"

#include "meta_data_info.hpp"

namespace meta_hpp::method_detail
{
    template < typename Method >
    struct method_traits;

    template < typename R, typename Base, typename... Args >
    struct method_traits<R(Base::*)(Args...)> {
        static constexpr bool is_const = false;
        static constexpr std::size_t arity = sizeof...(Args);
        using return_type = R;
        using instance_type = Base;
        using argument_types = std::tuple<Args...>;
    };

    template < typename R, typename Base, typename... Args >
    struct method_traits<R(Base::*)(Args...) const>
         : method_traits<R(Base::*)(Args...)> {
        static constexpr bool is_const = true;
    };

    template < typename R, typename Base, typename... Args >
    struct method_traits<R(Base::*)(Args...) noexcept>
         : method_traits<R(Base::*)(Args...)> {};

    template < typename R, typename Base, typename... Args >
    struct method_traits<R(Base::*)(Args...) const noexcept>
         : method_traits<R(Base::*)(Args...) const> {};

    template < auto Method, std::size_t... Is >
    value invoke([[maybe_unused]] void* instance, value* args, std::index_sequence<Is...>) {
        using mt = method_traits<decltype(Method)>;
        using return_type = typename mt::return_type;
        using instance_type = typename mt::instance_type;
        using argument_types = typename mt::argument_types;

        [[maybe_unused]] auto typed_arguments = std::make_tuple(
            (args + Is)->try_cast<std::tuple_element_t<Is, argument_types>>()...);

        if ( !(std::get<Is>(typed_arguments) && ...) ) {
            throw std::logic_error("an attempt to call a method with incorrect argument types");
        }

        if constexpr ( std::is_void_v<return_type> ) {
            std::invoke(Method,
                std::ref(*static_cast<instance_type*>(instance)),
                *std::get<Is>(typed_arguments)...);
            return value{};
        } else {
            return_type return_value = std::invoke(Method,
                std::ref(*static_cast<instance_type*>(instance)),
                *std::get<Is>(typed_arguments)...);
            return value{std::move(return_value)};
        }
    }

    template < auto Method >
    value invoke(void* instance, value* args, std::size_t arg_count) {
        using mt = method_traits<decltype(Method)>;

        if ( arg_count != mt::arity ) {
            throw std::logic_error("an attempt to call a method with an incorrect arity");
        }

        return invoke<Method>(instance, args, std::make_index_sequence<mt::arity>());
    }

    template < auto Method, std::size_t... Is >
    value cinvoke([[maybe_unused]] const void* instance, value* args, std::index_sequence<Is...>) {
        using mt = method_traits<decltype(Method)>;
        using return_type = typename mt::return_type;
        using instance_type = typename mt::instance_type;
        using argument_types = typename mt::argument_types;

        [[maybe_unused]] auto typed_arguments = std::make_tuple(
            (args + Is)->try_cast<std::tuple_element_t<Is, argument_types>>()...);

        if ( !(std::get<Is>(typed_arguments) && ...) ) {
            throw std::logic_error("an attempt to call a method with incorrect argument types");
        }

        if constexpr ( mt::is_const ) {
            if constexpr ( std::is_void_v<return_type> ) {
                std::invoke(Method,
                    std::ref(*static_cast<const instance_type*>(instance)),
                    *std::get<Is>(typed_arguments)...);
                return value{};
            } else {
                return_type return_value = std::invoke(Method,
                    std::ref(*static_cast<const instance_type*>(instance)),
                    *std::get<Is>(typed_arguments)...);
                return value{std::move(return_value)};
            }
        } else {
            throw std::logic_error("an attempt to call a non-constant method by constant instance");
        }
    }

    template < auto Method >
    value cinvoke(const void* instance, value* args, std::size_t arg_count) {
        using mt = method_traits<decltype(Method)>;

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

        template < typename... Args >
        value invoke(void* instance, Args&&... args) const {
            std::array<value, sizeof...(Args)> vargs{{std::forward<Args>(args)...}};
            return invoke_(instance, vargs.data(), vargs.size());
        }

        template < typename... Args >
        value invoke(const void* instance, Args&&... args) const {
            std::array<value, sizeof...(Args)> vargs{{std::forward<Args>(args)...}};
            return cinvoke_(instance, vargs.data(), vargs.size());
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

        void merge(const method_info& other) {
            if ( fid() != other.fid() ) {
                throw std::logic_error("method_info::merge failed");
            }
            detail::merge_with(datas_, other.datas_, &data_info::merge);
        }
    private:
        template < auto Method >
        friend class method_;

        template < typename MethodType, MethodType Method >
        method_info(detail::auto_arg_t<Method>, std::string id)
        : fid_{get_family_id<MethodType>()}
        , id_{std::move(id)}
        , invoke_{&method_detail::invoke<Method>}
        , cinvoke_{&method_detail::cinvoke<Method>} {}
    private:
        family_id fid_;
        std::string id_;
        value(*invoke_)(void*, value*, std::size_t);
        value(*cinvoke_)(const void*, value*, std::size_t);
        std::map<std::string, data_info, std::less<>> datas_;
    };
}
