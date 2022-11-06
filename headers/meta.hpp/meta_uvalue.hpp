/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_base.hpp"
#include "meta_types.hpp"

namespace meta_hpp::detail
{
    template < typename T >
    inline constexpr bool is_value_kind_v = std::is_same_v<T, uvalue>;

    template < typename T >
    concept value_kind = is_value_kind_v<T>;

    template < typename T >
    concept decay_value_kind = value_kind<std::decay_t<T>>;

    template < typename T >
    concept decay_non_value_kind = !decay_value_kind<T>;
}

namespace meta_hpp
{
    class uvalue final {
    public:
        uvalue() = default;
        ~uvalue();

        uvalue(uvalue&& other) noexcept;
        uvalue(const uvalue& other);

        uvalue& operator=(uvalue&& other) noexcept;
        uvalue& operator=(const uvalue& other);

        template < detail::decay_non_value_kind T >
            requires stdex::copy_constructible<std::decay_t<T>>
        explicit uvalue(T&& val);

        template < detail::decay_non_value_kind T >
            requires stdex::copy_constructible<std::decay_t<T>>
        uvalue& operator=(T&& val);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        void reset();
        void swap(uvalue& other) noexcept;

        [[nodiscard]] const any_type& get_type() const noexcept;

        [[nodiscard]] void* data() noexcept;
        [[nodiscard]] const void* data() const noexcept;
        [[nodiscard]] const void* cdata() const noexcept;

        [[nodiscard]] uvalue operator*() const;
        [[nodiscard]] uvalue operator[](std::size_t index) const;

        template < typename T >
        [[nodiscard]] auto get_as()
            -> std::conditional_t<detail::pointer_kind<T>, T, T&>;

        template < typename T >
        [[nodiscard]] auto get_as() const
            -> std::conditional_t<detail::pointer_kind<T>, T, const T&>;

        template < typename T >
        [[nodiscard]] auto try_get_as() noexcept
            -> std::conditional_t<detail::pointer_kind<T>, T, T*>;

        template < typename T >
        [[nodiscard]] auto try_get_as() const noexcept
            -> std::conditional_t<detail::pointer_kind<T>, T, const T*>;

        friend bool operator<(const uvalue& l, const uvalue& r);
        friend bool operator==(const uvalue& l, const uvalue& r);
        friend std::istream& operator>>(std::istream& is, uvalue& v);
        friend std::ostream& operator<<(std::ostream& os, const uvalue& v);
    private:
        struct vtable_t;
        vtable_t* vtable_{};
    private:
        struct buffer_t final {
            // NOLINTNEXTLINE(*-avoid-c-arrays)
            alignas(std::max_align_t) std::byte data[sizeof(void*) * 2];
        };
        using storage_u = std::variant<std::monostate, void*, buffer_t>;
        storage_u storage_{};
    };

    inline void swap(uvalue& l, uvalue& r) noexcept {
        l.swap(r);
    }
}
