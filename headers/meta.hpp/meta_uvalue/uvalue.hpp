/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_registry.hpp"
#include "../meta_uvalue.hpp"

#include "../meta_detail/value_traits/deref_traits.hpp"
#include "../meta_detail/value_traits/index_traits.hpp"
#include "../meta_detail/value_traits/unmap_traits.hpp"

#include "../meta_detail/value_utilities/utraits.hpp"

namespace meta_hpp
{
    struct uvalue::vtable_t final {
        const any_type type;

        void (*const move)(uvalue&& self, uvalue& to) noexcept;
        void (*const copy)(const uvalue& self, uvalue& to);
        void (*const reset)(uvalue& self) noexcept;

        uvalue (*const deref)(const storage_u& self);
        uvalue (*const index)(const storage_u& self, std::size_t i);
        uvalue (*const unmap)(const storage_u& self);

        template < typename T >
        inline static constexpr bool in_internal_v = //
            (sizeof(T) <= sizeof(internal_storage_t)) && (alignof(internal_storage_t) % alignof(T) == 0)
            && std::is_nothrow_destructible_v<T> && std::is_nothrow_move_constructible_v<T>;

        template < typename T >
        inline static constexpr bool in_trivial_internal_v = //
            in_internal_v<T> && std::is_trivially_copyable_v<T>;

        static std::pair<storage_e, const vtable_t*> unpack_vtag(const uvalue& self) noexcept {
            constexpr std::uintptr_t tag_mask{0b11};
            const std::uintptr_t vtag{self.storage_.vtag};
            return std::make_pair(
                static_cast<storage_e>(vtag & tag_mask),
                // NOLINTNEXTLINE(*-no-int-to-ptr, *-reinterpret-cast)
                reinterpret_cast<const vtable_t*>(vtag & ~tag_mask)
            );
        }

        template < typename T >
        static T* storage_cast(storage_u& storage) noexcept {
            if constexpr ( in_internal_v<T> ) {
                // NOLINTNEXTLINE(*-union-access, *-reinterpret-cast)
                return std::launder(reinterpret_cast<T*>(storage.internal.data));
            } else {
                // NOLINTNEXTLINE(*-union-access)
                return static_cast<T*>(storage.external.ptr);
            }
        }

        template < typename T >
        static const T* storage_cast(const storage_u& storage) noexcept {
            if constexpr ( in_internal_v<T> ) {
                // NOLINTNEXTLINE(*-union-access, *-reinterpret-cast)
                return std::launder(reinterpret_cast<const T*>(storage.internal.data));
            } else {
                // NOLINTNEXTLINE(*-union-access)
                return static_cast<const T*>(storage.external.ptr);
            }
        }

        template < typename T, typename... Args, typename Tp = std::decay_t<T> >
        static Tp& do_ctor(uvalue& dst, Args&&... args) {
            META_HPP_ASSERT(!dst);

            if constexpr ( in_internal_v<Tp> ) {
                std::construct_at(storage_cast<Tp>(dst.storage_), std::forward<Args>(args)...);
                dst.storage_.vtag = in_trivial_internal_v<Tp> ? detail::to_underlying(storage_e::trivial)
                                                              : detail::to_underlying(storage_e::internal);
            } else {
                // NOLINTNEXTLINE(*-union-access, *-owning-memory)
                dst.storage_.external.ptr = new Tp(std::forward<Args>(args)...);
                dst.storage_.vtag = detail::to_underlying(storage_e::external);
            }

            // NOLINTNEXTLINE(*-reinterpret-cast)
            dst.storage_.vtag |= reinterpret_cast<std::uintptr_t>(vtable_t::get<Tp>());
            return *storage_cast<Tp>(dst.storage_);
        }

