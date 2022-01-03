/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_traits.hpp"

namespace meta_hpp::detail
{
    namespace impl
    {
        template < class_kind T >
        struct class_traits_base {
            static constexpr std::size_t arity{0};

            using argument_types = type_list<>;

            static bitflags<class_flags> make_flags() noexcept {
                return {};
            }

            static std::vector<any_type> make_argument_types() {
                return {};
            }
        };

        template < template < typename... > typename T, typename... Args >
        struct class_traits_base<T<Args...>> {
            static constexpr std::size_t arity{sizeof...(Args)};

            using argument_types = type_list<Args...>;

            static bitflags<class_flags> make_flags() noexcept {
                return class_flags::is_template_instantiation;
            }

            static std::vector<any_type> make_argument_types() {
                return { resolve_type<Args>()... };
            }
        };
    }

    template < class_kind T >
    struct class_traits : impl::class_traits_base<T> {
        static constexpr std::size_t size{sizeof(T)};

        static bitflags<class_flags> make_flags() noexcept {
            bitflags<class_flags> flags;

            if constexpr ( std::is_empty_v<T> ) {
                flags.set(class_flags::is_empty);
            }

            if constexpr ( std::is_final_v<T> ) {
                flags.set(class_flags::is_final);
            }

            if constexpr ( std::is_abstract_v<T> ) {
                flags.set(class_flags::is_abstract);
            }

            if constexpr ( std::is_polymorphic_v<T> ) {
                flags.set(class_flags::is_polymorphic);
            }

            return flags | impl::class_traits_base<T>::make_flags();
        }
    };
}
