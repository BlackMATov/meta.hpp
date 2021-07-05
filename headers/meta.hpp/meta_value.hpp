/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_fwd.hpp"

namespace meta_hpp
{
    class value;
}

namespace meta_hpp::value_detail
{
    struct traits {
        void*(*data)(value&);
        const void*(*cdata)(const value&);
        bool(*equal)(const value&, const value&){};
        bool(*not_equal)(const value&, const value&){};
    };

    template < typename T >
    void* value_data(value& v);

    template < typename T >
    const void* value_cdata(const value& v);

    template < typename T >
    bool value_equal(const value& l, const value& r);

    template < typename T >
    bool value_not_equal(const value& l, const value& r);

    template < typename T >
    const traits* get_traits() noexcept {
        static traits traits{
            &value_data<T>,
            &value_cdata<T>,
            &value_equal<T>,
            &value_not_equal<T>,
        };
        return &traits;
    }
}

namespace meta_hpp
{
    class value {
    public:
        value() = delete;

        value(value&&) = default;
        value(const value&) = default;

        value& operator=(value&&) = default;
        value& operator=(const value&) = default;

        template < typename T
                 , typename U = std::decay_t<T>
                 , typename = std::enable_if_t<!std::is_same_v<U, value>> >
        value(T&& val)
        : raw_{std::forward<T>(val)}
        , fid_{get_type_family_id<U>()}
        , traits_{value_detail::get_traits<U>()} {}

        template < typename T
                 , typename U = std::decay_t<T>
                 , typename = std::enable_if_t<!std::is_same_v<U, value>> >
        value& operator=(T&& val) {
            value{std::forward<T>(val)}.swap(*this);
            return *this;
        }

        template < typename T, typename... Args >
        explicit value(std::in_place_type_t<T>, Args&&... args)
        : raw_{std::in_place_type<T>, std::forward<Args>(args)...}
        , fid_{get_type_family_id<T>()}
        , traits_{value_detail::get_traits<T>()} {}

        template < typename T, typename U, typename... Args >
        explicit value(std::in_place_type_t<T>, std::initializer_list<U> ilist, Args&&... args)
        : raw_{std::in_place_type<T>, ilist, std::forward<Args>(args)...}
        , fid_{get_type_family_id<T>()}
        , traits_{value_detail::get_traits<T>()} {}

        template < typename T >
        T cast() && {
            return std::any_cast<T>(std::move(raw_));
        }

        template < typename T >
        T cast() & {
            return std::any_cast<T>(raw_);
        }

        template < typename T >
        T cast() const & {
            return std::any_cast<T>(raw_);
        }

        template < typename T >
        std::add_pointer_t<T>
        try_cast() noexcept {
            return std::any_cast<T>(&raw_);
        }

        template < typename T >
        std::add_pointer_t<std::add_const_t<T>>
        try_cast() const noexcept {
            return std::any_cast<T>(&raw_);
        }

        family_id fid() const noexcept {
            return fid_;
        }

        void* data() noexcept {
            return traits_->data(*this);
        }

        const void* data() const noexcept {
            return traits_->cdata(*this);
        }

        void swap(value& other) noexcept {
            using std::swap;
            swap(raw_, other.raw_);
            swap(fid_, other.fid_);
            swap(traits_, other.traits_);
        }

        template < typename T
                 , typename U = std::decay_t<T>
                 , typename = std::enable_if_t<!std::is_same_v<U, value>>
                 , typename = std::enable_if_t<std::is_invocable_v<std::equal_to<>, U, U>> >
        friend bool operator==(const value& l, T&& r) {
            return l.fid() == get_type_family_id<U>()
                && std::equal_to<>{}(*l.try_cast<U>(), std::forward<T>(r));
        }

        template < typename T
                 , typename U = std::decay_t<T>
                 , typename = std::enable_if_t<!std::is_same_v<U, value>>
                 , typename = std::enable_if_t<std::is_invocable_v<std::equal_to<>, U, U>> >
        friend bool operator==(T&& l, const value& r) {
            return get_type_family_id<U>() == r.fid()
                && std::equal_to<>{}(std::forward<T>(l), *r.try_cast<U>());
        }

        template < typename T
                 , typename U = std::decay_t<T>
                 , typename = std::enable_if_t<!std::is_same_v<U, value>>
                 , typename = std::enable_if_t<std::is_invocable_v<std::equal_to<>, U, U>> >
        friend bool operator!=(const value& l, T&& r) {
            return l.fid() != get_type_family_id<U>()
                || std::not_equal_to<>{}(*l.try_cast<U>(), std::forward<T>(r));
        }

