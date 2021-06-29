/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_fwd.hpp"

#include "meta_field.hpp"
#include "meta_function.hpp"
#include "meta_method.hpp"
#include "meta_variable.hpp"

namespace meta_hpp
{
    class class_info {
    public:
        class_info() = delete;

        class_info(class_info&&) = default;
        class_info(const class_info&) = default;

        class_info& operator=(class_info&&) = default;
        class_info& operator=(const class_info&) = default;

        class_info(std::string id)
        : id_(std::move(id)) {}

        const std::string& id() const noexcept {
            return id_;
        }

        std::optional<class_info> get_class(std::string_view id) const {
            return detail::find_opt(classes_, id);
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
            detail::merge_with(classes_, other.classes_, &class_info::merge_with_);
            detail::merge_with(fields_, other.fields_, &field_info::merge_with_);
            detail::merge_with(functions_, other.functions_, &function_info::merge_with_);
            detail::merge_with(methods_, other.methods_, &method_info::merge_with_);
            detail::merge_with(variables_, other.variables_, &variable_info::merge_with_);
        }
    private:
        std::string id_;
        std::map<std::string, class_info, std::less<>> classes_;
        std::map<std::string, field_info, std::less<>> fields_;
        std::map<std::string, function_info, std::less<>> functions_;
        std::map<std::string, method_info, std::less<>> methods_;
        std::map<std::string, variable_info, std::less<>> variables_;
    };
}

namespace meta_hpp
{
    template < typename Class >
    class class_ {
    public:
        explicit class_(std::string id)
        : info_(std::move(id)) {}

        const class_info& info() const noexcept {
            return info_;
        }

        template < typename... Internals >
        class_& operator()(Internals&&...internals) {
            (add_(std::forward<Internals>(internals)), ...);
            return *this;
        }
    private:
        template < typename InternalClass >
        void add_(const class_<InternalClass>& internal) {
            detail::merge_with(
                info_.classes_,
                internal.info().id(),
                internal.info(),
                &class_info::merge_with_);
        }

        template < auto InternalField >
        void add_(const field_<InternalField>& internal) {
            detail::merge_with(
                info_.fields_,
                internal.info().id(),
                internal.info(),
                &field_info::merge_with_);
        }

        template < auto InternalFunction >
        void add_(const function_<InternalFunction>& internal) {
            detail::merge_with(
                info_.functions_,
                internal.info().id(),
                internal.info(),
                &function_info::merge_with_);
        }

        template < auto InternalMethod >
        void add_(const method_<InternalMethod>& internal) {
            detail::merge_with(
                info_.methods_,
                internal.info().id(),
                internal.info(),
                &method_info::merge_with_);
        }

        template < auto InternalVariable >
        void add_(const variable_<InternalVariable>& internal) {
            detail::merge_with(
                info_.variables_,
                internal.info().id(),
                internal.info(),
                &variable_info::merge_with_);
        }
    private:
        class_info info_;
    };
}
