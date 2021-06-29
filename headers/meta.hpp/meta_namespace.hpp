/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_fwd.hpp"

#include "meta_class.hpp"
#include "meta_function.hpp"
#include "meta_variable.hpp"

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

        const std::string& id() const noexcept {
            return id_;
        }

        std::optional<class_info> get_class(std::string_view id) const {
            return detail::find_opt(classes_, id);
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
            detail::merge_with(functions_, other.functions_, &function_info::merge_with_);
            detail::merge_with(namespaces_, other.namespaces_, &namespace_info::merge_with_);
            detail::merge_with(variables_, other.variables_, &variable_info::merge_with_);
        }
    private:
        std::string id_;
        std::map<std::string, class_info, std::less<>> classes_;
        std::map<std::string, function_info, std::less<>> functions_;
        std::map<std::string, namespace_info, std::less<>> namespaces_;
        std::map<std::string, variable_info, std::less<>> variables_;
    };
}

namespace meta_hpp
{
    class namespace_ {
    public:
        explicit namespace_(std::string id)
        : info_(std::move(id)) {}

        const namespace_info& info() const noexcept {
            return info_;
        }

        template < typename... Internals >
        namespace_& operator()(Internals&&...internals) {
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

        template < auto InternalFunction >
        void add_(const function_<InternalFunction>& internal) {
            detail::merge_with(
                info_.functions_,
                internal.info().id(),
                internal.info(),
                &function_info::merge_with_);
        }

        void add_(const namespace_& internal) {
            detail::merge_with(
                info_.namespaces_,
                internal.info().id(),
                internal.info(),
                &namespace_info::merge_with_);
        }

        template < auto Internalvariable >
        void add_(const variable_<Internalvariable>& internal) {
            detail::merge_with(
                info_.variables_,
                internal.info().id(),
                internal.info(),
                &variable_info::merge_with_);
        }
    private:
        namespace_info info_;
    };
}
