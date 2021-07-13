/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_fwd.hpp"

#include "meta_value.hpp"

#include "meta_data_info.hpp"

namespace meta_hpp::variable_detail
{
    template < typename VariableType >
    value raw_getter(
        VariableType variable)
    {
        using vt = detail::variable_traits<std::remove_reference_t<VariableType>>;
        using value_type = typename vt::value_type;

        value_type typed_value{*variable};
        return value{std::move(typed_value)};
    }

    template < typename VariableType >
    void raw_setter(
        [[maybe_unused]] VariableType variable,
        [[maybe_unused]] value value)
    {
        using vt = detail::variable_traits<std::remove_reference_t<VariableType>>;
        using value_type = typename vt::value_type;

        if constexpr ( !vt::is_const ) {
            value_type* typed_value{value.try_cast<value_type>()};
            if ( !typed_value ) {
                throw std::logic_error("an attempt to set a variable with incorrect argument type");
            }

            *variable = std::move(*typed_value);
        } else {
            throw std::logic_error("an attempt to set a constant variable");
        }
    }

    using variable_getter = std::function<value()>;
    using variable_setter = std::function<void(value)>;

    template < typename VariableType >
    variable_getter make_getter(VariableType variable) {
        using namespace std::placeholders;
        return std::bind(&raw_getter<VariableType>, variable);
    }

    template < typename VariableType >
    variable_setter make_setter(VariableType variable) {
        using namespace std::placeholders;
        return std::bind(&raw_setter<VariableType>, variable, _1);
    }

    template < typename VariableType >
    family_id make_value_type() noexcept {
        using vt = detail::variable_traits<VariableType>;
        using value_type = typename vt::value_type;
        return get_family_id<value_type>();
    }
}

namespace meta_hpp
{
    class variable_info {
    public:
        variable_info() = delete;

        variable_info(variable_info&&) = default;
        variable_info(const variable_info&) = default;

        variable_info& operator=(variable_info&&) = default;
        variable_info& operator=(const variable_info&) = default;
    public:
        const std::string& id() const noexcept {
            return id_;
        }

        family_id family() const noexcept {
            return family_;
        }

        family_id value_type() const noexcept {
            return value_type_;
        }

        value get() const {
            return getter_();
        }

        template < typename R >
        R get_r() const {
            return get().cast<R>();
        }

        template < typename Value >
        void set(Value&& value) const {
            return setter_(std::forward<Value>(value));
        }

        value operator()() const {
            return get();
        }

        template < typename Value >
        void operator()(Value&& value) const {
            return set(std::forward<Value>(value));
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

        void merge(const variable_info& other) {
            if ( id() != other.id() || family() != other.family() ) {
                throw std::logic_error("variable_info::merge failed");
            }
            detail::merge_with(datas_, other.datas_, &data_info::merge);
        }
    private:
        template < typename VariableType >
        friend class variable_;

        template < typename VariableType >
        variable_info(std::string id, VariableType variable_ptr)
        : id_{std::move(id)}
        , family_{get_family_id<VariableType>()}
        , value_type_{variable_detail::make_value_type<VariableType>()}
        , getter_{variable_detail::make_getter(variable_ptr)}
        , setter_{variable_detail::make_setter(variable_ptr)} {}
    private:
        std::string id_;
        family_id family_;
        family_id value_type_;
        variable_detail::variable_getter getter_;
        variable_detail::variable_setter setter_;
        std::map<std::string, data_info, std::less<>> datas_;
    };
}
