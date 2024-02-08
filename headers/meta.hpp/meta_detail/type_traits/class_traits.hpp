/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../../meta_base.hpp"

#include "../type_kinds.hpp"

namespace meta_hpp::detail
{
    enum class class_flags : std::uint8_t {
        is_empty = 1 << 0,
        is_final = 1 << 1,
        is_abstract = 1 << 2,
        is_polymorphic = 1 << 3,
        is_template_instantiation = 1 << 4,
    };

    using class_bitflags = bitflags<class_flags>;
    META_HPP_DETAIL_BITFLAGS_OPERATORS_DECL(class_flags)
}

namespace meta_hpp::detail
{
    namespace impl
    {
        template < class_kind Class >
        struct class_argument_traits_impl {
            using argument_types = type_list<>;
            static constexpr std::tuple argument_values = std::make_tuple();
        };

        template < typename T >
        inline constexpr decltype(std::ignore) type_to_ignore_v = std::ignore;

        //
        // typename...
        //

        template < //
            template < typename... >
            typename Class,
            typename... Zs >
        struct class_argument_traits_impl<Class<Zs...>> {
            using argument_types = type_list<Zs...>;
            static constexpr std::tuple argument_values = std::make_tuple(type_to_ignore_v<Zs>...);
        };

        //
        // auto, typename...
        //

        template < //
            template < auto, typename... >
            typename Class,
            auto A,
            typename... Zs >
            requires(sizeof...(Zs) > 0)
        struct class_argument_traits_impl<Class<A, Zs...>> {
            using argument_types = type_list<decltype(A), Zs...>;
            static constexpr std::tuple argument_values = std::make_tuple(A, type_to_ignore_v<Zs>...);
        };

        //
        // auto, auto, typename...
        // typename, auto, typename...
        //

        template < //
            template < auto, auto, typename... >
            typename Class,
            auto A,
            auto B,
            typename... Zs >
            requires(sizeof...(Zs) > 0)
        struct class_argument_traits_impl<Class<A, B, Zs...>> {
            using argument_types = type_list<decltype(A), decltype(B), Zs...>;
            static constexpr std::tuple argument_values = std::make_tuple(A, B, type_to_ignore_v<Zs>...);
        };

        template < //
            template < typename, auto, typename... >
            typename Class,
            typename A,
            auto B,
            typename... Zs >
            requires(sizeof...(Zs) > 0)
        struct class_argument_traits_impl<Class<A, B, Zs...>> {
            using argument_types = type_list<A, decltype(B), Zs...>;
            static constexpr std::tuple argument_values = std::make_tuple(std::ignore, B, type_to_ignore_v<Zs>...);
        };

        //
        // auto, auto, auto, typename...
        // typename, auto, auto, typename...
        // auto, typename, auto, typename...
        // typename, typename, auto, typename...
        //

        template < //
            template < auto, auto, auto, typename... >
            typename Class,
            auto A,
            auto B,
            auto C,
            typename... Zs >
            requires(sizeof...(Zs) > 0)
        struct class_argument_traits_impl<Class<A, B, C, Zs...>> {
            using argument_types = type_list<decltype(A), decltype(B), decltype(C), Zs...>;
            static constexpr std::tuple argument_values = std::make_tuple(A, B, C, type_to_ignore_v<Zs>...);
        };

        template < //
            template < typename, auto, auto, typename... >
            typename Class,
            typename A,
            auto B,
            auto C,
            typename... Zs >
            requires(sizeof...(Zs) > 0)
        struct class_argument_traits_impl<Class<A, B, C, Zs...>> {
            using argument_types = type_list<A, decltype(B), decltype(C), Zs...>;
            static constexpr std::tuple argument_values = std::make_tuple(std::ignore, B, C, type_to_ignore_v<Zs>...);
        };

        template < //
            template < auto, typename, auto, typename... >
            typename Class,
            auto A,
            typename B,
            auto C,
            typename... Zs >
            requires(sizeof...(Zs) > 0)
        struct class_argument_traits_impl<Class<A, B, C, Zs...>> {
            using argument_types = type_list<decltype(A), B, decltype(C), Zs...>;
            static constexpr std::tuple argument_values = std::make_tuple(A, std::ignore, C, type_to_ignore_v<Zs>...);
        };

        template < //
            template < typename, typename, auto, typename... >
            typename Class,
            typename A,
            typename B,
            auto C,
            typename... Zs >
            requires(sizeof...(Zs) > 0)
        struct class_argument_traits_impl<Class<A, B, C, Zs...>> {
            using argument_types = type_list<A, B, decltype(C), Zs...>;
            static constexpr std::tuple argument_values = std::make_tuple(std::ignore, std::ignore, C, type_to_ignore_v<Zs>...);
        };

        //
        // auto...
        //

