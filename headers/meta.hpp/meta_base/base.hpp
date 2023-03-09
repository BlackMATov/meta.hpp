/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include <cassert>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include <algorithm>
#include <array>
#include <atomic>
#include <compare>
#include <concepts>
#include <deque>
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
#include <typeindex>
#include <typeinfo>
#include <utility>
#include <vector>
#include <version>

#if !defined(META_HPP_NO_EXCEPTIONS) && !defined(__cpp_exceptions)
#    define META_HPP_NO_EXCEPTIONS
#endif

#if !defined(META_HPP_NO_RTTI) && !defined(__cpp_rtti)
#    define META_HPP_NO_RTTI
#endif

#if !defined(META_HPP_FWD)
#    define META_HPP_FWD(v) std::forward<decltype(v)>(v)
#endif

#if !defined(META_HPP_ASSERT)
#    define META_HPP_ASSERT(...) assert(__VA_ARGS__) // NOLINT
#endif

#if defined(META_HPP_SANITIZERS)
#    define META_HPP_DEV_ASSERT(...) META_HPP_ASSERT(__VA_ARGS__)
#else
#    define META_HPP_DEV_ASSERT(...) (void)0
#endif

#if !defined(META_HPP_PP_CAT)
#    define META_HPP_PP_CAT(x, y) META_HPP_PP_CAT_I(x, y)
#    define META_HPP_PP_CAT_I(x, y) x##y
#endif

#if defined(_MSC_VER)
#    define META_HPP_MSVC
#elif defined(__clang__)
#    define META_HPP_CLANG
#elif defined(__GNUC__)
#    define META_HPP_GCC
#endif

#if defined(META_HPP_MSVC)
#    define META_HPP_MSVC_IGNORE_WARNING(w) __pragma(warning(disable : w))
#    define META_HPP_MSVC_IGNORE_WARNINGS_PUSH() __pragma(warning(push))
#    define META_HPP_MSVC_IGNORE_WARNINGS_POP() __pragma(warning(pop))
#else
#    define META_HPP_MSVC_IGNORE_WARNING(w)
#    define META_HPP_MSVC_IGNORE_WARNINGS_PUSH()
#    define META_HPP_MSVC_IGNORE_WARNINGS_POP()
#endif

#if defined(META_HPP_CLANG)
#    define META_HPP_CLANG_PRAGMA_TO_STR(x) _Pragma(#x)
#    define META_HPP_CLANG_IGNORE_WARNING(w) META_HPP_CLANG_PRAGMA_TO_STR(clang diagnostic ignored w)
#    define META_HPP_CLANG_IGNORE_WARNINGS_PUSH() _Pragma("clang diagnostic push")
#    define META_HPP_CLANG_IGNORE_WARNINGS_POP() _Pragma("clang diagnostic pop")
#else
#    define META_HPP_CLANG_PRAGMA_TO_STR(x)
#    define META_HPP_CLANG_IGNORE_WARNING(w)
#    define META_HPP_CLANG_IGNORE_WARNINGS_PUSH()
#    define META_HPP_CLANG_IGNORE_WARNINGS_POP()
#endif

#if defined(META_HPP_GCC)
#    define META_HPP_GCC_PRAGMA_TO_STR(x) _Pragma(#x)
#    define META_HPP_GCC_IGNORE_WARNING(w) META_HPP_GCC_PRAGMA_TO_STR(GCC diagnostic ignored w)
#    define META_HPP_GCC_IGNORE_WARNINGS_PUSH() _Pragma("GCC diagnostic push")
#    define META_HPP_GCC_IGNORE_WARNINGS_POP() _Pragma("GCC diagnostic pop")
#else
#    define META_HPP_GCC_PRAGMA_TO_STR(x)
#    define META_HPP_GCC_IGNORE_WARNING(w)
#    define META_HPP_GCC_IGNORE_WARNINGS_PUSH()
#    define META_HPP_GCC_IGNORE_WARNINGS_POP()
#endif

#define META_HPP_IGNORE_OVERRIDE_WARNINGS_PUSH() \
    META_HPP_MSVC_IGNORE_WARNINGS_PUSH() \
    META_HPP_CLANG_IGNORE_WARNINGS_PUSH() \
    META_HPP_GCC_IGNORE_WARNINGS_PUSH() \
    META_HPP_CLANG_IGNORE_WARNING("-Winconsistent-missing-override") \
    META_HPP_CLANG_IGNORE_WARNING("-Wsuggest-override")

#define META_HPP_IGNORE_OVERRIDE_WARNINGS_POP() \
    META_HPP_GCC_IGNORE_WARNINGS_POP() \
    META_HPP_CLANG_IGNORE_WARNINGS_POP() \
    META_HPP_MSVC_IGNORE_WARNINGS_POP()
