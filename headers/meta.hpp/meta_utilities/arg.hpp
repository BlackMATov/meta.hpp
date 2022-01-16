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
    template < decay_value_kind T >
    // NOLINTNEXTLINE(readability-named-parameter)
    arg_base::arg_base(T&&)
    : arg_base{type_list<T&&>{}} {}

    template < decay_non_uvalue_kind T >
    // NOLINTNEXTLINE(readability-named-parameter)
    arg_base::arg_base(T&&)
    : arg_base{type_list<T&&>{}} {}

    template < arg_lvalue_ref_kind T >
        requires decay_non_uvalue_kind<T>
    // NOLINTNEXTLINE(readability-named-parameter)
    arg_base::arg_base(type_list<T>)
    : ref_type_{std::is_const_v<std::remove_reference_t<T>>
        ? ref_types::const_lvalue
        : ref_types::lvalue}
    , raw_type_{resolve_type<std::remove_cvref_t<T>>()} {}

    template < arg_rvalue_ref_kind T >
        requires decay_non_uvalue_kind<T>
    // NOLINTNEXTLINE(readability-named-parameter)
    arg_base::arg_base(type_list<T>)
    : ref_type_{std::is_const_v<std::remove_reference_t<T>>
        ? ref_types::const_rvalue
        : ref_types::rvalue}
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
    // NOLINTNEXTLINE(readability-function-cognitive-complexity)
    bool arg_base::can_cast_to() const noexcept {
        using to_raw_type_cv = std::remove_reference_t<To>;
        using to_raw_type = std::remove_cv_t<to_raw_type_cv>;

        static_assert(
            !(std::is_reference_v<To> && std::is_pointer_v<to_raw_type>),
            "references to pointers are not supported yet");

        const any_type& from_type = get_raw_type();
        const any_type& to_type = resolve_type<to_raw_type>();

        const auto is_a = [](const any_type& base, const any_type& derived){
            return (base == derived)
                || (base.is_class() && derived.is_class() && base.as_class().is_base_of(derived.as_class()));
        };

        if constexpr ( std::is_pointer_v<To> ) {
            if ( to_type.is_pointer() && from_type.is_nullptr() ) {
                return true;
            }

            if ( to_type.is_pointer() && from_type.is_array() ) {
                const pointer_type& to_type_ptr = to_type.as_pointer();
                const bool to_type_ptr_readonly = to_type_ptr.get_flags().has(pointer_flags::is_readonly);

                const array_type& from_type_array = from_type.as_array();
                const bool from_type_array_readonly = is_const();

                const any_type& to_data_type = to_type_ptr.get_data_type();
                const any_type& from_data_type = from_type_array.get_data_type();

                if ( to_type_ptr_readonly >= from_type_array_readonly ) {
                    if ( to_data_type.is_void() || is_a(to_data_type, from_data_type) ) {
                        return true;
                    }
                }
            }

            if ( to_type.is_pointer() && from_type.is_pointer() ) {
                const pointer_type& to_type_ptr = to_type.as_pointer();
                const bool to_type_ptr_readonly = to_type_ptr.get_flags().has(pointer_flags::is_readonly);

                const pointer_type& from_type_ptr = from_type.as_pointer();
                const bool from_type_ptr_readonly = from_type_ptr.get_flags().has(pointer_flags::is_readonly);

                const any_type& to_data_type = to_type_ptr.get_data_type();
                const any_type& from_data_type = from_type_ptr.get_data_type();

                if ( to_type_ptr_readonly >= from_type_ptr_readonly ) {
                    if ( to_data_type.is_void() || is_a(to_data_type, from_data_type) ) {
                        return true;
                    }
                }
            }
        }

        if constexpr ( std::is_reference_v<To> ) {
            const auto is_convertible = [this](){
                switch ( get_ref_type() ) {
                case ref_types::lvalue:
                    return std::is_convertible_v<noncopyable&, copy_cvref_t<To, noncopyable>>;
                case ref_types::const_lvalue:
                    return std::is_convertible_v<const noncopyable&, copy_cvref_t<To, noncopyable>>;
                case ref_types::rvalue:
                    return std::is_convertible_v<noncopyable&&, copy_cvref_t<To, noncopyable>>;
                case ref_types::const_rvalue:
                    return std::is_convertible_v<const noncopyable&&, copy_cvref_t<To, noncopyable>>;
                default:
                    return false;
                }
            };

            if ( is_a(to_type, from_type) && is_convertible() ) {
                return true;
            }
        }

        if constexpr ( !std::is_pointer_v<To> && !std::is_reference_v<To> ) {
            const auto is_constructible = [this](){
                switch ( get_ref_type() ) {
                case ref_types::lvalue:
                    return std::is_constructible_v<To, to_raw_type&> && can_cast_to<to_raw_type&>();
                case ref_types::const_lvalue:
                    return std::is_constructible_v<To, const to_raw_type&> && can_cast_to<const to_raw_type&>();
                case ref_types::rvalue:
                    return std::is_constructible_v<To, to_raw_type&&> && can_cast_to<to_raw_type&&>();
                case ref_types::const_rvalue:
                    return std::is_constructible_v<To, const to_raw_type&&> && can_cast_to<const to_raw_type&&>();
                default:
                    return false;
                }
            };

            if ( is_a(to_type, from_type) && is_constructible() ) {
                return true;
            }
        }

        return false;
    }
}

