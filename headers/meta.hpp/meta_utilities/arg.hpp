/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_fwd.hpp"

namespace meta_hpp
{
    class arg final {
    public:
        enum class ref_types {
            ref,
            rref,
            cref,
            crref,
        };
    public:
        arg() = delete;

        arg(arg&&) = delete;
        arg& operator=(arg&&) = delete;

        arg(const arg&) = delete;
        arg& operator=(const arg&) = delete;

        template < typename T
                 , std::enable_if_t<!std::is_reference_v<T>, int> = 0 >
        explicit arg(T&& v);

        template < typename T
                 , std::enable_if_t<std::is_lvalue_reference_v<T>, int> = 0 >
        explicit arg(T&& v);

        template < typename To >
        To cast() const;

        template < typename To >
        bool can_cast() const noexcept;

        any_type raw_type() const noexcept;
        ref_types ref_type() const noexcept;

        bool is_const() const noexcept;
        bool is_lvalue() const noexcept;
        bool is_rvalue() const noexcept;
    private:
        void* data_{};
        any_type raw_type_{};
        ref_types ref_type_{};
    };
}

namespace meta_hpp
{
    template < typename T
             , std::enable_if_t<!std::is_reference_v<T>, int> >
    inline arg::arg(T&& v)
    : data_{const_cast<std::add_pointer_t<stdex::remove_cvref_t<T>>>(std::addressof(v))}
    , raw_type_{type_db::get<stdex::remove_cvref_t<T>>()}
    , ref_type_{std::is_const_v<T> ? ref_types::crref : ref_types::rref} {}

    template < typename T
             , std::enable_if_t<std::is_lvalue_reference_v<T>, int> >
    inline arg::arg(T&& v)
    : data_{const_cast<std::add_pointer_t<stdex::remove_cvref_t<T>>>(std::addressof(v))}
    , raw_type_{type_db::get<stdex::remove_cvref_t<T>>()}
    , ref_type_{std::is_const_v<std::remove_reference_t<T>> ? ref_types::cref : ref_types::ref} {}

    template < typename To >
    inline To arg::cast() const {
        if ( !can_cast<To>() ) {
            throw std::logic_error("bad argument cast");
        }

        if constexpr ( std::is_pointer_v<To> ) {
            return *static_cast<To*>(data_);
        }

        if constexpr ( std::is_reference_v<To> ) {
            using raw_type = stdex::remove_cvref_t<To>;

            if constexpr ( std::is_lvalue_reference_v<To> ) {
                return *static_cast<raw_type*>(data_);
            }

            if constexpr ( std::is_rvalue_reference_v<To> ) {
                return std::move(*static_cast<raw_type*>(data_));
            }
        }

        if constexpr ( !std::is_pointer_v<To> && !std::is_reference_v<To> ) {
            using raw_type = stdex::remove_cvref_t<To>;

            if ( ref_type() == ref_types::ref ) {
                if constexpr ( std::is_constructible_v<To, raw_type&> ) {
                    return To{*static_cast<raw_type*>(data_)};
                }
            }

            if ( ref_type() == ref_types::cref ) {
                if constexpr ( std::is_constructible_v<To, const raw_type&> ) {
                    return To{std::as_const(*static_cast<raw_type*>(data_))};
                }
            }

            if ( ref_type() == ref_types::rref ) {
                if constexpr ( std::is_constructible_v<To, raw_type&&> ) {
                    return To{std::move(*static_cast<raw_type*>(data_))};
                }
            }

            if ( ref_type() == ref_types::crref ) {
                if constexpr ( std::is_constructible_v<To, const raw_type&&> ) {
                    return To{std::move(std::as_const(*static_cast<raw_type*>(data_)))};
                }
            }
        }

        throw std::logic_error("bad argument cast");
    }

    template < typename To >
    inline bool arg::can_cast() const noexcept {
        if constexpr ( std::is_pointer_v<To> ) {
            using to_raw_type = std::remove_cv_t<To>;
            using to_raw_ptr_type = std::remove_cv_t<std::remove_pointer_t<to_raw_type>>;
            return raw_type().id() == type_db::get<to_raw_type>().id()
                || raw_type().id() == type_db::get<std::add_pointer_t<to_raw_ptr_type>>().id();
        }

        if constexpr ( std::is_lvalue_reference_v<To> ) {
            constexpr bool to_const = std::is_const_v<std::remove_reference_t<To>>;

            if ( !to_const && is_rvalue() ) {
                return false;
            }

            if ( !to_const && is_const() ) {
                return false;
            }

            using to_raw_type = stdex::remove_cvref_t<To>;
            if ( raw_type().id() == type_db::get<to_raw_type>().id() ) {
                return true;
            }

            if constexpr ( to_const && std::is_pointer_v<to_raw_type> ) {
                using to_raw_ptr_type = std::remove_cv_t<std::remove_pointer_t<to_raw_type>>;
                return raw_type().id() == type_db::get<std::add_pointer_t<to_raw_ptr_type>>().id();
            }

            return false;
        }

        if constexpr ( std::is_rvalue_reference_v<To> ) {
            constexpr bool to_const = std::is_const_v<std::remove_reference_t<To>>;

            if ( !is_rvalue() ) {
                return false;
            }

            if ( !to_const && is_const() ) {
                return false;
            }

            using to_raw_type = stdex::remove_cvref_t<To>;
            if ( raw_type().id() == type_db::get<to_raw_type>().id() ) {
                return true;
            }

            if constexpr ( to_const && std::is_pointer_v<to_raw_type> ) {
                using to_raw_ptr_type = std::remove_cv_t<std::remove_pointer_t<to_raw_type>>;
                return raw_type().id() == type_db::get<std::add_pointer_t<to_raw_ptr_type>>().id();
            }

            return false;
        }

        if constexpr ( !std::is_pointer_v<To> && !std::is_reference_v<To> ) {
            using to_raw_type = std::remove_cv_t<To>;
            if ( raw_type().id() != type_db::get<to_raw_type>().id() ) {
                return false;
            }

            return (ref_type() == ref_types::ref && std::is_constructible_v<To, to_raw_type&>)
                || (ref_type() == ref_types::cref && std::is_constructible_v<To, const to_raw_type&>)
                || (ref_type() == ref_types::rref && std::is_constructible_v<To, to_raw_type&&>)
                || (ref_type() == ref_types::crref && std::is_constructible_v<To, const to_raw_type&&>);
        }
    }

    inline any_type arg::raw_type() const noexcept {
        return raw_type_;
    }

    inline arg::ref_types arg::ref_type() const noexcept {
        return ref_type_;
    }

    inline bool arg::is_const() const noexcept {
        return ref_type_ == ref_types::cref
            || ref_type_ == ref_types::crref;
    }

    inline bool arg::is_lvalue() const noexcept {
        return ref_type_ == ref_types::ref
            || ref_type_ == ref_types::cref;
    }

    inline bool arg::is_rvalue() const noexcept {
        return ref_type_ == ref_types::rref
            || ref_type_ == ref_types::crref;
    }
}
