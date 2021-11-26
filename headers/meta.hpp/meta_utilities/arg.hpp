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
        (std::is_pointer_v<T> || std::is_lvalue_reference_v<T>)
    , int> >
    arg_base::arg_base(type_list<T>)
    : raw_type_{resolve_type<std::remove_cvref_t<T>>()}
    , ref_type_{std::is_const_v<std::remove_reference_t<T>> ? ref_types::cref : ref_types::ref} {}

    template < typename T, std::enable_if_t<
        (std::is_rvalue_reference_v<T>) ||
        (!std::is_pointer_v<T> && !std::is_reference_v<T>)
    , int> >
    arg_base::arg_base(type_list<T>)
    : raw_type_{resolve_type<std::remove_cvref_t<T>>()}
    , ref_type_{std::is_const_v<std::remove_reference_t<T>> ? ref_types::crref : ref_types::rref} {}

    inline arg_base::arg_base(value& v)
    : raw_type_{v.get_type()}
    , ref_type_{ref_types::ref} {}

    inline arg_base::arg_base(value&& v)
    : raw_type_{v.get_type()}
    , ref_type_{ref_types::rref} {}

    inline arg_base::arg_base(const value& v)
    : raw_type_{v.get_type()}
    , ref_type_{ref_types::cref} {}

    inline arg_base::arg_base(const value&& v)
    : raw_type_{v.get_type()}
    , ref_type_{ref_types::crref} {}

    inline bool arg_base::is_const() const noexcept {
        return ref_type_ == ref_types::cref
            || ref_type_ == ref_types::crref;
    }

    inline bool arg_base::is_lvalue() const noexcept {
        return ref_type_ == ref_types::ref
            || ref_type_ == ref_types::cref;
    }

    inline bool arg_base::is_rvalue() const noexcept {
        return ref_type_ == ref_types::rref
            || ref_type_ == ref_types::crref;
    }

    inline any_type arg_base::get_raw_type() const noexcept {
        return raw_type_;
    }

    inline arg_base::ref_types arg_base::get_ref_type() const noexcept {
        return ref_type_;
    }

    template < typename To >
    bool arg_base::can_cast_to() const noexcept {
        if constexpr ( std::is_pointer_v<To> ) {
            using to_raw_type = std::remove_cv_t<To>;
            using to_raw_ptr_type = std::remove_cv_t<std::remove_pointer_t<to_raw_type>>;
            return get_raw_type() == resolve_type<to_raw_type>()
                || get_raw_type() == resolve_type<to_raw_ptr_type*>();
        }

        if constexpr ( std::is_reference_v<To> ) {
            constexpr bool to_const = std::is_const_v<std::remove_reference_t<To>>;

            if constexpr ( !to_const ) {
                if ( is_const() ) {
                    return false;
                }
            }

            if constexpr ( std::is_lvalue_reference_v<To> ) {
                if constexpr ( !to_const ) {
                    if ( is_rvalue() ) {
                        return false;
                    }
                }
            }

            if constexpr ( std::is_rvalue_reference_v<To> ) {
                if ( !is_rvalue() ) {
                    return false;
                }
            }

            using to_raw_type = std::remove_cvref_t<To>;
            if ( get_raw_type() == resolve_type<to_raw_type>() ) {
                return true;
            }

            if constexpr ( to_const && std::is_pointer_v<to_raw_type> ) {
                using to_raw_ptr_type = std::remove_cv_t<std::remove_pointer_t<to_raw_type>>;
                return get_raw_type() == resolve_type<to_raw_ptr_type*>();
            }

            return false;
        }

        if constexpr ( !std::is_pointer_v<To> && !std::is_reference_v<To> ) {
            using to_raw_type = std::remove_cv_t<To>;
            if ( get_raw_type() != resolve_type<to_raw_type>() ) {
                return false;
            }

            return (get_ref_type() == ref_types::ref && std::is_constructible_v<To, to_raw_type&>)
                || (get_ref_type() == ref_types::cref && std::is_constructible_v<To, const to_raw_type&>)
                || (get_ref_type() == ref_types::rref && std::is_constructible_v<To, to_raw_type&&>)
                || (get_ref_type() == ref_types::crref && std::is_constructible_v<To, const to_raw_type&&>);
        }
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

            if ( get_ref_type() == ref_types::ref ) {
                if constexpr ( std::is_constructible_v<To, raw_type&> ) {
                    return To{*static_cast<raw_type*>(data_)};
                }
            }

            if ( get_ref_type() == ref_types::cref ) {
                if constexpr ( std::is_constructible_v<To, const raw_type&> ) {
                    return To{std::as_const(*static_cast<raw_type*>(data_))};
                }
            }

            if ( get_ref_type() == ref_types::rref ) {
                if constexpr ( std::is_constructible_v<To, raw_type&&> ) {
                    return To{std::move(*static_cast<raw_type*>(data_))};
                }
            }

            if ( get_ref_type() == ref_types::crref ) {
                if constexpr ( std::is_constructible_v<To, const raw_type&&> ) {
                    return To{std::move(std::as_const(*static_cast<raw_type*>(data_)))};
                }
            }
        }

        throw std::logic_error("bad argument cast");
    }
}
