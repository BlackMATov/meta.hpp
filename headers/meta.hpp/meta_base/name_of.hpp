/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "base.hpp"

namespace meta_hpp::detail
{
    namespace impl
    {
        template < typename T >
        struct name_of_wrapper_impl {};

        template < typename T >
        [[nodiscard]] constexpr std::string_view name_of_impl() noexcept {
#if META_HPP_DETAIL_COMPILER_ID == META_HPP_DETAIL_CLANG_COMPILER_ID
            constexpr auto prefix = std::string_view{"[T = "};
            constexpr auto suffix = std::string_view{"]"};
            constexpr auto fnsign = std::string_view{__PRETTY_FUNCTION__};
#elif META_HPP_DETAIL_COMPILER_ID == META_HPP_DETAIL_GCC_COMPILER_ID
            constexpr auto prefix = std::string_view{"with T = "};
            constexpr auto suffix = std::string_view{"]"};
            constexpr auto fnsign = std::string_view{__PRETTY_FUNCTION__};
#elif META_HPP_DETAIL_COMPILER_ID == META_HPP_DETAIL_MSVC_COMPILER_ID
            constexpr auto prefix = std::string_view{"name_of_wrapper_impl<"};
            constexpr auto suffix = std::string_view{">>(void)"};
            constexpr auto fnsign = std::string_view{__FUNCSIG__};
#else
            constexpr auto prefix = std::string_view{"<"};
            constexpr auto suffix = std::string_view{">"};
            constexpr auto fnsign = std::string_view{"<unknown>"};
#endif
            constexpr auto b = fnsign.find(prefix) + prefix.size();
            constexpr auto e = fnsign.rfind(suffix);
            static_assert(b < e);

            auto n = fnsign.substr(b, e - b);
            n.remove_prefix(std::min(n.find_first_not_of(' '), n.size()));
            n.remove_suffix(std::min(n.size() - n.find_last_not_of(' ') - 1, n.size()));
            return n;
        }
    }

    template < typename T >
    [[nodiscard]] constexpr std::string_view name_of() noexcept {
        return impl::name_of_impl<impl::name_of_wrapper_impl<T>>();
    }
}
