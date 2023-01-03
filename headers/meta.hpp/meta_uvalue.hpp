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
    concept uvalue_kind
        = std::is_same_v<T, uvalue>;

    template < typename T >
    concept any_uvalue_kind =
        std::is_same_v<T, uarg_base> ||
        std::is_same_v<T, uarg> ||
        std::is_same_v<T, uinst_base> ||
        std::is_same_v<T, uinst> ||
        std::is_same_v<T, uvalue>;
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

        template < typename T, typename Tp = std::decay_t<T> >
            requires (!detail::any_uvalue_kind<Tp>)
                  && (!detail::is_in_place_type_v<Tp>)
                  && (std::is_copy_constructible_v<Tp>)
        // NOLINTNEXTLINE(*-forwarding-reference-overload)
        explicit uvalue(T&& val);

        template < typename T, typename Tp = std::decay_t<T> >
            requires (!detail::any_uvalue_kind<Tp>)
                  && (!detail::is_in_place_type_v<Tp>)
                  && (std::is_copy_constructible_v<Tp>)
        uvalue& operator=(T&& val);

        template < typename T, typename... Args, typename Tp = std::decay_t<T> >
            requires std::is_copy_constructible_v<Tp>
                  && std::is_constructible_v<Tp, Args...>
        explicit uvalue(std::in_place_type_t<T>, Args&&... args);

        template < typename T, typename U, typename... Args, typename Tp = std::decay_t<T> >
            requires std::is_copy_constructible_v<Tp>
                  && std::is_constructible_v<Tp, std::initializer_list<U>&, Args...>
        explicit uvalue(std::in_place_type_t<T>, std::initializer_list<U> ilist, Args&&... args);

        template < typename T, typename... Args, typename Tp = std::decay_t<T> >
            requires std::is_copy_constructible_v<Tp>
                  && std::is_constructible_v<Tp, Args...>
        std::decay_t<T>& emplace(Args&&... args);

        template < typename T, typename U, typename... Args, typename Tp = std::decay_t<T> >
            requires std::is_copy_constructible_v<Tp>
                  && std::is_constructible_v<Tp, std::initializer_list<U>&, Args...>
        std::decay_t<T>& emplace(std::initializer_list<U> ilist, Args&&... args);

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

    template < typename T, typename... Args >
    uvalue make_uvalue(Args&&... args) {
        return uvalue(std::in_place_type<T>, std::forward<Args>(args)...);
    }

    template < typename T, typename U, typename... Args >
    uvalue make_uvalue(std::initializer_list<U> ilist, Args&&... args) {
        return uvalue(std::in_place_type<T>, ilist, std::forward<Args>(args)...);
    }
}
