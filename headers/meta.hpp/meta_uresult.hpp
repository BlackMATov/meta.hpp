/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_base.hpp"
#include "meta_uvalue.hpp"

namespace meta_hpp
{
    class uerror final {
    public:
        uerror() = default;
        ~uerror() = default;

        uerror(uerror&&) noexcept = default;
        uerror(const uerror&) noexcept = default;

        uerror& operator=(uerror&&) noexcept = default;
        uerror& operator=(const uerror&) noexcept = default;

        explicit uerror(error_code error) noexcept;
        uerror& operator=(error_code error) noexcept;

        [[nodiscard]] bool has_error() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] error_code operator*() const noexcept;
        [[nodiscard]] error_code get_error() const noexcept;

        void reset() noexcept;
        void swap(uerror& other) noexcept;

        [[nodiscard]] std::size_t get_hash() const noexcept;
        [[nodiscard]] std::strong_ordering operator<=>(const uerror& other) const = default;

    private:
        error_code error_{error_code::no_error};
    };

    inline void swap(uerror& l, uerror& r) noexcept {
        l.swap(r);
    }

    inline uerror make_uerror(error_code error) {
        return uerror{error};
    }
}

namespace std
{
    template <>
    struct hash<meta_hpp::uerror> {
        size_t operator()(meta_hpp::uerror ue) const noexcept {
            return ue.get_hash();
        }
    };
}

namespace meta_hpp
{
    class uresult final {
    public:
        uresult() = default;
        ~uresult() = default;

        uresult(uresult&&) noexcept = default;
        uresult(const uresult&) = default;

        uresult& operator=(uresult&&) noexcept = default;
        uresult& operator=(const uresult&) = default;

        explicit(false) uresult(uerror error) noexcept;
        explicit(false) uresult(uvalue value) noexcept;

        uresult& operator=(uerror error) noexcept;
        uresult& operator=(uvalue value) noexcept;

        template <                                 //
            typename T,                            //
            typename Tp = std::decay_t<T>,         //
            typename = std::enable_if_t<           //
                !std::is_same_v<Tp, uresult> &&    //
                !detail::is_in_place_type_v<Tp> && //
                std::is_copy_constructible_v<Tp>>> //
        uresult(T&& val);

        template <                                 //
            typename T,                            //
            typename Tp = std::decay_t<T>,         //
            typename = std::enable_if_t<           //
                !std::is_same_v<Tp, uresult> &&    //
                std::is_copy_constructible_v<Tp>>> //
        uresult& operator=(T&& val);

        template < typename T, typename... Args, typename Tp = std::decay_t<T> >
            requires std::is_copy_constructible_v<Tp>     //
                  && std::is_constructible_v<Tp, Args...> //
        explicit uresult(std::in_place_type_t<T>, Args&&... args);

        template < typename T, typename U, typename... Args, typename Tp = std::decay_t<T> >
            requires std::is_copy_constructible_v<Tp>                                //
                  && std::is_constructible_v<Tp, std::initializer_list<U>&, Args...> //
        explicit uresult(std::in_place_type_t<T>, std::initializer_list<U> ilist, Args&&... args);

        template < typename T, typename... Args, typename Tp = std::decay_t<T> >
            requires std::is_copy_constructible_v<Tp>     //
                  && std::is_constructible_v<Tp, Args...> //
        Tp& emplace(Args&&... args);

        template < typename T, typename U, typename... Args, typename Tp = std::decay_t<T> >
            requires std::is_copy_constructible_v<Tp>                                //
                  && std::is_constructible_v<Tp, std::initializer_list<U>&, Args...> //
        Tp& emplace(std::initializer_list<U> ilist, Args&&... args);

        [[nodiscard]] bool has_error() const noexcept;
        [[nodiscard]] bool has_value() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] uvalue* operator->() noexcept;
        [[nodiscard]] const uvalue* operator->() const noexcept;

        [[nodiscard]] uvalue& operator*() & noexcept;
        [[nodiscard]] uvalue&& operator*() && noexcept;
        [[nodiscard]] const uvalue& operator*() const& noexcept;
        [[nodiscard]] const uvalue&& operator*() const&& noexcept;

        [[nodiscard]] uvalue& get_value() & noexcept;
        [[nodiscard]] uvalue&& get_value() && noexcept;
        [[nodiscard]] const uvalue& get_value() const& noexcept;
        [[nodiscard]] const uvalue&& get_value() const&& noexcept;

        [[nodiscard]] error_code get_error() const noexcept;

        void reset() noexcept;
        void swap(uresult& other) noexcept;

    private:
        uvalue value_{};
        error_code error_{error_code::no_error};
    };

    inline void swap(uresult& l, uresult& r) noexcept {
        l.swap(r);
    }

    template < typename T, typename... Args >
    uresult make_uresult(Args&&... args) {
        return uresult(std::in_place_type<T>, std::forward<Args>(args)...);
    }

    template < typename T, typename U, typename... Args >
    uresult make_uresult(std::initializer_list<U> ilist, Args&&... args) {
        return uresult(std::in_place_type<T>, ilist, std::forward<Args>(args)...);
    }
}
