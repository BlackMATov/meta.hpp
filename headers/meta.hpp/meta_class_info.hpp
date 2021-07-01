/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_fwd.hpp"
#include "meta_value.hpp"

#include "meta_data_info.hpp"
#include "meta_field_info.hpp"
#include "meta_function_info.hpp"
#include "meta_method_info.hpp"
#include "meta_variable_info.hpp"

namespace meta_hpp
{
    class class_info {
    public:
        class_info() = delete;

        class_info(class_info&&) = default;
        class_info(const class_info&) = default;

        class_info& operator=(class_info&&) = default;
        class_info& operator=(const class_info&) = default;

        class_info(family_id fid, std::string id)
        : fid_(std::move(fid))
        , id_(std::move(id)) {}
    public:
        const family_id& fid() const noexcept {
            return fid_;
        }

        const std::string& id() const noexcept {
            return id_;
        }

        template < typename F >
        void each_class(F&& f) const {
            for ( auto [_, info] : classes_ ) {
                std::invoke(f, info);
            }
        }

        template < typename F >
        void each_data(F&& f) const {
            for ( auto [_, info] : datas_ ) {
                std::invoke(f, info);
            }
        }

        template < typename F >
        void each_field(F&& f) const {
            for ( auto [_, info] : fields_ ) {
                std::invoke(f, info);
            }
        }

        template < typename F >
        void each_function(F&& f) const {
            for ( auto [_, info] : functions_ ) {
                std::invoke(f, info);
            }
        }

        template < typename F >
        void each_method(F&& f) const {
            for ( auto [_, info] : methods_ ) {
                std::invoke(f, info);
            }
        }

        template < typename F >
        void each_variable(F&& f) const {
            for ( auto [_, info] : variables_ ) {
                std::invoke(f, info);
            }
        }

        std::optional<class_info> get_class(std::string_view id) const {
            return detail::find_opt(classes_, id);
        }

        std::optional<data_info> get_data(std::string_view id) const {
            return detail::find_opt(datas_, id);
        }

        std::optional<field_info> get_field(std::string_view id) const {
            return detail::find_opt(fields_, id);
        }

        std::optional<function_info> get_function(std::string_view id) const {
            return detail::find_opt(functions_, id);
        }

        std::optional<method_info> get_method(std::string_view id) const {
            return detail::find_opt(methods_, id);
        }

        std::optional<variable_info> get_variable(std::string_view id) const {
            return detail::find_opt(variables_, id);
        }
    private:
        friend class namespace_info;

        template < typename Class > friend class class_;
        friend class namespace_;
    private:
        void merge_with_(const class_info& other) {
            if ( fid() != other.fid() ) {
                throw std::logic_error("class_info::merge failed");
            }
            detail::merge_with(classes_, other.classes_, &class_info::merge_with_);
            detail::merge_with(datas_, other.datas_, &data_info::merge_with_);
            detail::merge_with(fields_, other.fields_, &field_info::merge_with_);
            detail::merge_with(functions_, other.functions_, &function_info::merge_with_);
            detail::merge_with(methods_, other.methods_, &method_info::merge_with_);
            detail::merge_with(variables_, other.variables_, &variable_info::merge_with_);
        }
    private:
        family_id fid_;
        std::string id_;
        std::map<std::string, class_info, std::less<>> classes_;
        std::map<std::string, data_info, std::less<>> datas_;
        std::map<std::string, field_info, std::less<>> fields_;
        std::map<std::string, function_info, std::less<>> functions_;
        std::map<std::string, method_info, std::less<>> methods_;
        std::map<std::string, variable_info, std::less<>> variables_;
    };
}
