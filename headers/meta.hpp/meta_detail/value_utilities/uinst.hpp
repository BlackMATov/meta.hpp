/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../../meta_base.hpp"
#include "../../meta_value.hpp"

#include "utraits.hpp"

namespace meta_hpp::detail
{
    class uinst_base {
    public:
        enum class ref_types {
            lvalue,
            const_lvalue,
            rvalue,
            const_rvalue,
        };
    public:
        uinst_base() = delete;

        uinst_base(uinst_base&&) = default;
        uinst_base(const uinst_base&) = default;

        uinst_base& operator=(uinst_base&&) = delete;
        uinst_base& operator=(const uinst_base&) = delete;

        virtual ~uinst_base() = default;

        template < decay_value_kind T >
        // NOLINTNEXTLINE(readability-named-parameter)
        explicit uinst_base(T&&)
        : uinst_base{type_list<T&&>{}} {}

        template < decay_non_uvalue_kind T >
        // NOLINTNEXTLINE(readability-named-parameter)
        explicit uinst_base(T&&)
        : uinst_base{type_list<T&&>{}} {}

        template < inst_class_lvalue_ref_kind T >
        // NOLINTNEXTLINE(readability-named-parameter)
        explicit uinst_base(type_list<T>)
        : ref_type_{std::is_const_v<std::remove_reference_t<T>>
            ? ref_types::const_lvalue
            : ref_types::lvalue}
        , raw_type_{resolve_type<std::remove_cvref_t<T>>()} {}

        template < inst_class_rvalue_ref_kind T >
        // NOLINTNEXTLINE(readability-named-parameter)
        explicit uinst_base(type_list<T>)
        : ref_type_{std::is_const_v<std::remove_reference_t<T>>
            ? ref_types::const_rvalue
            : ref_types::rvalue}
        , raw_type_{resolve_type<std::remove_cvref_t<T>>()} {}

        explicit uinst_base(uvalue& v)
        : ref_type_{ref_types::lvalue}
        , raw_type_{v.get_type()} {}

        explicit uinst_base(const uvalue& v)
        : ref_type_{ref_types::const_lvalue}
        , raw_type_{v.get_type()} {}

        explicit uinst_base(uvalue&& v)
        : ref_type_{ref_types::rvalue}
        , raw_type_{v.get_type()} {}

        explicit uinst_base(const uvalue&& v)
        : ref_type_{ref_types::const_rvalue}
        , raw_type_{v.get_type()} {}

        [[nodiscard]] bool is_const() const noexcept {
            return ref_type_ == ref_types::const_lvalue
                || ref_type_ == ref_types::const_rvalue;
        }

        [[nodiscard]] bool is_lvalue() const noexcept {
            return ref_type_ == ref_types::lvalue
                || ref_type_ == ref_types::const_lvalue;
        }

        [[nodiscard]] bool is_rvalue() const noexcept {
            return ref_type_ == ref_types::rvalue
                || ref_type_ == ref_types::const_rvalue;
        }

        [[nodiscard]] ref_types get_ref_type() const noexcept {
            return ref_type_;
        }

        [[nodiscard]] const any_type& get_raw_type() const noexcept {
            return raw_type_;
        }

        template < inst_class_ref_kind Q >
        [[nodiscard]] bool can_cast_to() const noexcept;
    private:
        ref_types ref_type_{};
        any_type raw_type_{};
    };
}

namespace meta_hpp::detail
{
    class uinst final : public uinst_base {
    public:
        uinst() = delete;

        uinst(uinst&&) = default;
        uinst(const uinst&) = default;

        uinst& operator=(uinst&&) = delete;
        uinst& operator=(const uinst&) = delete;

        ~uinst() override = default;

        template < decay_value_kind T >
        explicit uinst(T&& v)
        : uinst_base{std::forward<T>(v)}
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
        , data_{const_cast<void*>(v.data())} {}

        template < decay_non_uvalue_kind T >
        explicit uinst(T&& v)
        : uinst_base{std::forward<T>(v)}
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
        , data_{const_cast<std::remove_cvref_t<T>*>(std::addressof(v))} {}

        template < inst_class_ref_kind Q >
        [[nodiscard]] decltype(auto) cast() const;
    private:
        void* data_{};
    };
}

namespace meta_hpp::detail
{
    template < inst_class_ref_kind Q >
    bool uinst_base::can_cast_to() const noexcept {
        using inst_class = typename inst_traits<Q>::class_type;
        using inst_method = typename inst_traits<Q>::method_type;

        const any_type& from_type = get_raw_type();
        const any_type& to_type = resolve_type<inst_class>();

        const auto is_a = [](const any_type& base, const any_type& derived){
            return (base == derived)
                || (base.is_class() && derived.is_class() && base.as_class().is_base_of(derived.as_class()));
        };

        const auto is_invocable = [this](){
            switch ( get_ref_type() ) {
            case ref_types::lvalue:
                return std::is_invocable_v<inst_method, inst_class&>;
            case ref_types::const_lvalue:
                return std::is_invocable_v<inst_method, const inst_class&>;
            case ref_types::rvalue:
                return std::is_invocable_v<inst_method, inst_class&&>;
            case ref_types::const_rvalue:
                return std::is_invocable_v<inst_method, const inst_class&&>;
            default:
                return false;
            }
        };

        return is_a(to_type, from_type) && is_invocable();
    }
}

namespace meta_hpp::detail
{
    template < inst_class_ref_kind Q >
    decltype(auto) uinst::cast() const {
        if ( !can_cast_to<Q>() ) {
            throw_exception_with("bad instance cast");
        }

        using inst_class_cv = std::remove_reference_t<Q>;
        using inst_class = std::remove_cv_t<inst_class_cv>;

        const any_type& from_type = get_raw_type();
        const any_type& to_type = resolve_type<inst_class>();

        if ( from_type.is_class() && to_type.is_class() ) {
            const class_type& from_class = from_type.as_class();
            const class_type& to_class = to_type.as_class();

            void* to_ptr = pointer_upcast(data_, from_class, to_class);

            if constexpr ( !std::is_reference_v<Q> ) {
                return *static_cast<inst_class_cv*>(to_ptr);
            }

            if constexpr ( std::is_lvalue_reference_v<Q> ) {
                return *static_cast<inst_class_cv*>(to_ptr);
            }

            if constexpr ( std::is_rvalue_reference_v<Q> ) {
                return std::move(*static_cast<inst_class_cv*>(to_ptr));
            }
        }

        throw_exception_with("bad instance cast");
    }
}