        template < typename T
                 , typename U = std::decay_t<T>
                 , typename = std::enable_if_t<!std::is_same_v<U, value>>
                 , typename = std::enable_if_t<std::is_invocable_v<std::equal_to<>, U, U>> >
        friend bool operator!=(T&& l, const value& r) {
            return get_type_family_id<U>() != r.fid()
                || std::not_equal_to<>{}(std::forward<T>(l), *r.try_cast<U>());
        }

        friend bool operator==(const value& l, const value& r) {
            return l.traits_->equal(l, r);
        }

        friend bool operator!=(const value& l, const value& r) {
            return l.traits_->not_equal(l, r);
        }
    public:
        bool to_bool() const { return cast<bool>(); }
        int to_int() const { return cast<int>(); }
        unsigned to_uint() const { return cast<unsigned>(); }
        float to_float() const { return cast<float>(); }
        double to_double() const { return cast<double>(); }
        std::string to_string() const { return cast<std::string>(); }

        std::int8_t to_int8() const { return cast<std::int8_t>(); }
        std::int16_t to_int16() const { return cast<std::int16_t>(); }
        std::int32_t to_int32() const { return cast<std::int32_t>(); }
        std::int64_t to_int64() const { return cast<std::int64_t>(); }
        std::ptrdiff_t to_ptrdiff_t() const { return cast<std::ptrdiff_t>(); }
        std::intptr_t to_intptr_t() const { return cast<std::intptr_t>(); }

        std::uint8_t to_uint8() const { return cast<std::uint8_t>(); }
        std::uint16_t to_uint16() const { return cast<std::uint16_t>(); }
        std::uint32_t to_uint32() const { return cast<std::uint32_t>(); }
        std::uint64_t to_uint64() const { return cast<std::uint64_t>(); }
        std::size_t to_size_t() const { return cast<std::size_t>(); }
        std::uintptr_t to_uintptr_t() const { return cast<std::uintptr_t>(); }
    public:
        bool is_bool() const noexcept { return !!try_cast<bool>(); }
        bool is_int() const noexcept { return !!try_cast<int>(); }
        bool is_uint() const noexcept { return !!try_cast<unsigned>(); }
        bool is_float() const noexcept { return !!try_cast<float>(); }
        bool is_double() const noexcept { return !!try_cast<double>(); }
        bool is_string() const noexcept { return !!try_cast<std::string>(); }

        bool is_int8() const noexcept { return !!try_cast<std::int8_t>(); }
        bool is_int16() const noexcept { return !!try_cast<std::int16_t>(); }
        bool is_int32() const noexcept { return !!try_cast<std::int32_t>(); }
        bool is_int64() const noexcept { return !!try_cast<std::int64_t>(); }
        bool is_ptrdiff_t() const noexcept { return !!try_cast<std::ptrdiff_t>(); }
        bool is_intptr_t() const noexcept { return !!try_cast<std::intptr_t>(); }

        bool is_uint8() const noexcept { return !!try_cast<std::uint8_t>(); }
        bool is_uint16() const noexcept { return !!try_cast<std::uint16_t>(); }
        bool is_uint32() const noexcept { return !!try_cast<std::uint32_t>(); }
        bool is_uint64() const noexcept { return !!try_cast<std::uint64_t>(); }
        bool is_size_t() const noexcept { return !!try_cast<std::size_t>(); }
        bool is_uintptr_t() const noexcept { return !!try_cast<std::uintptr_t>(); }
    private:
        std::any raw_;
        family_id fid_;
        const value_detail::traits* traits_{};
    };

    inline void* data(value& v) noexcept {
        return v.data();
    }

    inline const void* data(const value& v) noexcept {
        return v.data();
    }

    inline void swap(value& l, value& r) noexcept {
        l.swap(r);
    }
}

namespace meta_hpp::value_detail
{
    template < typename T >
    void* value_data(value& v) {
        return v.try_cast<T>();
    }

    template < typename T >
    const void* value_cdata(const value& v) {
        return v.try_cast<T>();
    }

    template < typename T >
    bool value_equal(const value& l, const value& r) {
        if ( l.fid() != r.fid() ) {
            return false;
        }
        if constexpr ( std::is_invocable_v<std::equal_to<>, T, T> ) {
            return std::equal_to<>{}(*l.try_cast<T>(), *(r.try_cast<T>()));
        } else {
            return std::addressof(l) == std::addressof(r);
        }
    }

    template < typename T >
    bool value_not_equal(const value& l, const value& r) {
        if ( l.fid() != r.fid() ) {
            return true;
        }
        if constexpr ( std::is_invocable_v<std::not_equal_to<>, T, T> ) {
            return std::not_equal_to<>{}(*l.try_cast<T>(), *(r.try_cast<T>()));
        } else {
            return std::addressof(l) != std::addressof(r);
        }
    }
}
