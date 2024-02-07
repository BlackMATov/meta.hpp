/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
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

    inline bool uerror::has_error() const noexcept {
        return error_ != error_code::no_error;
    }

    inline uerror::operator bool() const noexcept {
        return has_error();
    }

    inline error_code uerror::operator*() const noexcept {
        return error_;
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
        return detail::hash_composer{} << error_;
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

    template < typename T, typename >
    uresult::uresult(T&& val)
    : value_{std::forward<T>(val)} {}

    template < typename T, typename >
    uresult& uresult::operator=(T&& val) {
        value_ = std::forward<T>(val);
        error_ = error_code::no_error;
        return *this;
    }

    template < typename T, typename... Args >
        requires std::is_constructible_v<std::decay_t<T>, Args...> //
    uresult::uresult(std::in_place_type_t<T>, Args&&... args)
    : value_{std::in_place_type<T>, std::forward<Args>(args)...} {}

    template < typename T, typename U, typename... Args >
        requires std::is_constructible_v<std::decay_t<T>, std::initializer_list<U>&, Args...> //
    uresult::uresult(std::in_place_type_t<T>, std::initializer_list<U> ilist, Args&&... args)
    : value_{std::in_place_type<T>, ilist, std::forward<Args>(args)...} {}

    template < typename T, typename... Args >
        requires std::is_constructible_v<std::decay_t<T>, Args...> //
    std::decay_t<T>& uresult::emplace(Args&&... args) {
        std::decay_t<T>& val{value_.emplace<std::decay_t<T>>(std::forward<Args>(args)...)};
        error_ = error_code::no_error;
        return val;
    }

    template < typename T, typename U, typename... Args >
        requires std::is_constructible_v<std::decay_t<T>, std::initializer_list<U>&, Args...> //
    std::decay_t<T>& uresult::emplace(std::initializer_list<U> ilist, Args&&... args) {
        std::decay_t<T>& val{value_.emplace<std::decay_t<T>>(ilist, std::forward<Args>(args)...)};
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

    inline uvalue* uresult::operator->() noexcept {
        return std::addressof(value_);
    }

    inline const uvalue* uresult::operator->() const noexcept {
        return std::addressof(value_);
    }

    inline uvalue& uresult::operator*() & noexcept {
        return value_;
    }

    inline uvalue&& uresult::operator*() && noexcept {
        return std::move(value_);
    }

    inline const uvalue& uresult::operator*() const& noexcept {
        return value_;
    }

    inline const uvalue&& uresult::operator*() const&& noexcept {
        // NOLINTNEXTLINE(*-move-const-arg)
        return std::move(value_);
    }

    inline uvalue& uresult::get_value() & noexcept {
        return value_;
    }

    inline uvalue&& uresult::get_value() && noexcept {
        return std::move(value_);
    }

    inline const uvalue& uresult::get_value() const& noexcept {
        return value_;
    }

    inline const uvalue&& uresult::get_value() const&& noexcept {
        // NOLINTNEXTLINE(*-move-const-arg)
        return std::move(value_);
    }

    inline error_code uresult::get_error() const noexcept {
        return error_;
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
}
