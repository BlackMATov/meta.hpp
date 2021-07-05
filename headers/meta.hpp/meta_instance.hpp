/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_fwd.hpp"

#include "meta_value.hpp"

namespace meta_hpp
{
    class instance;
    class cinstance;
}

namespace meta_hpp
{
    class instance {
    public:
        instance() = delete;

        instance(instance&&) = default;
        instance(const instance&) = default;

        instance& operator=(instance&&) = default;
        instance& operator=(const instance&) = default;

        template < typename T
                 , typename = std::enable_if_t<!std::is_same_v<T, value>>
                 , typename = std::enable_if_t<!std::is_same_v<T, instance>>
                 , typename = std::enable_if_t<!std::is_same_v<T, cinstance>> >
        instance(T& v)
        : data_{std::addressof(v)}
        , fid_{get_type_family_id<T>()} {}

        instance(value& v)
        : data_{v.data()}
        , fid_{v.fid()} {}

        void* data() noexcept {
            return data_;
        }

        const void* data() const noexcept {
            return data_;
        }

        family_id fid() const noexcept {
            return fid_;
        }

        template < typename T >
        std::add_pointer_t<T>
        try_cast() noexcept {
            static_assert(!std::is_reference_v<T>);
            return fid() == get_type_family_id<T>()
                ? static_cast<std::add_pointer_t<T>>(data_)
                : nullptr;
        }

        template < typename T >
        std::add_pointer_t<std::add_const_t<T>>
        try_cast() const noexcept {
            static_assert(!std::is_reference_v<T>);
            return fid() == get_type_family_id<T>()
                ? static_cast<std::add_pointer_t<std::add_const_t<T>>>(data_)
                : nullptr;
        }
    private:
        void* data_{};
        family_id fid_{};
    };
}

namespace meta_hpp
{
    class cinstance {
    public:
        cinstance() = delete;

        cinstance(cinstance&&) = default;
        cinstance(const cinstance&) = default;

        cinstance& operator=(cinstance&&) = default;
        cinstance& operator=(const cinstance&) = default;

        template < typename T
                 , typename = std::enable_if_t<!std::is_same_v<T, value>>
                 , typename = std::enable_if_t<!std::is_same_v<T, instance>>
                 , typename = std::enable_if_t<!std::is_same_v<T, cinstance>> >
        cinstance(const T& data)
        : data_{std::addressof(data)}
        , fid_{get_type_family_id<T>()} {}

        cinstance(const value& v)
        : data_{v.data()}
        , fid_{v.fid()} {}

        cinstance(const instance& v)
        : data_{v.data()}
        , fid_{v.fid()} {}

        const void* data() const noexcept {
            return data_;
        }

        family_id fid() const noexcept {
            return fid_;
        }

        template < typename T >
        std::add_pointer_t<std::add_const_t<T>>
        try_cast() const noexcept {
            static_assert(!std::is_reference_v<T>);
            return fid() == get_type_family_id<T>()
                ? static_cast<std::add_pointer_t<std::add_const_t<T>>>(data_)
                : nullptr;
        }
    private:
        const void* data_{};
        family_id fid_{};
    };
}
