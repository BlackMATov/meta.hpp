/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include <algorithm>
#include <array>
#include <compare>
#include <exception>
#include <functional>
#include <initializer_list>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <span>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>
#include <version>

//
//
//

#if !defined(META_HPP_VERSION_MAJOR)
#    define META_HPP_VERSION_MAJOR 0
#endif

#if !defined(META_HPP_VERSION_MINOR)
#    define META_HPP_VERSION_MINOR 0
#endif

#if !defined(META_HPP_VERSION_PATCH)
#    define META_HPP_VERSION_PATCH 1
#endif

//
//
//

#if META_HPP_VERSION_MINOR < 0 || META_HPP_VERSION_MINOR > 99
#    error "meta.hpp: incorrect minor version number"
#endif

#if META_HPP_VERSION_PATCH < 0 || META_HPP_VERSION_PATCH > 99
#    error "meta.hpp: incorrect patch version number"
#endif

#define META_HPP_VERSION META_HPP_VERSION_MAJOR * 10000 + META_HPP_VERSION_MINOR * 100 + META_HPP_VERSION_PATCH

//
//
//

#if !defined(META_HPP_NO_EXCEPTIONS) && !defined(__cpp_exceptions)
#    define META_HPP_NO_EXCEPTIONS
#endif

#if !defined(META_HPP_NO_RTTI) && !defined(__cpp_rtti)
#    define META_HPP_NO_RTTI
#endif

//
//
//

#if !defined(META_HPP_FWD)
#    define META_HPP_FWD(v) std::forward<decltype(v)>(v)
#endif

#if !defined(META_HPP_ASSERT)
#    define META_HPP_ASSERT(...) assert(__VA_ARGS__) // NOLINT
#endif

#if !defined(META_HPP_DEV_ASSERT)
#    if defined(META_HPP_SANITIZERS)
#        define META_HPP_DEV_ASSERT(...) META_HPP_ASSERT(__VA_ARGS__)
#    else
#        define META_HPP_DEV_ASSERT(...) (void)0
#    endif
#endif

//
//
//

#define META_HPP_PP_CAT(x, y) META_HPP_PP_CAT_I(x, y)
#define META_HPP_PP_CAT_I(x, y) x##y

//
//
//

#define META_HPP_DETAIL_CLANG_COMPILER_ID 1
#define META_HPP_DETAIL_GCC_COMPILER_ID 2
#define META_HPP_DETAIL_MSVC_COMPILER_ID 3
#define META_HPP_DETAIL_UNKNOWN_COMPILER_ID 4

#if defined(__clang__)
#    define META_HPP_DETAIL_COMPILER_ID META_HPP_DETAIL_CLANG_COMPILER_ID
#elif defined(__GNUC__)
#    define META_HPP_DETAIL_COMPILER_ID META_HPP_DETAIL_GCC_COMPILER_ID
#elif defined(_MSC_VER)
#    define META_HPP_DETAIL_COMPILER_ID META_HPP_DETAIL_MSVC_COMPILER_ID
#else
#    define META_HPP_DETAIL_COMPILER_ID META_HPP_DETAIL_UNKNOWN_COMPILER_ID
#endif

//
//
//

#if META_HPP_DETAIL_COMPILER_ID == META_HPP_DETAIL_MSVC_COMPILER_ID
#    define META_HPP_ALLOCA(size) _alloca(size)
#else
#    define META_HPP_ALLOCA(size) __builtin_alloca(size)
#endif

//
//
//

#if META_HPP_DETAIL_COMPILER_ID == META_HPP_DETAIL_CLANG_COMPILER_ID
#    define META_HPP_DETAIL_CLANG_PRAGMA_TO_STR(x) _Pragma(#x)
#    define META_HPP_DETAIL_CLANG_IGNORE_WARNING(w) META_HPP_DETAIL_CLANG_PRAGMA_TO_STR(clang diagnostic ignored w)
#    define META_HPP_DETAIL_CLANG_IGNORE_WARNINGS_PUSH() _Pragma("clang diagnostic push")
#    define META_HPP_DETAIL_CLANG_IGNORE_WARNINGS_POP() _Pragma("clang diagnostic pop")
#else
#    define META_HPP_DETAIL_CLANG_PRAGMA_TO_STR(x)
#    define META_HPP_DETAIL_CLANG_IGNORE_WARNING(w)
#    define META_HPP_DETAIL_CLANG_IGNORE_WARNINGS_PUSH()
#    define META_HPP_DETAIL_CLANG_IGNORE_WARNINGS_POP()
#endif

