/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../../meta_base.hpp"
#include "../../meta_registry.hpp"
#include "../../meta_uresult.hpp"
#include "../../meta_uvalue.hpp"

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
        uinst_base() = default;
        ~uinst_base() = default;

        uinst_base(uinst_base&&) = default;
        uinst_base& operator=(uinst_base&&) = default;

        uinst_base(const uinst_base&) = delete;
        uinst_base& operator=(const uinst_base&) = delete;

        template < typename T, typename Tp = std::decay_t<T> >
            requires(!uvalue_family<Tp>)
        // NOLINTNEXTLINE(*-missing-std-forward)
        explicit uinst_base(type_registry& registry, T&&)
        : uinst_base{registry, type_list<T&&>{}} {}

        template < inst_class_lvalue_ref_kind T >
        explicit uinst_base(type_registry& registry, type_list<T>)
        : ref_type_{std::is_const_v<std::remove_reference_t<T>> ? ref_types::const_lvalue : ref_types::lvalue}
        , raw_type_{registry.resolve_by_type<std::remove_cvref_t<T>>()} {}

        template < inst_class_rvalue_ref_kind T >
        explicit uinst_base(type_registry& registry, type_list<T>)
        : ref_type_{std::is_const_v<std::remove_reference_t<T>> ? ref_types::const_rvalue : ref_types::rvalue}
        , raw_type_{registry.resolve_by_type<std::remove_cvref_t<T>>()} {}

        explicit uinst_base(type_registry&, uvalue& v)
        : ref_type_{ref_types::lvalue}
        , raw_type_{v.get_type()} {}

        explicit uinst_base(type_registry&, const uvalue& v)
        : ref_type_{ref_types::const_lvalue}
        , raw_type_{v.get_type()} {}

        // NOLINTNEXTLINE(*-param-not-moved)
        explicit uinst_base(type_registry&, uvalue&& v)
        : ref_type_{ref_types::rvalue}
        , raw_type_{v.get_type()} {}

        explicit uinst_base(type_registry&, const uvalue&& v)
        : ref_type_{ref_types::const_rvalue}
        , raw_type_{v.get_type()} {}

        template < typename T, typename Tp = std::decay_t<T> >
            requires std::is_same_v<Tp, uresult>
        explicit uinst_base(type_registry& registry, T&& v)
        : uinst_base{registry, *std::forward<T>(v)} {}

        [[nodiscard]] bool is_inst_const() const noexcept {
            if ( raw_type_.is_pointer() ) {
                const pointer_type& from_type_ptr = raw_type_.as_pointer();
                const bool from_type_ptr_readonly = from_type_ptr.get_flags().has(pointer_flags::is_readonly);
                return from_type_ptr_readonly;
            }
            return ref_type_ == ref_types::const_lvalue //
                || ref_type_ == ref_types::const_rvalue;
        }

        [[nodiscard]] ref_types get_ref_type() const noexcept {
            return ref_type_;
        }

        [[nodiscard]] any_type get_raw_type() const noexcept {
            return raw_type_;
        }

        template < inst_class_ref_kind Q >
        [[nodiscard]] bool can_cast_to(type_registry& registry) const noexcept;

    private:
        ref_types ref_type_{};
        any_type raw_type_{};
    };
}

namespace meta_hpp::detail
{
    class uinst final : public uinst_base {
    public:
        uinst() = default;
        ~uinst() = default;

        uinst(uinst&&) = default;
        uinst& operator=(uinst&&) = default;

        uinst(const uinst&) = delete;
        uinst& operator=(const uinst&) = delete;

        template < typename T, typename Tp = std::decay_t<T> >
            requires std::is_same_v<Tp, uvalue>
        explicit uinst(type_registry& registry, T&& v)
        : uinst_base{registry, std::forward<T>(v)}
        , data_{const_cast<void*>(v.get_data())} { // NOLINT(*-const-cast)
            // there is no 'use after move' here because
            // 'uinst_base' doesn't actually move 'v', just gets its type
        }

