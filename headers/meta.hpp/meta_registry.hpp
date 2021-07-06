/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_fwd.hpp"

#include "meta_type.hpp"

#include "meta_class.hpp"
#include "meta_field.hpp"
#include "meta_function.hpp"
#include "meta_method.hpp"
#include "meta_namespace.hpp"
#include "meta_variable.hpp"

namespace meta_hpp
{
    class registry {
    public:
        template < typename T >
        std::optional<type> resolve() const {
            const family_id fid = get_type_family_id<T>();
            return detail::find_opt(types_, fid);
        }

        template < typename T, T V >
        std::optional<type> resolve() const {
            const family_id fid = get_value_family_id<T, V>();
            return detail::find_opt(types_, fid);
        }

        template < typename T >
        std::optional<type> resolve(T&&) const {
            const family_id fid = get_type_family_id<std::decay_t<T>>();
            return detail::find_opt(types_, fid);
        }

        std::optional<class_info> get_class_by_name(std::string_view name) const {
            return detail::find_opt(classes_, name);
        }

        std::optional<field_info> get_field_by_name(std::string_view name) const {
            return detail::find_opt(fields_, name);
        }

        std::optional<function_info> get_function_by_name(std::string_view name) const {
            return detail::find_opt(functions_, name);
        }

        std::optional<method_info> get_method_by_name(std::string_view name) const {
            return detail::find_opt(methods_, name);
        }

        std::optional<namespace_info> get_namespace_by_name(std::string_view name) const {
            return detail::find_opt(namespaces_, name);
        }

        std::optional<variable_info> get_variable_by_name(std::string_view name) const {
            return detail::find_opt(variables_, name);
        }

        template < typename F >
        void each_type(F&& f) const {
            for ( auto&& id_type : types_ ) {
                std::invoke(f, id_type.second);
            }
        }

        template < typename F >
        void each_class(F&& f) const {
            for ( auto&& id_info : classes_ ) {
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
        void each_namespace(F&& f) const {
            for ( auto&& id_info : namespaces_ ) {
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
            each_field(f);
            each_function(f);
            each_method(f);
            each_namespace(f);
            each_variable(f);
        }

        template < typename... Internals >
        registry& operator()(Internals&&...internals) {
            (add_(std::string{}, std::forward<Internals>(internals)), ...);
            return *this;
        }
    private:
        void add_(const std::string& prefix, const class_info& info) {
            const std::string name = prefix.empty()
                ? info.id()
                : prefix + "::" + info.id();

            detail::merge_with(types_, info.fid(), info, &type::merge);
            detail::merge_with(classes_, name, info, &class_info::merge);

            info.visit(overloaded {
                [](const data_info&) {},
                [this, &name](const auto& internal){
                    add_(name, internal);
                }
            });
        }

        void add_(const std::string& prefix, const field_info& info) {
            const std::string name = prefix.empty()
                ? info.id()
                : prefix + "::" + info.id();

            detail::merge_with(types_, info.fid(), info, &type::merge);
            detail::merge_with(fields_, name, info, &field_info::merge);

            info.visit(overloaded {
                [](const data_info&) {},
                [this, &name](const auto& internal){
                    add_(name, internal);
                }
            });
        }

        void add_(const std::string& prefix, const function_info& info) {
            const std::string name = prefix.empty()
                ? info.id()
                : prefix + "::" + info.id();

            detail::merge_with(types_, info.fid(), info, &type::merge);
            detail::merge_with(functions_, name, info, &function_info::merge);

            info.visit(overloaded {
                [](const data_info&) {},
                [this, &name](const auto& internal){
                    add_(name, internal);
                }
            });
        }

        void add_(const std::string& prefix, const method_info& info) {
            const std::string name = prefix.empty()
                ? info.id()
                : prefix + "::" + info.id();

            detail::merge_with(types_, info.fid(), info, &type::merge);
            detail::merge_with(methods_, name, info, &method_info::merge);

            info.visit(overloaded {
                [](const data_info&) {},
                [this, &name](const auto& internal){
                    add_(name, internal);
                }
            });
        }

        void add_(const std::string& prefix, const namespace_info& info) {
            const std::string name = prefix.empty()
                ? info.id()
                : prefix + "::" + info.id();

            detail::merge_with(namespaces_, name, info, &namespace_info::merge);

            info.visit(overloaded {
                [](const data_info&) {},
                [this, &name](const auto& internal){
                    add_(name, internal);
                }
            });
        }

        void add_(const std::string& prefix, const variable_info& info) {
            const std::string name = prefix.empty()
                ? info.id()
                : prefix + "::" + info.id();

            detail::merge_with(types_, info.fid(), info, &type::merge);
            detail::merge_with(variables_, name, info, &variable_info::merge);

            info.visit(overloaded {
                [](const data_info&) {},
                [this, &name](const auto& internal){
                    add_(name, internal);
                }
            });
        }
    private:
        std::map<family_id, type, std::less<>> types_;
        std::map<std::string, class_info, std::less<>> classes_;
        std::map<std::string, field_info, std::less<>> fields_;
        std::map<std::string, function_info, std::less<>> functions_;
        std::map<std::string, method_info, std::less<>> methods_;
        std::map<std::string, namespace_info, std::less<>> namespaces_;
        std::map<std::string, variable_info, std::less<>> variables_;
    };
}
