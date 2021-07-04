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
            return detail::find_opt(family_to_types_, get_family_id<T>());
        }

        template < auto T >
        std::optional<type> resolve() const {
            return resolve<decltype(T)>();
        }

        template < typename T >
        std::optional<type> resolve(T&&) const {
            return resolve<std::decay_t<T>>();
        }

        std::optional<class_info> get_class_by_name(std::string_view name) const {
            return detail::find_opt(name_to_classes_, name);
        }

        std::optional<field_info> get_field_by_name(std::string_view name) const {
            return detail::find_opt(name_to_fields_, name);
        }

        std::optional<function_info> get_function_by_name(std::string_view name) const {
            return detail::find_opt(name_to_functions_, name);
        }

        std::optional<method_info> get_method_by_name(std::string_view name) const {
            return detail::find_opt(name_to_methods_, name);
        }

        std::optional<namespace_info> get_namespace_by_name(std::string_view name) const {
            return detail::find_opt(name_to_namespaces_, name);
        }

        std::optional<variable_info> get_variable_by_name(std::string_view name) const {
            return detail::find_opt(name_to_variables_, name);
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

            detail::merge_with(name_to_classes_, name, info, &class_info::merge);
            detail::merge_with(family_to_types_, info.fid(), info, &type::merge);

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

            detail::merge_with(name_to_fields_, name, info, &field_info::merge);
            detail::merge_with(family_to_types_, info.fid(), info, &type::merge);

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

            detail::merge_with(name_to_functions_, name, info, &function_info::merge);
            detail::merge_with(family_to_types_, info.fid(), info, &type::merge);

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

            detail::merge_with(name_to_methods_, name, info, &method_info::merge);
            detail::merge_with(family_to_types_, info.fid(), info, &type::merge);

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

            detail::merge_with(name_to_namespaces_, name, info, &namespace_info::merge);

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

            detail::merge_with(name_to_variables_, name, info, &variable_info::merge);
            detail::merge_with(family_to_types_, info.fid(), info, &type::merge);

            info.visit(overloaded {
                [](const data_info&) {},
                [this, &name](const auto& internal){
                    add_(name, internal);
                }
            });
        }
    private:
        std::map<family_id, type, std::less<>> family_to_types_;
        std::map<std::string, class_info, std::less<>> name_to_classes_;
        std::map<std::string, field_info, std::less<>> name_to_fields_;
        std::map<std::string, function_info, std::less<>> name_to_functions_;
        std::map<std::string, method_info, std::less<>> name_to_methods_;
        std::map<std::string, namespace_info, std::less<>> name_to_namespaces_;
        std::map<std::string, variable_info, std::less<>> name_to_variables_;
    };
}
