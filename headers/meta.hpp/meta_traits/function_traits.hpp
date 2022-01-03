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
    template < function_kind Function >
    struct function_traits;

    template < typename R, typename... Args >
    struct function_traits<R(*)(Args...)> {
        static constexpr std::size_t arity{sizeof...(Args)};

        using return_type = R;
        using argument_types = type_list<Args...>;

        [[nodiscard]] static constexpr bitflags<function_flags> make_flags() noexcept {
            return {};
        }

        [[nodiscard]] static std::vector<any_type> make_argument_types() {
            return { resolve_type<Args>()... };
        }
    };

    template < typename R, typename... Args >
    struct function_traits<R(*)(Args...) noexcept> : function_traits<R(*)(Args...)> {
        [[nodiscard]] static constexpr bitflags<function_flags> make_flags() noexcept {
            return function_flags::is_noexcept;
        }
    };
}
