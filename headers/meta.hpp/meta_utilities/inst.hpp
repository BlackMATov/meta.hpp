/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_fwd.hpp"

namespace meta_hpp
{
    class inst_base {
    public:
        enum class ref_types {
            ref,
            rref,
            cref,
            crref,
        };
    public:
        inst_base() = delete;

        inst_base(inst_base&&) = delete;
        inst_base& operator=(inst_base&&) = delete;

        inst_base(const inst_base&) = delete;
        inst_base& operator=(const inst_base&) = delete;

        template < typename T, std::enable_if_t<
            std::is_lvalue_reference_v<T> && std::is_class_v<std::remove_reference_t<T>>
        , int> = 0>
        explicit inst_base(typename_arg_t<T>);

        template < typename T, std::enable_if_t<
            std::is_class_v<T> ||
            (std::is_rvalue_reference_v<T> && std::is_class_v<std::remove_reference_t<T>>)
        , int> = 0>
        explicit inst_base(typename_arg_t<T>);

        bool is_const() const noexcept;
        bool is_lvalue() const noexcept;
        bool is_rvalue() const noexcept;

        any_type raw_type() const noexcept;
        ref_types ref_type() const noexcept;

        template < typename To >
        bool can_cast_to() const noexcept;
    private:
        any_type raw_type_{};
        ref_types ref_type_{};
    };
}

namespace meta_hpp
{
    template < typename T, std::enable_if_t<
        std::is_lvalue_reference_v<T> && std::is_class_v<std::remove_reference_t<T>>
    , int> >
    inst_base::inst_base(typename_arg_t<T>)
    : raw_type_{type_db::get<stdex::remove_cvref_t<T>>()}
    , ref_type_{std::is_const_v<std::remove_reference_t<T>> ? ref_types::cref : ref_types::ref} {}

    template < typename T, std::enable_if_t<
        std::is_class_v<T> ||
        (std::is_rvalue_reference_v<T> && std::is_class_v<std::remove_reference_t<T>>)
    , int> >
    inst_base::inst_base(typename_arg_t<T>)
    : raw_type_{type_db::get<stdex::remove_cvref_t<T>>()}
    , ref_type_{std::is_const_v<std::remove_reference_t<T>> ? ref_types::crref : ref_types::rref} {}

    inline bool inst_base::is_const() const noexcept {
        return ref_type_ == ref_types::cref
            || ref_type_ == ref_types::crref;
    }

    inline bool inst_base::is_lvalue() const noexcept {
        return ref_type_ == ref_types::ref
            || ref_type_ == ref_types::cref;
    }

    inline bool inst_base::is_rvalue() const noexcept {
        return ref_type_ == ref_types::rref
            || ref_type_ == ref_types::crref;
    }

    inline any_type inst_base::raw_type() const noexcept {
        return raw_type_;
    }

    inline inst_base::ref_types inst_base::ref_type() const noexcept {
        return ref_type_;
    }

    template < typename To >
    bool inst_base::can_cast_to() const noexcept {
        static_assert(
            std::is_class_v<To> ||
            (std::is_reference_v<To> && std::is_class_v<std::remove_reference_t<To>>));

        constexpr bool to_const = std::is_const_v<std::remove_reference_t<To>>;

        if constexpr ( !to_const ) {
            if ( is_const() ) {
                return false;
            }
        }

        if constexpr ( std::is_lvalue_reference_v<To> ) {
            if ( !is_lvalue() ) {
                return false;
            }
        }

        if constexpr ( std::is_rvalue_reference_v<To> ) {
            if ( !is_rvalue() ) {
                return false;
            }
        }

        using to_raw_type = stdex::remove_cvref_t<To>;
        return raw_type() == type_db::get<to_raw_type>();
    }
}

namespace meta_hpp
{
    class inst final : public inst_base {
    public:
        inst() = delete;

        inst(inst&&) = delete;
        inst& operator=(inst&&) = delete;

        inst(const inst&) = delete;
        inst& operator=(const inst&) = delete;

        template < typename T, std::enable_if_t<
            std::is_class_v<T> ||
            (std::is_reference_v<T> && std::is_class_v<std::remove_reference_t<T>>)
        , int> = 0 >
        explicit inst(T&& v);

        template < typename To >
        decltype(auto) cast() const;
    private:
        void* data_{};
    };
}

namespace meta_hpp
{
    template < typename T, std::enable_if_t<
        std::is_class_v<T> ||
        (std::is_reference_v<T> && std::is_class_v<std::remove_reference_t<T>>)
    , int> >
    inst::inst(T&& v)
    : inst_base{typename_arg<T&&>}
    , data_{const_cast<stdex::remove_cvref_t<T>*>(std::addressof(v))} {}

    template < typename To >
    decltype(auto) inst::cast() const {
        if ( !can_cast_to<To>() ) {
            throw std::logic_error("bad inst cast");
        }

        if constexpr ( std::is_reference_v<To> ) {
            using raw_type_with_cv = std::remove_reference_t<To>;

            if constexpr ( std::is_lvalue_reference_v<To> ) {
                return *static_cast<raw_type_with_cv*>(data_);
            }

            if constexpr ( std::is_rvalue_reference_v<To> ) {
                return std::move(*static_cast<raw_type_with_cv*>(data_));
            }
        }

        if constexpr ( !std::is_reference_v<To>) {
            using raw_type_with_cv = To;
            return *static_cast<raw_type_with_cv*>(data_);
        }
    }
}
