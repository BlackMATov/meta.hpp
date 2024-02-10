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
    template < typename T >
    concept uarg_cast_to_object = !pointer_kind<T> || function_pointer_kind<T>;

    template < typename T >
    concept uarg_cast_to_pointer = pointer_kind<T> && !function_pointer_kind<T>;
}

namespace meta_hpp::detail
{
    class uarg_base {
    public:
        enum class ref_types {
            lvalue,
            const_lvalue,
            rvalue,
            const_rvalue,
        };

    public:
        uarg_base() = default;
        ~uarg_base() = default;

        uarg_base(uarg_base&&) = default;
        uarg_base& operator=(uarg_base&&) = default;

        uarg_base(const uarg_base&) = delete;
        uarg_base& operator=(const uarg_base&) = delete;

        template < typename T, typename Tp = std::decay_t<T> >
            requires(!uvalue_family<Tp>)
        // NOLINTNEXTLINE(*-missing-std-forward)
        explicit uarg_base(type_registry& registry, T&&)
        : uarg_base{registry, type_list<T&&>{}} {}

        template < arg_lvalue_ref_kind T >
        explicit uarg_base(type_registry& registry, type_list<T>)
        : ref_type_{std::is_const_v<std::remove_reference_t<T>> ? ref_types::const_lvalue : ref_types::lvalue}
        , raw_type_{registry.resolve_by_type<std::remove_cvref_t<T>>()} {}

        template < arg_rvalue_ref_kind T >
        explicit uarg_base(type_registry& registry, type_list<T>)
        : ref_type_{std::is_const_v<std::remove_reference_t<T>> ? ref_types::const_rvalue : ref_types::rvalue}
        , raw_type_{registry.resolve_by_type<std::remove_cvref_t<T>>()} {}

        explicit uarg_base(type_registry&, uvalue& v)
        : ref_type_{ref_types::lvalue}
        , raw_type_{v.get_type()} {}

        explicit uarg_base(type_registry&, const uvalue& v)
        : ref_type_{ref_types::const_lvalue}
        , raw_type_{v.get_type()} {}

        // NOLINTNEXTLINE(*-param-not-moved)
        explicit uarg_base(type_registry&, uvalue&& v)
        : ref_type_{ref_types::rvalue}
        , raw_type_{v.get_type()} {}

        explicit uarg_base(type_registry&, const uvalue&& v)
        : ref_type_{ref_types::const_rvalue}
        , raw_type_{v.get_type()} {}

        template < typename T, typename Tp = std::decay_t<T> >
            requires std::is_same_v<Tp, uresult>
        explicit uarg_base(type_registry& registry, T&& v)
        : uarg_base{registry, *std::forward<T>(v)} {}

        [[nodiscard]] bool is_ref_const() const noexcept {
            return ref_type_ == ref_types::const_lvalue //
                || ref_type_ == ref_types::const_rvalue;
        }

        [[nodiscard]] ref_types get_ref_type() const noexcept {
            return ref_type_;
        }

        [[nodiscard]] any_type get_raw_type() const noexcept {
            return raw_type_;
        }

        template < uarg_cast_to_pointer To >
        [[nodiscard]] bool can_cast_to(type_registry& registry) const noexcept;

        template < uarg_cast_to_object To >
        [[nodiscard]] bool can_cast_to(type_registry& registry) const noexcept;

    private:
        ref_types ref_type_{};
        any_type raw_type_{};
    };
}

namespace meta_hpp::detail
{
    class uarg final : public uarg_base {
    public:
        uarg() = default;
        ~uarg() = default;

        uarg(uarg&&) = default;
        uarg& operator=(uarg&&) = default;

        uarg(const uarg&) = delete;
        uarg& operator=(const uarg&) = delete;

        template < typename T, typename Tp = std::decay_t<T> >
            requires std::is_same_v<Tp, uvalue>
        explicit uarg(type_registry& registry, T&& v)
        : uarg_base{registry, std::forward<T>(v)}
        , data_{const_cast<void*>(v.get_data())} { // NOLINT(*-const-cast)
            // there is no 'use after move' here because
            // 'uarg_base' doesn't actually move 'v', just gets its type
        }

        template < typename T, typename Tp = std::decay_t<T> >
            requires std::is_same_v<Tp, uresult>
        explicit uarg(type_registry& registry, T&& v)
        : uarg_base{registry, std::forward<T>(v)}
        , data_{const_cast<void*>(v->get_data())} { // NOLINT(*-const-cast)
            // there is no 'use after move' here because
            // 'uarg_base' doesn't actually move 'v', just gets its type
        }

        template < typename T, typename Tp = std::decay_t<T> >
            requires(!uvalue_family<Tp>)
        explicit uarg(type_registry& registry, T&& v)
        : uarg_base{registry, std::forward<T>(v)}
        , data_{const_cast<std::remove_cvref_t<T>*>(std::addressof(v))} { // NOLINT(*-const-cast)
            // there is no 'use after move' here because
            // 'uarg_base' doesn't actually move 'v', just gets its type
        }

