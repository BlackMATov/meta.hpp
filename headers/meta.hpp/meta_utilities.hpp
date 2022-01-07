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
    template < typename From >
    struct cvref_traits {
        static constexpr bool is_lvalue = std::is_lvalue_reference_v<From>;
        static constexpr bool is_rvalue = std::is_rvalue_reference_v<From>;
        static constexpr bool is_const = std::is_const_v<std::remove_reference_t<From>>;
        static constexpr bool is_volatile = std::is_volatile_v<std::remove_reference_t<From>>;

        template < bool yesno, template < typename > typename Q, typename V >
        using apply_t_if = std::conditional_t<yesno, Q<V>, V>;

        template < typename To >
        using copy_to =
            apply_t_if<is_lvalue, std::add_lvalue_reference_t,
            apply_t_if<is_rvalue, std::add_rvalue_reference_t,
            apply_t_if<is_const, std::add_const_t,
            apply_t_if<is_volatile, std::add_volatile_t,
            std::remove_cvref_t<To>>>>>;
    };

    template < typename From, typename To >
    using copy_cvref_t = typename cvref_traits<From>::template copy_to<To>;
}

namespace meta_hpp::detail
{
    template < typename T >
    concept destructible =
        std::is_nothrow_destructible_v<T>;

    template < typename T, typename... Args >
    concept constructible_from =
        destructible<T> &&
        std::is_constructible_v<T, Args...>;

    template < typename Derived, typename Base >
    concept derived_from =
        std::is_base_of_v<Base, Derived> &&
        std::is_convertible_v<const volatile Derived*, const volatile Base*>;

    template < typename From, typename To >
    concept convertible_to =
        std::is_convertible_v<From, To> &&
        requires { static_cast<To>(std::declval<From>()); };
}

namespace meta_hpp::detail
{
    template < typename T >
    concept value_kind =
        std::same_as<T, value>;

    template < typename T >
    concept decay_value_kind =
        value_kind<std::decay_t<T>>;

    template < typename T >
    concept uvalue_kind =
        std::same_as<T, arg_base> ||
        std::same_as<T, arg> ||
        std::same_as<T, inst_base> ||
        std::same_as<T, inst> ||
        std::same_as<T, value>;

    template < typename T >
    concept decay_uvalue_kind =
        uvalue_kind<std::decay_t<T>>;

    template < typename T >
    concept decay_non_uvalue_kind =
        (!decay_uvalue_kind<T>);

    template < typename T >
    concept arg_lvalue_ref_kind =
        (std::is_lvalue_reference_v<T>);

    template < typename T >
    concept arg_rvalue_ref_kind =
        (!std::is_reference_v<T>) ||
        (std::is_rvalue_reference_v<T>);

    template < typename T >
    concept inst_class_ref_kind =
        (std::is_class_v<T>) ||
        (std::is_reference_v<T> && std::is_class_v<std::remove_reference_t<T>>);

    template < typename T >
    concept inst_class_lvalue_ref_kind =
        (std::is_lvalue_reference_v<T> && std::is_class_v<std::remove_reference_t<T>>);

    template < typename T >
    concept inst_class_rvalue_ref_kind =
        (std::is_class_v<T>) ||
        (std::is_rvalue_reference_v<T> && std::is_class_v<std::remove_reference_t<T>>);
}

namespace meta_hpp::detail
{
    template < typename T >
    concept has_less_op_kind = requires(const T& v) {
        { v < v } -> convertible_to<bool>;
    };

    template < typename T >
    concept has_equals_op_kind = requires(const T& v) {
        { v == v } -> convertible_to<bool>;
    };

    template < typename T >
    concept has_istream_op_kind = requires(std::istream& is, T& v) {
        { is >> v } -> convertible_to<std::istream&>;
    };

    template < typename T >
    concept has_ostream_op_kind = requires(std::ostream& os, const T& v) {
        { os << v } -> convertible_to<std::ostream&>;
    };
}

namespace meta_hpp::detail
{
    class noncopyable {
    protected:
        noncopyable() = default;
        ~noncopyable() = default;
    public:
        noncopyable(noncopyable&&) = delete;
        noncopyable(const noncopyable&) = delete;

        noncopyable& operator=(noncopyable&&) = delete;
        noncopyable& operator=(const noncopyable&) = delete;
    };
}

namespace meta_hpp
{
    class value final {
    public:
        value() = delete;

        // NOLINTNEXTLINE(performance-noexcept-move-constructor)
        value(value&& other);
        value(const value& other);

        // NOLINTNEXTLINE(performance-noexcept-move-constructor)
        value& operator=(value&& other);
        value& operator=(const value& other);

        ~value() = default;

        template < detail::decay_non_uvalue_kind T >
        explicit value(T&& val);

        template < detail::decay_non_uvalue_kind T >
        value& operator=(T&& val);

        void swap(value& other) noexcept;

