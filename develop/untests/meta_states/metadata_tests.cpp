/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "../meta_untests.hpp"

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

        [[maybe_unused]] explicit ivec2(int nv) : x{nv}, y{nv} {}
        [[maybe_unused]] ivec2(int nx, int ny) : x{nx}, y{ny} {}

        ivec2& add(const ivec2& other) noexcept {
            x += other.x;
            y += other.y;
            return *this;
        }

        static ivec2 iadd(const ivec2& l, const ivec2& r) noexcept {
            return {l.x + r.x, l.y + r.y};
        }
    };
}

TEST_CASE("meta/meta_states/metadata/enum") {
    namespace meta = meta_hpp;
    using namespace std::string_literals;

    meta::enum_<color>({
        {"desc1", "enum-desc1"s},
        {"desc2", "enum-desc2"s},
    })
    .evalue_("red", color::red, {
        .metadata{{"desc1", "red-color"s}}
    })
    .evalue_("green", color::green, {
        .metadata{{"desc1", "green-color"s}}
    })
    .evalue_("blue", color::blue, {
        .metadata{{"desc1", "blue-color"s}}
    });

    // metadata override

    meta::enum_<color>({
        {"desc2", "new-enum-desc2"s},
        {"desc3", "new-enum-desc3"s},
    });

    meta::enum_<color>()
        .evalue_("red", color::red, {
            .metadata{
                {"desc2", "new-red-color"s},
            }
        });

    //

    const meta::enum_type color_type = meta::resolve_type<color>();
    REQUIRE(color_type);

    SUBCASE("color") {
        CHECK(color_type.get_metadata().at("desc1").get_as<std::string>() == "enum-desc1"s);
        CHECK(color_type.get_metadata().at("desc2").get_as<std::string>() == "new-enum-desc2"s);
        CHECK(color_type.get_metadata().at("desc3").get_as<std::string>() == "new-enum-desc3"s);
    }

    SUBCASE("color::red") {
        const meta::evalue red_evalue = color_type.get_evalue("red");
        REQUIRE(red_evalue);
        CHECK_FALSE(red_evalue.get_metadata().contains("desc1"));
        CHECK(red_evalue.get_metadata().at("desc2").get_as<std::string>() == "new-red-color"s);
    }
}

