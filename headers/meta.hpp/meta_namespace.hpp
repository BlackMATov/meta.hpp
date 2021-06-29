/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_fwd.hpp"

#include "meta_namespace_info.hpp"

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