        template < uarg_cast_to_pointer To >
        [[nodiscard]] decltype(auto) cast(type_registry& registry) const;

        template < uarg_cast_to_object To >
        [[nodiscard]] decltype(auto) cast(type_registry& registry) const;

    private:
        void* data_{};
    };
}

namespace meta_hpp::detail
{
    template < uarg_cast_to_pointer To >
    [[nodiscard]] bool uarg_base::can_cast_to(type_registry& registry) const noexcept {
        using to_raw_type = std::remove_cv_t<To>;

        const any_type& from_type = get_raw_type();
        const pointer_type& to_type_ptr = registry.resolve_by_type<to_raw_type>();

        if ( from_type.is_nullptr() ) {
            return true;
        }

        if ( from_type.is_array() ) {
            const array_type& from_type_array = from_type.as_array();

            const bool to_type_ptr_readonly = to_type_ptr.get_flags().has(pointer_flags::is_readonly);
            const bool from_type_array_readonly = is_ref_const();

            const any_type& to_data_type = to_type_ptr.get_data_type();
            const any_type& from_data_type = from_type_array.get_data_type();

            if ( to_type_ptr_readonly >= from_type_array_readonly ) {
                if ( to_data_type.is_void() || is_a(to_data_type, from_data_type) ) {
                    return true;
                }
            }
        }

        if ( from_type.is_pointer() ) {
            const pointer_type& from_type_ptr = from_type.as_pointer();

            const bool to_type_ptr_readonly = to_type_ptr.get_flags().has(pointer_flags::is_readonly);
            const bool from_type_ptr_readonly = from_type_ptr.get_flags().has(pointer_flags::is_readonly);

            const any_type& to_data_type = to_type_ptr.get_data_type();
            const any_type& from_data_type = from_type_ptr.get_data_type();

            if ( to_type_ptr_readonly >= from_type_ptr_readonly ) {
                if ( to_data_type.is_void() || is_a(to_data_type, from_data_type) ) {
                    return true;
                }
            }
        }

        return false;
    }

    template < uarg_cast_to_object To >
    [[nodiscard]] bool uarg_base::can_cast_to(type_registry& registry) const noexcept {
        using to_raw_type_cv = std::remove_reference_t<To>;
        using to_raw_type = std::remove_cv_t<to_raw_type_cv>;

        static_assert( //
            !(std::is_reference_v<To> && pointer_kind<to_raw_type>),
            "references to pointers are not supported yet"
        );

        const any_type& from_type = get_raw_type();
        const any_type& to_type = registry.resolve_by_type<to_raw_type>();

        const auto is_convertible_to_ref = [this]<typename ToRef>(type_list<ToRef>) {
            switch ( get_ref_type() ) {
            case ref_types::lvalue:
                return std::is_convertible_v<noncopyable&, copy_cvref_t<ToRef, noncopyable>>;
            case ref_types::const_lvalue:
                return std::is_convertible_v<const noncopyable&, copy_cvref_t<ToRef, noncopyable>>;
            case ref_types::rvalue:
                return std::is_convertible_v<noncopyable&&, copy_cvref_t<ToRef, noncopyable>>;
            case ref_types::const_rvalue:
                return std::is_convertible_v<const noncopyable&&, copy_cvref_t<ToRef, noncopyable>>;
            }
            return false;
        };

        const auto is_constructible_from_type = [this, &is_convertible_to_ref]<typename FromType>(type_list<FromType>) {
            switch ( get_ref_type() ) {
            case ref_types::lvalue:
                return std::is_constructible_v<To, FromType&> && is_convertible_to_ref(type_list<FromType&>{});
            case ref_types::const_lvalue:
                return std::is_constructible_v<To, const FromType&> && is_convertible_to_ref(type_list<const FromType&>{});
            case ref_types::rvalue:
                return std::is_constructible_v<To, FromType&&> && is_convertible_to_ref(type_list<FromType&&>{});
            case ref_types::const_rvalue:
                return std::is_constructible_v<To, const FromType&&> && is_convertible_to_ref(type_list<const FromType&&>{});
            }
            return false;
        };

        if constexpr ( std::is_reference_v<To> ) {
            if ( is_a(to_type, from_type) && is_convertible_to_ref(type_list<To>{}) ) {
                return true;
            }
        }

        if constexpr ( !std::is_reference_v<To> ) {
            if ( is_a(to_type, from_type) && is_constructible_from_type(type_list<to_raw_type>{}) ) {
                return true;
            }
        }

        return false;
    }
}

