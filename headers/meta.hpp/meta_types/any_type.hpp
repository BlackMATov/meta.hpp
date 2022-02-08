/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_types.hpp"

namespace meta_hpp
{
    inline bool any_type::is_valid() const noexcept {
        return !!data_;
    }

    inline any_type::operator bool() const noexcept {
        return is_valid();
    }

    inline type_id any_type::get_id() const noexcept {
        return data_->id;
    }

    inline type_kind any_type::get_kind() const noexcept {
        return data_->kind;
    }

    inline any_type::any_type(const array_type& other) noexcept
    : data_{detail::type_access(other)} {}

    inline any_type::any_type(const class_type& other) noexcept
    : data_{detail::type_access(other)} {}

    inline any_type::any_type(const constructor_type& other) noexcept
    : data_{detail::type_access(other)} {}

    inline any_type::any_type(const destructor_type& other) noexcept
    : data_{detail::type_access(other)} {}

    inline any_type::any_type(const enum_type& other) noexcept
    : data_{detail::type_access(other)} {}

    inline any_type::any_type(const function_type& other) noexcept
    : data_{detail::type_access(other)} {}

    inline any_type::any_type(const member_type& other) noexcept
    : data_{detail::type_access(other)} {}

    inline any_type::any_type(const method_type& other) noexcept
    : data_{detail::type_access(other)} {}

    inline any_type::any_type(const nullptr_type& other) noexcept
    : data_{detail::type_access(other)} {}

    inline any_type::any_type(const number_type& other) noexcept
    : data_{detail::type_access(other)} {}

    inline any_type::any_type(const pointer_type& other) noexcept
    : data_{detail::type_access(other)} {}

    inline any_type::any_type(const reference_type& other) noexcept
    : data_{detail::type_access(other)} {}

    inline any_type::any_type(const void_type& other) noexcept
    : data_{detail::type_access(other)} {}

    inline bool any_type::is_array() const noexcept {
        return data_ && data_->kind == type_kind::array_;
    }

    inline bool any_type::is_class() const noexcept {
        return data_ && data_->kind == type_kind::class_;
    }

    inline bool any_type::is_constructor() const noexcept {
        return data_ && data_->kind == type_kind::constructor_;
    }

    inline bool any_type::is_destructor() const noexcept {
        return data_ && data_->kind == type_kind::destructor_;
    }

    inline bool any_type::is_enum() const noexcept {
        return data_ && data_->kind == type_kind::enum_;
    }

    inline bool any_type::is_function() const noexcept {
        return data_ && data_->kind == type_kind::function_;
    }

    inline bool any_type::is_member() const noexcept {
        return data_ && data_->kind == type_kind::member_;
    }

    inline bool any_type::is_method() const noexcept {
        return data_ && data_->kind == type_kind::method_;
    }

    inline bool any_type::is_nullptr() const noexcept {
        return data_ && data_->kind == type_kind::nullptr_;
    }

    inline bool any_type::is_number() const noexcept {
        return data_ && data_->kind == type_kind::number_;
    }

    inline bool any_type::is_pointer() const noexcept {
        return data_ && data_->kind == type_kind::pointer_;
    }

    inline bool any_type::is_reference() const noexcept {
        return data_ && data_->kind == type_kind::reference_;
    }

    inline bool any_type::is_void() const noexcept {
        return data_ && data_->kind == type_kind::void_;
    }

    inline array_type any_type::as_array() const noexcept {
        return is_array()
            ? array_type{std::static_pointer_cast<detail::array_type_data>(data_)}
            : array_type{};
    }

    inline class_type any_type::as_class() const noexcept {
        return is_class()
            ? class_type{std::static_pointer_cast<detail::class_type_data>(data_)}
            : class_type{};
    }

    inline constructor_type any_type::as_constructor() const noexcept {
        return is_constructor()
            ? constructor_type{std::static_pointer_cast<detail::constructor_type_data>(data_)}
            : constructor_type{};
    }

    inline destructor_type any_type::as_destructor() const noexcept {
        return is_destructor()
            ? destructor_type{std::static_pointer_cast<detail::destructor_type_data>(data_)}
            : destructor_type{};
    }

    inline enum_type any_type::as_enum() const noexcept {
        return is_enum()
            ? enum_type{std::static_pointer_cast<detail::enum_type_data>(data_)}
            : enum_type{};
    }

    inline function_type any_type::as_function() const noexcept {
        return is_function()
            ? function_type{std::static_pointer_cast<detail::function_type_data>(data_)}
            : function_type{};
    }

    inline member_type any_type::as_member() const noexcept {
        return is_member()
            ? member_type{std::static_pointer_cast<detail::member_type_data>(data_)}
            : member_type{};
    }

    inline method_type any_type::as_method() const noexcept {
        return is_method()
            ? method_type{std::static_pointer_cast<detail::method_type_data>(data_)}
            : method_type{};
    }

    inline nullptr_type any_type::as_nullptr() const noexcept {
        return is_nullptr()
            ? nullptr_type{std::static_pointer_cast<detail::nullptr_type_data>(data_)}
            : nullptr_type{};
    }

    inline number_type any_type::as_number() const noexcept {
        return is_number()
            ? number_type{std::static_pointer_cast<detail::number_type_data>(data_)}
            : number_type{};
    }

    inline pointer_type any_type::as_pointer() const noexcept {
        return is_pointer()
            ? pointer_type{std::static_pointer_cast<detail::pointer_type_data>(data_)}
            : pointer_type{};
    }

    inline reference_type any_type::as_reference() const noexcept {
        return is_reference()
            ? reference_type{std::static_pointer_cast<detail::reference_type_data>(data_)}
            : reference_type{};
    }

    inline void_type any_type::as_void() const noexcept {
        return is_void()
            ? void_type{std::static_pointer_cast<detail::void_type_data>(data_)}
            : void_type{};
    }
}
