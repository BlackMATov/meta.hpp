/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_fwd.hpp"

namespace meta_hpp
{
    class value final {
    public:
        value() = delete;

        value(value&& other);
        value(const value& other);

        value& operator=(value&& other);
        value& operator=(const value& other);

        template < typename T
                 , typename Tp = std::decay_t<T>
                 , std::enable_if_t<!std::is_same_v<Tp, arg>, int> = 0
                 , std::enable_if_t<!std::is_same_v<Tp, inst>, int> = 0
                 , std::enable_if_t<!std::is_same_v<Tp, value>, int> = 0 >
        explicit value(T&& val);

        template < typename T
                 , typename Tp = std::decay_t<T>
                 , std::enable_if_t<!std::is_same_v<Tp, arg>, int> = 0
                 , std::enable_if_t<!std::is_same_v<Tp, inst>, int> = 0
                 , std::enable_if_t<!std::is_same_v<Tp, value>, int> = 0 >
        value& operator=(T&& val);

        void swap(value& other) noexcept;

        const any_type& type() const noexcept;

        void* data() noexcept;
        const void* data() const noexcept;
        const void* cdata() const noexcept;

        template < typename T, typename Tp = std::decay_t<T> >
        Tp& cast() &;

        template < typename T, typename Tp = std::decay_t<T> >
        Tp&& cast() &&;

        template < typename T, typename Tp = std::decay_t<T> >
        const Tp& cast() const &;

        template < typename T, typename Tp = std::decay_t<T> >
        const Tp&& cast() const &&;

        template < typename T, typename Tp = std::decay_t<T> >
        Tp* try_cast() noexcept;

        template < typename T, typename Tp = std::decay_t<T> >
        const Tp* try_cast() const noexcept;

        template < typename T >
        friend bool operator==(const value& l, const T& r);
        template < typename T >
        friend bool operator==(const T& l, const value& r);
        friend bool operator==(const value& l, const value& r);

        friend std::istream& operator>>(std::istream& os, value& v);
        friend std::ostream& operator<<(std::ostream& os, const value& v);
    private:
        struct traits;
        std::any raw_{};
        const traits* traits_{};
    };

    inline void swap(value& l, value& r) noexcept {
        l.swap(r);
    }
}

namespace meta_hpp::detail
{
    template < typename T, typename = void >
    struct has_value_type_equality_operator
    : std::false_type {};

    template < typename T >
    struct has_value_type_equality_operator<T, std::void_t<decltype(
        std::declval<const T&>() == std::declval<const T&>()
    )>> : std::true_type {};

    template < typename T >
    inline constexpr bool has_value_type_equality_operator_v = has_value_type_equality_operator<T>::value;

    template < typename T, std::enable_if_t<
        has_value_type_equality_operator_v<T>
    , int> = 0 >
    bool value_equals_function(const value& l, const value& r) {
        assert(l.type() == r.type());
        return l.cast<T>() == r.cast<T>();
    }

    template < typename T, std::enable_if_t<
        !has_value_type_equality_operator_v<T>
    , int> = 0 >
    bool value_equals_function([[maybe_unused]] const value& l, [[maybe_unused]] const value& r) {
        throw std::logic_error("value type doesn't have equality operator");
    }
}

namespace meta_hpp::detail
{
    template < typename T, typename = void >
    struct has_value_type_istream_operator
    : std::false_type {};

    template < typename T >
    struct has_value_type_istream_operator<T, std::void_t<decltype(
        std::declval<std::istream&>() >> std::declval<T&>()
    )>> : std::true_type {};

    template < typename T >
    inline constexpr bool has_value_type_istream_operator_v = has_value_type_istream_operator<T>::value;

    template < typename T, std::enable_if_t<
        has_value_type_istream_operator_v<T>
    , int> = 0 >
    void value_istream_function(std::istream& os, value& v) {
        os >> v.cast<T>();
    }

    template < typename T, std::enable_if_t<
        !has_value_type_istream_operator_v<T>
    , int> = 0 >
    void value_istream_function([[maybe_unused]] std::istream& os, [[maybe_unused]] value& v) {
        throw std::logic_error("value type doesn't have istream operator");
    }
}

namespace meta_hpp::detail
{
    template < typename T, typename = void >
    struct has_value_type_ostream_operator
    : std::false_type {};

    template < typename T >
    struct has_value_type_ostream_operator<T, std::void_t<decltype(
        std::declval<std::ostream&>() << std::declval<const T&>()
    )>> : std::true_type {};

    template < typename T >
    inline constexpr bool has_value_type_ostream_operator_v = has_value_type_ostream_operator<T>::value;

    template < typename T, std::enable_if_t<
        has_value_type_ostream_operator_v<T>
    , int> = 0 >
    void value_ostream_function(std::ostream& os, const value& v) {
        os << v.cast<T>();
    }

    template < typename T, std::enable_if_t<
        !has_value_type_ostream_operator_v<T>
    , int> = 0 >
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
            type_db::get<T>(),

            // data
            +[](value& v) noexcept -> void* {
                return v.try_cast<T>();
            },

            // cdata
            +[](const value& v) noexcept -> const void* {
                return v.try_cast<T>();
            },

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
             , std::enable_if_t<!std::is_same_v<Tp, arg>, int>
             , std::enable_if_t<!std::is_same_v<Tp, inst>, int>
             , std::enable_if_t<!std::is_same_v<Tp, value>, int> >
    value::value(T&& val)
    : raw_{std::forward<T>(val)}
    , traits_{traits::get<Tp>()} {}

    template < typename T, typename Tp
             , std::enable_if_t<!std::is_same_v<Tp, arg>, int>
             , std::enable_if_t<!std::is_same_v<Tp, inst>, int>
             , std::enable_if_t<!std::is_same_v<Tp, value>, int> >
    value& value::operator=(T&& val) {
        raw_ = std::forward<T>(val);
        traits_ = type_db::get<Tp>();
        return *this;
    }

    inline void value::swap(value& other) noexcept {
        using std::swap;
        swap(raw_, other.raw_);
        swap(traits_, other.traits_);
    }

    inline const any_type& value::type() const noexcept {
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
        if ( type() != type_db::get<Tp>() ) {
            throw std::logic_error("bad value cast");
        }
        return std::any_cast<Tp&>(raw_);
    }

    template < typename T, typename Tp >
    Tp&& value::cast() && {
        if ( type() != type_db::get<Tp>() ) {
            throw std::logic_error("bad value cast");
        }
        return std::move(std::any_cast<Tp&>(raw_));
    }

    template < typename T, typename Tp >
    const Tp& value::cast() const & {
        if ( type() != type_db::get<Tp>() ) {
            throw std::logic_error("bad value cast");
        }
        return std::any_cast<const Tp&>(raw_);
    }

    template < typename T, typename Tp >
    const Tp&& value::cast() const && {
        if ( type() != type_db::get<Tp>() ) {
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
    bool operator==(const value& l, const T& r) {
        return l.type() == type_db::get<T>()
            && std::equal_to<>{}(l.cast<T>(), r);
    }

    template < typename T >
    bool operator==(const T& l, const value& r) {
        return type_db::get<T>() == r.type()
            && std::equal_to<>{}(l, r.cast<T>());
    }

    inline bool operator==(const value& l, const value& r) {
        return l.type() == r.type()
            && l.traits_->equals(l, r);
    }

    inline std::istream& operator>>(std::istream& is, value& v) {
        v.traits_->istream(is, v);
        return is;
    }

    inline std::ostream& operator<<(std::ostream& os, const value& v) {
        v.traits_->ostream(os, v);
        return os;
    }
}
