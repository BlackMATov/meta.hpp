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
    template < typename Variable >
    struct variable_traits;

    template < typename T >
    struct variable_traits<T*> {
        static constexpr bool is_const = false;
        using value_type = T;
    };

    template < typename T >
    struct variable_traits<const T*> {
        static constexpr bool is_const = true;
        using value_type = T;
    };

    template < auto Variable  >
    value getter() {
        using vt = variable_traits<decltype(Variable)>;
        using value_type = typename vt::value_type;

        value_type typed_value = *Variable;

        return value{std::move(typed_value)};
    }

    template < auto Variable  >
    void setter(value value) {
        using vt = variable_traits<decltype(Variable)>;
        using value_type = typename vt::value_type;

        if constexpr ( !vt::is_const ) {
            *Variable = value.cast<value_type>();
        } else {
            throw std::logic_error("an attempt to change a constant variable");
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

        variable_info(std::string id)
        : id_(std::move(id)) {}
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

        std::optional<data_info> get_data(std::string_view id) const {
            return detail::find_opt(datas_, id);
        }
    private:
        friend class class_info;
        friend class namespace_info;

        template < typename Class > friend class class_;
        friend class namespace_;

        template < auto Variable > friend class variable_;
    private:
        void merge_with_(const variable_info& other) {
            getter_ = other.getter_;
            setter_ = other.setter_;
            detail::merge_with(datas_, other.datas_, &data_info::merge_with_);
        }
    private:
        std::string id_;
        value(*getter_)();
        void(*setter_)(value);
        std::map<std::string, data_info, std::less<>> datas_;
    };
}
