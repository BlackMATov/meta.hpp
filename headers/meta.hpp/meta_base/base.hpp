/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#if !defined(META_HPP_NO_EXCEPTIONS) && !defined(__cpp_exceptions)
#    define META_HPP_NO_EXCEPTIONS
#endif

#if !defined(META_HPP_NO_RTTI) && !defined(__cpp_rtti)
#    define META_HPP_NO_RTTI
#endif

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
#include <functional>
#include <initializer_list>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <set>
#include <span>
#include <string>
#include <string_view>
#include <system_error>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#if !defined(META_HPP_NO_EXCEPTIONS)
#    include <stdexcept>
#endif

#if !defined(META_HPP_NO_RTTI)
#    include <typeindex>
#    include <typeinfo>
#endif

#if !defined(META_HPP_FWD)
#    define META_HPP_FWD(v) std::forward<decltype(v)>(v)
#endif

#if !defined(META_HPP_ASSERT)
#    include <cassert>
#    define META_HPP_ASSERT(...) assert(__VA_ARGS__) // NOLINT
#endif