namespace meta_hpp::detail
{
    template < decay_value_kind T >
    arg::arg(T&& v)
    : arg_base{std::forward<T>(v)}
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
    , data_{const_cast<void*>(v.data())} {}

    template < decay_non_uvalue_kind T >
    arg::arg(T&& v)
    : arg_base{std::forward<T>(v)}
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
    , data_{const_cast<std::remove_cvref_t<T>*>(std::addressof(v))} {}

    template < typename To >
    // NOLINTNEXTLINE(readability-function-cognitive-complexity)
    To arg::cast() const {
        if ( !can_cast_to<To>() ) {
            throw std::logic_error("bad argument cast");
        }

        using to_raw_type_cv = std::remove_reference_t<To>;
        using to_raw_type = std::remove_cv_t<to_raw_type_cv>;

        const any_type& from_type = get_raw_type();
        const any_type& to_type = resolve_type<to_raw_type>();

        if constexpr ( std::is_pointer_v<To> ) {
            if ( to_type.is_pointer() && from_type.is_nullptr() ) {
                return static_cast<to_raw_type_cv>(nullptr);
            }

            if ( to_type.is_pointer() && from_type.is_array() ) {
                const pointer_type& to_type_ptr = to_type.as_pointer();
                const array_type& from_type_array = from_type.as_array();

                const any_type& to_data_type = to_type_ptr.get_data_type();
                const any_type& from_data_type = from_type_array.get_data_type();

                if ( to_data_type.is_void() || to_data_type == from_data_type ) {
                    return static_cast<to_raw_type_cv>(data_);
                }

                if ( to_data_type.is_class() && from_data_type.is_class() ) {
                    const class_type& to_data_class = to_data_type.as_class();
                    const class_type& from_data_class = from_data_type.as_class();

                    void* to_ptr = detail::pointer_upcast(data_, from_data_class, to_data_class);
                    return static_cast<to_raw_type_cv>(to_ptr);
                }
            }

            if ( to_type.is_pointer() && from_type.is_pointer() ) {
                const pointer_type& to_type_ptr = to_type.as_pointer();
                const pointer_type& from_type_ptr = from_type.as_pointer();

                const any_type& to_data_type = to_type_ptr.get_data_type();
                const any_type& from_data_type = from_type_ptr.get_data_type();

                void** from_data_ptr = static_cast<void**>(data_);

                if ( to_data_type.is_void() || to_data_type == from_data_type ) {
                    return static_cast<to_raw_type_cv>(*from_data_ptr);
                }

                if ( to_data_type.is_class() && from_data_type.is_class() ) {
                    const class_type& to_data_class = to_data_type.as_class();
                    const class_type& from_data_class = from_data_type.as_class();

                    void* to_ptr = detail::pointer_upcast(*from_data_ptr, from_data_class, to_data_class);
                    return static_cast<to_raw_type_cv>(to_ptr);
                }
            }
        }

        if constexpr ( std::is_reference_v<To> ) {
            if ( to_type == from_type ) {
                if constexpr ( std::is_lvalue_reference_v<To> ) {
                    return *static_cast<to_raw_type_cv*>(data_);
                }

                if constexpr ( std::is_rvalue_reference_v<To> ) {
                    return std::move(*static_cast<to_raw_type_cv*>(data_));
                }
            }

            if ( to_type.is_class() && from_type.is_class() ) {
                const class_type& to_class = to_type.as_class();
                const class_type& from_class = from_type.as_class();

                void* to_ptr = detail::pointer_upcast(data_, from_class, to_class);

                if constexpr ( std::is_lvalue_reference_v<To> ) {
                    return *static_cast<to_raw_type_cv*>(to_ptr);
                }

                if constexpr ( std::is_rvalue_reference_v<To> ) {
                    return std::move(*static_cast<to_raw_type_cv*>(to_ptr));
                }
            }
        }

        if constexpr ( !std::is_pointer_v<To> && !std::is_reference_v<To> ) {
            if constexpr ( std::is_constructible_v<To, to_raw_type&> ) {
                if ( get_ref_type() == ref_types::lvalue ) {
                    return To{cast<to_raw_type&>()};
                }
            }

            if constexpr ( std::is_constructible_v<To, const to_raw_type&> ) {
                if ( get_ref_type() == ref_types::const_lvalue ) {
                    return To{cast<const to_raw_type&>()};
                }
            }

            if constexpr ( std::is_constructible_v<To, to_raw_type&&> ) {
                if ( get_ref_type() == ref_types::rvalue ) {
                    return To{cast<to_raw_type&&>()};
                }
            }

            if constexpr ( std::is_constructible_v<To, const to_raw_type&&> ) {
                if ( get_ref_type() == ref_types::const_rvalue ) {
                    return To{cast<const to_raw_type&&>()};
                }
            }
        }

        throw std::logic_error("bad argument cast");
    }
}
