/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
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
