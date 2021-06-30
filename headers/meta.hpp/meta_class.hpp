/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_fwd.hpp"

#include "meta_data.hpp"
#include "meta_field.hpp"
#include "meta_function.hpp"
#include "meta_method.hpp"
#include "meta_variable.hpp"

#include "meta_class_info.hpp"

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

        void add_(const data_& internal) {
            detail::merge_with(
                info_.datas_,
                internal.info().id(),
                internal.info(),
                &data_info::merge_with_);
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
