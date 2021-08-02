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
    template < typename K, typename V >
    using info_map = std::map<K, V, std::less<>>;

    using class_info_map = info_map<std::string, class_info>;
    using ctor_info_map = info_map<std::string, ctor_info>;
    using data_info_map = info_map<std::string, data_info>;
    using enum_info_map = info_map<std::string, enum_info>;
    using evalue_info_map = info_map<std::string, evalue_info>;
    using function_info_map = info_map<std::string, function_info>;
    using member_info_map = info_map<std::string, member_info>;
    using method_info_map = info_map<std::string, method_info>;
    using namespace_info_map = info_map<std::string, namespace_info>;
}
