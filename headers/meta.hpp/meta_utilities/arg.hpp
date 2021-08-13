/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_fwd.hpp"

namespace meta_hpp
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
            std::is_pointer_v<T> || std::is_lvalue_reference_v<T>
        , int> = 0 >
        explicit arg_base(typename_arg_t<T>);

        template < typename T, std::enable_if_t<
            std::is_rvalue_reference_v<T> ||
            (!std::is_pointer_v<T> && !std::is_reference_v<T>)
        , int> = 0 >
        explicit arg_base(typename_arg_t<T>);

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
        std::is_pointer_v<T> || std::is_lvalue_reference_v<T>
    , int> >
    arg_base::arg_base(typename_arg_t<T>)
    : raw_type_{type_db::get<stdex::remove_cvref_t<T>>()}
    , ref_type_{std::is_const_v<std::remove_reference_t<T>> ? ref_types::cref : ref_types::ref} {}

    template < typename T, std::enable_if_t<
        std::is_rvalue_reference_v<T> ||
        (!std::is_pointer_v<T> && !std::is_reference_v<T>)
    , int> >
    arg_base::arg_base(typename_arg_t<T>)
    : raw_type_{type_db::get<stdex::remove_cvref_t<T>>()}
    , ref_type_{std::is_const_v<std::remove_reference_t<T>> ? ref_types::crref : ref_types::rref} {}

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

    inline any_type arg_base::raw_type() const noexcept {
        return raw_type_;
    }

    inline arg_base::ref_types arg_base::ref_type() const noexcept {
        return ref_type_;
    }

    template < typename To >
    bool arg_base::can_cast_to() const noexcept {
        if constexpr ( std::is_pointer_v<To> ) {
            using to_raw_type = std::remove_cv_t<To>;
            using to_raw_ptr_type = std::remove_cv_t<std::remove_pointer_t<to_raw_type>>;
            return raw_type() == type_db::get<to_raw_type>()
                || raw_type() == type_db::get<to_raw_ptr_type*>();
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

            using to_raw_type = stdex::remove_cvref_t<To>;
            if ( raw_type() == type_db::get<to_raw_type>() ) {
                return true;
            }

            if constexpr ( to_const && std::is_pointer_v<to_raw_type> ) {
                using to_raw_ptr_type = std::remove_cv_t<std::remove_pointer_t<to_raw_type>>;
                return raw_type() == type_db::get<to_raw_ptr_type*>();
            }

            return false;
        }

        if constexpr ( !std::is_pointer_v<To> && !std::is_reference_v<To> ) {
            using to_raw_type = std::remove_cv_t<To>;
            if ( raw_type() != type_db::get<to_raw_type>() ) {
                return false;
            }

            return (ref_type() == ref_types::ref && std::is_constructible_v<To, to_raw_type&>)
                || (ref_type() == ref_types::cref && std::is_constructible_v<To, const to_raw_type&>)
                || (ref_type() == ref_types::rref && std::is_constructible_v<To, to_raw_type&&>)
                || (ref_type() == ref_types::crref && std::is_constructible_v<To, const to_raw_type&&>);
        }
    }
}

namespace meta_hpp
{
    class arg final : public arg_base {
    public:
        arg() = delete;

        arg(arg&&) = delete;
        arg& operator=(arg&&) = delete;

        arg(const arg&) = delete;
        arg& operator=(const arg&) = delete;

        template < typename T >
        explicit arg(T&& v);

        template < typename To >
        To cast() const;
    private:
        void* data_{};
    };
}

namespace meta_hpp
{
    template < typename T >
    arg::arg(T&& v)
    : arg_base{typename_arg<T&&>}
    , data_{const_cast<stdex::remove_cvref_t<T>*>(std::addressof(v))} {}

    template < typename To >
    To arg::cast() const {
        if ( !can_cast_to<To>() ) {
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
            using raw_type = std::remove_cv_t<To>;

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
}
