/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_fwd.hpp"
#include "meta_value.hpp"

namespace meta_hpp
{
    class data_info {
    public:
        data_info() = delete;

        data_info(data_info&&) = default;
        data_info(const data_info&) = default;

        data_info& operator=(data_info&&) = default;
        data_info& operator=(const data_info&) = default;

        data_info(std::string id, value value)
        : id_(std::move(id))
        , value_(std::move(value)) {}
    public:
        const std::string& id() const noexcept {
            return id_;
        }

        const value& get() const noexcept {
            return value_;
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
        friend class field_info;
        friend class function_info;
        friend class method_info;
        friend class namespace_info;
        friend class variable_info;

        template < typename Class > friend class class_;
        template < auto Field > friend class field_;
        template < auto Function > friend class function_;
        template < auto Method > friend class method_;
        friend class namespace_;
        template < auto Variable > friend class variable_;

        friend class data_;
    private:
        void merge_with_(const data_info& other) {
            value_ = other.value_;
            detail::merge_with(datas_, other.datas_, &data_info::merge_with_);
        }
    private:
        std::string id_;
        value value_;
        std::map<std::string, data_info, std::less<>> datas_;
    };
}
