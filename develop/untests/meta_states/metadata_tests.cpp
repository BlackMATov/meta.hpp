/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

namespace
{
    enum class color : unsigned {
        red = 0xFF0000,
        green = 0x00FF00,
        blue = 0x0000FF,
    };

    struct ivec2 {
        int x{};
        int y{};

        explicit ivec2(int nv) : x{nv}, y{nv} {}
        ivec2(int nx, int ny) : x{nx}, y{ny} {}

        ivec2& add(const ivec2& other) noexcept {
            x += other.x;
            y += other.y;
            return *this;
        }

        static ivec2 iadd(const ivec2& l, const ivec2& r) noexcept {
            return {l.x + r.x, l.y + r.y};
        }

        static const ivec2 zero;
        static const ivec2 unit;
    };

    const ivec2 ivec2::zero{0, 0};
    const ivec2 ivec2::unit{1, 1};
}

TEST_CASE("meta/meta_states/metadata/enum") {
    namespace meta = meta_hpp;
    using namespace std::string_literals;

    meta::enum_<color>(meta::metadata_()
        ("desc1", "enum-desc1"s)
        ("desc2", "enum-desc2"s))
    .evalue_("red", color::red,
        meta::metadata_()("desc1", "red-color"s))
    .evalue_("green", color::green,
        meta::metadata_()("desc1", "green-color"s))
    .evalue_("blue", color::blue,
        meta::metadata_()("desc1", "blue-color"s));

    // metadata override

    meta::enum_<color>(meta::metadata_()
        ("desc2", "new-enum-desc2"s)
        ("desc3", "new-enum-desc3"s));

    meta::enum_<color>()
        .evalue_("red", color::red,
            meta::metadata_()
                ("desc2", "new-red-color"s));

    //

    const meta::enum_type color_type = meta::resolve_type<color>();
    REQUIRE(color_type);

    SUBCASE("color") {
        CHECK(color_type.get_metadata().at("desc1").as<std::string>() == "enum-desc1"s);
        CHECK(color_type.get_metadata().at("desc2").as<std::string>() == "new-enum-desc2"s);
        CHECK(color_type.get_metadata().at("desc3").as<std::string>() == "new-enum-desc3"s);
    }

    SUBCASE("color::red") {
        const meta::evalue red_evalue = color_type.get_evalue("red");
        REQUIRE(red_evalue);
        CHECK_FALSE(red_evalue.get_metadata().contains("desc1"));
        CHECK(red_evalue.get_metadata().at("desc2").as<std::string>() == "new-red-color"s);
    }
}

