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

    template < typename FunctionType >
    std::optional<family_id> make_return_type() {
        using ft = detail::function_traits<FunctionType>;
        using return_type = typename ft::return_type;

        if constexpr ( !std::is_void_v<return_type> ) {
            return get_family_id<return_type>();
        } else {
            return std::nullopt;
        }
    }

    template < typename FunctionType, std::size_t... Is >
    std::vector<family_id> make_argument_types_impl(std::index_sequence<Is...>) {
        using ft = detail::function_traits<FunctionType>;
        using argument_types = typename ft::argument_types;
        return { get_family_id<std::tuple_element_t<Is, argument_types>>()... };
    }

    template < typename FunctionType >
    std::vector<family_id> make_argument_types() {
        using ft = detail::function_traits<FunctionType>;
        return make_argument_types_impl<FunctionType>(std::make_index_sequence<ft::arity>());
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

        family_id family() const noexcept {
            return family_;
        }

        std::size_t arity() const noexcept {
            return argument_types_.size();
        }

        std::optional<family_id> return_type() const noexcept {
            return return_type_;
        }

        std::optional<family_id> argument_type(std::size_t index) const noexcept {
            if ( index < argument_types_.size() ) {
                return argument_types_[index];
            }
            return std::nullopt;
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

        template < typename R, typename Rp = std::decay_t<R>, typename... Args >
        std::optional<Rp> invoke_r(Args&&... args) const {
            if ( std::optional<value> r = invoke(std::forward<Args>(args)...) ) {
                return std::move(r)->template cast<Rp>();
            }
            return std::nullopt;
        }

        template < typename... Args >
        std::optional<value> operator()(Args&&... args) const {
            return invoke(std::forward<Args>(args)...);
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
            if ( id() != other.id() || family() != other.family() ) {
                throw std::logic_error("function_info::merge failed");
            }
            detail::merge_with(datas_, other.datas_, &data_info::merge);
        }
    private:
        template < typename FunctionType >
        friend class function_;

        template < typename FunctionType >
        function_info(std::string id, FunctionType function_ptr)
        : id_{std::move(id)}
        , family_{get_family_id<FunctionType>()}
        , return_type_{function_detail::make_return_type<FunctionType>()}
        , argument_types_{function_detail::make_argument_types<FunctionType>()}
        , invoke_{function_detail::make_invoke(function_ptr)} {}
    private:
        std::string id_;
        family_id family_;
        std::optional<family_id> return_type_;
        std::vector<family_id> argument_types_;
        function_detail::function_invoke invoke_;
        std::map<std::string, data_info, std::less<>> datas_;
    };
}
