/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_fwd.hpp"
#include "meta_value.hpp"

#include "meta_class_info.hpp"
#include "meta_data_info.hpp"
#include "meta_function_info.hpp"
#include "meta_variable_info.hpp"

namespace meta_hpp
{
    class namespace_info {
    public:
        namespace_info() = delete;

        namespace_info(namespace_info&&) = default;
        namespace_info(const namespace_info&) = default;

        namespace_info& operator=(namespace_info&&) = default;
        namespace_info& operator=(const namespace_info&) = default;

        namespace_info(std::string id)
        : id_(std::move(id)) {}
    public:
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
        void each_function(F&& f) const {
            for ( auto [_, info] : functions_ ) {
                std::invoke(f, info);
            }
        }

        template < typename F >
        void each_namespace(F&& f) const {
            for ( auto [_, info] : namespaces_ ) {
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

        std::optional<function_info> get_function(std::string_view id) const {
            return detail::find_opt(functions_, id);
        }

        std::optional<namespace_info> get_namespace(std::string_view id) const {
            return detail::find_opt(namespaces_, id);
        }

        std::optional<variable_info> get_variable(std::string_view id) const {
            return detail::find_opt(variables_, id);
        }
    private:
        friend class namespace_;
    private:
        void merge_with_(const namespace_info& other) {
            detail::merge_with(classes_, other.classes_, &class_info::merge_with_);
            detail::merge_with(datas_, other.datas_, &data_info::merge_with_);
            detail::merge_with(functions_, other.functions_, &function_info::merge_with_);
            detail::merge_with(namespaces_, other.namespaces_, &namespace_info::merge_with_);
            detail::merge_with(variables_, other.variables_, &variable_info::merge_with_);
        }
    private:
        std::string id_;
        std::map<std::string, class_info, std::less<>> classes_;
        std::map<std::string, data_info, std::less<>> datas_;
        std::map<std::string, function_info, std::less<>> functions_;
        std::map<std::string, namespace_info, std::less<>> namespaces_;
        std::map<std::string, variable_info, std::less<>> variables_;
    };
}