TEST_CASE("meta/meta_states/metadata/class") {
    namespace meta = meta_hpp;
    using namespace std::string_literals;

    meta::class_<ivec2>({
        {"desc1", "class-desc1"s},
        {"desc2", "class-desc2"s},
    })
    .constructor_<int>({
        .arguments{{
            .name{"v"},
            .metadata{{"desc", "the ctor arg"s}},
        }},
        .metadata{{"desc", "one arg 2d vector ctor"s}},
    })
    .constructor_<int, int>({
        .arguments{{
            .name{"x"},
            .metadata{{"desc", "the 1st ctor arg"s}},
        },{
            .name{"y"},
            .metadata{{"desc", "the 2nd ctor arg"s}},
        }},
        .metadata{{"desc", "two args 2d vector ctor"s}}
    })
    .member_("x", &ivec2::x, {
        .metadata{{"desc", "x-member"s}}
    })
    .member_("y", &ivec2::y, {
        .metadata{{"desc", "y-member"s}}
    })
    .method_("add", &ivec2::add, {
        .arguments{{
            .name{"other"},
            .metadata{{"desc", "other-arg"s}}
        }},
        .metadata{{"desc", "add-method"s}}
    })
    .function_("iadd", &ivec2::iadd, {
        .arguments{{
            .name{"l"},
            .metadata{{"desc", "l-arg"s}}
        },{
            .name{"r"},
            .metadata{{"desc", "r-arg"s}}
        }},
        .metadata{{"desc", "iadd-function"s}}
    });

    // metadata override

    meta::class_<ivec2>({
        {"desc2", "new-class-desc2"s},
        {"desc3", "new-class-desc3"s},
    });

    //

    const meta::class_type ivec2_type = meta::resolve_type<ivec2>();
    REQUIRE(ivec2_type);

    SUBCASE("ivec2") {
        CHECK(ivec2_type.get_metadata().at("desc1").get_as<std::string>() == "class-desc1"s);
        CHECK(ivec2_type.get_metadata().at("desc2").get_as<std::string>() == "new-class-desc2"s);
        CHECK(ivec2_type.get_metadata().at("desc3").get_as<std::string>() == "new-class-desc3"s);
    }

    SUBCASE("ivec2(int)") {
        const meta::constructor ivec2_ctor = ivec2_type.get_constructor_with<int>();
        REQUIRE(ivec2_ctor);

        REQUIRE(ivec2_ctor.get_metadata().contains("desc"));
        CHECK(ivec2_ctor.get_metadata().at("desc").get_as<std::string>() == "one arg 2d vector ctor"s);

        REQUIRE(ivec2_ctor.get_argument(0));
        CHECK(ivec2_ctor.get_argument(0).get_name() == "v");
        CHECK(ivec2_ctor.get_argument(0).get_position() == 0);
        CHECK(ivec2_ctor.get_argument(0).get_type() == meta::resolve_type<int>());
        REQUIRE(ivec2_ctor.get_argument(0).get_metadata().contains("desc"));
        CHECK(ivec2_ctor.get_argument(0).get_metadata().at("desc").get_as<std::string>() == "the ctor arg"s);

        REQUIRE_FALSE(ivec2_ctor.get_argument(1));
    }

    SUBCASE("ivec2(int, int)") {
        const meta::constructor ivec2_ctor = ivec2_type.get_constructor_with<int, int>();
        REQUIRE(ivec2_ctor);

        REQUIRE(ivec2_ctor.get_metadata().contains("desc"));
        CHECK(ivec2_ctor.get_metadata().at("desc").get_as<std::string>() == "two args 2d vector ctor"s);

        REQUIRE(ivec2_ctor.get_argument(0));
        CHECK(ivec2_ctor.get_argument(0).get_name() == "x");
        CHECK(ivec2_ctor.get_argument(0).get_position() == 0);
        CHECK(ivec2_ctor.get_argument(0).get_type() == meta::resolve_type<int>());
        REQUIRE(ivec2_ctor.get_argument(0).get_metadata().contains("desc"));
        CHECK(ivec2_ctor.get_argument(0).get_metadata().at("desc").get_as<std::string>() == "the 1st ctor arg"s);

        REQUIRE(ivec2_ctor.get_argument(1));
        CHECK(ivec2_ctor.get_argument(1).get_name() == "y");
        CHECK(ivec2_ctor.get_argument(1).get_position() == 1);
        CHECK(ivec2_ctor.get_argument(1).get_type() == meta::resolve_type<int>());
        REQUIRE(ivec2_ctor.get_argument(1).get_metadata().contains("desc"));
        CHECK(ivec2_ctor.get_argument(1).get_metadata().at("desc").get_as<std::string>() == "the 2nd ctor arg"s);

        REQUIRE_FALSE(ivec2_ctor.get_argument(2));
    }

    SUBCASE("ivec2::x") {
        const meta::member ivec2_x = ivec2_type.get_member("x");
        REQUIRE(ivec2_x);

        REQUIRE(ivec2_x.get_metadata().contains("desc"));
        CHECK(ivec2_x.get_metadata().at("desc").get_as<std::string>() == "x-member"s);
    }

    SUBCASE("ivec2::y") {
        const meta::member ivec2_y = ivec2_type.get_member("y");
        REQUIRE(ivec2_y);

        REQUIRE(ivec2_y.get_metadata().contains("desc"));
        CHECK(ivec2_y.get_metadata().at("desc").get_as<std::string>() == "y-member"s);
    }

    SUBCASE("ivec2::add") {
        const meta::method ivec2_add = ivec2_type.get_method("add");
        REQUIRE(ivec2_add);

        REQUIRE(ivec2_add.get_metadata().contains("desc"));
        CHECK(ivec2_add.get_metadata().at("desc").get_as<std::string>() == "add-method"s);

        REQUIRE(ivec2_add.get_argument(0));
        REQUIRE(ivec2_add.get_argument(0).get_metadata().contains("desc"));
        CHECK(ivec2_add.get_argument(0).get_metadata().at("desc").get_as<std::string>() == "other-arg"s);
    }

    SUBCASE("ivec2::iadd") {
        const meta::function ivec2_iadd = ivec2_type.get_function("iadd");
        REQUIRE(ivec2_iadd);

        REQUIRE(ivec2_iadd.get_metadata().contains("desc"));
        CHECK(ivec2_iadd.get_metadata().at("desc").get_as<std::string>() == "iadd-function"s);

        REQUIRE(ivec2_iadd.get_argument(0));
        REQUIRE(ivec2_iadd.get_argument(0).get_metadata().contains("desc"));
        CHECK(ivec2_iadd.get_argument(0).get_metadata().at("desc").get_as<std::string>() == "l-arg"s);

        REQUIRE(ivec2_iadd.get_argument(1));
        REQUIRE(ivec2_iadd.get_argument(1).get_metadata().contains("desc"));
        CHECK(ivec2_iadd.get_argument(1).get_metadata().at("desc").get_as<std::string>() == "r-arg"s);
    }
}

