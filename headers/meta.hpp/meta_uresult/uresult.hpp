/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_uresult.hpp"

#include "../meta_uvalue/uvalue.hpp"

namespace meta_hpp
{
    inline uerror::uerror(error_code error) noexcept
    : error_{error} {}

    inline uerror& uerror::operator=(error_code error) noexcept {
        error_ = error;
        return *this;
    }

    inline error_code uerror::get_error() const noexcept {
        return error_;
    }

    inline void uerror::reset() noexcept {
        error_ = error_code::no_error;
    }

    inline void uerror::swap(uerror& other) noexcept {
        using std::swap;
        swap(error_, other.error_);
    }

    inline std::size_t uerror::get_hash() const noexcept {
        return std::hash<error_code>{}(error_);
    }
}

namespace meta_hpp
{
    inline uresult::uresult(uerror error) noexcept
    : error_{error.get_error()} {}

    inline uresult::uresult(uvalue value) noexcept
    : value_{std::move(value)} {}

    inline uresult& uresult::operator=(uerror error) noexcept {
        value_ = uvalue{};
        error_ = error.get_error();
        return *this;
    }

    inline uresult& uresult::operator=(uvalue value) noexcept {
        value_ = std::move(value);
        error_ = error_code::no_error;
        return *this;
    }

    template < typename T, typename Tp >
        requires(!std::is_same_v<Tp, uerror>)      //
             && (!std::is_same_v<Tp, uvalue>)      //
             && (!std::is_same_v<Tp, uresult>)     //
             && (!detail::is_in_place_type_v<Tp>)  //
             && (std::is_copy_constructible_v<Tp>) //
    // NOLINTNEXTLINE(*-forwarding-reference-overload)
    uresult::uresult(T&& val)
    : value_{std::forward<T>(val)} {}

    template < typename T, typename Tp >
        requires(!std::is_same_v<Tp, uerror>)      //
             && (!std::is_same_v<Tp, uvalue>)      //
             && (!std::is_same_v<Tp, uresult>)     //
             && (std::is_copy_constructible_v<Tp>) //
    uresult& uresult::operator=(T&& val) {
        value_ = std::forward<T>(val);
        error_ = error_code::no_error;
        return *this;
    }

    template < typename T, typename... Args, typename Tp >
        requires std::is_copy_constructible_v<Tp>     //
              && std::is_constructible_v<Tp, Args...> //
    uresult::uresult(std::in_place_type_t<T>, Args&&... args)
    : value_{std::in_place_type<T>, std::forward<Args>(args)...} {}

    template < typename T, typename U, typename... Args, typename Tp >
        requires std::is_copy_constructible_v<Tp>                                //
              && std::is_constructible_v<Tp, std::initializer_list<U>&, Args...> //
    uresult::uresult(std::in_place_type_t<T>, std::initializer_list<U> ilist, Args&&... args)
    : value_{std::in_place_type<T>, ilist, std::forward<Args>(args)...} {}

    template < typename T, typename... Args, typename Tp >
        requires std::is_copy_constructible_v<Tp>     //
              && std::is_constructible_v<Tp, Args...> //
    Tp& uresult::emplace(Args&&... args) {
        Tp& val{value_.emplace<Tp>(std::forward<Args>(args)...)};
        error_ = error_code::no_error;
        return val;
    }

    template < typename T, typename U, typename... Args, typename Tp >
        requires std::is_copy_constructible_v<Tp>                                //
              && std::is_constructible_v<Tp, std::initializer_list<U>&, Args...> //
    Tp& uresult::emplace(std::initializer_list<U> ilist, Args&&... args) {
        Tp& val{value_.emplace<Tp>(ilist, std::forward<Args>(args)...)};
        error_ = error_code::no_error;
        return val;
    }

    inline bool uresult::has_error() const noexcept {
        return error_ != error_code::no_error;
    }

    inline bool uresult::has_value() const noexcept {
        return error_ == error_code::no_error;
    }

    inline uresult::operator bool() const noexcept {
        return has_value();
    }

    inline void uresult::reset() noexcept {
        value_ = uvalue{};
        error_ = error_code::no_error;
    }

    inline void uresult::swap(uresult& other) noexcept {
        using std::swap;
        swap(value_, other.value_);
        swap(error_, other.error_);
    }

    inline uvalue& uresult::get_value() & {
        return value_;
    }

    inline uvalue&& uresult::get_value() && {
        return std::move(value_);
    }

    inline const uvalue& uresult::get_value() const& {
        return value_;
    }

    inline const uvalue&& uresult::get_value() const&& {
        // NOLINTNEXTLINE(*-move-const-arg)
        return std::move(value_);
    }

    inline error_code uresult::get_error() const noexcept {
        return error_;
    }
}
