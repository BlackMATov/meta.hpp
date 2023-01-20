/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "../meta_untests.hpp"

namespace
{

}

TEST_CASE("meta/meta_types/ops") {
    namespace meta = meta_hpp;

    SUBCASE("operator<") {
        // NOLINTBEGIN(*-redundant-expression)
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::any_type>() < std::declval<meta::any_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::array_type>() < std::declval<meta::array_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::class_type>() < std::declval<meta::class_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::constructor_type>() < std::declval<meta::constructor_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::destructor_type>() < std::declval<meta::destructor_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::enum_type>() < std::declval<meta::enum_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::function_type>() < std::declval<meta::function_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::member_type>() < std::declval<meta::member_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::method_type>() < std::declval<meta::method_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::nullptr_type>() < std::declval<meta::nullptr_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::number_type>() < std::declval<meta::number_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::pointer_type>() < std::declval<meta::pointer_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::reference_type>() < std::declval<meta::reference_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::void_type>() < std::declval<meta::void_type>())>);

        static_assert(std::is_same_v<bool, decltype(std::declval<meta::any_type>() < std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::array_type>() < std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::class_type>() < std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::constructor_type>() < std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::destructor_type>() < std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::enum_type>() < std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::function_type>() < std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::member_type>() < std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::method_type>() < std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::method_type>() < std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::nullptr_type>() < std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::nullptr_type>() < std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::number_type>() < std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::number_type>() < std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::pointer_type>() < std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::pointer_type>() < std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::reference_type>() < std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::reference_type>() < std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::void_type>() < std::declval<meta::type_id>())>);

        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() < std::declval<meta::any_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() < std::declval<meta::array_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() < std::declval<meta::class_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() < std::declval<meta::constructor_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() < std::declval<meta::destructor_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() < std::declval<meta::enum_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() < std::declval<meta::function_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() < std::declval<meta::member_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() < std::declval<meta::method_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() < std::declval<meta::method_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() < std::declval<meta::nullptr_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() < std::declval<meta::nullptr_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() < std::declval<meta::number_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() < std::declval<meta::number_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() < std::declval<meta::pointer_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() < std::declval<meta::pointer_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() < std::declval<meta::reference_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() < std::declval<meta::reference_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() < std::declval<meta::void_type>())>);
        // NOLINTEND(*-redundant-expression)
    }

    SUBCASE("operator==") {
        // NOLINTBEGIN(*-redundant-expression)
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::any_type>() == std::declval<meta::any_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::array_type>() == std::declval<meta::array_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::class_type>() == std::declval<meta::class_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::constructor_type>() == std::declval<meta::constructor_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::destructor_type>() == std::declval<meta::destructor_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::enum_type>() == std::declval<meta::enum_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::function_type>() == std::declval<meta::function_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::member_type>() == std::declval<meta::member_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::method_type>() == std::declval<meta::method_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::method_type>() == std::declval<meta::method_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::nullptr_type>() == std::declval<meta::nullptr_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::nullptr_type>() == std::declval<meta::nullptr_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::number_type>() == std::declval<meta::number_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::number_type>() == std::declval<meta::number_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::pointer_type>() == std::declval<meta::pointer_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::pointer_type>() == std::declval<meta::pointer_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::reference_type>() == std::declval<meta::reference_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::reference_type>() == std::declval<meta::reference_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::void_type>() == std::declval<meta::void_type>())>);

        static_assert(std::is_same_v<bool, decltype(std::declval<meta::any_type>() == std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::array_type>() == std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::class_type>() == std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::constructor_type>() == std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::destructor_type>() == std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::enum_type>() == std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::function_type>() == std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::member_type>() == std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::method_type>() == std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::method_type>() == std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::nullptr_type>() == std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::nullptr_type>() == std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::number_type>() == std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::number_type>() == std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::pointer_type>() == std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::pointer_type>() == std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::reference_type>() == std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::reference_type>() == std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::void_type>() == std::declval<meta::type_id>())>);

        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() == std::declval<meta::any_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() == std::declval<meta::array_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() == std::declval<meta::class_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() == std::declval<meta::constructor_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() == std::declval<meta::destructor_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() == std::declval<meta::enum_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() == std::declval<meta::function_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() == std::declval<meta::member_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() == std::declval<meta::method_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() == std::declval<meta::method_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() == std::declval<meta::nullptr_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() == std::declval<meta::nullptr_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() == std::declval<meta::number_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() == std::declval<meta::number_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() == std::declval<meta::pointer_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() == std::declval<meta::pointer_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() == std::declval<meta::reference_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() == std::declval<meta::reference_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() == std::declval<meta::void_type>())>);
        // NOLINTEND(*-redundant-expression)
    }

    SUBCASE("operator!=") {
        // NOLINTBEGIN(*-redundant-expression)
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::any_type>() != std::declval<meta::any_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::array_type>() != std::declval<meta::array_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::class_type>() != std::declval<meta::class_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::constructor_type>() != std::declval<meta::constructor_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::destructor_type>() != std::declval<meta::destructor_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::enum_type>() != std::declval<meta::enum_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::function_type>() != std::declval<meta::function_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::member_type>() != std::declval<meta::member_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::method_type>() != std::declval<meta::method_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::method_type>() != std::declval<meta::method_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::nullptr_type>() != std::declval<meta::nullptr_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::nullptr_type>() != std::declval<meta::nullptr_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::number_type>() != std::declval<meta::number_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::number_type>() != std::declval<meta::number_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::pointer_type>() != std::declval<meta::pointer_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::pointer_type>() != std::declval<meta::pointer_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::reference_type>() != std::declval<meta::reference_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::reference_type>() != std::declval<meta::reference_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::void_type>() != std::declval<meta::void_type>())>);

        static_assert(std::is_same_v<bool, decltype(std::declval<meta::any_type>() != std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::array_type>() != std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::class_type>() != std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::constructor_type>() != std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::destructor_type>() != std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::enum_type>() != std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::function_type>() != std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::member_type>() != std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::method_type>() != std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::method_type>() != std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::nullptr_type>() != std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::nullptr_type>() != std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::number_type>() != std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::number_type>() != std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::pointer_type>() != std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::pointer_type>() != std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::reference_type>() != std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::reference_type>() != std::declval<meta::type_id>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::void_type>() != std::declval<meta::type_id>())>);

        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() != std::declval<meta::any_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() != std::declval<meta::array_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() != std::declval<meta::class_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() != std::declval<meta::constructor_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() != std::declval<meta::destructor_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() != std::declval<meta::enum_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() != std::declval<meta::function_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() != std::declval<meta::member_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() != std::declval<meta::method_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() != std::declval<meta::method_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() != std::declval<meta::nullptr_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() != std::declval<meta::nullptr_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() != std::declval<meta::number_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() != std::declval<meta::number_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() != std::declval<meta::pointer_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() != std::declval<meta::pointer_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() != std::declval<meta::reference_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() != std::declval<meta::reference_type>())>);
        static_assert(std::is_same_v<bool, decltype(std::declval<meta::type_id>() != std::declval<meta::void_type>())>);
        // NOLINTEND(*-redundant-expression)
    }
}
