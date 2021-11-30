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
    template < typename T, std::enable_if_t<
        (std::is_lvalue_reference_v<T> && std::is_class_v<std::remove_reference_t<T>>)
    , int> >
    inst_base::inst_base(type_list<T>)
    : ref_type_{std::is_const_v<std::remove_reference_t<T>> ? ref_types::const_lvalue : ref_types::lvalue}
    , raw_type_{resolve_type<std::remove_cvref_t<T>>()} {}

    template < typename T, std::enable_if_t<
        (std::is_class_v<T>) ||
        (std::is_rvalue_reference_v<T> && std::is_class_v<std::remove_reference_t<T>>)
    , int> >
    inst_base::inst_base(type_list<T>)
    : ref_type_{std::is_const_v<std::remove_reference_t<T>> ? ref_types::const_rvalue : ref_types::rvalue}
    , raw_type_{resolve_type<std::remove_cvref_t<T>>()} {}

    inline inst_base::inst_base(value& v)
    : ref_type_{ref_types::lvalue}
    , raw_type_{v.get_type().as_class()} {
        if ( !v.get_type().is_class() ) {
            throw std::logic_error("an attempt to create an instance with a non-class value type");
        }
    }

    inline inst_base::inst_base(const value& v)
    : ref_type_{ref_types::const_lvalue}
    , raw_type_{v.get_type().as_class()} {
        if ( !v.get_type().is_class() ) {
            throw std::logic_error("an attempt to create an instance with a non-class value type");
        }
    }

    inline inst_base::inst_base(value&& v)
    : ref_type_{ref_types::rvalue}
    , raw_type_{v.get_type().as_class()} {
        if ( !v.get_type().is_class() ) {
            throw std::logic_error("an attempt to create an instance with a non-class value type");
        }
    }

    inline inst_base::inst_base(const value&& v)
    : ref_type_{ref_types::const_rvalue}
    , raw_type_{v.get_type().as_class()} {
        if ( !v.get_type().is_class() ) {
            throw std::logic_error("an attempt to create an instance with a non-class value type");
        }
    }

    inline bool inst_base::is_const() const noexcept {
        return ref_type_ == ref_types::const_lvalue
            || ref_type_ == ref_types::const_rvalue;
    }

    inline bool inst_base::is_lvalue() const noexcept {
        return ref_type_ == ref_types::lvalue
            || ref_type_ == ref_types::const_lvalue;
    }

    inline bool inst_base::is_rvalue() const noexcept {
        return ref_type_ == ref_types::rvalue
            || ref_type_ == ref_types::const_rvalue;
    }

    inline inst_base::ref_types inst_base::get_ref_type() const noexcept {
        return ref_type_;
    }

    inline const class_type& inst_base::get_raw_type() const noexcept {
        return raw_type_;
    }

    template < typename To, std::enable_if_t<
        (std::is_class_v<To>) ||
        (std::is_reference_v<To> && std::is_class_v<std::remove_reference_t<To>>)
    , int> >
    bool inst_base::can_cast_to() const noexcept {
        using to_raw_type = std::remove_cvref_t<To>;
        using to_raw_type_cv = std::remove_reference_t<To>;

        if constexpr ( !std::is_const_v<to_raw_type_cv> ) {
            if ( is_const() ) {
                return false;
            }
        }

        if constexpr ( std::is_reference_v<To> ) {
            const auto check_qualifiers = [](ref_types self_ref_type){
                switch ( self_ref_type ) {
                case ref_types::lvalue:       return std::is_convertible_v<to_raw_type&, To>;
                case ref_types::const_lvalue: return std::is_convertible_v<const to_raw_type&, To>;
                case ref_types::rvalue:       return std::is_convertible_v<to_raw_type&&, To>;
                case ref_types::const_rvalue: return std::is_convertible_v<const to_raw_type&&, To>;
                }
            };

            if ( !check_qualifiers(get_ref_type()) ) {
                return false;
            }
        }

        return get_raw_type() == resolve_type<to_raw_type>()
            || get_raw_type().is_derived_from(resolve_type<to_raw_type>());
    }
}

namespace meta_hpp::detail
{
    template < typename T, typename Tp
             , std::enable_if_t<std::is_same_v<Tp, value>, int> >
    inst::inst(T&& v)
    : inst_base{std::forward<T>(v)}
    , data_{const_cast<void*>(v.data())} {}

    template < typename T, class_kind Tp
             , std::enable_if_t<!std::is_same_v<Tp, arg>, int>
             , std::enable_if_t<!std::is_same_v<Tp, inst>, int>
             , std::enable_if_t<!std::is_same_v<Tp, value>, int> >
    inst::inst(T&& v)
    : inst_base{type_list<T&&>{}}
    , data_{const_cast<std::remove_cvref_t<T>*>(std::addressof(v))} {}

    template < typename To, std::enable_if_t<
        (std::is_class_v<To>) ||
        (std::is_reference_v<To> && std::is_class_v<std::remove_reference_t<To>>)
    , int> >
    decltype(auto) inst::cast() const {
        if ( !can_cast_to<To>() ) {
            throw std::logic_error("bad instance cast");
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