        template < typename T, typename Tp = std::decay_t<T> >
            requires std::is_same_v<Tp, uresult>
        explicit uinst(type_registry& registry, T&& v)
        : uinst_base{registry, std::forward<T>(v)}
        , data_{const_cast<void*>(v->get_data())} { // NOLINT(*-const-cast)
            // there is no 'use after move' here because
            // 'uinst_base' doesn't actually move 'v', just gets its type
        }

        template < typename T, typename Tp = std::decay_t<T> >
            requires(!uvalue_family<Tp>)
        explicit uinst(type_registry& registry, T&& v)
        : uinst_base{registry, std::forward<T>(v)}
        , data_{const_cast<std::remove_cvref_t<T>*>(std::addressof(v))} { // NOLINT(*-const-cast)
            // there is no 'use after move' here because
            // 'uinst_base' doesn't actually move 'v', just gets its type
        }

        template < inst_class_ref_kind Q >
        [[nodiscard]] decltype(auto) cast(type_registry& registry) const;

    private:
        void* data_{};
    };
}

namespace meta_hpp::detail
{
    template < inst_class_ref_kind Q >
    bool uinst_base::can_cast_to(type_registry& registry) const noexcept {
        using inst_class = typename inst_traits<Q>::class_type;
        using inst_method = typename inst_traits<Q>::method_type;

        const any_type& from_type = get_raw_type();
        const any_type& to_type = registry.resolve_by_type<inst_class>();

        if ( from_type.is_class() ) {
            const auto is_invocable = [this]() {
                switch ( get_ref_type() ) {
                case ref_types::lvalue:
                    return std::is_invocable_v<inst_method, inst_class&>;
                case ref_types::const_lvalue:
                    return std::is_invocable_v<inst_method, const inst_class&>;
                case ref_types::rvalue:
                    return std::is_invocable_v<inst_method, inst_class&&>;
                case ref_types::const_rvalue:
                    return std::is_invocable_v<inst_method, const inst_class&&>;
                }
                return false;
            };

            return is_invocable() && is_a(to_type, from_type);
        }

        if ( from_type.is_pointer() ) {
            const pointer_type& from_type_ptr = from_type.as_pointer();
            const bool from_type_ptr_readonly = from_type_ptr.get_flags().has(pointer_flags::is_readonly);
            const any_type& from_data_type = from_type_ptr.get_data_type();

            const auto is_invocable = [from_type_ptr_readonly]() {
                return from_type_ptr_readonly ? std::is_invocable_v<inst_method, const inst_class&>
                                              : std::is_invocable_v<inst_method, inst_class&>;
            };

            return is_invocable() && is_a(to_type, from_data_type);
        }

        return false;
    }
}

namespace meta_hpp::detail
{
    template < inst_class_ref_kind Q >
    decltype(auto) uinst::cast(type_registry& registry) const {
        META_HPP_DEV_ASSERT(can_cast_to<Q>(registry) && "bad instance cast");

        using inst_class_cv = std::remove_reference_t<Q>;
        using inst_class = std::remove_cv_t<inst_class_cv>;

        const any_type& from_type = get_raw_type();
        const any_type& to_type = registry.resolve_by_type<inst_class>();

        if ( from_type.is_class() && to_type.is_class() ) {
            void* to_ptr = pointer_upcast( //
                data_,
                from_type.as_class(),
                to_type.as_class()
            );
            META_HPP_ASSERT(to_ptr);

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

        if ( from_type.is_pointer() ) {
            const pointer_type& from_type_ptr = from_type.as_pointer();
            const any_type& from_data_type = from_type_ptr.get_data_type();

            if ( from_data_type.is_class() && to_type.is_class() ) {
                void* to_ptr = pointer_upcast( //
                    *static_cast<void**>(data_),
                    from_data_type.as_class(),
                    to_type.as_class()
                );
                META_HPP_ASSERT(to_ptr);

                if constexpr ( !std::is_reference_v<Q> ) {
                    return *static_cast<inst_class_cv*>(to_ptr);
                }

                if constexpr ( std::is_lvalue_reference_v<Q> ) {
                    return *static_cast<inst_class_cv*>(to_ptr);
                }
            }
        }

        throw_exception(error_code::bad_instance_cast);
    }
}
