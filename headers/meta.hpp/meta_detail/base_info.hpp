/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_types.hpp"
#include "../meta_uvalue.hpp"

#include "type_registry.hpp"

namespace meta_hpp::detail
{
    struct poly_info final {
        const void* ptr{};
        class_type type{};
    };

    template < typename T >
    concept check_base_info_enabled //
        = requires { typename T::meta_base_info; };

    template < check_base_info_enabled T >
    using get_meta_base_info = typename T::meta_base_info;

    template < typename T >
    concept poly_info_enabled //
        = requires(type_registry& r, const T& v) { v.meta_poly_info(r); };

    template < poly_info_enabled T >
    poly_info get_meta_poly_info(type_registry& r, const T& v) {
        return v.meta_poly_info(r);
    }
}

#define META_HPP_ENABLE_BASE_INFO(...) \
public: \
    using meta_base_info = ::meta_hpp::detail::type_list<__VA_ARGS__>; \
\
private:

#define META_HPP_ENABLE_POLY_INFO(...) \
    META_HPP_ENABLE_BASE_INFO(__VA_ARGS__) \
public: \
    META_HPP_DETAIL_IGNORE_OVERRIDE_WARNINGS_PUSH() \
    virtual ::meta_hpp::uvalue meta_poly_ptr() { \
        return ::meta_hpp::uvalue{this}; \
    } \
    virtual ::meta_hpp::uvalue meta_poly_ptr() const { \
        return ::meta_hpp::uvalue{this}; \
    } \
    virtual ::meta_hpp::detail::poly_info meta_poly_info(::meta_hpp::detail::type_registry& registry) const { \
        using self_type = std::remove_cvref_t<decltype(*this)>; \
        return ::meta_hpp::detail::poly_info{.ptr = this, .type = registry.resolve_by_type<self_type>()}; \
    } \
    META_HPP_DETAIL_IGNORE_OVERRIDE_WARNINGS_POP() \
private:
