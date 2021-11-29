/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_utilities.hpp"

namespace meta_hpp::detail
{
    template < typename T, typename = void >
    struct has_value_type_less_op : std::false_type {};

    template < typename T >
    struct has_value_type_less_op<T, std::void_t<decltype(
        std::declval<const T&>() < std::declval<const T&>()
    )>> : std::true_type {};

    template < typename T >
    concept has_value_type_less_op_kind = has_value_type_less_op<T>::value;

    template < typename T >
    concept has_not_value_type_less_op_kind = !has_value_type_less_op<T>::value;

    template < has_value_type_less_op_kind T >
    bool value_less_function(const value& l, const value& r) {
        assert(l.get_type() == r.get_type());
        return l.cast<T>() < r.cast<T>();
    }

    template < has_not_value_type_less_op_kind T >
    bool value_less_function([[maybe_unused]] const value& l, [[maybe_unused]] const value& r) {
        throw std::logic_error("value type doesn't have less operator");
    }
}

namespace meta_hpp::detail
{
    template < typename T, typename = void >
    struct has_value_type_equals_op : std::false_type {};

    template < typename T >
    struct has_value_type_equals_op<T, std::void_t<decltype(
        std::declval<const T&>() == std::declval<const T&>()
    )>> : std::true_type {};

    template < typename T >
    concept has_value_type_equals_op_kind = has_value_type_equals_op<T>::value;

    template < typename T >
    concept has_not_value_type_equals_op_kind = !has_value_type_equals_op<T>::value;

    template < has_value_type_equals_op_kind T >
    bool value_equals_function(const value& l, const value& r) {
        assert(l.get_type() == r.get_type());
        return l.cast<T>() == r.cast<T>();
    }

    template < has_not_value_type_equals_op_kind T >
    bool value_equals_function([[maybe_unused]] const value& l, [[maybe_unused]] const value& r) {
        throw std::logic_error("value type doesn't have equality operator");
    }
}

namespace meta_hpp::detail
{
    template < typename T, typename = void >
    struct has_value_type_istream_op : std::false_type {};

    template < typename T >
    struct has_value_type_istream_op<T, std::void_t<decltype(
        std::declval<std::istream&>() >> std::declval<T&>()
    )>> : std::true_type {};

    template < typename T >
    concept has_value_type_istream_op_kind = has_value_type_istream_op<T>::value;

    template < typename T >
    concept has_not_value_type_istream_op_kind = !has_value_type_istream_op<T>::value;

    template < has_value_type_istream_op_kind T >
    void value_istream_function(std::istream& os, value& v) {
        os >> v.cast<T>();
    }

    template < has_not_value_type_istream_op_kind T >
    void value_istream_function([[maybe_unused]] std::istream& os, [[maybe_unused]] value& v) {
        throw std::logic_error("value type doesn't have istream operator");
    }
}

namespace meta_hpp::detail
{
    template < typename T, typename = void >
    struct has_value_type_ostream_op : std::false_type {};

    template < typename T >
    struct has_value_type_ostream_op<T, std::void_t<decltype(
        std::declval<std::ostream&>() << std::declval<const T&>()
    )>> : std::true_type {};

    template < typename T >
    concept has_value_type_ostream_op_kind = has_value_type_ostream_op<T>::value;

    template < typename T >
    concept has_not_value_type_ostream_op_kind = !has_value_type_ostream_op<T>::value;

    template < has_value_type_ostream_op_kind T >
    void value_ostream_function(std::ostream& os, const value& v) {
        os << v.cast<T>();
    }

    template < has_not_value_type_ostream_op_kind T >
    void value_ostream_function([[maybe_unused]] std::ostream& os, [[maybe_unused]] const value& v) {
        throw std::logic_error("value type doesn't have ostream operator");
    }
}

namespace meta_hpp
{
    struct value::traits final {
        const any_type type;

        void* (*const data)(value&) noexcept;
        const void* (*const cdata)(const value&) noexcept;

        bool (*const less)(const value&, const value&);
        bool (*const equals)(const value&, const value&);