        template < //
            template < auto... >
            typename Class,
            auto... Zs >
        struct class_argument_traits_impl<Class<Zs...>> {
            using argument_types = type_list<decltype(Zs)...>;
            static constexpr std::tuple argument_values = std::make_tuple(Zs...);
        };

        //
        // typename, auto...
        //

        template < //
            template < typename, auto... >
            typename Class,
            typename A,
            auto... Zs >
            requires(sizeof...(Zs) > 0)
        struct class_argument_traits_impl<Class<A, Zs...>> {
            using argument_types = type_list<A, decltype(Zs)...>;
            static constexpr std::tuple argument_values = std::make_tuple(std::ignore, Zs...);
        };

        //
        // auto, typename, auto...
        // typename, typename, auto...
        //

        template < //
            template < auto, typename, auto... >
            typename Class,
            auto A,
            typename B,
            auto... Zs >
            requires(sizeof...(Zs) > 0)
        struct class_argument_traits_impl<Class<A, B, Zs...>> {
            using argument_types = type_list<decltype(A), B, decltype(Zs)...>;
            static constexpr std::tuple argument_values = std::make_tuple(A, std::ignore, Zs...);
        };

        template < //
            template < typename, typename, auto... >
            typename Class,
            typename A,
            typename B,
            auto... Zs >
            requires(sizeof...(Zs) > 0)
        struct class_argument_traits_impl<Class<A, B, Zs...>> {
            using argument_types = type_list<A, B, decltype(Zs)...>;
            static constexpr std::tuple argument_values = std::make_tuple(std::ignore, std::ignore, Zs...);
        };

        //
        // auto, auto, typename, auto...
        // typename, auto, typename, auto...
        // auto, typename, typename, auto...
        // typename, typename, typename, auto...
        //

        template < //
            template < auto, auto, typename, auto... >
            typename Class,
            auto A,
            auto B,
            typename C,
            auto... Zs >
            requires(sizeof...(Zs) > 0)
        struct class_argument_traits_impl<Class<A, B, C, Zs...>> {
            using argument_types = type_list<decltype(A), decltype(B), C, decltype(Zs)...>;
            static constexpr std::tuple argument_values = std::make_tuple(A, B, std::ignore, Zs...);
        };

        template < //
            template < typename, auto, typename, auto... >
            typename Class,
            typename A,
            auto B,
            typename C,
            auto... Zs >
            requires(sizeof...(Zs) > 0)
        struct class_argument_traits_impl<Class<A, B, C, Zs...>> {
            using argument_types = type_list<A, decltype(B), C, decltype(Zs)...>;
            static constexpr std::tuple argument_values = std::make_tuple(std::ignore, B, std::ignore, Zs...);
        };

        template < //
            template < auto, typename, typename, auto... >
            typename Class,
            auto A,
            typename B,
            typename C,
            auto... Zs >
            requires(sizeof...(Zs) > 0)
        struct class_argument_traits_impl<Class<A, B, C, Zs...>> {
            using argument_types = type_list<decltype(A), B, C, decltype(Zs)...>;
            static constexpr std::tuple argument_values = std::make_tuple(A, std::ignore, std::ignore, Zs...);
        };

        template < //
            template < typename, typename, typename, auto... >
            typename Class,
            typename A,
            typename B,
            typename C,
            auto... Zs >
            requires(sizeof...(Zs) > 0)
        struct class_argument_traits_impl<Class<A, B, C, Zs...>> {
            using argument_types = type_list<A, B, C, decltype(Zs)...>;
            static constexpr std::tuple argument_values = std::make_tuple(std::ignore, std::ignore, std::ignore, Zs...);
        };
    }

    template < class_kind Class >
    struct class_traits {
        static constexpr std::size_t size{sizeof(Class)};
        static constexpr std::size_t align{alignof(Class)};

        using argument_types = typename impl::class_argument_traits_impl<Class>::argument_types;
        static constexpr std::tuple argument_values = impl::class_argument_traits_impl<Class>::argument_values;
        static_assert(type_list_arity_v<argument_types> == std::tuple_size_v<decltype(argument_values)>);

        [[nodiscard]] static constexpr class_bitflags make_flags() noexcept {
            class_bitflags flags{};

            if constexpr ( std::is_empty_v<Class> ) {
                flags.set(class_flags::is_empty);
            }

            if constexpr ( std::is_final_v<Class> ) {
                flags.set(class_flags::is_final);
            }

            if constexpr ( std::is_abstract_v<Class> ) {
                flags.set(class_flags::is_abstract);
            }

            if constexpr ( std::is_polymorphic_v<Class> ) {
                flags.set(class_flags::is_polymorphic);
            }

            if constexpr ( type_list_arity_v<argument_types> > 0 ) {
                flags.set(class_flags::is_template_instantiation);
            }

            return flags;
        }
    };
}