namespace meta_hpp::detail
{
    template < uarg_cast_to_pointer To >
    [[nodiscard]] decltype(auto) uarg::cast(type_registry& registry) const {
        META_HPP_DEV_ASSERT(can_cast_to<To>(registry) && "bad argument cast");

        using to_raw_type = std::remove_cv_t<To>;

        const any_type& from_type = get_raw_type();
        const pointer_type& to_type_ptr = registry.resolve_by_type<to_raw_type>();

        if ( from_type.is_nullptr() ) {
            return static_cast<To>(nullptr);
        }

        if ( from_type.is_array() ) {
            const array_type& from_type_array = from_type.as_array();

            void* to_ptr = pointer_upcast( //
                data_,
                from_type_array.get_data_type(),
                to_type_ptr.get_data_type()
            );
            META_HPP_ASSERT(to_ptr);

            return static_cast<To>(to_ptr);
        }

        if ( from_type.is_pointer() ) {
            const pointer_type& from_type_ptr = from_type.as_pointer();

            void* to_ptr = pointer_upcast( //
                *static_cast<void**>(data_),
                from_type_ptr.get_data_type(),
                to_type_ptr.get_data_type()
            );
            META_HPP_ASSERT(to_ptr);

            return static_cast<To>(to_ptr);
        }

        throw_exception(error_code::bad_argument_cast);
    }

    template < uarg_cast_to_object To >
    [[nodiscard]] decltype(auto) uarg::cast(type_registry& registry) const {
        META_HPP_DEV_ASSERT(can_cast_to<To>(registry) && "bad argument cast");

        using to_raw_type_cv = std::remove_reference_t<To>;
        using to_raw_type = std::remove_cv_t<to_raw_type_cv>;

        static_assert( //
            !(std::is_reference_v<To> && pointer_kind<to_raw_type>),
            "references to pointers are not supported yet"
        );

        const any_type& from_type = get_raw_type();
        const any_type& to_type = registry.resolve_by_type<to_raw_type>();

        void* to_ptr = pointer_upcast(data_, from_type, to_type);
        META_HPP_ASSERT(to_ptr);

        if constexpr ( std::is_lvalue_reference_v<To> ) {
            return *static_cast<to_raw_type_cv*>(to_ptr);
        }

        if constexpr ( std::is_rvalue_reference_v<To> ) {
            return std::move(*static_cast<to_raw_type_cv*>(to_ptr));
        }

        if constexpr ( !std::is_reference_v<To> ) {
            switch ( get_ref_type() ) {
            case ref_types::lvalue:
                if constexpr ( std::is_constructible_v<To, to_raw_type&> ) {
                    return To{*static_cast<to_raw_type*>(to_ptr)};
                }
                break;
            case ref_types::const_lvalue:
                if constexpr ( std::is_constructible_v<To, const to_raw_type&> ) {
                    return To{*static_cast<const to_raw_type*>(to_ptr)};
                }
                break;
            case ref_types::rvalue:
                if constexpr ( std::is_constructible_v<To, to_raw_type&&> ) {
                    return To{std::move(*static_cast<to_raw_type*>(to_ptr))};
                }
                break;
            case ref_types::const_rvalue:
                if constexpr ( std::is_constructible_v<To, const to_raw_type&&> ) {
                    return To{std::move(*static_cast<const to_raw_type*>(to_ptr))};
                }
                break;
            }
            throw_exception(error_code::bad_argument_cast);
        }
    }
}

namespace meta_hpp::detail
{
    template < typename ArgTypeList >
    bool can_cast_all_uargs(type_registry& registry, std::span<const uarg> args) noexcept {
        if ( args.size() != type_list_arity_v<ArgTypeList> ) {
            return false;
        }
        return [args, &registry]<std::size_t... Is>(std::index_sequence<Is...>) {
            return (... && args[Is].can_cast_to<type_list_at_t<Is, ArgTypeList>>(registry));
        }(std::make_index_sequence<type_list_arity_v<ArgTypeList>>());
    }

    template < typename ArgTypeList >
    bool can_cast_all_uargs(type_registry& registry, std::span<const uarg_base> args) noexcept {
        if ( args.size() != type_list_arity_v<ArgTypeList> ) {
            return false;
        }
        return [args, &registry]<std::size_t... Is>(std::index_sequence<Is...>) {
            return (... && args[Is].can_cast_to<type_list_at_t<Is, ArgTypeList>>(registry));
        }(std::make_index_sequence<type_list_arity_v<ArgTypeList>>());
    }

    template < typename ArgTypeList, typename F >
    auto unchecked_call_with_uargs(type_registry& registry, std::span<const uarg> args, F&& f) {
        META_HPP_DEV_ASSERT(args.size() == type_list_arity_v<ArgTypeList>);
        return [args, &registry]<std::size_t... Is>(auto&& captured_f, std::index_sequence<Is...>) {
            return std::invoke(META_HPP_FWD(captured_f), args[Is].cast<type_list_at_t<Is, ArgTypeList>>(registry)...);
        }(META_HPP_FWD(f), std::make_index_sequence<type_list_arity_v<ArgTypeList>>());
    }
}
