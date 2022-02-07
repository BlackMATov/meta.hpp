/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>

#include <algorithm>
#include <array>
#include <atomic>
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

#include "meta_base/cvref_traits.hpp"
#include "meta_base/enum_bitflags.hpp"
#include "meta_base/enum.hpp"
#include "meta_base/fixed_function.hpp"
#include "meta_base/noncopyable.hpp"
#include "meta_base/overloaded.hpp"
#include "meta_base/select_overload.hpp"
#include "meta_base/stdex.hpp"
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

    using enum_hpp::bitflags::bitflags;
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
    class value;

    namespace detail
    {
        class arg_base;
        class arg;

        class inst_base;
        class inst;
    }
}

namespace meta_hpp
{
    class ctor;
    class dtor;
    class evalue;
    class function;
    class member;
    class method;
    class parameter;
    class scope;
    class variable;

    namespace detail
    {
        struct ctor_state;
        struct dtor_state;
        struct evalue_state;
        struct function_state;
        struct member_state;
        struct method_state;
        struct parameter_state;
        struct scope_state;
        struct variable_state;

        using ctor_state_ptr = std::shared_ptr<ctor_state>;
        using dtor_state_ptr = std::shared_ptr<dtor_state>;
        using evalue_state_ptr = std::shared_ptr<evalue_state>;
        using function_state_ptr = std::shared_ptr<function_state>;
        using member_state_ptr = std::shared_ptr<member_state>;
        using method_state_ptr = std::shared_ptr<method_state>;
        using parameter_state_ptr = std::shared_ptr<parameter_state>;
        using scope_state_ptr = std::shared_ptr<scope_state>;
        using variable_state_ptr = std::shared_ptr<variable_state>;
    }
}

namespace meta_hpp
{
    class any_type;
    class array_type;
    class class_type;
    class ctor_type;
    class dtor_type;
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
        struct ctor_type_data;
        struct dtor_type_data;
        struct enum_type_data;
        struct function_type_data;
        struct member_type_data;
        struct method_type_data;
        struct nullptr_type_data;
        struct number_type_data;
        struct pointer_type_data;
        struct reference_type_data;
        struct void_type_data;

        using type_data_base_ptr = std::shared_ptr<type_data_base>;
        using array_type_data_ptr = std::shared_ptr<array_type_data>;
        using class_type_data_ptr = std::shared_ptr<class_type_data>;
        using ctor_type_data_ptr = std::shared_ptr<ctor_type_data>;
        using dtor_type_data_ptr = std::shared_ptr<dtor_type_data>;
        using enum_type_data_ptr = std::shared_ptr<enum_type_data>;
        using function_type_data_ptr = std::shared_ptr<function_type_data>;
        using member_type_data_ptr = std::shared_ptr<member_type_data>;
        using method_type_data_ptr = std::shared_ptr<method_type_data>;
        using nullptr_type_data_ptr = std::shared_ptr<nullptr_type_data>;
        using number_type_data_ptr = std::shared_ptr<number_type_data>;
        using pointer_type_data_ptr = std::shared_ptr<pointer_type_data>;
        using reference_type_data_ptr = std::shared_ptr<reference_type_data>;
        using void_type_data_ptr = std::shared_ptr<void_type_data>;
    }
}

namespace meta_hpp
{
    class ctor_index;
    class dtor_index;
    class evalue_index;
    class function_index;
    class member_index;
    class method_index;
    class parameter_index;
    class scope_index;
    class variable_index;

    using class_set = std::set<class_type, std::less<>>;
    using class_map = std::map<std::string, class_type, std::less<>>;

    using enum_set = std::set<enum_type, std::less<>>;
    using enum_map = std::map<std::string, enum_type, std::less<>>;

    using ctor_map = std::map<ctor_index, ctor, std::less<>>;
    using dtor_map = std::map<dtor_index, dtor, std::less<>>;
    using evalue_map = std::map<evalue_index, evalue, std::less<>>;
    using function_map = std::map<function_index, function, std::less<>>;
    using member_map = std::map<member_index, member, std::less<>>;
    using method_map = std::map<method_index, method, std::less<>>;
    using scope_map = std::map<scope_index, scope, std::less<>>;
    using variable_map = std::map<variable_index, variable, std::less<>>;
}
