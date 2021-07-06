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

namespace meta_hpp::field_detail
{
    template < typename FieldType  >
    value raw_getter(
        FieldType field,
        cinstance instance)
    {
        using ft = detail::field_traits<FieldType>;
        using value_type = typename ft::value_type;
        using instance_type = typename ft::instance_type;

        const instance_type* typed_instance{instance.try_cast<instance_type>()};
        if ( !typed_instance ) {
            throw std::logic_error("an attempt to get a field with incorrect instance type");
        }

        value_type typed_value{std::invoke(field, *typed_instance)};
        return value{std::move(typed_value)};
    }

    template < typename FieldType  >
    void raw_setter(
        [[maybe_unused]] FieldType field,
        [[maybe_unused]] instance instance,
        [[maybe_unused]] value value)
    {
        using ft = detail::field_traits<FieldType>;
        using value_type = typename ft::value_type;
        using instance_type = typename ft::instance_type;

        if constexpr ( !ft::is_const ) {
            instance_type* typed_instance{instance.try_cast<instance_type>()};
            if ( !typed_instance ) {
                throw std::logic_error("an attempt to set a field with incorrect instance type");
            }

            value_type* typed_value{value.try_cast<value_type>()};
            if ( !typed_value ) {
                throw std::logic_error("an attempt to set a field with incorrect argument type");
            }

            std::invoke(field, *typed_instance) = std::move(*typed_value);
        } else {
            throw std::logic_error("an attempt to change a constant field");
        }
    }

    using field_getter = std::function<value(cinstance)>;
    using field_setter = std::function<void(instance, value)>;

    template < typename FieldType >
    field_getter make_getter(FieldType field) {
        using namespace std::placeholders;
        return std::bind(&raw_getter<FieldType>, field, _1);
    }

    template < typename FieldType >
    field_setter make_setter(FieldType field) {
        using namespace std::placeholders;
        return std::bind(&raw_setter<FieldType>, field, _1, _2);
    }
}

namespace meta_hpp
{
    class field_info {
    public:
        field_info() = delete;

        field_info(field_info&&) = default;
        field_info(const field_info&) = default;

        field_info& operator=(field_info&&) = default;
        field_info& operator=(const field_info&) = default;
    public:
        const std::string& id() const noexcept {
            return id_;
        }

        value get(cinstance instance) const {
            return getter_(instance);
        }

        template < typename Value >
        void set(instance instance, Value&& value) const {
            return setter_(instance, std::forward<Value>(value));
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

        void merge(const field_info& other) {
            if ( id() != other.id() ) {
                throw std::logic_error("field_info::merge failed");
            }
            detail::merge_with(datas_, other.datas_, &data_info::merge);
        }
    private:
        template < typename FieldType >
        friend class field_;

        template < typename FieldType  >
        field_info(std::string id, FieldType field)
        : id_{std::move(id)}
        , getter_{field_detail::make_getter(field)}
        , setter_{field_detail::make_setter(field)} {}
    private:
        std::string id_;
        field_detail::field_getter getter_;
        field_detail::field_setter setter_;
        std::map<std::string, data_info, std::less<>> datas_;
    };
}
