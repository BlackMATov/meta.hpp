/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_fwd.hpp"

#include "meta_value.hpp"

namespace meta_hpp::field_detail
{
    template < typename Field >
    struct field_traits;

    template < typename T, typename Base >
    struct field_traits<T Base::*> {
        static constexpr bool is_const = false;
        using value_type = T;
        using instance_type = Base;
    };

    template < typename T, typename Base >
    struct field_traits<const T Base::*> {
        static constexpr bool is_const = true;
        using value_type = T;
        using instance_type = Base;
    };

    template < auto Field  >
    value getter(const void* instance) {
        using ft = field_traits<decltype(Field)>;
        using value_type = typename ft::value_type;
        using instance_type = typename ft::instance_type;

        auto instance_ptr = static_cast<const instance_type*>(instance);
        value_type typed_value = std::invoke(Field, *instance_ptr);

        return value{std::move(typed_value)};
    }

    template < auto Field  >
    void setter(void* instance, value value) {
        using ft = field_traits<decltype(Field)>;
        using value_type = typename ft::value_type;
        using instance_type = typename ft::instance_type;

        if constexpr ( !ft::is_const ) {
            auto instance_ptr = static_cast<instance_type*>(instance);
            std::invoke(Field, *instance_ptr) = value.cast<value_type>();
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

        field_info(std::string id)
        : id_(std::move(id)) {}

        const std::string& id() const noexcept {
            return id_;
        }
    public:
        value get(const void* instance) const {
            return getter_(instance);
        }

        template < typename Value >
        void set(void* instance, Value&& value) const {
            return setter_(instance, std::forward<Value>(value));
        }
    private:
        friend class class_info;
        template < typename Class > friend class class_;
        template < auto Field > friend class field_;
    private:
        void merge_with_(const field_info& other) {
            (void)other;
        }
    private:
        std::string id_;
        value(*getter_)(const void*);
        void(*setter_)(void*, value);
    };
}

namespace meta_hpp
{
    template < auto Field >
    class field_ {
    public:
        static_assert(std::is_member_object_pointer_v<decltype(Field)>);

        explicit field_(std::string id)
        : info_(std::move(id)) {
            info_.getter_ = &field_detail::getter<Field>;
            info_.setter_ = &field_detail::setter<Field>;
        }

        const field_info& info() const noexcept {
            return info_;
        }
    private:
        field_info info_;
    };
}