        static void do_move(uvalue&& self, uvalue& to) noexcept {
            META_HPP_ASSERT(!to);

            auto&& [tag, vtable] = unpack_vtag(self);

            switch ( tag ) {
            case storage_e::nothing:
                break;
            case storage_e::trivial:
                to.storage_ = self.storage_;
                self.storage_.vtag = 0;
                break;
            case storage_e::internal:
            case storage_e::external:
                vtable->move(std::move(self), to);
                break;
            }
        }

        static void do_copy(const uvalue& self, uvalue& to) noexcept {
            META_HPP_ASSERT(!to);

            auto&& [tag, vtable] = unpack_vtag(self);

            switch ( tag ) {
            case storage_e::nothing:
                break;
            case storage_e::trivial:
                to.storage_ = self.storage_;
                break;
            case storage_e::internal:
            case storage_e::external:
                vtable->copy(self, to);
                break;
            }
        }

        static void do_reset(uvalue& self) noexcept {
            auto&& [tag, vtable] = unpack_vtag(self);

            switch ( tag ) {
            case storage_e::nothing:
                break;
            case storage_e::trivial:
                self.storage_.vtag = 0;
                break;
            case storage_e::internal:
            case storage_e::external:
                vtable->reset(self);
                break;
            }
        }

        static void do_swap(uvalue& l, uvalue& r) noexcept {
            if ( (&l == &r) || (!l && !r) ) {
                return;
            }

            if ( l && r ) {
                if ( unpack_vtag(l).first == storage_e::external ) {
                    r = std::exchange(l, std::move(r));
                } else {
                    l = std::exchange(r, std::move(l));
                }
            } else {
                if ( l ) {
                    r = std::move(l);
                } else {
                    l = std::move(r);
                }
            }
        }

        template < typename Tp >
        // NOLINTNEXTLINE(*-cognitive-complexity)
        static vtable_t* get() {
            static_assert(std::is_same_v<Tp, std::decay_t<Tp>>);

            static vtable_t table{
                .type = resolve_type<Tp>(),

                .move{[](uvalue&& self, uvalue& to) noexcept {
                    META_HPP_ASSERT(!to);
                    META_HPP_ASSERT(self);

                    Tp* src = storage_cast<Tp>(self.storage_);

                    if constexpr ( in_internal_v<Tp> ) {
                        do_ctor<Tp>(to, std::move(*src));
                        do_reset(self);
                    } else {
                        // NOLINTNEXTLINE(*-union-access)
                        to.storage_.external.ptr = src;
                        std::swap(to.storage_.vtag, self.storage_.vtag);
                    }
                }},

                .copy{[](const uvalue& self, uvalue& to) {
                    META_HPP_ASSERT(!to);
                    META_HPP_ASSERT(self);

                    const Tp* src = storage_cast<Tp>(self.storage_);

                    if constexpr ( in_internal_v<Tp> ) {
                        do_ctor<Tp>(to, *src);
                    } else {
                        // NOLINTNEXTLINE(*-union-access, *-owning-memory)
                        to.storage_.external.ptr = new Tp(*src);
                        to.storage_.vtag = self.storage_.vtag;
                    }
                }},

                .reset{[](uvalue& self) noexcept {
                    META_HPP_ASSERT(self);

                    Tp* src = storage_cast<Tp>(self.storage_);

                    if constexpr ( in_internal_v<Tp> ) {
                        std::destroy_at(src);
                    } else {
                        // NOLINTNEXTLINE(*-owning-memory)
                        delete src;
                    }

                    self.storage_.vtag = 0;
                }},

                .deref{[]() {
                    if constexpr ( detail::has_deref_traits<Tp> ) {
                        return +[](const storage_u& self) -> uvalue {
                            return detail::deref_traits<Tp>{}(*storage_cast<Tp>(self));
                        };
                    } else {
                        return nullptr;
                    }
                }()},

                .index{[]() {
                    if constexpr ( detail::has_index_traits<Tp> ) {
                        return +[](const storage_u& self, std::size_t i) -> uvalue {
                            return detail::index_traits<Tp>{}(*storage_cast<Tp>(self), i);
                        };
                    } else {
                        return nullptr;
                    }
                }()},

                .unmap{[]() {
                    if constexpr ( detail::has_unmap_traits<Tp> ) {
                        return +[](const storage_u& self) -> uvalue {
                            return detail::unmap_traits<Tp>{}(*storage_cast<Tp>(self));
                        };
                    } else {
                        return nullptr;
                    }
                }()},
            };

            return &table;
        }
    };
}

