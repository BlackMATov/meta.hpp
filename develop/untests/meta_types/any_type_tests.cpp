/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

namespace
{
    struct class_t {
        class_t() = default;
        class_t(int i): member_v{i} {}

        int member_v{};
        void method_v() {}
    };

    enum class enum_t {
        evalue_v,
    };

    int array_v[42];
    const class_t class_v;
    const enum_t enum_v{enum_t::evalue_v};
    void function_v() {}
    const int number_v{42};
    const int* pointer_v{&number_v};
}

TEST_CASE("meta/meta_types/any_type") {
    namespace meta = meta_hpp;

    meta::class_<class_t>()
        .constructor_<>()
        .constructor_<int>();

    SUBCASE("") {
        const meta::any_type type{};
        CHECK_FALSE(type);
        CHECK_FALSE(type.is_valid());

        CHECK_FALSE(type.is_array());
        CHECK_FALSE(type.as_array());
    }

    SUBCASE("array") {
        const meta::any_type& type = meta::resolve_type(array_v);

        REQUIRE(type);
        REQUIRE(type == meta::resolve_type<decltype(array_v)>());
        REQUIRE(type.get_id() == meta::resolve_type<decltype(array_v)>().get_id());

        CHECK(type.is_array());
        CHECK(type.get_kind() == meta::type_kind::array_);

        CHECK_FALSE(type.is_class());
        CHECK_FALSE(type.as_class());

        const meta::array_type& specific_type = type.as_array();
        REQUIRE(specific_type);
        CHECK(specific_type.get_id() == type.get_id());
    }

    SUBCASE("class") {
        const meta::any_type& type = meta::resolve_type(class_v);

        REQUIRE(type);
        REQUIRE(type == meta::resolve_type<decltype(class_v)>());
        REQUIRE(type.get_id() == meta::resolve_type<decltype(class_v)>().get_id());

        CHECK(type.is_class());
        CHECK(type.get_kind() == meta::type_kind::class_);

        CHECK_FALSE(type.is_constructor());
        CHECK_FALSE(type.as_constructor());

        const meta::class_type& specific_type = type.as_class();
        REQUIRE(specific_type);
        CHECK(specific_type.get_id() == type.get_id());
    }

    SUBCASE("ctor") {
        const meta::any_type& type = meta::resolve_type<class_t>()
            .get_constructor_with<>()
            .get_type();

        REQUIRE(type);

        CHECK(type.is_constructor());
        CHECK(type.get_kind() == meta::type_kind::constructor_);

        CHECK_FALSE(type.is_enum());
        CHECK_FALSE(type.as_enum());

        const meta::constructor_type& specific_type = type.as_constructor();
        REQUIRE(specific_type);
        CHECK(specific_type.get_id() == type.get_id());
    }

    SUBCASE("enum") {
        const meta::any_type& type = meta::resolve_type(enum_v);

        REQUIRE(type);
        REQUIRE(type == meta::resolve_type<decltype(enum_v)>());
        REQUIRE(type.get_id() == meta::resolve_type<decltype(enum_v)>().get_id());

        CHECK(type.is_enum());
        CHECK(type.get_kind() == meta::type_kind::enum_);

        CHECK_FALSE(type.is_function());
        CHECK_FALSE(type.as_function());

        const meta::enum_type& specific_type = type.as_enum();
        REQUIRE(specific_type);
        CHECK(specific_type.get_id() == type.get_id());
    }

    SUBCASE("function") {
        const meta::any_type& type = meta::resolve_type(&function_v);

        REQUIRE(type);
        REQUIRE(type == meta::resolve_type<decltype(&function_v)>());
        REQUIRE(type.get_id() == meta::resolve_type<decltype(&function_v)>().get_id());

        CHECK(type.is_function());
        CHECK(type.get_kind() == meta::type_kind::function_);

        CHECK_FALSE(type.is_member());
        CHECK_FALSE(type.as_member());

        const meta::function_type& specific_type = type.as_function();
        REQUIRE(specific_type);
        CHECK(specific_type.get_id() == type.get_id());
    }

    SUBCASE("member") {
        const meta::any_type& type = meta::resolve_type(&class_t::member_v);

        REQUIRE(type);
        REQUIRE(type == meta::resolve_type<decltype(&class_t::member_v)>());
        REQUIRE(type.get_id() == meta::resolve_type<decltype(&class_t::member_v)>().get_id());

        CHECK(type.is_member());
        CHECK(type.get_kind() == meta::type_kind::member_);

        CHECK_FALSE(type.is_method());
        CHECK_FALSE(type.as_method());

        const meta::member_type& specific_type = type.as_member();
        REQUIRE(specific_type);
        CHECK(specific_type.get_id() == type.get_id());
    }

    SUBCASE("method") {
        const meta::any_type& type = meta::resolve_type(&class_t::method_v);

        REQUIRE(type);
        REQUIRE(type == meta::resolve_type<decltype(&class_t::method_v)>());
        REQUIRE(type.get_id() == meta::resolve_type<decltype(&class_t::method_v)>().get_id());

        CHECK(type.is_method());
        CHECK(type.get_kind() == meta::type_kind::method_);

        CHECK_FALSE(type.is_nullptr());
        CHECK_FALSE(type.as_nullptr());

        const meta::method_type& specific_type = type.as_method();
        REQUIRE(specific_type);
        CHECK(specific_type.get_id() == type.get_id());
    }

    SUBCASE("nullptr") {
        const meta::any_type& type = meta::resolve_type(nullptr);

        REQUIRE(type);
        REQUIRE(type == meta::resolve_type<std::nullptr_t>());
        REQUIRE(type.get_id() == meta::resolve_type<std::nullptr_t>().get_id());

        CHECK(type.is_nullptr());
        CHECK(type.get_kind() == meta::type_kind::nullptr_);

        CHECK_FALSE(type.is_number());
        CHECK_FALSE(type.as_number());

        const meta::nullptr_type& specific_type = type.as_nullptr();
        REQUIRE(specific_type);
        CHECK(specific_type.get_id() == type.get_id());
    }

    SUBCASE("number") {
        const meta::any_type& type = meta::resolve_type(number_v);

        REQUIRE(type);
        REQUIRE(type == meta::resolve_type<decltype(number_v)>());
        REQUIRE(type.get_id() == meta::resolve_type<decltype(number_v)>().get_id());

        CHECK(type.is_number());
        CHECK(type.get_kind() == meta::type_kind::number_);

        CHECK_FALSE(type.is_pointer());
        CHECK_FALSE(type.as_pointer());

        const meta::number_type& specific_type = type.as_number();
        REQUIRE(specific_type);
        CHECK(specific_type.get_id() == type.get_id());
    }

    SUBCASE("pointer") {
        const meta::any_type& type = meta::resolve_type(pointer_v);

        REQUIRE(type);
        REQUIRE(type == meta::resolve_type<decltype(pointer_v)>());
        REQUIRE(type.get_id() == meta::resolve_type<decltype(pointer_v)>().get_id());

        CHECK(type.is_pointer());
        CHECK(type.get_kind() == meta::type_kind::pointer_);

        CHECK_FALSE(type.is_reference());
        CHECK_FALSE(type.as_reference());

        const meta::pointer_type& specific_type = type.as_pointer();
        REQUIRE(specific_type);
        CHECK(specific_type.get_id() == type.get_id());
    }

    SUBCASE("reference") {
        const meta::any_type& type = meta::resolve_type<const int&>();

        REQUIRE(type);

        CHECK(type.is_reference());
        CHECK(type.get_kind() == meta::type_kind::reference_);

        CHECK_FALSE(type.is_void());
        CHECK_FALSE(type.as_void());

        const meta::reference_type& specific_type = type.as_reference();
        REQUIRE(specific_type);
        CHECK(specific_type.get_id() == type.get_id());
    }

    SUBCASE("void") {
        const meta::any_type& type = meta::resolve_type<void>();

        REQUIRE(type);

        CHECK(type.is_void());
        CHECK(type.get_kind() == meta::type_kind::void_);

        CHECK_FALSE(type.is_array());
        CHECK_FALSE(type.as_array());

        const meta::void_type& specific_type = type.as_void();
        REQUIRE(specific_type);
        CHECK(specific_type.get_id() == type.get_id());
    }

    SUBCASE("is/as") {
        const meta::any_type type{meta::resolve_type<class_t>()};
        CHECK(type.is<meta::any_type>());

        const meta::any_type type2{type.as<meta::any_type>()};
        CHECK(type2.as<meta::class_type>() == meta::resolve_type<class_t>());
    }
}