TEST_CASE("meta/meta_states/metadata/class") {
    namespace meta = meta_hpp;
    using namespace std::string_literals;

    meta::class_<ivec2>(meta::metadata_()
        ("desc1", "class-desc1"s)
        ("desc2", "class-desc2"s))
    .constructor_<int>(
        meta::arguments_()
            ("v", meta::metadata_()("desc", "the ctor arg"s)),
        meta::metadata_()("desc", "one arg 2d vector ctor"s))
    .constructor_<int, int>(
        meta::argument_("x", meta::metadata_("desc", "the 1st ctor arg"s)),
        meta::argument_("y", meta::metadata_("desc", "the 2nd ctor arg"s)),
        meta::metadata_()("desc", "two args 2d vector ctor"s))
    .member_("x", &ivec2::x, meta::metadata_()("desc", "x-member"s))
    .member_("y", &ivec2::y, meta::metadata_()("desc", "y-member"s))
    .method_("add", &ivec2::add,
        meta::arguments_()
            ("other", meta::metadata_()("desc", "other-arg"s)),
        meta::metadata_()("desc", "add-method"s))
    .function_("iadd", &ivec2::iadd,
        meta::arguments_()
            ("l", meta::metadata_()("desc", "l-arg"s)),
        meta::arguments_()
            ("r", meta::metadata_()("desc", "r-arg"s)),
        meta::metadata_()("desc", "iadd-function"s))
    .variable_("zero", &ivec2::zero, meta::metadata_("desc", "{0,0} vector"s))
    .variable_("unit", &ivec2::unit, meta::metadata_("desc", "{1,1} vector"s));

    // metadata override

    meta::class_<ivec2>(meta::metadata_()
        ("desc2", "new-class-desc2"s)
        ("desc3", "new-class-desc3"s));

    //

    const meta::class_type ivec2_type = meta::resolve_type<ivec2>();
    REQUIRE(ivec2_type);

    SUBCASE("ivec2") {
        CHECK(ivec2_type.get_metadata().at("desc1").as<std::string>() == "class-desc1"s);
        CHECK(ivec2_type.get_metadata().at("desc2").as<std::string>() == "new-class-desc2"s);
        CHECK(ivec2_type.get_metadata().at("desc3").as<std::string>() == "new-class-desc3"s);
    }

    SUBCASE("ivec2(int)") {
        const meta::constructor ivec2_ctor = ivec2_type.get_constructor_with<int>();
        REQUIRE(ivec2_ctor);

        CHECK(ivec2_ctor.get_arity() == 1);

        REQUIRE(ivec2_ctor.get_metadata().contains("desc"));
        CHECK(ivec2_ctor.get_metadata().at("desc").as<std::string>() == "one arg 2d vector ctor"s);

        REQUIRE(ivec2_ctor.get_argument(0));
        CHECK(ivec2_ctor.get_argument(0).get_name() == "v");
        CHECK(ivec2_ctor.get_argument(0).get_position() == 0);
        CHECK(ivec2_ctor.get_argument(0).get_type() == meta::resolve_type<int>());
        REQUIRE(ivec2_ctor.get_argument(0).get_metadata().contains("desc"));
        CHECK(ivec2_ctor.get_argument(0).get_metadata().at("desc").as<std::string>() == "the ctor arg"s);

        REQUIRE_FALSE(ivec2_ctor.get_argument(1));
    }

    SUBCASE("ivec2(int, int)") {
        const meta::constructor ivec2_ctor = ivec2_type.get_constructor_with<int, int>();
        REQUIRE(ivec2_ctor);

        CHECK(ivec2_ctor.get_arity() == 2);

        REQUIRE(ivec2_ctor.get_metadata().contains("desc"));
        CHECK(ivec2_ctor.get_metadata().at("desc").as<std::string>() == "two args 2d vector ctor"s);

        REQUIRE(ivec2_ctor.get_argument(0));
        CHECK(ivec2_ctor.get_argument(0).get_name() == "x");
        CHECK(ivec2_ctor.get_argument(0).get_position() == 0);
        CHECK(ivec2_ctor.get_argument(0).get_type() == meta::resolve_type<int>());
        REQUIRE(ivec2_ctor.get_argument(0).get_metadata().contains("desc"));
        CHECK(ivec2_ctor.get_argument(0).get_metadata().at("desc").as<std::string>() == "the 1st ctor arg"s);

        REQUIRE(ivec2_ctor.get_argument(1));
        CHECK(ivec2_ctor.get_argument(1).get_name() == "y");
        CHECK(ivec2_ctor.get_argument(1).get_position() == 1);
        CHECK(ivec2_ctor.get_argument(1).get_type() == meta::resolve_type<int>());
        REQUIRE(ivec2_ctor.get_argument(1).get_metadata().contains("desc"));
        CHECK(ivec2_ctor.get_argument(1).get_metadata().at("desc").as<std::string>() == "the 2nd ctor arg"s);

        REQUIRE_FALSE(ivec2_ctor.get_argument(2));
    }

    SUBCASE("ivec2::x") {
        const meta::member ivec2_x = ivec2_type.get_member("x");
        REQUIRE(ivec2_x);

        REQUIRE(ivec2_x.get_metadata().contains("desc"));
        CHECK(ivec2_x.get_metadata().at("desc").as<std::string>() == "x-member"s);
    }

    SUBCASE("ivec2::y") {
        const meta::member ivec2_y = ivec2_type.get_member("y");
        REQUIRE(ivec2_y);

        REQUIRE(ivec2_y.get_metadata().contains("desc"));
        CHECK(ivec2_y.get_metadata().at("desc").as<std::string>() == "y-member"s);
    }

    SUBCASE("ivec2::add") {
        const meta::method ivec2_add = ivec2_type.get_method("add");
        REQUIRE(ivec2_add);

        REQUIRE(ivec2_add.get_metadata().contains("desc"));
        CHECK(ivec2_add.get_metadata().at("desc").as<std::string>() == "add-method"s);

        REQUIRE(ivec2_add.get_argument(0));
        REQUIRE(ivec2_add.get_argument(0).get_metadata().contains("desc"));
        CHECK(ivec2_add.get_argument(0).get_metadata().at("desc").as<std::string>() == "other-arg"s);
    }

    SUBCASE("ivec2::iadd") {
        const meta::function ivec2_iadd = ivec2_type.get_function("iadd");
        REQUIRE(ivec2_iadd);

        CHECK(ivec2_iadd.get_arity() == 2);

        REQUIRE(ivec2_iadd.get_metadata().contains("desc"));
        CHECK(ivec2_iadd.get_metadata().at("desc").as<std::string>() == "iadd-function"s);

        REQUIRE(ivec2_iadd.get_argument(0));
        REQUIRE(ivec2_iadd.get_argument(0).get_metadata().contains("desc"));
        CHECK(ivec2_iadd.get_argument(0).get_metadata().at("desc").as<std::string>() == "l-arg"s);

        REQUIRE(ivec2_iadd.get_argument(1));
        REQUIRE(ivec2_iadd.get_argument(1).get_metadata().contains("desc"));
        CHECK(ivec2_iadd.get_argument(1).get_metadata().at("desc").as<std::string>() == "r-arg"s);
    }

    SUBCASE("ivec2::zero") {
        const meta::variable ivec2_zero = ivec2_type.get_variable("zero");
        REQUIRE(ivec2_zero);

        REQUIRE(ivec2_zero.get_metadata().contains("desc"));
        CHECK(ivec2_zero.get_metadata().at("desc").as<std::string>() == "{0,0} vector"s);
    }

    SUBCASE("ivec2::unit") {
        const meta::variable ivec2_unit = ivec2_type.get_variable("unit");
        REQUIRE(ivec2_unit);

        REQUIRE(ivec2_unit.get_metadata().contains("desc"));
        CHECK(ivec2_unit.get_metadata().at("desc").as<std::string>() == "{1,1} vector"s);
    }
}