namespace meta_hpp
{
    inline uvalue::~uvalue() {
        reset();
    }

    inline uvalue::uvalue(uvalue&& other) noexcept {
        vtable_t::do_move(std::move(other), *this);
    }

    inline uvalue::uvalue(const uvalue& other) {
        vtable_t::do_copy(other, *this);
    }

    inline uvalue& uvalue::operator=(uvalue&& other) noexcept {
        if ( this != &other ) {
            vtable_t::do_reset(*this);
            vtable_t::do_move(std::move(other), *this);
        }
        return *this;
    }

    inline uvalue& uvalue::operator=(const uvalue& other) {
        if ( this != &other ) {
            vtable_t::do_reset(*this);
            vtable_t::do_copy(other, *this);
        }
        return *this;
    }

    template < typename T, typename Tp >
        requires(!detail::any_uvalue_kind<Tp>)     //
             && (!detail::is_in_place_type_v<Tp>)  //
             && (std::is_copy_constructible_v<Tp>) //
    // NOLINTNEXTLINE(*-forwarding-reference-overload)
    uvalue::uvalue(T&& val) {
        vtable_t::do_ctor<T>(*this, std::forward<T>(val));
    }

    template < typename T, typename Tp >
        requires(!detail::any_uvalue_kind<Tp>)     //
             && (!detail::is_in_place_type_v<Tp>)  //
             && (std::is_copy_constructible_v<Tp>) //
    uvalue& uvalue::operator=(T&& val) {
        vtable_t::do_reset(*this);
        vtable_t::do_ctor<T>(*this, std::forward<T>(val));
        return *this;
    }

    template < typename T, typename... Args, typename Tp >
        requires std::is_copy_constructible_v<Tp> //
              && std::is_constructible_v<Tp, Args...>
    uvalue::uvalue(std::in_place_type_t<T>, Args&&... args) {
        vtable_t::do_ctor<T>(*this, std::forward<Args>(args)...);
    }

    template < typename T, typename U, typename... Args, typename Tp >
        requires std::is_copy_constructible_v<Tp> //
              && std::is_constructible_v<Tp, std::initializer_list<U>&, Args...>
    uvalue::uvalue(std::in_place_type_t<T>, std::initializer_list<U> ilist, Args&&... args) {
        vtable_t::do_ctor<T>(*this, ilist, std::forward<Args>(args)...);
    }

    template < typename T, typename... Args, typename Tp >
        requires std::is_copy_constructible_v<Tp> //
              && std::is_constructible_v<Tp, Args...>
    Tp& uvalue::emplace(Args&&... args) {
        vtable_t::do_reset(*this);
        return vtable_t::do_ctor<T>(*this, std::forward<Args>(args)...);
    }

    template < typename T, typename U, typename... Args, typename Tp >
        requires std::is_copy_constructible_v<Tp> //
              && std::is_constructible_v<Tp, std::initializer_list<U>&, Args...>
    Tp& uvalue::emplace(std::initializer_list<U> ilist, Args&&... args) {
        vtable_t::do_reset(*this);
        return vtable_t::do_ctor<T>(*this, ilist, std::forward<Args>(args)...);
    }

    inline bool uvalue::is_valid() const noexcept {
        return storage_.vtag != 0;
    }

    inline uvalue::operator bool() const noexcept {
        return is_valid();
    }

    inline void uvalue::reset() {
        vtable_t::do_reset(*this);
    }

