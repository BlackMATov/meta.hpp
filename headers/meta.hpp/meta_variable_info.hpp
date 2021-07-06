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
    template < typename VariableType, VariableType Variable >
    value getter() {
        using vt = detail::variable_traits<std::remove_reference_t<VariableType>>;
        using value_type = typename vt::value_type;

        value_type typed_value{*Variable};
        return value{std::move(typed_value)};
    }

    template < typename VariableType, VariableType Variable >
    void setter(value value) {
        using vt = detail::variable_traits<std::remove_reference_t<VariableType>>;
        using value_type = typename vt::value_type;

        if constexpr ( !vt::is_const ) {
            value_type* typed_value{value.try_cast<value_type>()};
            if ( !typed_value ) {
                throw std::logic_error("an attempt to set a variable with incorrect argument type");
            }

            *Variable = std::move(*typed_value);
        } else {
            throw std::logic_error("an attempt to set a constant variable");
        }
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

        value get() const {
            return getter_();
        }

        template < typename Value >
        void set(Value&& value) const {
            return setter_(std::forward<Value>(value));
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
            if ( id() != other.id() ) {
                throw std::logic_error("variable_info::merge failed");
            }
            detail::merge_with(datas_, other.datas_, &data_info::merge);
        }
    private:
        template < auto Variable >
        friend class variable_;

        template < typename VariableType, VariableType Variable >
        variable_info(detail::auto_arg_t<VariableType, Variable>, std::string id)
        : id_{std::move(id)}
        , getter_{&variable_detail::getter<VariableType, Variable>}
        , setter_{&variable_detail::setter<VariableType, Variable>} {}
    private:
        std::string id_;
        value(*getter_)();
        void(*setter_)(value);
        std::map<std::string, data_info, std::less<>> datas_;
    };
}
