/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_utilities.hpp"

#include "value_traits/deref_traits.hpp"
#include "value_traits/equals_traits.hpp"
#include "value_traits/index_traits.hpp"
#include "value_traits/istream_traits.hpp"
#include "value_traits/less_traits.hpp"
#include "value_traits/ostream_traits.hpp"

namespace meta_hpp
{
    struct value::traits final {
        const any_type type;

        void* (*const data)(value&) noexcept;
        const void* (*const cdata)(const value&) noexcept;

        value (*const deref)(const value&);
        value (*const index)(const value&, std::size_t);

        bool (*const less)(const value&, const value&);
        bool (*const equals)(const value&, const value&);

        void (*const move_ctor)(std::any&, value&&);
        void (*const copy_ctor)(std::any&, const value&);

        std::istream& (*const istream)(std::istream&, value&);
        std::ostream& (*const ostream)(std::ostream&, const value&);

        template < typename T >
        static const traits* get() noexcept;
    };

    template < typename T >
    const value::traits* value::traits::get() noexcept {
        static_assert(std::is_same_v<T, std::decay_t<T>>);

        static const traits traits{
            .type = resolve_type<T>(),

            .data = +[](value& v) noexcept -> void* {
                return v.try_cast<T>();
            },

            .cdata = +[](const value& v) noexcept -> const void* {
                return v.try_cast<T>();
            },

            .deref = +[]([[maybe_unused]] const value& v) -> value {
                if constexpr ( detail::has_value_deref_traits<T> ) {
                    return detail::value_deref_traits<T>{}(v.cast<T>());
                } else {
                    throw std::logic_error("value type doesn't have value deref traits");
                }
            },

            .index = +[]([[maybe_unused]] const value& v, [[maybe_unused]] std::size_t index) -> value {
                if constexpr ( detail::has_value_index_traits<T> ) {
                    return detail::value_index_traits<T>{}(v.cast<T>(), index);
                } else {
                    throw std::logic_error("value type doesn't have value index traits");
                }
            },

            .less = +[]([[maybe_unused]] const value& l, [[maybe_unused]] const value& r) -> bool {
                if constexpr ( detail::has_value_less_traits<T> ) {
                    return detail::value_less_traits<T>{}(l.cast<T>(), r.cast<T>());
                } else {
                    throw std::logic_error("value type doesn't have value less traits");
                }
            },

            .equals = +[]([[maybe_unused]] const value& l, [[maybe_unused]] const value& r) -> bool {
                if constexpr ( detail::has_value_equals_traits<T> ) {
                    return detail::value_equals_traits<T>{}(l.cast<T>(), r.cast<T>());
                } else {
                    throw std::logic_error("value type doesn't have value equals traits");
                }
            },

            .move_ctor = +[]([[maybe_unused]] std::any& dst, [[maybe_unused]] value&& src) {
                if constexpr ( std::is_move_constructible_v<T> ) {
                    dst.emplace<T>(std::move(src).cast<T>());
                } else {
                    throw std::logic_error("value type is not move constructible");
                }
            },

            .copy_ctor = +[]([[maybe_unused]] std::any& dst, [[maybe_unused]] const value& src) {
                if constexpr ( std::is_copy_constructible_v<T> ) {
                    dst.emplace<T>(src.cast<T>());
                } else {
                    throw std::logic_error("value type is not copy constructible");
                }
            },

            .istream = +[]([[maybe_unused]] std::istream& is, [[maybe_unused]] value& v) -> std::istream& {
                if constexpr ( detail::has_value_istream_traits<T> ) {
                    return detail::value_istream_traits<T>{}(is, v.cast<T>());
                } else {
                    throw std::logic_error("value type doesn't have value istream traits");
                }
            },

            .ostream = +[]([[maybe_unused]] std::ostream& os, [[maybe_unused]] const value& v) -> std::ostream& {
                if constexpr ( detail::has_value_ostream_traits<T> ) {
                    return detail::value_ostream_traits<T>{}(os, v.cast<T>());
                } else {
                    throw std::logic_error("value type doesn't have value ostream traits");
                }
            },
        };

        return &traits;
    }
}

namespace meta_hpp
{
    // NOLINTNEXTLINE(performance-noexcept-move-constructor)
    inline value::value(value&& other)
    : traits_{other.traits_} {
        traits_->move_ctor(raw_, std::move(other));
    }

    inline value::value(const value& other)
    : traits_{other.traits_} {
        traits_->copy_ctor(raw_, other);
    }

