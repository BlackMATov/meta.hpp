/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_base.hpp"
#include "meta_types.hpp"

namespace meta_hpp
{
    class uvalue final {
    public:
        uvalue() = default;
        ~uvalue() noexcept;

        uvalue(uvalue&& other) noexcept;
        uvalue(const uvalue& other);

        uvalue& operator=(uvalue&& other) noexcept;
        uvalue& operator=(const uvalue& other);

        template <                                 //
            typename T,                            //
            typename Tp = std::decay_t<T>,         //
            typename = std::enable_if_t<           //
                !std::is_same_v<Tp, uvalue> &&     //
                !detail::is_in_place_type_v<Tp> && //
                std::is_copy_constructible_v<Tp>>> //
        uvalue(T&& val);

        template <                                 //
            typename T,                            //
            typename Tp = std::decay_t<T>,         //
            typename = std::enable_if_t<           //
                !std::is_same_v<Tp, uvalue> &&     //
                std::is_copy_constructible_v<Tp>>> //
        uvalue& operator=(T&& val);

        template < typename T, typename... Args, typename Tp = std::decay_t<T> >
            requires std::is_copy_constructible_v<Tp>     //
                  && std::is_constructible_v<Tp, Args...> //
        explicit uvalue(std::in_place_type_t<T>, Args&&... args);

        template < typename T, typename U, typename... Args, typename Tp = std::decay_t<T> >
            requires std::is_copy_constructible_v<Tp>                                //
                  && std::is_constructible_v<Tp, std::initializer_list<U>&, Args...> //
        explicit uvalue(std::in_place_type_t<T>, std::initializer_list<U> ilist, Args&&... args);

        template < typename T, typename... Args, typename Tp = std::decay_t<T> >
            requires std::is_copy_constructible_v<Tp>     //
                  && std::is_constructible_v<Tp, Args...> //
        Tp& emplace(Args&&... args);

        template < typename T, typename U, typename... Args, typename Tp = std::decay_t<T> >
            requires std::is_copy_constructible_v<Tp>                                //
                  && std::is_constructible_v<Tp, std::initializer_list<U>&, Args...> //
        Tp& emplace(std::initializer_list<U> ilist, Args&&... args);

        [[nodiscard]] bool has_value() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        void reset() noexcept;
        void swap(uvalue& other) noexcept;

        [[nodiscard]] any_type get_type() const noexcept;

        [[nodiscard]] void* get_data() noexcept;
        [[nodiscard]] const void* get_data() const noexcept;
        [[nodiscard]] const void* get_cdata() const noexcept;

        [[nodiscard]] uvalue operator*() const;
        [[nodiscard]] bool has_deref_op() const noexcept;

        [[nodiscard]] uvalue operator[](std::size_t index) const;
        [[nodiscard]] bool has_index_op() const noexcept;

        [[nodiscard]] uvalue unmap() const;
        [[nodiscard]] bool has_unmap_op() const noexcept;

        template < typename T >
        [[nodiscard]] bool is() const noexcept;

        template < typename T >
            requires detail::pointer_kind<T>
        [[nodiscard]] T as();

        template < typename T >
            requires detail::pointer_kind<T>
        [[nodiscard]] T as() const;

        template < typename T >
            requires(!detail::pointer_kind<T>)
        [[nodiscard]] T as() &&;

        template < typename T >
            requires(!detail::pointer_kind<T>)
        [[nodiscard]] T& as() &;

        template < typename T >
            requires(!detail::pointer_kind<T>)
        [[nodiscard]] const T& as() const&;

        template < typename T >
            requires(!detail::pointer_kind<T>)
        [[nodiscard]] const T&& as() const&&;

        template < typename T >
            requires detail::pointer_kind<T>
        [[nodiscard]] T try_as() noexcept;

        template < typename T >
            requires detail::pointer_kind<T>
        [[nodiscard]] T try_as() const noexcept;

        template < typename T >
            requires(!detail::pointer_kind<T>)
        [[nodiscard]] T* try_as() noexcept;

        template < typename T >
            requires(!detail::pointer_kind<T>)
        [[nodiscard]] const T* try_as() const noexcept;

    private:
        struct vtable_t;

        struct alignas(std::max_align_t) internal_storage_t final {
            // NOLINTNEXTLINE(*-avoid-c-arrays)
            std::byte data[sizeof(void*) * 2];
        };

        struct external_storage_t final {
            // NOLINTNEXTLINE(*-avoid-c-arrays)
            std::byte padding[sizeof(internal_storage_t) - sizeof(void*)];
            void* ptr;
        };

        enum class storage_e : std::uintptr_t {
            nothing,
            trivial,
            internal,
            external,
        };

        struct storage_u final {
            union {
                internal_storage_t internal;
                external_storage_t external;
            };

            std::uintptr_t vtag;
        } storage_{};

        static_assert(std::is_standard_layout_v<storage_u>);
        static_assert(alignof(storage_u) == alignof(std::max_align_t));
        static_assert(sizeof(internal_storage_t) == sizeof(external_storage_t));
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
