/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include <algorithm>
#include <any>
#include <array>
#include <atomic>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <map>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string_view>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

#include <enum.hpp/enum.hpp>
#include <enum.hpp/enum_bitflags.hpp>

#include <kari.hpp/kari.hpp>

namespace meta_hpp
{
    template < typename Enum >
    using bitflags = enum_hpp::bitflags::bitflags<Enum>;
}

namespace meta_hpp
{
    class class_info;
    class ctor_info;
    class data_info;
    class enum_info;
    class evalue_info;
    class function_info;
    class member_info;
    class method_info;
    class namespace_info;
}

namespace meta_hpp
{
    template < typename Class > class class_;
    template < typename... Args > class ctor_;
    class data_;
    template < typename Enum > class enum_;
    template < typename Enum > class evalue_;
    template < typename Function > class function_;
    template < typename Member > class member_;
    template < typename Method > class method_;
    class namespace_;
}

namespace meta_hpp
{
    class type_id;

    class arithmetic_type;
    class array_type;
    class class_type;
    class ctor_type;
    class enum_type;
    class function_type;
    class member_type;
    class method_type;
    class pointer_type;
    class reference_type;
    class void_type;
}

namespace meta_hpp
{
    class arg;
    class value;
}

namespace meta_hpp
{
    template < typename K, typename V >
    using info_map = std::map<K, V, std::less<>>;

    using class_info_map = info_map<std::string, class_info>;
    using ctor_info_map = info_map<type_id, ctor_info>;
    using data_info_map = info_map<std::string, data_info>;
    using enum_info_map = info_map<std::string, enum_info>;
    using evalue_info_map = info_map<std::string, evalue_info>;
    using function_info_map = info_map<std::string, function_info>;
    using member_info_map = info_map<std::string, member_info>;
    using method_info_map = info_map<std::string, method_info>;
    using namespace_info_map = info_map<std::string, namespace_info>;
}

namespace meta_hpp
{
    template < typename... Args >
    struct typename_arg_t {};

    template < typename... Args >
    inline typename_arg_t<Args...> typename_arg;
}

namespace meta_hpp::detail
{
    template < typename K, typename V, typename C, typename A, typename K2, typename U >
    V find_or(const std::map<K, V, C, A>& src, K2&& key, U&& def) {
        if ( auto iter = src.find(std::forward<K2>(key)); iter != src.end() ) {
            return iter->second;
        }
        return std::forward<U>(def);
    }

    template < typename K, typename V, typename C, typename A, typename K2 >
    V find_or_default(const std::map<K, V, C, A>& src, K2&& key) {
        return find_or(src, std::forward<K2>(key), V{});
    }

    template < typename K, typename V, typename C, typename A, typename K2, typename V2, typename F >
    void merge_with(std::map<K, V, C, A>& dst, K2&& key, V2&& value, F&& f) {
        if ( auto iter = dst.find(key); iter != dst.end() ) {
            std::invoke(std::forward<F>(f), iter->second, std::forward<V2>(value));
        } else {
            dst.emplace(std::forward<K2>(key), std::forward<V2>(value));
        }
    }
}
