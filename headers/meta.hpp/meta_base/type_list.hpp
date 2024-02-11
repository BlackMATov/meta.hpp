/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "base.hpp"

namespace meta_hpp::detail
{
    template < typename... Types >
    struct type_list {};

    template < std::size_t I >
    using size_constant = std::integral_constant<std::size_t, I>;

    template < std::size_t I >
    using index_constant = std::integral_constant<std::size_t, I>;
}

namespace meta_hpp::detail
{
    template < std::size_t Index, typename TypeList >
    struct type_list_at;

    template < std::size_t Index, typename... Types >
    struct type_list_at<Index, type_list<Types...>> {
        using type = std::tuple_element_t<Index, std::tuple<Types...>>;
    };

    template < std::size_t Index, typename TypeList >
    using type_list_at_t = typename type_list_at<Index, TypeList>::type;
}

namespace meta_hpp::detail
{
    template < typename TypeList >
    struct type_list_arity;

    template < typename... Types >
    struct type_list_arity<type_list<Types...>> : size_constant<sizeof...(Types)> {};

    template < typename TypeList >
    inline constexpr std::size_t type_list_arity_v = type_list_arity<TypeList>::value;
}

namespace meta_hpp::detail
{
    template < template < typename > class Pred, typename TypeList >
    struct type_list_count_of;

    template < template < typename > class Pred, typename... Types >
    struct type_list_count_of<Pred, type_list<Types...>> : size_constant<(0 + ... + Pred<Types>::value)> {};

    template < template < typename > class Pred, typename TypeList >
    inline constexpr std::size_t type_list_count_of_v = type_list_count_of<Pred, TypeList>::value;
}

namespace meta_hpp::detail
{
    template < template < typename > class Pred, typename Default, typename TypeList >
    struct type_list_first_of;

    template < template < typename > class Pred, typename Default >
    struct type_list_first_of<Pred, Default, type_list<>> {
        using type = Default;
    };

    template < template < typename > class Pred, typename Default, typename Type, typename... Types >
    struct type_list_first_of<Pred, Default, type_list<Type, Types...>> {
        using type = std::conditional_t< //
            Pred<Type>::value,
            Type,
            typename type_list_first_of<Pred, Default, type_list<Types...>>::type>;
    };

    template < template < typename > class Pred, typename Default, typename TypeList >
    using type_list_first_of_t = typename type_list_first_of<Pred, Default, TypeList>::type;
}

namespace meta_hpp::detail
{
    template < template < typename > class Pred, typename TypeList >
    struct type_list_and;

    template < template < typename > class Pred, typename... Types >
    struct type_list_and<Pred, type_list<Types...>> : std::bool_constant<(... && Pred<Types>::value)> {};

    template < template < typename > class Pred, typename TypeList >
    inline constexpr bool type_list_and_v = type_list_and<Pred, TypeList>::value;
}

namespace meta_hpp::detail
{
    template < template < typename > class Pred, typename TypeList >
    struct type_list_or;

    template < template < typename > class Pred, typename... Types >
    struct type_list_or<Pred, type_list<Types...>> : std::bool_constant<(... || Pred<Types>::value)> {};

    template < template < typename > class Pred, typename TypeList >
    inline constexpr bool type_list_or_v = type_list_or<Pred, TypeList>::value;
}
