/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../../meta_base.hpp"
#include "../../meta_uvalue.hpp"

namespace meta_hpp::detail
{
    template < typename T >
    struct unmap_traits;

    template < typename T >
    concept has_unmap_traits //
        = requires(const T& v) { unmap_traits<T>{}(v); };
}

namespace meta_hpp::detail
{
    template < typename T >
    struct unmap_traits<std::shared_ptr<T>> {
        uvalue operator()(const std::shared_ptr<T>& v) const {
            return uvalue{v.get()};
        }
    };

    template < typename T, typename D >
    struct unmap_traits<std::unique_ptr<T, D>> {
        uvalue operator()(const std::unique_ptr<T, D>& v) const {
            return uvalue{v.get()};
        }
    };

    template < typename T >
    struct unmap_traits<std::reference_wrapper<T>> {
        uvalue operator()(const std::reference_wrapper<T>& v) const {
            return uvalue{std::addressof(v.get())};
        }
    };
}