TEST_CASE("meta/meta_states/metadata/scope") {
    namespace meta = meta_hpp;
    using namespace std::string_literals;

    SUBCASE("local_scope") {
        const meta::scope lscope = meta::local_scope_("local-scope", meta::metadata_()
            ("desc", "scope-desc"s));
        CHECK(lscope.get_metadata().at("desc").as<std::string>() == "scope-desc"s);
    }

    SUBCASE("static_scope") {
        meta::static_scope_("meta/meta_states/metadata/scope/static-scope", meta::metadata_()
            ("desc", "scope-desc"s));
        CHECK(meta::resolve_scope("meta/meta_states/metadata/scope/static-scope").get_metadata().at("desc").as<std::string>() == "scope-desc"s);
    }
}

TEST_CASE("meta/meta_states/metadata/other") {
    namespace meta = meta_hpp;
    using namespace std::string_literals;

    SUBCASE("array") {
        meta::array_<int[]>(meta::metadata_()
            ("desc", "int[]-type"s));
        CHECK(meta::resolve_type<int[]>().get_metadata().at("desc").as<std::string>() == "int[]-type"s);
    }

    SUBCASE("function") {
        meta::function_<int(int)>(meta::metadata_()
            ("desc", "int->int"s));
        CHECK(meta::resolve_type<int(int)>().get_metadata().at("desc").as<std::string>() == "int->int"s);
    }

    SUBCASE("member") {
        meta::member_<int ivec2::*>(meta::metadata_()
            ("desc", "ivec2::int"s));
        CHECK(meta::resolve_type<int ivec2::*>().get_metadata().at("desc").as<std::string>() == "ivec2::int"s);
    }

    SUBCASE("method") {
        meta::method_<int (ivec2::*)(int)>(meta::metadata_()
            ("desc", "ivec2(int -> int)"s));
        CHECK(meta::resolve_type<int (ivec2::*)(int)>().get_metadata().at("desc").as<std::string>() == "ivec2(int -> int)"s);
    }

    SUBCASE("nullptr") {
        meta::nullptr_<std::nullptr_t>(meta::metadata_()
            ("desc", "nullptr_t"s));
        CHECK(meta::resolve_type<std::nullptr_t>().get_metadata().at("desc").as<std::string>() == "nullptr_t"s);
    }

    SUBCASE("number") {
        meta::number_<int>(meta::metadata_()
            ("desc", "int-type"s));
        CHECK(meta::resolve_type<int>().get_metadata().at("desc").as<std::string>() == "int-type"s);
    }

    SUBCASE("pointer") {
        meta::pointer_<int*>(meta::metadata_()
            ("desc", "int*-type"s));
        CHECK(meta::resolve_type<int*>().get_metadata().at("desc").as<std::string>() == "int*-type"s);
    }

    SUBCASE("reference") {
        meta::reference_<int&>(meta::metadata_()
            ("desc", "int&-type"s));
        CHECK(meta::resolve_type<int&>().get_metadata().at("desc").as<std::string>() == "int&-type"s);
    }

    SUBCASE("void") {
        meta::void_<void>(meta::metadata_()
            ("desc", "void-type"s));
        CHECK(meta::resolve_type<void>().get_metadata().at("desc").as<std::string>() == "void-type"s);
    }
}