        void (*const move_ctor)(std::any&, value&&);
        void (*const copy_ctor)(std::any&, const value&);

        void (*const istream)(std::istream&, value&);
        void (*const ostream)(std::ostream&, const value&);

        template < typename T >
        static const traits* get() noexcept;
    };

    template < typename T >
    const value::traits* value::traits::get() noexcept {
        static const traits traits{
            // type
            resolve_type<T>(),

            // data
            +[](value& v) noexcept -> void* {
                return v.try_cast<T>();
            },

            // cdata
            +[](const value& v) noexcept -> const void* {
                return v.try_cast<T>();
            },

            // less
            &detail::value_less_function<T>,

            // equals
            &detail::value_equals_function<T>,

            // move_ctor
            +[](std::any& dst, value&& src) {
                if constexpr ( std::is_move_constructible_v<T> ) {
                    dst.emplace<T>(std::move(src).cast<T>());
                } else {
                    throw std::logic_error("value type is not move constructible");
                }
            },

            // copy_ctor
            +[](std::any& dst, const value& src) {
                if constexpr ( std::is_copy_constructible_v<T> ) {
                    dst.emplace<T>(src.cast<T>());
                } else {
                    throw std::logic_error("value type is not copy constructible");
                }
            },

            // istream
            &detail::value_istream_function<T>,

            // ostream
            &detail::value_ostream_function<T>,
        };
        return &traits;
    }
}

namespace meta_hpp
{
    inline value::value(value&& other) {
        traits_ = other.traits_;
        traits_->move_ctor(raw_, std::move(other));
    }

    inline value::value(const value& other) {
        traits_ = other.traits_;
        traits_->copy_ctor(raw_, other);
    }

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

    template < typename T, typename Tp
             , std::enable_if_t<!std::is_same_v<Tp, value>, int>
             , std::enable_if_t<!std::is_same_v<Tp, detail::arg>, int>
             , std::enable_if_t<!std::is_same_v<Tp, detail::inst>, int> >
    value::value(T&& val)
    : raw_{std::forward<T>(val)}
    , traits_{traits::get<Tp>()} {}

    template < typename T, typename Tp
             , std::enable_if_t<!std::is_same_v<Tp, value>, int>
             , std::enable_if_t<!std::is_same_v<Tp, detail::arg>, int>
             , std::enable_if_t<!std::is_same_v<Tp, detail::inst>, int> >
    value& value::operator=(T&& val) {
        raw_ = std::forward<T>(val);
        traits_ = resolve_type<Tp>();
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
    bool operator<(const value& l, const T& r) {
        return (l.get_type() < resolve_type<T>())
            || (l.get_type() == resolve_type<T>() && std::less<>{}(l.cast<T>(), r));
    }

    template < typename T >
    bool operator<(const T& l, const value& r) {
        return (resolve_type<T>() < r.get_type())
            || (resolve_type<T>() == r.get_type() && std::less<>{}(l, r.cast<T>()));
    }

    inline bool operator<(const value& l, const value& r) {
        return (l.get_type() < r.get_type())
            || (l.get_type() == r.get_type() && l.traits_->less(l, r));
    }
}

namespace meta_hpp
{
    template < typename T >
    bool operator==(const value& l, const T& r) {
        return l.get_type() == resolve_type<T>() && std::equal_to<>{}(l.cast<T>(), r);
    }

    template < typename T >
    bool operator==(const T& l, const value& r) {
        return resolve_type<T>() == r.get_type() && std::equal_to<>{}(l, r.cast<T>());
    }

    inline bool operator==(const value& l, const value& r) {
        return l.get_type() == r.get_type() && l.traits_->equals(l, r);
    }
}

namespace meta_hpp
{
    inline std::istream& operator>>(std::istream& is, value& v) {
        v.traits_->istream(is, v);
        return is;
    }

    inline std::ostream& operator<<(std::ostream& os, const value& v) {
        v.traits_->ostream(os, v);
        return os;
    }
}
