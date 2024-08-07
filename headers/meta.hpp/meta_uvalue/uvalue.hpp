/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_registry.hpp"
#include "../meta_uvalue.hpp"

#include "../meta_detail/value_traits/copy_traits.hpp"
#include "../meta_detail/value_traits/deref_traits.hpp"
#include "../meta_detail/value_traits/equals_traits.hpp"
#include "../meta_detail/value_traits/index_traits.hpp"
#include "../meta_detail/value_traits/less_traits.hpp"
#include "../meta_detail/value_traits/unmap_traits.hpp"

#include "../meta_detail/value_utilities/uarg.hpp"
#include "../meta_detail/value_utilities/utraits.hpp"

namespace meta_hpp
{
    struct uvalue::vtable_t final {
        // NOLINTBEGIN(*-avoid-const-or-ref-data-members)
        const any_type type;

        void (*const move)(uvalue&& self, uvalue& to) noexcept;
        void (*const reset)(uvalue& self) noexcept;

        uvalue (*const copy)(const storage_u& self);
        uvalue (*const deref)(const storage_u& self);
        uvalue (*const index)(const storage_u& self, std::size_t i);
        uvalue (*const unmap)(const storage_u& self);

        bool (*const less)(const storage_u& l, const storage_u& r);
        bool (*const equals)(const storage_u& l, const storage_u& r);
        // NOLINTEND(*-avoid-const-or-ref-data-members)

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
            META_HPP_DEV_ASSERT(!dst);

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
            META_HPP_DEV_ASSERT(!to);

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
                    uvalue o;
                    do_move(std::move(l), o);
                    do_move(std::move(r), l);
                    do_move(std::move(o), r);
                } else {
                    uvalue o;
                    do_move(std::move(r), o);
                    do_move(std::move(l), r);
                    do_move(std::move(o), l);
                }
            } else {
                if ( l ) {
                    do_move(std::move(l), r);
                } else {
                    do_move(std::move(r), l);
                }
            }
        }

        template < typename Tp >
        // NOLINTNEXTLINE(*-cognitive-complexity)
        static vtable_t* get() {
            static_assert(std::is_same_v<Tp, std::decay_t<Tp>>);

            static vtable_t table{
                .type = resolve_type<Tp>(),

                // NOLINTNEXTLINE(*-param-not-moved)
                .move{[](uvalue&& self, uvalue& to) noexcept {
                    META_HPP_DEV_ASSERT(!to);
                    META_HPP_DEV_ASSERT(self);

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

                .reset{[](uvalue& self) noexcept {
                    META_HPP_DEV_ASSERT(self);

                    Tp* src = storage_cast<Tp>(self.storage_);

                    if constexpr ( in_internal_v<Tp> ) {
                        std::destroy_at(src);
                    } else {
                        // NOLINTNEXTLINE(*-owning-memory)
                        delete src;
                    }

                    self.storage_.vtag = 0;
                }},

                .copy{[]() {
                    if constexpr ( detail::has_copy_traits<Tp> ) {
                        return +[](const storage_u& self) -> uvalue {
                            return detail::copy_traits<Tp>{}(*storage_cast<Tp>(self));
                        };
                    } else {
                        return nullptr;
                    }
                }()},

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

                .less{[]() {
                    if constexpr ( detail::has_less_traits<Tp> ) {
                        return +[](const storage_u& l, const storage_u& r) -> bool {
                            return detail::less_traits<Tp>{}(*storage_cast<Tp>(l), *storage_cast<Tp>(r));
                        };
                    } else {
                        return nullptr;
                    }
                }()},

                .equals{[]() {
                    if constexpr ( detail::has_equals_traits<Tp> ) {
                        return +[](const storage_u& l, const storage_u& r) -> bool {
                            return detail::equals_traits<Tp>{}(*storage_cast<Tp>(l), *storage_cast<Tp>(r));
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
    inline uvalue::~uvalue() noexcept {
        reset();
    }

    inline uvalue::uvalue(uvalue&& other) noexcept {
        vtable_t::do_move(std::move(other), *this);
    }

    inline uvalue& uvalue::operator=(uvalue&& other) noexcept {
        if ( this != &other ) {
            uvalue{std::move(other)}.swap(*this);
        }
        return *this;
    }

    template < typename T, typename >
    uvalue::uvalue(T&& val) {
        vtable_t::do_ctor<T>(*this, std::forward<T>(val));
    }

    template < typename T, typename >
    uvalue& uvalue::operator=(T&& val) {
        uvalue{std::forward<T>(val)}.swap(*this);
        return *this;
    }

    template < typename T, typename... Args >
        requires std::is_constructible_v<std::decay_t<T>, Args...>
    uvalue::uvalue(std::in_place_type_t<T>, Args&&... args) {
        vtable_t::do_ctor<T>(*this, std::forward<Args>(args)...);
    }

    template < typename T, typename U, typename... Args >
        requires std::is_constructible_v<std::decay_t<T>, std::initializer_list<U>&, Args...>
    uvalue::uvalue(std::in_place_type_t<T>, std::initializer_list<U> ilist, Args&&... args) {
        vtable_t::do_ctor<T>(*this, ilist, std::forward<Args>(args)...);
    }

    template < typename T, typename... Args >
        requires std::is_constructible_v<std::decay_t<T>, Args...>
    std::decay_t<T>& uvalue::emplace(Args&&... args) {
        vtable_t::do_reset(*this);
        return vtable_t::do_ctor<T>(*this, std::forward<Args>(args)...);
    }

    template < typename T, typename U, typename... Args >
        requires std::is_constructible_v<std::decay_t<T>, std::initializer_list<U>&, Args...>
    std::decay_t<T>& uvalue::emplace(std::initializer_list<U> ilist, Args&&... args) {
        vtable_t::do_reset(*this);
        return vtable_t::do_ctor<T>(*this, ilist, std::forward<Args>(args)...);
    }

    inline bool uvalue::has_value() const noexcept {
        return storage_.vtag != 0;
    }

    inline uvalue::operator bool() const noexcept {
        return has_value();
    }

    inline void uvalue::reset() noexcept {
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

        if ( tag != storage_e::nothing && vtable->deref != nullptr ) {
            return vtable->deref(storage_);
        }

        throw_exception(error_code::bad_uvalue_operation);
    }

    inline bool uvalue::has_deref_op() const noexcept {
        auto&& [tag, vtable] = vtable_t::unpack_vtag(*this);
        return tag != storage_e::nothing && vtable->deref != nullptr;
    }

    inline uvalue uvalue::operator[](std::size_t index) const {
        auto&& [tag, vtable] = vtable_t::unpack_vtag(*this);

        if ( tag != storage_e::nothing && vtable->index != nullptr ) {
            return vtable->index(storage_, index);
        }

        throw_exception(error_code::bad_uvalue_operation);
    }

    inline bool uvalue::has_index_op() const noexcept {
        auto&& [tag, vtable] = vtable_t::unpack_vtag(*this);
        return tag != storage_e::nothing && vtable->index != nullptr;
    }

    inline uvalue uvalue::copy() const {
        auto&& [tag, vtable] = vtable_t::unpack_vtag(*this);

        if ( tag == storage_e::nothing ) {
            return uvalue{};
        }

        if ( vtable->copy != nullptr ) {
            return vtable->copy(storage_);
        }

        throw_exception(error_code::bad_uvalue_operation);
    }

    inline bool uvalue::has_copy_op() const noexcept {
        auto&& [tag, vtable] = vtable_t::unpack_vtag(*this);
        return tag == storage_e::nothing || vtable->copy != nullptr;
    }

    inline uvalue uvalue::unmap() const {
        auto&& [tag, vtable] = vtable_t::unpack_vtag(*this);

        if ( tag != storage_e::nothing && vtable->unmap != nullptr ) {
            return vtable->unmap(storage_);
        }

        throw_exception(error_code::bad_uvalue_operation);
    }

    inline bool uvalue::has_unmap_op() const noexcept {
        auto&& [tag, vtable] = vtable_t::unpack_vtag(*this);
        return tag != storage_e::nothing && vtable->unmap != nullptr;
    }

    inline bool uvalue::less(const uvalue& other) const {
        if ( this == &other ) {
            return false;
        }

        auto&& [l_tag, l_vtable] = vtable_t::unpack_vtag(*this);
        auto&& [r_tag, r_vtable] = vtable_t::unpack_vtag(other);

        if ( l_tag != r_tag || l_tag == storage_e::nothing ) {
            return l_tag < r_tag;
        }

        if ( l_vtable != r_vtable ) {
            return l_vtable->type < r_vtable->type;
        }

        if ( l_vtable->less != nullptr ) {
            return l_vtable->less(storage_, other.storage_);
        }

        throw_exception(error_code::bad_uvalue_operation);
    }

    inline bool uvalue::has_less_op() const noexcept {
        auto&& [tag, vtable] = vtable_t::unpack_vtag(*this);
        return tag == storage_e::nothing || vtable->less != nullptr;
    }

    inline bool uvalue::equals(const uvalue& other) const {
        if ( this == &other ) {
            return true;
        }

        auto&& [l_tag, l_vtable] = vtable_t::unpack_vtag(*this);
        auto&& [r_tag, r_vtable] = vtable_t::unpack_vtag(other);

        if ( l_tag != r_tag || l_tag == storage_e::nothing ) {
            return l_tag == r_tag;
        }

        if ( l_vtable != r_vtable ) {
            return l_vtable->type == r_vtable->type;
        }

        if ( l_vtable->equals != nullptr ) {
            return l_vtable->equals(storage_, other.storage_);
        }

        throw_exception(error_code::bad_uvalue_operation);
    }

    inline bool uvalue::has_equals_op() const noexcept {
        auto&& [tag, vtable] = vtable_t::unpack_vtag(*this);
        return tag == storage_e::nothing || vtable->equals != nullptr;
    }

    template < typename T >
    bool uvalue::is() const noexcept {
        static_assert(std::is_same_v<T, std::decay_t<T>>);
        return detail::is_a(resolve_type<T>(), get_type());
    }

    template < any_pointer_family T >
    T uvalue::as() {
        static_assert(std::is_same_v<T, std::decay_t<T>>);

        if ( T ptr = try_as<T>(); ptr || get_type().is_nullptr() ) {
            return ptr;
        }

        throw_exception(error_code::bad_uvalue_access);
    }

    template < any_pointer_family T >
    T uvalue::as() const {
        static_assert(std::is_same_v<T, std::decay_t<T>>);

        if ( T ptr = try_as<T>(); ptr || get_type().is_nullptr() ) {
            return ptr;
        }

        throw_exception(error_code::bad_uvalue_access);
    }

    template < not_any_pointer_family T >
    T& uvalue::as() & {
        static_assert(std::is_same_v<T, std::decay_t<T>>);

        if ( T* ptr = try_as<T>() ) {
            return *ptr;
        }

        throw_exception(error_code::bad_uvalue_access);
    }

    template < not_any_pointer_family T >
    const T& uvalue::as() const& {
        static_assert(std::is_same_v<T, std::decay_t<T>>);

        if ( const T* ptr = try_as<T>() ) {
            return *ptr;
        }

        throw_exception(error_code::bad_uvalue_access);
    }

    template < not_any_pointer_family T >
    T uvalue::as() && {
        static_assert(std::is_same_v<T, std::decay_t<T>>);

        if ( T* ptr = try_as<T>() ) {
            return std::move(*ptr);
        }

        throw_exception(error_code::bad_uvalue_access);
    }

    template < not_any_pointer_family T >
    const T&& uvalue::as() const&& {
        static_assert(std::is_same_v<T, std::decay_t<T>>);

        if ( const T* ptr = try_as<T>() ) {
            return std::move(*ptr);
        }

        throw_exception(error_code::bad_uvalue_access);
    }

    template < any_pointer_family T >
    T uvalue::try_as() noexcept {
        static_assert(std::is_same_v<T, std::decay_t<T>>);

        using namespace detail;
        type_registry& registry{type_registry::instance()};

        if ( const uarg varg{registry, *this}; varg.can_cast_to<T>(registry) ) {
            return varg.cast<T>(registry);
        }

        return nullptr;
    }

    template < any_pointer_family T >
    T uvalue::try_as() const noexcept {
        static_assert(std::is_same_v<T, std::decay_t<T>>);

        using namespace detail;
        type_registry& registry{type_registry::instance()};

        if ( const uarg varg{registry, *this}; varg.can_cast_to<T>(registry) ) {
            return varg.cast<T>(registry);
        }

        return nullptr;
    }

    template < not_any_pointer_family T >
    T* uvalue::try_as() & noexcept {
        static_assert(std::is_same_v<T, std::decay_t<T>>);

        using namespace detail;
        type_registry& registry{type_registry::instance()};

        if ( const uarg varg{registry, *this}; varg.can_cast_to<T&>(registry) ) {
            return std::addressof(varg.cast<T&>(registry));
        }

        return nullptr;
    }

    template < not_any_pointer_family T >
    const T* uvalue::try_as() const& noexcept {
        static_assert(std::is_same_v<T, std::decay_t<T>>);

        using namespace detail;
        type_registry& registry{type_registry::instance()};

        if ( const uarg varg{registry, *this}; varg.can_cast_to<const T&>(registry) ) {
            return std::addressof(varg.cast<const T&>(registry));
        }

        return nullptr;
    }
}
