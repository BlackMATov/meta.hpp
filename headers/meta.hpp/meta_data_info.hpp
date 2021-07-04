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
    public:
        const std::string& id() const noexcept {
            return id_;
        }

        const value& get() const noexcept {
            return value_;
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

        void merge(const data_info& other) {
            value_ = other.value_;
            detail::merge_with(datas_, other.datas_, &data_info::merge);
        }
    private:
        friend class data_;

        data_info(std::string id, value value)
        : id_{std::move(id)}
        , value_{std::move(value)} {}
    private:
        std::string id_;
        value value_;
        std::map<std::string, data_info, std::less<>> datas_;
    };
}
