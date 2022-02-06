/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_base.hpp"
#include "meta_types.hpp"

namespace meta_hpp::detail
{
    template < typename T >
    inline constexpr bool is_value_kind_v = std::is_same_v<T, value>;

    template < typename T >
    concept value_kind = is_value_kind_v<T>;

    template < typename T >
    concept decay_value_kind = value_kind<std::decay_t<T>>;

    template < typename T >
    concept decay_non_value_kind = !decay_value_kind<T>;
}

namespace meta_hpp
{
    class value final {
    public:
        value() = default;
        ~value();

        value(value&& other) noexcept;
        value(const value& other);

        value& operator=(value&& other) noexcept;
        value& operator=(const value& other);

        template < detail::decay_non_value_kind T >
            requires stdex::copy_constructible<std::decay_t<T>>
        explicit value(T&& val);

        template < detail::decay_non_value_kind T >
            requires stdex::copy_constructible<std::decay_t<T>>
        value& operator=(T&& val);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        void reset();
        void swap(value& other) noexcept;

        [[nodiscard]] const any_type& get_type() const noexcept;

        [[nodiscard]] void* data() noexcept;
        [[nodiscard]] const void* data() const noexcept;
        [[nodiscard]] const void* cdata() const noexcept;

        [[nodiscard]] value operator*() const;
        [[nodiscard]] value operator[](std::size_t index) const;

        template < typename T >
        [[nodiscard]] std::decay_t<T>& cast() &;

        template < typename T >
        [[nodiscard]] std::decay_t<T>&& cast() &&;

        template < typename T >
        [[nodiscard]] const std::decay_t<T>& cast() const &;

        template < typename T >
        [[nodiscard]] const std::decay_t<T>&& cast() const &&;

        template < typename T >
        [[nodiscard]] std::decay_t<T>* try_cast() noexcept;

        template < typename T >
        [[nodiscard]] const std::decay_t<T>* try_cast() const noexcept;

        friend bool operator<(const value& l, const value& r);
        friend bool operator==(const value& l, const value& r);
        friend std::istream& operator>>(std::istream& is, value& v);
        friend std::ostream& operator<<(std::ostream& os, const value& v);
    private:
        struct vtable_t;
        vtable_t* vtable_{};
    private:
        using buffer_t = std::aligned_storage_t<sizeof(void*) * 2>;
        using storage_u = std::variant<std::monostate, void*, buffer_t>;
        storage_u storage_{};
    };

    inline void swap(value& l, value& r) noexcept {
        l.swap(r);
    }
}