TEST_CASE("meta/meta_states/metadata/scope") {
    namespace meta = meta_hpp;
    using namespace std::string_literals;

    SUBCASE("local_scope") {
        const meta::scope lscope = meta::local_scope_("local-scope", {
            {"desc", "scope-desc"s}
        });
        CHECK(lscope.get_metadata().at("desc").get_as<std::string>() == "scope-desc"s);
    }

    SUBCASE("static_scope") {
        meta::static_scope_("meta/meta_states/metadata/scope/static-scope", {
            {"desc", "scope-desc"s}
        });
        CHECK(meta::resolve_scope("meta/meta_states/metadata/scope/static-scope").get_metadata().at("desc").get_as<std::string>() == "scope-desc"s);
    }
}

TEST_CASE("meta/meta_states/metadata/other") {
    namespace meta = meta_hpp;
    using namespace std::string_literals;

    SUBCASE("array") {
        meta::array_<int[]>({
            {"desc", "int[]-type"s}
        });
        CHECK(meta::resolve_type<int[]>().get_metadata().at("desc").get_as<std::string>() == "int[]-type"s);
    }

    SUBCASE("function") {
        meta::function_<int(*)(int)>({
            {"desc", "int->int"s}
        });
        CHECK(meta::resolve_type<int(*)(int)>().get_metadata().at("desc").get_as<std::string>() == "int->int"s);
    }

    SUBCASE("member") {
        meta::member_<int ivec2::*>({
            {"desc", "ivec2::int"s}
        });
        CHECK(meta::resolve_type<int ivec2::*>().get_metadata().at("desc").get_as<std::string>() == "ivec2::int"s);
    }

    SUBCASE("method") {
        meta::method_<int (ivec2::*)(int)>({
            {"desc", "ivec2(int -> int)"s}
        });
        CHECK(meta::resolve_type<int (ivec2::*)(int)>().get_metadata().at("desc").get_as<std::string>() == "ivec2(int -> int)"s);
    }

    SUBCASE("nullptr") {
        meta::nullptr_<std::nullptr_t>({
            {"desc", "nullptr_t"s}
        });
        CHECK(meta::resolve_type<std::nullptr_t>().get_metadata().at("desc").get_as<std::string>() == "nullptr_t"s);
    }

    SUBCASE("number") {
        meta::number_<int>({
            {"desc", "int-type"s}
        });
        CHECK(meta::resolve_type<int>().get_metadata().at("desc").get_as<std::string>() == "int-type"s);
    }

    SUBCASE("pointer") {
        meta::pointer_<int*>({
            {"desc", "int*-type"s}
        });
        CHECK(meta::resolve_type<int*>().get_metadata().at("desc").get_as<std::string>() == "int*-type"s);
    }

    SUBCASE("reference") {
        meta::reference_<int&>({
            {"desc", "int&-type"s}
        });
        CHECK(meta::resolve_type<int&>().get_metadata().at("desc").get_as<std::string>() == "int&-type"s);
    }

    SUBCASE("void") {
        meta::void_<void>({
            {"desc", "void-type"s}
        });
        CHECK(meta::resolve_type<void>().get_metadata().at("desc").get_as<std::string>() == "void-type"s);
    }
}
