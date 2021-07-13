/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_fwd.hpp"

#include "meta_value.hpp"

#include "meta_data_info.hpp"

namespace meta_hpp::ctor_detail
{
    template < typename Class, typename... Args, std::size_t... Is >
    value raw_invoke_impl(
        value* args,
        std::index_sequence<Is...>)
    {
        using ct = detail::ctor_traits<Class, Args...>;
        using return_type = typename ct::return_type;
        using argument_types = typename ct::argument_types;

        [[maybe_unused]] auto typed_arguments = std::make_tuple(
            (args + Is)->try_cast<std::tuple_element_t<Is, argument_types>>()...);

        if ( !(std::get<Is>(typed_arguments) && ...) ) {
            throw std::logic_error("an attempt to call a ctor with incorrect argument types");
        }

        return_type return_value{std::move(*std::get<Is>(typed_arguments))...};
        return value{std::move(return_value)};
    }

    template < typename Class, typename... Args >
    value raw_invoke(
        value* args,
        std::size_t arg_count)
    {
        using ct = detail::ctor_traits<Class, Args...>;

        if ( arg_count != ct::arity ) {
            throw std::logic_error("an attempt to call a ctor with an incorrect arity");
        }

        return raw_invoke_impl<Class, Args...>(args, std::make_index_sequence<ct::arity>());
    }

    using ctor_invoke = std::function<value(value*, std::size_t)>;

    template < typename Class, typename... Args >
    ctor_invoke make_invoke() {
        using namespace std::placeholders;
        return std::bind(&raw_invoke<Class, Args...>, _1, _2);
    }

    template < typename Class, typename... Args >
    family_id make_return_type() {
        using ct = detail::ctor_traits<Class, Args...>;
        using return_type = typename ct::return_type;
        return get_family_id<return_type>();
    }

    template < typename Class, typename... Args, std::size_t... Is >
    std::vector<family_id> make_argument_types_impl(std::index_sequence<Is...>) {
        using ct = detail::ctor_traits<Class, Args...>;
        using argument_types = typename ct::argument_types;
        return { get_family_id<std::tuple_element_t<Is, argument_types>>()... };
    }

    template < typename Class, typename... Args >
    std::vector<family_id> make_argument_types() {
        using ct = detail::ctor_traits<Class, Args...>;
        return make_argument_types_impl<Class, Args...>(std::make_index_sequence<ct::arity>());
    }

    template < typename... Args, std::size_t... Is >
    bool parameters_equal(const std::vector<family_id>& parameters, std::index_sequence<Is...>) {
        if ( parameters.size() != sizeof...(Args) ) {
            return false;
        }
        return ((get_family_id<std::tuple_element_t<Is, std::tuple<Args...>>>() == parameters[Is]) && ... );
    }

    template < typename... Args >
    bool parameters_equal(const std::vector<family_id>& parameters) {
        return parameters_equal<Args...>(parameters, std::make_index_sequence<sizeof...(Args)>());
    }
}

namespace meta_hpp
{
    class ctor_info {
    public:
        ctor_info() = delete;

        ctor_info(ctor_info&&) = default;
        ctor_info(const ctor_info&) = default;

        ctor_info& operator=(ctor_info&&) = default;
        ctor_info& operator=(const ctor_info&) = default;
    public:
        family_id family() const noexcept {
            return family_;
        }

        std::size_t arity() const noexcept {
            return argument_types_.size();
        }

        family_id return_type() const noexcept {
            return return_type_;
        }

        std::optional<family_id> argument_type(std::size_t index) const noexcept {
            if ( index < argument_types_.size() ) {
                return argument_types_[index];
            }
            return std::nullopt;
        }

        template < typename... Args >
        value invoke(Args&&... args) const {
            if constexpr ( sizeof...(Args) > 0u ) {
                std::array<value, sizeof...(Args)> vargs{std::forward<Args>(args)...};
                return invoke_(vargs.data(), vargs.size());
            } else {
                return invoke_(nullptr, 0u);
            }
        }

        template < typename R, typename Rp = std::decay_t<R>, typename... Args >
        Rp invoke_r(Args&&... args) const {
            return invoke(std::forward<Args>(args)...).template cast<Rp>();
        }

        template < typename... Args >
        value operator()(Args&&... args) const {
            return invoke(std::forward<Args>(args)...);
        }

        template < typename... Args >
        bool is_invocable() const noexcept {
            return ctor_detail::parameters_equal<Args...>(argument_types_);
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

        void merge(const ctor_info& other) {
            if ( family() != other.family() ) {
                throw std::logic_error("ctor_info::merge failed");
            }
            detail::merge_with(datas_, other.datas_, &data_info::merge);
        }
    private:
        template < typename... Args >
        friend class ctor_;

        template < typename Class, typename... Args >
        ctor_info(detail::typename_arg_t<Class>, detail::typename_args_t<Args...>)
        : family_{get_family_id<detail::ctor_traits<Class, Args...>>()}
        , return_type_{ctor_detail::make_return_type<Class, Args...>()}
        , argument_types_{ctor_detail::make_argument_types<Class, Args...>()}
        , invoke_{ctor_detail::make_invoke<Class, Args...>()} {}
    private:
        family_id family_;
        family_id return_type_;
        std::vector<family_id> argument_types_;
        ctor_detail::ctor_invoke invoke_;
        std::map<std::string, data_info, std::less<>> datas_;
    };
}
