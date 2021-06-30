/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_fwd.hpp"

#include "meta_class_info.hpp"
#include "meta_field_info.hpp"
#include "meta_function_info.hpp"
#include "meta_method_info.hpp"
#include "meta_namespace_info.hpp"
#include "meta_variable_info.hpp"

namespace meta_hpp
{
    class type {
    public:
        type() = delete;

        type(type&&) = default;
        type(const type&) = default;

        type& operator=(type&&) = default;
        type& operator=(const type&) = default;

        template < typename Info >
        type(Info&& info)
        : info_(std::forward<Info>(info)) {}

        bool is_class() const noexcept { return std::holds_alternative<class_info>(info_); }
        bool is_field() const noexcept { return std::holds_alternative<field_info>(info_); }
        bool is_function() const noexcept { return std::holds_alternative<function_info>(info_); }
        bool is_method() const noexcept { return std::holds_alternative<method_info>(info_); }
        bool is_namespace() const noexcept { return std::holds_alternative<namespace_info>(info_); }
        bool is_variable() const noexcept { return std::holds_alternative<variable_info>(info_); }

        const class_info& get_class_info() const { return std::get<class_info>(info_); }
        const field_info& get_field_info() const { return std::get<field_info>(info_); }
        const function_info& get_function_info() const { return std::get<function_info>(info_); }
        const method_info& get_method_info() const { return std::get<method_info>(info_); }
        const namespace_info& get_namespace_info() const { return std::get<namespace_info>(info_); }
        const variable_info& get_variable_info() const { return std::get<variable_info>(info_); }
    private:
        std::variant<
            class_info,
            field_info,
            function_info,
            method_info,
            namespace_info,
            variable_info> info_;
    };
}
