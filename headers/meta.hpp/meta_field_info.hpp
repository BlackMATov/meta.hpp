/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_fwd.hpp"
#include "meta_value.hpp"

#include "meta_data_info.hpp"

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
    struct field_traits<const T Base::*>
         : field_traits<T Base::*> {
        static constexpr bool is_const = true;
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
    void setter([[maybe_unused]] void* instance, value value) {
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

        field_info(family_id fid, std::string id)
        : fid_(std::move(fid))
        , id_(std::move(id)) {}
    public:
        const family_id& fid() const noexcept {
            return fid_;
        }

        const std::string& id() const noexcept {
            return id_;
        }
        value get(const void* instance) const {
            return getter_(instance);
        }

        template < typename Value >
        void set(void* instance, Value&& value) const {
            return setter_(instance, std::forward<Value>(value));
        }

        template < typename F >
        void each_data(F&& f) const {
            for ( auto [_, info] : datas_ ) {
                std::invoke(f, info);
            }
        }

        std::optional<data_info> get_data(std::string_view id) const {
            return detail::find_opt(datas_, id);
        }
    private:
        friend class class_info;

        template < typename Class > friend class class_;
        template < auto Field > friend class field_;
    private:
        void merge_with_(const field_info& other) {
            if ( fid() != other.fid() ) {
                throw std::logic_error("field_info::merge failed");
            }
            detail::merge_with(datas_, other.datas_, &data_info::merge_with_);
        }
    private:
        family_id fid_;
        std::string id_;
        value(*getter_)(const void*);
        void(*setter_)(void*, value);
        std::map<std::string, data_info, std::less<>> datas_;
    };
}
