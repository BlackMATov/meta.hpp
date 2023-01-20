/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "base.hpp"

#if !defined(META_HPP_NO_EXCEPTIONS)
#  define META_HPP_TRY try
#  define META_HPP_CATCH(e) catch(e)
#  define META_HPP_RETHROW() throw
#  define META_HPP_THROW_AS(e, m) throw e{m}
#else
#  define META_HPP_TRY if ( true )
#  define META_HPP_CATCH(e) if ( false )
#  define META_HPP_RETHROW() std::abort()
#  define META_HPP_THROW_AS(e, m) std::terminate()
#endif

namespace meta_hpp::detail
{
#if !defined(META_HPP_NO_EXCEPTIONS)
    class exception final : public std::runtime_error {
    public:
        explicit exception(const char* what)
        : std::runtime_error(what) {}
    };
#endif
}
