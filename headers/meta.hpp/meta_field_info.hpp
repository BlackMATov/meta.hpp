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
    template < auto Field  >
    value getter(cinstance instance) {
        using ft = detail::field_traits<decltype(Field)>;
        using value_type = typename ft::value_type;
        using instance_type = typename ft::instance_type;

        const instance_type* typed_instance{instance.try_cast<instance_type>()};
        if ( !typed_instance ) {
            throw std::logic_error("an attempt to get a field with incorrect instance type");
        }

        value_type typed_value{std::invoke(Field, *typed_instance)};
        return value{std::move(typed_value)};
    }

    template < auto Field  >
    void setter([[maybe_unused]] instance instance, value value) {
        using ft = detail::field_traits<decltype(Field)>;
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

            std::invoke(Field, *typed_instance) = std::move(*typed_value);
        } else {
            throw std::logic_error("an attempt to change a constant field");
        }
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
        const family_id& fid() const noexcept {
            return fid_;
        }

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
            if ( fid() != other.fid() ) {
                throw std::logic_error("field_info::merge failed");
            }
            detail::merge_with(datas_, other.datas_, &data_info::merge);
        }
    private:
        template < auto Field >
        friend class field_;

        template < auto Field >
        field_info(detail::auto_arg_t<Field>, std::string id)
        : fid_{get_value_family_id<Field>()}
        , id_{std::move(id)}
        , getter_{&field_detail::getter<Field>}
        , setter_{&field_detail::setter<Field>} {}
    private:
        family_id fid_;
        std::string id_;
        value(*getter_)(cinstance);
        void(*setter_)(instance, value);
        std::map<std::string, data_info, std::less<>> datas_;
    };
}
