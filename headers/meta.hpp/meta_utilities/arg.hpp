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
        (std::is_lvalue_reference_v<T>)
    , int> >
    arg_base::arg_base(type_list<T>)
    : ref_type_{std::is_const_v<std::remove_reference_t<T>> ? ref_types::const_lvalue : ref_types::lvalue}
    , raw_type_{resolve_type<std::remove_cvref_t<T>>()} {}

    template < typename T, std::enable_if_t<
        (std::is_rvalue_reference_v<T> || !std::is_reference_v<T>)
    , int> >
    arg_base::arg_base(type_list<T>)
    : ref_type_{std::is_const_v<std::remove_reference_t<T>> ? ref_types::const_rvalue : ref_types::rvalue}
    , raw_type_{resolve_type<std::remove_cvref_t<T>>()} {}

    inline arg_base::arg_base(value& v)
    : ref_type_{ref_types::lvalue}
    , raw_type_{v.get_type()} {}

    inline arg_base::arg_base(const value& v)
    : ref_type_{ref_types::const_lvalue}
    , raw_type_{v.get_type()} {}

    inline arg_base::arg_base(value&& v)
    : ref_type_{ref_types::rvalue}
    , raw_type_{v.get_type()} {}

    inline arg_base::arg_base(const value&& v)
    : ref_type_{ref_types::const_rvalue}
    , raw_type_{v.get_type()} {}

    inline bool arg_base::is_const() const noexcept {
        return ref_type_ == ref_types::const_lvalue
            || ref_type_ == ref_types::const_rvalue;
    }

    inline bool arg_base::is_lvalue() const noexcept {
        return ref_type_ == ref_types::lvalue
            || ref_type_ == ref_types::const_lvalue;
    }

    inline bool arg_base::is_rvalue() const noexcept {
        return ref_type_ == ref_types::rvalue
            || ref_type_ == ref_types::const_rvalue;
    }

    inline arg_base::ref_types arg_base::get_ref_type() const noexcept {
        return ref_type_;
    }

    inline const any_type& arg_base::get_raw_type() const noexcept {
        return raw_type_;
    }

    template < typename To >
    bool arg_base::can_cast_to() const noexcept {
        using to_raw_type_cv = std::remove_reference_t<To>;
        using to_raw_type = std::remove_cv_t<to_raw_type_cv>;

        static_assert(
            !(std::is_reference_v<To> && std::is_pointer_v<to_raw_type>),
            "references to pointers are not supported yet");

        const auto check_qualifiers = [this](){
            switch ( get_ref_type() ) {
            case ref_types::lvalue:       return std::is_convertible_v<to_raw_type&, To>;
            case ref_types::const_lvalue: return std::is_convertible_v<const to_raw_type&, To>;
            case ref_types::rvalue:       return std::is_convertible_v<to_raw_type&&, To>;
            case ref_types::const_rvalue: return std::is_convertible_v<const to_raw_type&&, To>;
            }
        };

        const auto check_convertible = [this](){
            const auto is_a = [](const any_type& base, const any_type& derived){
                return (base == derived)
                    || (base.is_class() && derived.is_class() && base.as_class().is_base_of(derived.as_class()));
            };

            const any_type& from_type = get_raw_type();
            const any_type& to_type = resolve_type<to_raw_type>();

            if ( is_a(to_type, from_type) ) {
                return true;
            }

            if ( std::is_lvalue_reference_v<To> && !std::is_const_v<to_raw_type_cv> ) {
                return false;
            }

            if ( from_type.is_pointer() && to_type.is_pointer() ) {
                const pointer_type& from_type_ptr = from_type.as_pointer();
                const bool from_type_ptr_readonly = from_type_ptr.get_flags().has(pointer_flags::is_readonly);

                const pointer_type& to_type_ptr = to_type.as_pointer();
                const bool to_type_ptr_readonly = to_type_ptr.get_flags().has(pointer_flags::is_readonly);

                return to_type_ptr_readonly >= from_type_ptr_readonly
                    && is_a(to_type_ptr.get_data_type(), from_type_ptr.get_data_type());
            }

            return false;
        };

        return check_qualifiers() && check_convertible();
    }
}

namespace meta_hpp::detail
{
    template < typename T, typename Tp
             , std::enable_if_t<std::is_same_v<Tp, value>, int> >
    arg::arg(T&& v)
    : arg_base{std::forward<T>(v)}
    , data_{const_cast<void*>(v.data())} {}

    template < typename T, typename Tp
             , std::enable_if_t<!std::is_same_v<Tp, arg>, int>
             , std::enable_if_t<!std::is_same_v<Tp, inst>, int>
             , std::enable_if_t<!std::is_same_v<Tp, value>, int> >
    arg::arg(T&& v)
    : arg_base{type_list<T&&>{}}
    , data_{const_cast<std::remove_cvref_t<T>*>(std::addressof(v))} {}

    template < typename To >
    To arg::cast() const {
        if ( !can_cast_to<To>() ) {
            throw std::logic_error("bad argument cast");
        }

        if constexpr ( std::is_pointer_v<To> ) {
            return *static_cast<To*>(data_);
        }

        if constexpr ( std::is_reference_v<To> ) {
            using raw_type = std::remove_cvref_t<To>;

            if constexpr ( std::is_lvalue_reference_v<To> ) {
                return *static_cast<raw_type*>(data_);
            }

            if constexpr ( std::is_rvalue_reference_v<To> ) {
                return std::move(*static_cast<raw_type*>(data_));
            }
        }

        if constexpr ( !std::is_pointer_v<To> && !std::is_reference_v<To> ) {
            using raw_type = std::remove_cv_t<To>;

            if ( get_ref_type() == ref_types::lvalue ) {
                if constexpr ( std::is_constructible_v<To, raw_type&> ) {
                    return To{*static_cast<raw_type*>(data_)};
                }
            }

            if ( get_ref_type() == ref_types::const_lvalue ) {
                if constexpr ( std::is_constructible_v<To, const raw_type&> ) {
                    return To{std::as_const(*static_cast<raw_type*>(data_))};
                }
            }

            if ( get_ref_type() == ref_types::rvalue ) {
                if constexpr ( std::is_constructible_v<To, raw_type&&> ) {
                    return To{std::move(*static_cast<raw_type*>(data_))};
                }
            }

            if ( get_ref_type() == ref_types::const_rvalue ) {
                if constexpr ( std::is_constructible_v<To, const raw_type&&> ) {
                    return To{std::move(std::as_const(*static_cast<raw_type*>(data_)))};
                }
            }
        }

        throw std::logic_error("bad argument cast");
    }
}
