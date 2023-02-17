/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../../meta_base.hpp"
#include "../../meta_registry.hpp"
#include "../../meta_uvalue.hpp"

#include "utraits.hpp"

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
        uarg_base(const uarg_base&) = default;

        uarg_base& operator=(uarg_base&&) = delete;
        uarg_base& operator=(const uarg_base&) = delete;

        template < typename T, typename Tp = std::decay_t<T> >
            requires(!any_uvalue_kind<Tp>)
        explicit uarg_base(type_registry& registry, T&&)
        : uarg_base{registry, type_list<T&&>{}} {}

        template < arg_lvalue_ref_kind T >
        explicit uarg_base(type_registry& registry, type_list<T>)
        : ref_type_{std::is_const_v<std::remove_reference_t<T>> ? ref_types::const_lvalue : ref_types::lvalue}
        , raw_type_{registry.resolve_type<std::remove_cvref_t<T>>()} {}

        template < arg_rvalue_ref_kind T >
        explicit uarg_base(type_registry& registry, type_list<T>)
        : ref_type_{std::is_const_v<std::remove_reference_t<T>> ? ref_types::const_rvalue : ref_types::rvalue}
        , raw_type_{registry.resolve_type<std::remove_cvref_t<T>>()} {}

        explicit uarg_base(type_registry&, uvalue& v)
        : ref_type_{ref_types::lvalue}
        , raw_type_{v.get_type()} {}

        explicit uarg_base(type_registry&, const uvalue& v)
        : ref_type_{ref_types::const_lvalue}
        , raw_type_{v.get_type()} {}

        explicit uarg_base(type_registry&, uvalue&& v)
        : ref_type_{ref_types::rvalue}
        , raw_type_{v.get_type()} {}

        explicit uarg_base(type_registry&, const uvalue&& v)
        : ref_type_{ref_types::const_rvalue}
        , raw_type_{v.get_type()} {}

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

        template < typename To >
            requires(std::is_pointer_v<To>)
        [[nodiscard]] bool can_cast_to(type_registry& registry) const noexcept;

        template < typename To >
            requires(!std::is_pointer_v<To>)
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
        uarg(const uarg&) = default;

        uarg& operator=(uarg&&) = delete;
        uarg& operator=(const uarg&) = delete;

        template < typename T, typename Tp = std::decay_t<T> >
            requires std::is_same_v<Tp, uvalue>
        explicit uarg(type_registry& registry, T&& v)
        : uarg_base{registry, std::forward<T>(v)}
        , data_{const_cast<void*>(v.get_data())} {} // NOLINT(*-const-cast)

        template < typename T, typename Tp = std::decay_t<T> >
            requires(!any_uvalue_kind<Tp>)
        explicit uarg(type_registry& registry, T&& v)
        : uarg_base{registry, std::forward<T>(v)}
        , data_{const_cast<std::remove_cvref_t<T>*>(std::addressof(v))} {} // NOLINT(*-const-cast)

        template < typename To >
            requires(std::is_pointer_v<To>)
        [[nodiscard]] decltype(auto) cast(type_registry& registry) const;

        template < typename To >
            requires(!std::is_pointer_v<To>)
        [[nodiscard]] decltype(auto) cast(type_registry& registry) const;

    private:
        void* data_{};
    };
}