    inline void uvalue::swap(uvalue& other) noexcept {
        vtable_t::do_swap(*this, other);
    }

    inline any_type uvalue::get_type() const noexcept {
        auto&& [tag, vtable] = vtable_t::unpack_vtag(*this);
        return tag == storage_e::nothing ? any_type{} : vtable->type;
    }

    inline void* uvalue::get_data() noexcept {
        switch ( vtable_t::unpack_vtag(*this).first ) {
        case storage_e::nothing:
            return nullptr;
        case storage_e::trivial:
        case storage_e::internal:
            // NOLINTNEXTLINE(*-union-access)
            return static_cast<void*>(storage_.internal.data);
        case storage_e::external:
            // NOLINTNEXTLINE(*-union-access)
            return storage_.external.ptr;
        }

        META_HPP_ASSERT(false);
        return nullptr;
    }

    inline const void* uvalue::get_data() const noexcept {
        switch ( vtable_t::unpack_vtag(*this).first ) {
        case storage_e::nothing:
            return nullptr;
        case storage_e::trivial:
        case storage_e::internal:
            // NOLINTNEXTLINE(*-union-access)
            return static_cast<const void*>(storage_.internal.data);
        case storage_e::external:
            // NOLINTNEXTLINE(*-union-access)
            return storage_.external.ptr;
        }

        META_HPP_ASSERT(false);
        return nullptr;
    }

    inline const void* uvalue::get_cdata() const noexcept {
        switch ( vtable_t::unpack_vtag(*this).first ) {
        case storage_e::nothing:
            return nullptr;
        case storage_e::trivial:
        case storage_e::internal:
            // NOLINTNEXTLINE(*-union-access)
            return static_cast<const void*>(storage_.internal.data);
        case storage_e::external:
            // NOLINTNEXTLINE(*-union-access)
            return storage_.external.ptr;
        }

        META_HPP_ASSERT(false);
        return nullptr;
    }

    inline uvalue uvalue::operator*() const {
        auto&& [tag, vtable] = vtable_t::unpack_vtag(*this);
        return tag != storage_e::nothing && vtable->deref != nullptr //
                 ? vtable->deref(storage_)
                 : uvalue{};
    }

    inline bool uvalue::has_deref_op() const noexcept {
        auto&& [tag, vtable] = vtable_t::unpack_vtag(*this);
        return tag != storage_e::nothing && vtable->deref != nullptr;
    }

    inline uvalue uvalue::operator[](std::size_t index) const {
        auto&& [tag, vtable] = vtable_t::unpack_vtag(*this);
        return tag != storage_e::nothing && vtable->index != nullptr //
                 ? vtable->index(storage_, index)
                 : uvalue{};
    }

    inline bool uvalue::has_index_op() const noexcept {
        auto&& [tag, vtable] = vtable_t::unpack_vtag(*this);
        return tag != storage_e::nothing && vtable->index != nullptr;
    }

    inline uvalue uvalue::unmap() const {
        auto&& [tag, vtable] = vtable_t::unpack_vtag(*this);
        return tag != storage_e::nothing && vtable->unmap != nullptr //
                 ? vtable->unmap(storage_)
                 : uvalue{};
    }

    inline bool uvalue::has_unmap_op() const noexcept {
        auto&& [tag, vtable] = vtable_t::unpack_vtag(*this);
        return tag != storage_e::nothing && vtable->unmap != nullptr;
    }

    template < typename T >
    T uvalue::get_as() && {
        static_assert(std::is_same_v<T, std::decay_t<T>>);

        if constexpr ( detail::pointer_kind<T> ) {
            if ( T ptr = try_get_as<T>(); ptr || get_type().is_nullptr() ) {
                return ptr;
            }
        } else {
            if ( T* ptr = try_get_as<T>() ) {
                return std::move(*ptr);
            }
        }

        META_HPP_THROW("bad value cast");
    }

