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
        : info_{get_family_id<Class>(), std::move(id)} {}

        const class_info& info() const noexcept {
            return info_;
        }

        operator const class_info&() const noexcept {
            return info_;
        }

        template < typename... Internals >
        class_& operator()(Internals&&...internals) {
            (add_(std::forward<Internals>(internals)), ...);
            return *this;
        }
    private:
        void add_(const class_info& info) {
            detail::merge_with(info_.classes_, info.id(), info, &class_info::merge);
        }

        void add_(const data_info& info) {
            detail::merge_with(info_.datas_, info.id(), info, &data_info::merge);
        }

        void add_(const field_info& info) {
            detail::merge_with(info_.fields_, info.id(), info, &field_info::merge);
        }

        void add_(const function_info& info) {
            detail::merge_with(info_.functions_, info.id(), info, &function_info::merge);
        }

        void add_(const method_info& info) {
            detail::merge_with(info_.methods_, info.id(), info, &method_info::merge);
        }

        void add_(const variable_info& info) {
            detail::merge_with(info_.variables_, info.id(), info, &variable_info::merge);
        }
    private:
        class_info info_;
    };
}