        [[nodiscard]] const any_type& get_type() const noexcept;

        [[nodiscard]] void* data() noexcept;
        [[nodiscard]] const void* data() const noexcept;
        [[nodiscard]] const void* cdata() const noexcept;

        template < typename T, typename Tp = std::decay_t<T> >
        [[nodiscard]] Tp& cast() &;

        template < typename T, typename Tp = std::decay_t<T> >
        [[nodiscard]] Tp&& cast() &&;

        template < typename T, typename Tp = std::decay_t<T> >
        [[nodiscard]] const Tp& cast() const &;

        template < typename T, typename Tp = std::decay_t<T> >
        [[nodiscard]] const Tp&& cast() const &&;

        template < typename T, typename Tp = std::decay_t<T> >
        [[nodiscard]] Tp* try_cast() noexcept;

        template < typename T, typename Tp = std::decay_t<T> >
        [[nodiscard]] const Tp* try_cast() const noexcept;

        friend bool operator<(const value& l, const value& r);
        friend bool operator==(const value& l, const value& r);
        friend std::istream& operator>>(std::istream& is, value& v);
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
    class arg_base {
    public:
        enum class ref_types {
            lvalue,
            const_lvalue,
            rvalue,
            const_rvalue,
        };
    public:
        arg_base() = delete;

        arg_base(arg_base&&) = default;
        arg_base(const arg_base&) = default;

        arg_base& operator=(arg_base&&) = delete;
        arg_base& operator=(const arg_base&) = delete;

        virtual ~arg_base() = default;

        template < arg_lvalue_ref_kind T >
            requires decay_non_uvalue_kind<T>
        explicit arg_base(type_list<T>);

        template < arg_rvalue_ref_kind T >
            requires decay_non_uvalue_kind<T>
        explicit arg_base(type_list<T>);

        explicit arg_base(value& v);
        explicit arg_base(const value& v);

        explicit arg_base(value&& v);
        explicit arg_base(const value&& v);

        [[nodiscard]] bool is_const() const noexcept;
        [[nodiscard]] bool is_lvalue() const noexcept;
        [[nodiscard]] bool is_rvalue() const noexcept;

        [[nodiscard]] ref_types get_ref_type() const noexcept;
        [[nodiscard]] const any_type& get_raw_type() const noexcept;

        template < typename To >
        [[nodiscard]] bool can_cast_to() const noexcept;
    private:
        ref_types ref_type_{};
        any_type raw_type_{};
    };
}

namespace meta_hpp::detail
{
    class arg final : public arg_base {
    public:
        arg() = delete;

        arg(arg&&) = default;
        arg(const arg&) = default;

        arg& operator=(arg&&) = delete;
        arg& operator=(const arg&) = delete;

        ~arg() override = default;

        template < decay_value_kind T >
        explicit arg(T&& v);

        template < decay_non_uvalue_kind T >
        explicit arg(T&& v);

        template < typename To >
        [[nodiscard]] To cast() const;
    private:
        void* data_{};
    };
}

namespace meta_hpp::detail
{
    class inst_base {
    public:
        enum class ref_types {
            lvalue,
            const_lvalue,
            rvalue,
            const_rvalue,
        };
    public:
        inst_base() = delete;

        inst_base(inst_base&&) = default;
        inst_base(const inst_base&) = default;

        inst_base& operator=(inst_base&&) = delete;
        inst_base& operator=(const inst_base&) = delete;

        virtual ~inst_base() = default;

        template < inst_class_lvalue_ref_kind T >
        explicit inst_base(type_list<T>);

        template < inst_class_rvalue_ref_kind T >
        explicit inst_base(type_list<T>);

        explicit inst_base(value& v);
        explicit inst_base(const value& v);

        explicit inst_base(value&& v);
        explicit inst_base(const value&& v);

        [[nodiscard]] bool is_const() const noexcept;
        [[nodiscard]] bool is_lvalue() const noexcept;
        [[nodiscard]] bool is_rvalue() const noexcept;

        [[nodiscard]] ref_types get_ref_type() const noexcept;
        [[nodiscard]] const class_type& get_raw_type() const noexcept;

        template < inst_class_ref_kind Q >
        [[nodiscard]] bool can_cast_to() const noexcept;
    private:
        ref_types ref_type_{};
        class_type raw_type_{};
    };
}

namespace meta_hpp::detail
{
    class inst final : public inst_base {
    public:
        inst() = delete;

        inst(inst&&) = default;
        inst(const inst&) = default;

        inst& operator=(inst&&) = delete;
        inst& operator=(const inst&) = delete;

        ~inst() override = default;

        template < decay_value_kind T >
        explicit inst(T&& v);

        template < decay_non_uvalue_kind T >
        explicit inst(T&& v);

        template < inst_class_ref_kind Q >
        [[nodiscard]] decltype(auto) cast() const;
    private:
        void* data_{};
    };
}