#if META_HPP_DETAIL_COMPILER_ID == META_HPP_DETAIL_GCC_COMPILER_ID
#    define META_HPP_DETAIL_GCC_PRAGMA_TO_STR(x) _Pragma(#x)
#    define META_HPP_DETAIL_GCC_IGNORE_WARNING(w) META_HPP_DETAIL_GCC_PRAGMA_TO_STR(GCC diagnostic ignored w)
#    define META_HPP_DETAIL_GCC_IGNORE_WARNINGS_PUSH() _Pragma("GCC diagnostic push")
#    define META_HPP_DETAIL_GCC_IGNORE_WARNINGS_POP() _Pragma("GCC diagnostic pop")
#else
#    define META_HPP_DETAIL_GCC_PRAGMA_TO_STR(x)
#    define META_HPP_DETAIL_GCC_IGNORE_WARNING(w)
#    define META_HPP_DETAIL_GCC_IGNORE_WARNINGS_PUSH()
#    define META_HPP_DETAIL_GCC_IGNORE_WARNINGS_POP()
#endif

#if META_HPP_DETAIL_COMPILER_ID == META_HPP_DETAIL_MSVC_COMPILER_ID
#    define META_HPP_DETAIL_MSVC_IGNORE_WARNING(w) __pragma(warning(disable : w))
#    define META_HPP_DETAIL_MSVC_IGNORE_WARNINGS_PUSH() __pragma(warning(push))
#    define META_HPP_DETAIL_MSVC_IGNORE_WARNINGS_POP() __pragma(warning(pop))
#else
#    define META_HPP_DETAIL_MSVC_IGNORE_WARNING(w)
#    define META_HPP_DETAIL_MSVC_IGNORE_WARNINGS_PUSH()
#    define META_HPP_DETAIL_MSVC_IGNORE_WARNINGS_POP()
#endif

//
//
//

#define META_HPP_DETAIL_IGNORE_ALLOCA_WARNINGS_PUSH() \
    META_HPP_DETAIL_CLANG_IGNORE_WARNINGS_PUSH() \
    META_HPP_DETAIL_CLANG_IGNORE_WARNING("-Walloca")

#define META_HPP_DETAIL_IGNORE_ALLOCA_WARNINGS_POP() META_HPP_DETAIL_CLANG_IGNORE_WARNINGS_POP()

//
//
//

#define META_HPP_DETAIL_IGNORE_OVERRIDE_WARNINGS_PUSH() \
    META_HPP_DETAIL_CLANG_IGNORE_WARNINGS_PUSH() \
    META_HPP_DETAIL_CLANG_IGNORE_WARNING("-Wunknown-warning-option") \
    META_HPP_DETAIL_CLANG_IGNORE_WARNING("-Winconsistent-missing-override") \
    META_HPP_DETAIL_CLANG_IGNORE_WARNING("-Wsuggest-override")

#define META_HPP_DETAIL_IGNORE_OVERRIDE_WARNINGS_POP() META_HPP_DETAIL_CLANG_IGNORE_WARNINGS_POP()

//
//
//

#define META_HPP_DETAIL_IGNORE_COMPARISON_WARNINGS_PUSH() \
    META_HPP_DETAIL_CLANG_IGNORE_WARNINGS_PUSH() \
    META_HPP_DETAIL_CLANG_IGNORE_WARNING("-Wunknown-warning-option") \
    META_HPP_DETAIL_CLANG_IGNORE_WARNING("-Wfloat-equal") \
    META_HPP_DETAIL_CLANG_IGNORE_WARNING("-Wordered-compare-function-pointers")

#define META_HPP_DETAIL_IGNORE_COMPARISON_WARNINGS_POP() META_HPP_DETAIL_CLANG_IGNORE_WARNINGS_POP()
