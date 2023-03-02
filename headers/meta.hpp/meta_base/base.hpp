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

#if !defined(META_HPP_PP_CAT)
#    define META_HPP_PP_CAT(x, y) META_HPP_PP_CAT_I(x, y)
#    define META_HPP_PP_CAT_I(x, y) x##y
#endif
