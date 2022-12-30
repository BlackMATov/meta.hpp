/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>

#include <algorithm>
#include <array>
#include <atomic>
#include <concepts>
#include <functional>
#include <initializer_list>
#include <iosfwd>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <span>
#include <stdexcept>
#include <string_view>
#include <string>
#include <tuple>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <utility>
#include <variant>
#include <vector>

#include "meta_base/bitflags.hpp"
#include "meta_base/cv_traits.hpp"
#include "meta_base/cvref_traits.hpp"
#include "meta_base/fixed_function.hpp"
#include "meta_base/hash_combiner.hpp"
#include "meta_base/is_in_place_type.hpp"
#include "meta_base/noncopyable.hpp"
#include "meta_base/overloaded.hpp"
#include "meta_base/select_overload.hpp"
#include "meta_base/to_underlying.hpp"
#include "meta_base/type_id.hpp"
#include "meta_base/type_kinds.hpp"
#include "meta_base/type_list.hpp"

#if !defined(__cpp_exceptions)
#  define META_HPP_NO_EXCEPTIONS
#endif

#if !defined(__cpp_rtti)
#  define META_HPP_NO_RTTI
#endif

namespace meta_hpp
{
    using detail::select_const;
    using detail::select_non_const;
    using detail::select_overload;

    using detail::type_id;
    using detail::type_kind;
    using detail::type_list;
}

namespace meta_hpp
{
    class exception final : public std::runtime_error {
    public:
        explicit exception(const char* what)
        : std::runtime_error(what) {}
    };

    namespace detail
    {
        inline void throw_exception_with [[noreturn]] (const char* what) {
        #ifndef META_HPP_NO_EXCEPTIONS
            throw ::meta_hpp::exception(what);
        #else
            (void)what;
            std::abort();
        #endif
        }
    }
}

namespace meta_hpp
{
    class uvalue;

    namespace detail
    {
        class uarg_base;
        class uarg;

        class uinst_base;
        class uinst;
    }
}

namespace meta_hpp
{
    class argument;
    class constructor;
    class destructor;
    class evalue;
    class function;
    class member;
    class method;
    class scope;
    class variable;

    namespace detail
    {
        struct argument_state;
        struct constructor_state;
        struct destructor_state;
        struct evalue_state;
        struct function_state;
        struct member_state;
        struct method_state;
        struct scope_state;
        struct variable_state;

        using argument_state_ptr = std::shared_ptr<argument_state>;
        using constructor_state_ptr = std::shared_ptr<constructor_state>;
        using destructor_state_ptr = std::shared_ptr<destructor_state>;
        using evalue_state_ptr = std::shared_ptr<evalue_state>;
        using function_state_ptr = std::shared_ptr<function_state>;
        using member_state_ptr = std::shared_ptr<member_state>;
        using method_state_ptr = std::shared_ptr<method_state>;
        using scope_state_ptr = std::shared_ptr<scope_state>;
        using variable_state_ptr = std::shared_ptr<variable_state>;
    }
}

namespace meta_hpp
{
    class any_type;
    class array_type;
    class class_type;
    class constructor_type;
    class destructor_type;
    class enum_type;
    class function_type;
    class member_type;
    class method_type;
    class nullptr_type;
    class number_type;
    class pointer_type;
    class reference_type;
    class void_type;

    namespace detail
    {
        struct type_data_base;
        struct array_type_data;
        struct class_type_data;
        struct constructor_type_data;
        struct destructor_type_data;
        struct enum_type_data;
        struct function_type_data;
        struct member_type_data;
        struct method_type_data;
        struct nullptr_type_data;
        struct number_type_data;
        struct pointer_type_data;
        struct reference_type_data;
        struct void_type_data;
    }
}

namespace meta_hpp
{
    class argument_index;
    class constructor_index;
    class destructor_index;
    class evalue_index;
    class function_index;
    class member_index;
    class method_index;
    class scope_index;
    class variable_index;
}

namespace meta_hpp
{
    using any_type_list = std::vector<any_type>;
    using argument_list = std::vector<argument>;

    using metadata_map = std::map<std::string, uvalue, std::less<>>;
    using typedef_map = std::map<std::string, any_type, std::less<>>;

    using class_set = std::set<class_type, std::less<>>;
    using class_map = std::map<std::string, class_type, std::less<>>;

    using enum_set = std::set<enum_type, std::less<>>;
    using enum_map = std::map<std::string, enum_type, std::less<>>;

    using constructor_map = std::map<constructor_index, constructor, std::less<>>;
    using destructor_map = std::map<destructor_index, destructor, std::less<>>;
    using evalue_map = std::map<evalue_index, evalue, std::less<>>;
    using function_map = std::map<function_index, function, std::less<>>;
    using member_map = std::map<member_index, member, std::less<>>;
    using method_map = std::map<method_index, method, std::less<>>;
    using scope_map = std::map<scope_index, scope, std::less<>>;
    using variable_map = std::map<variable_index, variable, std::less<>>;
}
