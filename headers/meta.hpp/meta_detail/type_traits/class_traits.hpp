/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../../meta_base.hpp"

namespace meta_hpp::detail
{
    enum class class_flags : std::uint32_t {
        is_empty = 1 << 0,
        is_final = 1 << 1,
        is_abstract = 1 << 2,
        is_polymorphic = 1 << 3,
        is_template_instantiation = 1 << 4,
    };

    ENUM_HPP_OPERATORS_DECL(class_flags)
    using class_bitflags = bitflags<class_flags>;
}

namespace meta_hpp::detail
{
    namespace impl
    {
        template < class_kind Class >
        struct class_traits_base {
            static constexpr std::size_t arity{0};

            using argument_types = type_list<>;

            [[nodiscard]] static constexpr class_bitflags make_flags() noexcept {
                return {};
            }
        };

        template < template < typename... > typename Class, typename... Args >
        struct class_traits_base<Class<Args...>> {
            static constexpr std::size_t arity{sizeof...(Args)};

            using argument_types = type_list<Args...>;

            [[nodiscard]] static constexpr class_bitflags make_flags() noexcept {
                return class_flags::is_template_instantiation;
            }
        };
    }

    template < class_kind Class >
    struct class_traits : impl::class_traits_base<Class> {
        static constexpr std::size_t size{sizeof(Class)};

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

            return flags | impl::class_traits_base<Class>::make_flags();
        }
    };
}
