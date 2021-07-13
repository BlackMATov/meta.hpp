/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_fwd.hpp"
#include "meta_value.hpp"

#include "meta_ctor_info.hpp"
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
    public:
        const std::string& id() const noexcept {
            return id_;
        }

        family_id family() const noexcept {
            return family_;
        }

        template < typename F >
        void each_class(F&& f) const {
            for ( auto&& id_info : classes_ ) {
                std::invoke(f, id_info.second);
            }
        }

        template < typename F >
        void each_ctor(F&& f) const {
            for ( auto&& family_info : ctors_ ) {
                std::invoke(f, family_info.second);
            }
        }

        template < typename F >
        void each_data(F&& f) const {
            for ( auto&& id_info : datas_ ) {
                std::invoke(f, id_info.second);
            }
        }

        template < typename F >
        void each_field(F&& f) const {
            for ( auto&& id_info : fields_ ) {
                std::invoke(f, id_info.second);
            }
        }

        template < typename F >
        void each_function(F&& f) const {
            for ( auto&& id_info : functions_ ) {
                std::invoke(f, id_info.second);
            }
        }

        template < typename F >
        void each_method(F&& f) const {
            for ( auto&& id_info : methods_ ) {
                std::invoke(f, id_info.second);
            }
        }

        template < typename F >
        void each_variable(F&& f) const {
            for ( auto&& id_info : variables_ ) {
                std::invoke(f, id_info.second);
            }
        }

        template < typename F >
        void visit(F&& f) const {
            each_class(f);
            each_ctor(f);
            each_data(f);
            each_field(f);
            each_function(f);
            each_method(f);
            each_variable(f);
        }

        std::optional<class_info> get_class(std::string_view id) const {
            return detail::find_opt(classes_, id);
        }

        template < typename... Args >
        std::optional<ctor_info> get_ctor() const {
            for ( auto&& family_info : ctors_ ) {
                if ( family_info.second.is_invocable_with<Args...>() ) {
                    return family_info.second;
                }
            }
            return std::nullopt;
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

        void merge(const class_info& other) {
            if ( id() != other.id() || family() != other.family() ) {
                throw std::logic_error("class_info::merge failed");
            }
            detail::merge_with(classes_, other.classes_, &class_info::merge);
            detail::merge_with(ctors_, other.ctors_, &ctor_info::merge);
            detail::merge_with(datas_, other.datas_, &data_info::merge);
            detail::merge_with(fields_, other.fields_, &field_info::merge);
            detail::merge_with(functions_, other.functions_, &function_info::merge);
            detail::merge_with(methods_, other.methods_, &method_info::merge);
            detail::merge_with(variables_, other.variables_, &variable_info::merge);
        }
    private:
        template < typename Class >
        friend class class_;

        template < typename Class >
        class_info(detail::typename_arg_t<Class>, std::string id)
        : id_{std::move(id)}
        , family_{get_family_id<Class>()} {}
    private:
        std::string id_;
        family_id family_;
        std::map<std::string, class_info, std::less<>> classes_;
        std::map<family_id, ctor_info, std::less<>> ctors_;
        std::map<std::string, data_info, std::less<>> datas_;
        std::map<std::string, field_info, std::less<>> fields_;
        std::map<std::string, function_info, std::less<>> functions_;
        std::map<std::string, method_info, std::less<>> methods_;
        std::map<std::string, variable_info, std::less<>> variables_;
    };
}