    // NOLINTNEXTLINE(performance-noexcept-move-constructor)
    inline value& value::operator=(value&& other) {
        if ( this != &other ) {
            traits_ = other.traits_;
            traits_->move_ctor(raw_, std::move(other));
        }
        return *this;
    }

    inline value& value::operator=(const value& other) {
        if ( this != &other ) {
            traits_ = other.traits_;
            traits_->copy_ctor(raw_, other);
        }
        return *this;
    }

    template < detail::decay_non_uvalue_kind T, typename Tp >
        requires detail::stdex::copy_constructible<Tp>
    value::value(T&& val)
    : raw_{std::forward<T>(val)}
    , traits_{traits::get<std::decay_t<T>>()} {}

    template < detail::decay_non_uvalue_kind T, typename Tp >
        requires detail::stdex::copy_constructible<Tp>
    value& value::operator=(T&& val) {
        value temp{std::forward<T>(val)};
        swap(temp);
        return *this;
    }

    inline void value::swap(value& other) noexcept {
        using std::swap;
        swap(raw_, other.raw_);
        swap(traits_, other.traits_);
    }

    inline const any_type& value::get_type() const noexcept {
        return traits_->type;
    }

    inline void* value::data() noexcept {
        return traits_->data(*this);
    }

    inline const void* value::data() const noexcept {
        return traits_->cdata(*this);
    }

    inline const void* value::cdata() const noexcept {
        return traits_->cdata(*this);
    }

    inline value value::operator*() const {
        return traits_->deref(*this);
    }

    inline value value::operator[](std::size_t index) const {
        return traits_->index(*this, index);
    }

    template < typename T, typename Tp >
    Tp& value::cast() & {
        if ( get_type() != resolve_type<Tp>() ) {
            throw std::logic_error("bad value cast");
        }
        return std::any_cast<Tp&>(raw_);
    }

    template < typename T, typename Tp >
    Tp&& value::cast() && {
        if ( get_type() != resolve_type<Tp>() ) {
            throw std::logic_error("bad value cast");
        }
        return std::move(std::any_cast<Tp&>(raw_));
    }

    template < typename T, typename Tp >
    const Tp& value::cast() const & {
        if ( get_type() != resolve_type<Tp>() ) {
            throw std::logic_error("bad value cast");
        }
        return std::any_cast<const Tp&>(raw_);
    }

    template < typename T, typename Tp >
    const Tp&& value::cast() const && {
        if ( get_type() != resolve_type<Tp>() ) {
            throw std::logic_error("bad value cast");
        }
        return std::move(std::any_cast<const Tp&>(raw_));
    }

    template < typename T, typename Tp >
    Tp* value::try_cast() noexcept {
        return std::any_cast<Tp>(&raw_);
    }

    template < typename T, typename Tp >
    const Tp* value::try_cast() const noexcept {
        return std::any_cast<Tp>(&raw_);
    }
}

namespace meta_hpp
{
    template < typename T >
    [[nodiscard]] bool operator<(const value& l, const T& r) {
        const any_type& r_type = resolve_type<T>();
        return (l.get_type() < r_type)
            || (l.get_type() == r_type && std::less<>{}(l.cast<T>(), r));
    }

    template < typename T >
    [[nodiscard]] bool operator<(const T& l, const value& r) {
        const any_type& l_type = resolve_type<T>();
        return (l_type < r.get_type())
            || (l_type == r.get_type() && std::less<>{}(l, r.cast<T>()));
    }

    [[nodiscard]] inline bool operator<(const value& l, const value& r) {
        return (l.get_type() < r.get_type())
            || (l.get_type() == r.get_type() && l.traits_->less(l, r));
    }
}

namespace meta_hpp
{
    template < typename T >
    [[nodiscard]] bool operator==(const value& l, const T& r) {
        return l.get_type() == resolve_type<T>()
            && std::equal_to<>{}(l.cast<T>(), r);
    }

    template < typename T >
    [[nodiscard]] bool operator==(const T& l, const value& r) {
        return resolve_type<T>() == r.get_type()
            && std::equal_to<>{}(l, r.cast<T>());
    }

    [[nodiscard]] inline bool operator==(const value& l, const value& r) {
        return l.get_type() == r.get_type()
            && l.traits_->equals(l, r);
    }
}

namespace meta_hpp
{
    inline std::istream& operator>>(std::istream& is, value& v) {
        return v.traits_->istream(is, v);
    }

    inline std::ostream& operator<<(std::ostream& os, const value& v) {
        return v.traits_->ostream(os, v);
    }
}
