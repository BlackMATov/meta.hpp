/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "_infos_fwd.hpp"

namespace
{
    using namespace meta_hpp;
    using namespace std::string_literals;
}

namespace
{
    struct ivec2 {
        int x{};
        int y{};

        [[maybe_unused]] ivec2() = default;
        [[maybe_unused]] ivec2(const ivec2& other) = default;
        [[maybe_unused]] explicit ivec2(int v): x{v}, y{v} {}
        [[maybe_unused]] ivec2(int x, int y): x{x}, y{y} {}
    };
}

TEST_CASE("features/infos/ctor") {
    registry db;

    db(
        namespace_{"vmath"}(
            class_<ivec2>{"ivec2"}(
                ctor_<>{}(data_{"info", "void ctor"s}),
                ctor_<const ivec2&>{}(data_{"info", "copy ctor"s}),
                ctor_<int>{}(data_{"info", "int ctor"s}),
                ctor_<int, int>{}(data_{"info", "int,int ctor"s})
            )
        )
    );

    const class_info ivec2_info = db.get_class_by_path("vmath::ivec2");
    REQUIRE(ivec2_info);

    SUBCASE("void") {
        const ctor_info ci = ivec2_info.get_ctor_by_args();
        REQUIRE(ci);

        REQUIRE(ci.get_data_by_name("info"));
        CHECK(ci.get_data_by_name("info").value().equals("void ctor"s));

        {
            const ctor_info ci2 = ivec2_info.get_ctor_by_args(std::array<any_type, 0>{});
            const ctor_info ci3 = ivec2_info.get_ctor_by_args(std::vector<any_type>{});
            REQUIRE(ci2);
            REQUIRE(ci3);
            CHECK(ci.type().id() == ci2.type().id());
            CHECK(ci.type().id() == ci2.type().id());
        }
    }

    SUBCASE("int") {
        const ctor_info ci = ivec2_info.get_ctor_by_args<int>();
        REQUIRE(ci);

        REQUIRE(ci.get_data_by_name("info"));
        CHECK(ci.get_data_by_name("info").value().equals("int ctor"s));

        {
            const ctor_info ci2 = ivec2_info.get_ctor_by_args(std::array<any_type, 1>{
                type_db::get<int>()});
            const ctor_info ci3 = ivec2_info.get_ctor_by_args(std::vector<any_type>{
                type_db::get<int>()});
            REQUIRE(ci2);
            REQUIRE(ci3);
            CHECK(ci.type().id() == ci2.type().id());
            CHECK(ci.type().id() == ci2.type().id());
        }
    }

    SUBCASE("const ivec2&") {
        const ctor_info ci = ivec2_info.get_ctor_by_args<const ivec2&>();
        REQUIRE(ci);

        REQUIRE(ci.get_data_by_name("info"));
        CHECK(ci.get_data_by_name("info").value().equals("copy ctor"s));

        {
            const ctor_info ci2 = ivec2_info.get_ctor_by_args(std::array<any_type, 1>{
                type_db::get<const ivec2&>()});
            const ctor_info ci3 = ivec2_info.get_ctor_by_args(std::vector<any_type>{
                type_db::get<const ivec2&>()});
            REQUIRE(ci2);
            REQUIRE(ci3);
            CHECK(ci.type().id() == ci2.type().id());
            CHECK(ci.type().id() == ci2.type().id());
        }
    }

    SUBCASE("int,int") {
        const ctor_info ci = ivec2_info.get_ctor_by_args<int,int>();
        REQUIRE(ci);

        REQUIRE(ci.get_data_by_name("info"));
        CHECK(ci.get_data_by_name("info").value().equals("int,int ctor"s));

        {
            const ctor_info ci2 = ivec2_info.get_ctor_by_args(std::array<any_type, 2>{
                type_db::get<int>(),
                type_db::get<int>()});
            const ctor_info ci3 = ivec2_info.get_ctor_by_args(std::vector<any_type>{
                type_db::get<int>(),
                type_db::get<int>()});
            REQUIRE(ci2);
            REQUIRE(ci3);
            CHECK(ci.type().id() == ci2.type().id());
            CHECK(ci.type().id() == ci2.type().id());
        }
    }
}