    template < typename T >
    auto uvalue::get_as() & -> std::conditional_t<detail::pointer_kind<T>, T, T&> {
        static_assert(std::is_same_v<T, std::decay_t<T>>);

        if constexpr ( detail::pointer_kind<T> ) {
            if ( T ptr = try_get_as<T>(); ptr || get_type().is_nullptr() ) {
                return ptr;
            }
        } else {
            if ( T* ptr = try_get_as<T>() ) {
                return *ptr;
            }
        }

        META_HPP_THROW("bad value cast");
    }

    template < typename T >
    auto uvalue::get_as() const& -> std::conditional_t<detail::pointer_kind<T>, T, const T&> {
        static_assert(std::is_same_v<T, std::decay_t<T>>);

        if constexpr ( detail::pointer_kind<T> ) {
            if ( T ptr = try_get_as<T>(); ptr || get_type().is_nullptr() ) {
                return ptr;
            }
        } else {
            if ( const T* ptr = try_get_as<T>() ) {
                return *ptr;
            }
        }

        META_HPP_THROW("bad value cast");
    }

    template < typename T >
    // NOLINTNEXTLINE(*-cognitive-complexity)
    auto uvalue::try_get_as() noexcept -> std::conditional_t<detail::pointer_kind<T>, T, T*> {
        static_assert(std::is_same_v<T, std::decay_t<T>>);

        using detail::type_registry;
        type_registry& registry{type_registry::instance()};

        const any_type& from_type = get_type();
        const any_type& to_type = registry.resolve_type<T>();

        const auto is_a = [](const any_type& base, const any_type& derived) {
            return (base == derived) //
                || (base.is_class() && derived.is_class() && base.as_class().is_base_of(derived.as_class()));
        };

        if constexpr ( detail::pointer_kind<T> ) {
            if ( to_type.is_pointer() && from_type.is_nullptr() ) {
                return static_cast<T>(nullptr);
            }

            if ( to_type.is_pointer() && from_type.is_pointer() ) {
                const pointer_type& to_type_ptr = to_type.as_pointer();
                const bool to_type_ptr_readonly = to_type_ptr.get_flags().has(pointer_flags::is_readonly);

                const pointer_type& from_type_ptr = from_type.as_pointer();
                const bool from_type_ptr_readonly = from_type_ptr.get_flags().has(pointer_flags::is_readonly);

                const any_type& to_data_type = to_type_ptr.get_data_type();
                const any_type& from_data_type = from_type_ptr.get_data_type();

                if ( to_type_ptr_readonly >= from_type_ptr_readonly ) {
                    void** from_data_ptr = static_cast<void**>(get_data());

                    if ( to_data_type.is_void() || to_data_type == from_data_type ) {
                        void* to_ptr = *from_data_ptr;
                        return static_cast<T>(to_ptr);
                    }

                    if ( is_a(to_data_type, from_data_type) ) {
                        const class_type& to_data_class = to_data_type.as_class();
                        const class_type& from_data_class = from_data_type.as_class();

                        void* to_ptr = detail::pointer_upcast(registry, *from_data_ptr, from_data_class, to_data_class);
                        return static_cast<T>(to_ptr);
                    }
                }
            }
        }

        if constexpr ( !detail::pointer_kind<T> ) {
            if ( from_type == to_type ) {
                T* to_ptr = static_cast<T*>(get_data());
                return to_ptr;
            }

            if ( is_a(to_type, from_type) ) {
                const class_type& to_class = to_type.as_class();
                const class_type& from_class = from_type.as_class();

                T* to_ptr = static_cast<T*>(detail::pointer_upcast(registry, get_data(), from_class, to_class));
                return to_ptr;
            }
        }

        return nullptr;
    }

