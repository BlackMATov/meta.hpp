/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_base.hpp"

namespace meta_hpp
{
    namespace constructor_policy
    {
        inline constexpr struct as_object_t final {
        } as_object{};

        inline constexpr struct as_raw_pointer_t final {
        } as_raw_pointer{};

        inline constexpr struct as_shared_pointer_t final {
        } as_shared_pointer{};

        inline constexpr struct as_unique_pointer_t final {
        } as_unique_pointer{};
    }

    namespace function_policy
    {
        inline constexpr struct as_copy_t final {
        } as_copy{};

        inline constexpr struct discard_return_t final {
        } discard_return{};

        inline constexpr struct return_reference_as_pointer_t final {
        } return_reference_as_pointer{};
    }

    namespace member_policy
    {
        inline constexpr struct as_copy_t final {
        } as_copy{};

        inline constexpr struct as_pointer_t final {
        } as_pointer{};

        inline constexpr struct as_reference_wrapper_t final {
        } as_reference_wrapper{};
    }

    namespace method_policy
    {
        inline constexpr struct as_copy_t final {
        } as_copy{};

        inline constexpr struct discard_return_t final {
        } discard_return{};

        inline constexpr struct return_reference_as_pointer_t final {
        } return_reference_as_pointer{};
    }

    namespace variable_policy
    {
        inline constexpr struct as_copy_t final {
        } as_copy{};

        inline constexpr struct as_pointer_t final {
        } as_pointer{};

        inline constexpr struct as_reference_wrapper_t final {
        } as_reference_wrapper{};
    }
}

namespace meta_hpp::detail
{
    template < typename Policy >
    concept constructor_policy_family                                      //
        = std::is_same_v<Policy, constructor_policy::as_object_t>          //
       || std::is_same_v<Policy, constructor_policy::as_raw_pointer_t>     //
       || std::is_same_v<Policy, constructor_policy::as_shared_pointer_t>  //
       || std::is_same_v<Policy, constructor_policy::as_unique_pointer_t>; //

    template < typename Policy >
    concept function_policy_family                                                //
        = std::is_same_v<Policy, function_policy::as_copy_t>                      //
       || std::is_same_v<Policy, function_policy::discard_return_t>               //
       || std::is_same_v<Policy, function_policy::return_reference_as_pointer_t>; //

    template < typename Policy >
    concept member_policy_family                                         //
        = std::is_same_v<Policy, member_policy::as_copy_t>               //
       || std::is_same_v<Policy, member_policy::as_pointer_t>            //
       || std::is_same_v<Policy, member_policy::as_reference_wrapper_t>; //

    template < typename Policy >
    concept method_policy_family                                                //
        = std::is_same_v<Policy, method_policy::as_copy_t>                      //
       || std::is_same_v<Policy, method_policy::discard_return_t>               //
       || std::is_same_v<Policy, method_policy::return_reference_as_pointer_t>; //

    template < typename Policy >
    concept variable_policy_family                                         //
        = std::is_same_v<Policy, variable_policy::as_copy_t>               //
       || std::is_same_v<Policy, variable_policy::as_pointer_t>            //
       || std::is_same_v<Policy, variable_policy::as_reference_wrapper_t>; //

    template < typename T >
    using is_constructor_policy = std::bool_constant<constructor_policy_family<T>>;

    template < typename T >
    using is_function_policy = std::bool_constant<function_policy_family<T>>;

    template < typename T >
    using is_member_policy = std::bool_constant<member_policy_family<T>>;

    template < typename T >
    using is_method_policy = std::bool_constant<method_policy_family<T>>;

    template < typename T >
    using is_variable_policy = std::bool_constant<variable_policy_family<T>>;
}
