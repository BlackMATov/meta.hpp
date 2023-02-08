/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"

namespace meta_hpp::detail
{
    template < typename T >
    concept type_family                       //
        = std::is_same_v<T, any_type>         //
       || std::is_same_v<T, array_type>       //
       || std::is_same_v<T, class_type>       //
       || std::is_same_v<T, constructor_type> //
       || std::is_same_v<T, destructor_type>  //
       || std::is_same_v<T, enum_type>        //
       || std::is_same_v<T, function_type>    //
       || std::is_same_v<T, member_type>      //
       || std::is_same_v<T, method_type>      //
       || std::is_same_v<T, nullptr_type>     //
       || std::is_same_v<T, number_type>      //
       || std::is_same_v<T, pointer_type>     //
       || std::is_same_v<T, reference_type>   //
       || std::is_same_v<T, void_type>;       //

    template < type_family Type >
    [[nodiscard]] typename Type::data_ptr type_access(const Type& type) {
        return type.data_;
    }
}