    template < typename T >
    // NOLINTNEXTLINE(*-cognitive-complexity)
    auto uvalue::try_get_as() const noexcept -> std::conditional_t<detail::pointer_kind<T>, T, const T*> {
        static_assert(std::is_same_v<T, std::decay_t<T>>);

        using detail::type_registry;
        type_registry& registry{type_registry::instance()};

        const any_type& from_type = get_type();
        const any_type& to_type = registry.resolve_type<T>();

        const auto is_a = [](const any_type& base, const any_type& derived) {
            return (base == derived) //
                || (base.is_class() && derived.is_class() && base.as_class().is_base_of(derived.as_class()));
        };

        if constexpr ( detail::pointer_kind<T> ) {
            if ( to_type.is_pointer() && from_type.is_nullptr() ) {
                return static_cast<T>(nullptr);
            }

            if ( to_type.is_pointer() && from_type.is_pointer() ) {
                const pointer_type& to_type_ptr = to_type.as_pointer();
                const bool to_type_ptr_readonly = to_type_ptr.get_flags().has(pointer_flags::is_readonly);

                const pointer_type& from_type_ptr = from_type.as_pointer();
                const bool from_type_ptr_readonly = from_type_ptr.get_flags().has(pointer_flags::is_readonly);

                const any_type& to_data_type = to_type_ptr.get_data_type();
                const any_type& from_data_type = from_type_ptr.get_data_type();

                if ( to_type_ptr_readonly >= from_type_ptr_readonly ) {
                    void* const* from_data_ptr = static_cast<void* const*>(get_data());

                    if ( to_data_type.is_void() || to_data_type == from_data_type ) {
                        void* to_ptr = *from_data_ptr;
                        return static_cast<T>(to_ptr);
                    }

                    if ( is_a(to_data_type, from_data_type) ) {
                        const class_type& to_data_class = to_data_type.as_class();
                        const class_type& from_data_class = from_data_type.as_class();

                        void* to_ptr = detail::pointer_upcast(registry, *from_data_ptr, from_data_class, to_data_class);
                        return static_cast<T>(to_ptr);
                    }
                }
            }
        }

        if constexpr ( !detail::pointer_kind<T> ) {
            if ( from_type == to_type ) {
                const T* to_ptr = static_cast<const T*>(get_data());
                return to_ptr;
            }

            if ( is_a(to_type, from_type) ) {
                const class_type& to_class = to_type.as_class();
                const class_type& from_class = from_type.as_class();

                const T* to_ptr = static_cast<const T*>(detail::pointer_upcast(registry, get_data(), from_class, to_class));
                return to_ptr;
            }
        }

        return nullptr;
    }

    template < typename T >
    std::optional<T> uvalue::safe_get_as() && {
        static_assert(std::is_same_v<T, std::decay_t<T>>);

        if constexpr ( detail::pointer_kind<T> ) {
            if ( T ptr = try_get_as<T>(); ptr || get_type().is_nullptr() ) {
                return ptr;
            }
        } else {
            if ( T* ptr = try_get_as<T>() ) {
                return std::move(*ptr);
            }
        }

        return std::nullopt;
    }

    template < typename T >
    std::optional<T> uvalue::safe_get_as() & {
        static_assert(std::is_same_v<T, std::decay_t<T>>);

        if constexpr ( detail::pointer_kind<T> ) {
            if ( T ptr = try_get_as<T>(); ptr || get_type().is_nullptr() ) {
                return ptr;
            }
        } else {
            if ( T* ptr = try_get_as<T>() ) {
                return *ptr;
            }
        }

        return std::nullopt;
    }

    template < typename T >
    std::optional<T> uvalue::safe_get_as() const& {
        static_assert(std::is_same_v<T, std::decay_t<T>>);

        if constexpr ( detail::pointer_kind<T> ) {
            if ( T ptr = try_get_as<T>(); ptr || get_type().is_nullptr() ) {
                return ptr;
            }
        } else {
            if ( const T* ptr = try_get_as<T>() ) {
                return *ptr;
            }
        }

        return std::nullopt;
    }
}
