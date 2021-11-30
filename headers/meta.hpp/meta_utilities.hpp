/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_base.hpp"

namespace meta_hpp
{
    class value final {
    public:
        value() = delete;

        value(value&& other);
        value(const value& other);

        value& operator=(value&& other);
        value& operator=(const value& other);

        template < typename T, typename Tp = std::decay_t<T>
                 , std::enable_if_t<!std::is_same_v<Tp, value>, int> = 0
                 , std::enable_if_t<!std::is_same_v<Tp, detail::arg>, int> = 0
                 , std::enable_if_t<!std::is_same_v<Tp, detail::inst>, int> = 0 >
        explicit value(T&& val);

        template < typename T, typename Tp = std::decay_t<T>
                 , std::enable_if_t<!std::is_same_v<Tp, value>, int> = 0
                 , std::enable_if_t<!std::is_same_v<Tp, detail::arg>, int> = 0
                 , std::enable_if_t<!std::is_same_v<Tp, detail::inst>, int> = 0 >
        value& operator=(T&& val);

        void swap(value& other) noexcept;

        const any_type& get_type() const noexcept;

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
        friend bool operator<(const value& l, const T& r);
        template < typename T >
        friend bool operator<(const T& l, const value& r);
        friend bool operator<(const value& l, const value& r);

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
    class arg_base {
    public:
        enum class ref_types {
            ref,
            rref,
            cref,
            crref,
        };
    public:
        arg_base() = delete;

        arg_base(arg_base&&) = delete;
        arg_base& operator=(arg_base&&) = delete;

        arg_base(const arg_base&) = delete;
        arg_base& operator=(const arg_base&) = delete;

        template < typename T, std::enable_if_t<
            (std::is_pointer_v<T> || std::is_lvalue_reference_v<T>)
        , int> = 0 >
        explicit arg_base(type_list<T>);

        template < typename T, std::enable_if_t<
            (std::is_rvalue_reference_v<T>) ||
            (!std::is_pointer_v<T> && !std::is_reference_v<T>)
        , int> = 0 >
        explicit arg_base(type_list<T>);

        explicit arg_base(value& v);
        explicit arg_base(value&& v);
        explicit arg_base(const value& v);
        explicit arg_base(const value&& v);

        bool is_const() const noexcept;
        bool is_lvalue() const noexcept;
        bool is_rvalue() const noexcept;

        any_type get_raw_type() const noexcept;
        ref_types get_ref_type() const noexcept;

        template < typename To >
        bool can_cast_to() const noexcept;
    private:
        any_type raw_type_{};
        ref_types ref_type_{};
    };
}

namespace meta_hpp::detail
{
    class arg final : public arg_base {
    public:
        arg() = delete;

        arg(arg&&) = delete;
        arg& operator=(arg&&) = delete;

        arg(const arg&) = delete;
        arg& operator=(const arg&) = delete;

        template < typename T, typename Tp = std::decay_t<T>
                 , std::enable_if_t<std::is_same_v<Tp, value>, int> = 0 >
        explicit arg(T&& v);

        template < typename T, typename Tp = std::decay_t<T>
                 , std::enable_if_t<!std::is_same_v<Tp, arg>, int> = 0
                 , std::enable_if_t<!std::is_same_v<Tp, inst>, int> = 0
                 , std::enable_if_t<!std::is_same_v<Tp, value>, int> = 0 >
        explicit arg(T&& v);

        template < typename To >
        To cast() const;
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

        inst_base(inst_base&&) = delete;
        inst_base& operator=(inst_base&&) = delete;

        inst_base(const inst_base&) = delete;
        inst_base& operator=(const inst_base&) = delete;

        template < typename T, std::enable_if_t<
            (std::is_lvalue_reference_v<T> && std::is_class_v<std::remove_reference_t<T>>)
        , int> = 0>
        explicit inst_base(type_list<T>);

        template < typename T, std::enable_if_t<
            (std::is_class_v<T>) ||
            (std::is_rvalue_reference_v<T> && std::is_class_v<std::remove_reference_t<T>>)
        , int> = 0>
        explicit inst_base(type_list<T>);

        explicit inst_base(value& v);
        explicit inst_base(const value& v);

        explicit inst_base(value&& v);
        explicit inst_base(const value&& v);

        bool is_const() const noexcept;
        bool is_lvalue() const noexcept;
        bool is_rvalue() const noexcept;

        ref_types get_ref_type() const noexcept;
        const class_type& get_raw_type() const noexcept;

        template < typename To, std::enable_if_t<
            (std::is_class_v<To>) ||
            (std::is_reference_v<To> && std::is_class_v<std::remove_reference_t<To>>)
        , int> = 0>
        bool can_cast_to() const noexcept;
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

        inst(inst&&) = delete;
        inst& operator=(inst&&) = delete;

        inst(const inst&) = delete;
        inst& operator=(const inst&) = delete;

        template < typename T, typename Tp = std::decay_t<T>
                 , std::enable_if_t<std::is_same_v<Tp, value>, int> = 0 >
        explicit inst(T&& v);

        template < typename T, class_kind Tp = std::decay_t<T>
                 , std::enable_if_t<!std::is_same_v<Tp, arg>, int> = 0
                 , std::enable_if_t<!std::is_same_v<Tp, inst>, int> = 0
                 , std::enable_if_t<!std::is_same_v<Tp, value>, int> = 0 >
        explicit inst(T&& v);

        template < typename To, std::enable_if_t<
            (std::is_class_v<To>) ||
            (std::is_reference_v<To> && std::is_class_v<std::remove_reference_t<To>>)
        , int> = 0>
        decltype(auto) cast() const;
    private:
        void* data_{};
    };
}