namespace meta_hpp::detail
{
    template < typename To >
        requires(std::is_pointer_v<To>)
    [[nodiscard]] bool uarg_base::can_cast_to(type_registry& registry) const noexcept {
        using to_raw_type_cv = std::remove_reference_t<To>;
        using to_raw_type = std::remove_cv_t<to_raw_type_cv>;

        const any_type& from_type = get_raw_type();
        const any_type& to_type = registry.resolve_type<to_raw_type>();

        const auto is_a = [](const any_type& base, const any_type& derived) {
            return (base == derived) //
                || (base.is_class() && derived.is_class() && base.as_class().is_base_of(derived.as_class()));
        };

        if ( from_type.is_nullptr() && to_type.is_pointer() ) {
            return true;
        }

        if ( to_type.is_pointer() && from_type.is_array() ) {
            const pointer_type& to_type_ptr = to_type.as_pointer();
            const bool to_type_ptr_readonly = to_type_ptr.get_flags().has(pointer_flags::is_readonly);

            const array_type& from_type_array = from_type.as_array();
            const bool from_type_array_readonly = is_ref_const();

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

        return false;
    }

    template < typename To >
        requires(!std::is_pointer_v<To>)
    [[nodiscard]] bool uarg_base::can_cast_to(type_registry& registry) const noexcept {
        using to_raw_type_cv = std::remove_reference_t<To>;
        using to_raw_type = std::remove_cv_t<to_raw_type_cv>;

        static_assert( //
            !(std::is_reference_v<To> && std::is_pointer_v<to_raw_type>),
            "references to pointers are not supported yet"
        );

        const any_type& from_type = get_raw_type();
        const any_type& to_type = registry.resolve_type<to_raw_type>();

        const auto is_a = [](const any_type& base, const any_type& derived) {
            return (base == derived) //
                || (base.is_class() && derived.is_class() && base.as_class().is_base_of(derived.as_class()));
        };

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

        if constexpr ( !std::is_pointer_v<To> && !std::is_reference_v<To> ) {
            if ( is_a(to_type, from_type) && is_constructible_from_type(type_list<to_raw_type>{}) ) {
                return true;
            }
        }

        return false;
    }
}

namespace meta_hpp::detail
{
    template < typename To >
        requires(std::is_pointer_v<To>)
    [[nodiscard]] decltype(auto) uarg::cast(type_registry& registry) const {
        META_HPP_ASSERT(can_cast_to<To>(registry) && "bad argument cast");

        using to_raw_type_cv = std::remove_reference_t<To>;
        using to_raw_type = std::remove_cv_t<to_raw_type_cv>;

        const any_type& from_type = get_raw_type();
        const any_type& to_type = registry.resolve_type<to_raw_type>();

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

                void* to_ptr = pointer_upcast(registry, data_, from_data_class, to_data_class);
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

                void* to_ptr = pointer_upcast(registry, *from_data_ptr, from_data_class, to_data_class);
                return static_cast<to_raw_type_cv>(to_ptr);
            }
        }

        throw_exception(error_code::bad_argument_cast);
    }

    template < typename To >
        requires(!std::is_pointer_v<To>)
    [[nodiscard]] decltype(auto) uarg::cast(type_registry& registry) const {
        META_HPP_ASSERT(can_cast_to<To>(registry) && "bad argument cast");

        using to_raw_type_cv = std::remove_reference_t<To>;
        using to_raw_type = std::remove_cv_t<to_raw_type_cv>;

        static_assert( //
            !(std::is_reference_v<To> && std::is_pointer_v<to_raw_type>),
            "references to pointers are not supported yet"
        );

        const any_type& from_type = get_raw_type();
        const any_type& to_type = registry.resolve_type<to_raw_type>();

        void* to_ptr = to_type == from_type //
                         ? data_
                         : pointer_upcast(registry, data_, from_type.as_class(), to_type.as_class());

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
    template < typename ArgTypeList, typename F >
    auto call_with_uargs(type_registry& registry, std::span<const uarg> args, F&& f) {
        META_HPP_ASSERT(args.size() == type_list_arity_v<ArgTypeList>);
        return [ args, &registry, &f ]<std::size_t... Is>(std::index_sequence<Is...>) {
            return f(args[Is].cast<type_list_at_t<Is, ArgTypeList>>(registry)...);
        }
        (std::make_index_sequence<type_list_arity_v<ArgTypeList>>());
    }

    template < typename ArgTypeList >
    bool can_cast_all_uargs(type_registry& registry, std::span<const uarg> args) {
        META_HPP_ASSERT(args.size() == type_list_arity_v<ArgTypeList>);
        return [ args, &registry ]<std::size_t... Is>(std::index_sequence<Is...>) {
            return (... && args[Is].can_cast_to<type_list_at_t<Is, ArgTypeList>>(registry));
        }
        (std::make_index_sequence<type_list_arity_v<ArgTypeList>>());
    }

    template < typename ArgTypeList >
    bool can_cast_all_uargs(type_registry& registry, std::span<const uarg_base> args) {
        META_HPP_ASSERT(args.size() == type_list_arity_v<ArgTypeList>);
        return [ args, &registry ]<std::size_t... Is>(std::index_sequence<Is...>) {
            return (... && args[Is].can_cast_to<type_list_at_t<Is, ArgTypeList>>(registry));
        }
        (std::make_index_sequence<type_list_arity_v<ArgTypeList>>());
    }
}
