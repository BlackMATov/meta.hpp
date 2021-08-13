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
    struct clazz {
        clazz() = default;

        clazz(clazz&&) = delete;
        clazz(const clazz&) = delete;

        clazz& operator=(clazz&&) = delete;
        clazz& operator=(const clazz&) = delete;

        //

        int non_const_method() { return 1; }
        int non_const_method_noexcept() noexcept { return 2; }

        int const_method() const { return 3; }
        int const_method_noexcept() const noexcept { return 4; }

        int non_const_method_ref() & { return 5; }
        int non_const_method_noexcept_ref() & noexcept { return 6; }

        int const_method_ref() const & { return 7; }
        int const_method_noexcept_ref() const & noexcept { return 8; }

        int non_const_method_rref() && { return 9; }
        int non_const_method_noexcept_rref() && noexcept { return 10; }

        int const_method_rref() const && { return 11; }
        int const_method_noexcept_rref() const && noexcept { return 12; }

        //

        int non_const_method_volatile() volatile { return 1; }
        int non_const_method_noexcept_volatile() volatile noexcept { return 2; }

        int const_method_volatile() volatile const { return 3; }
        int const_method_noexcept_volatile() volatile const noexcept { return 4; }

        int non_const_method_ref_volatile() volatile & { return 5; }
        int non_const_method_noexcept_ref_volatile() volatile & noexcept { return 6; }

        int const_method_ref_volatile() volatile const & { return 7; }
        int const_method_noexcept_ref_volatile() volatile const & noexcept { return 8; }

        int non_const_method_rref_volatile() volatile && { return 9; }
        int non_const_method_noexcept_rref_volatile() volatile && noexcept { return 10; }

        int const_method_rref_volatile() volatile const && { return 11; }
        int const_method_noexcept_rref_volatile() volatile const && noexcept { return 12; }
    };

    struct clazz2 {};
}

TEST_CASE("features/infos/method") {
    registry db;

    db(
        class_<clazz>{"clazz"}(
            method_{"non_const_method", &clazz::non_const_method},
            method_{"non_const_method_noexcept", &clazz::non_const_method_noexcept},

            method_{"const_method", &clazz::const_method},
            method_{"const_method_noexcept", &clazz::const_method_noexcept},

            method_{"non_const_method_ref", &clazz::non_const_method_ref},
            method_{"non_const_method_noexcept_ref", &clazz::non_const_method_noexcept_ref},

            method_{"const_method_ref", &clazz::const_method_ref},
            method_{"const_method_noexcept_ref", &clazz::const_method_noexcept_ref},

            method_{"non_const_method_rref", &clazz::non_const_method_rref},
            method_{"non_const_method_noexcept_rref", &clazz::non_const_method_noexcept_rref},

            method_{"const_method_rref", &clazz::const_method_rref},
            method_{"const_method_noexcept_rref", &clazz::const_method_noexcept_rref},

            //

            method_{"non_const_method_volatile", &clazz::non_const_method_volatile},
            method_{"non_const_method_noexcept_volatile", &clazz::non_const_method_noexcept_volatile},

            method_{"const_method_volatile", &clazz::const_method_volatile},
            method_{"const_method_noexcept_volatile", &clazz::const_method_noexcept_volatile},

            method_{"non_const_method_ref_volatile", &clazz::non_const_method_ref_volatile},
            method_{"non_const_method_noexcept_ref_volatile", &clazz::non_const_method_noexcept_ref_volatile},

            method_{"const_method_ref_volatile", &clazz::const_method_ref_volatile},
            method_{"const_method_noexcept_ref_volatile", &clazz::const_method_noexcept_ref_volatile},

            method_{"non_const_method_rref_volatile", &clazz::non_const_method_rref_volatile},
            method_{"non_const_method_noexcept_rref_volatile", &clazz::non_const_method_noexcept_rref_volatile},

            method_{"const_method_rref_volatile", &clazz::const_method_rref_volatile},
            method_{"const_method_noexcept_rref_volatile", &clazz::const_method_noexcept_rref_volatile}
        )
    );

    SUBCASE("non_const_method") {
        const method_info mi = db.get_method_by_path("clazz::non_const_method");
        REQUIRE(mi);

        const method_info mi2 = db.get_method_by_path("clazz::non_const_method_volatile");
        REQUIRE(mi2);

        {
            CHECK(mi.type().arity() == 0);
            CHECK(mi.type().class_type() == type_db::get<clazz>());
            CHECK(mi.type().return_type() == type_db::get<int>());
            CHECK(mi.type().flags() == method_flags{});

            CHECK(mi2.type().arity() == 0);
            CHECK(mi2.type().class_type() == type_db::get<clazz>());
            CHECK(mi2.type().return_type() == type_db::get<int>());
            CHECK(mi2.type().flags() == method_flags::is_volatile);
        }

        {
            CHECK(mi.is_invocable_with<clazz&>());
            CHECK_FALSE(mi.is_invocable_with<const clazz&>());
            CHECK(mi.is_invocable_with<clazz&&>());
            CHECK_FALSE(mi.is_invocable_with<const clazz&&>());

            CHECK(mi2.is_invocable_with<clazz&>());
            CHECK_FALSE(mi2.is_invocable_with<const clazz&>());
            CHECK(mi2.is_invocable_with<clazz&&>());
            CHECK_FALSE(mi2.is_invocable_with<const clazz&&>());
        }

        {
            clazz cl;

            CHECK(mi.invoke(cl)->equals(1));
            CHECK_THROWS(mi.invoke(std::as_const(cl)));
            CHECK(mi.invoke(std::move(cl))->equals(1));
            CHECK_THROWS(mi.invoke(std::move(std::as_const(cl))));

            CHECK(mi2.invoke(cl)->equals(1));
            CHECK_THROWS(mi2.invoke(std::as_const(cl)));
            CHECK(mi2.invoke(std::move(cl))->equals(1));
            CHECK_THROWS(mi2.invoke(std::move(std::as_const(cl))));
        }

        static_assert(std::is_invocable_v<decltype(&clazz::non_const_method), clazz&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method), const clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::non_const_method), clazz&&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method), const clazz&&>);

        static_assert(std::is_invocable_v<decltype(&clazz::non_const_method_volatile), clazz&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_volatile), const clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::non_const_method_volatile), clazz&&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_volatile), const clazz&&>);
    }

    SUBCASE("non_const_method_noexcept") {
        const method_info mi = db.get_method_by_path("clazz::non_const_method_noexcept");
        REQUIRE(mi);

        const method_info mi2 = db.get_method_by_path("clazz::non_const_method_noexcept_volatile");
        REQUIRE(mi2);

        {
            CHECK(mi.type().arity() == 0);
            CHECK(mi.type().class_type() == type_db::get<clazz>());
            CHECK(mi.type().return_type() == type_db::get<int>());
            CHECK(mi.type().flags() == method_flags::is_noexcept);

            CHECK(mi2.type().arity() == 0);
            CHECK(mi2.type().class_type() == type_db::get<clazz>());
            CHECK(mi2.type().return_type() == type_db::get<int>());
            CHECK(mi2.type().flags() == (method_flags::is_noexcept | method_flags::is_volatile));
        }

        {
            CHECK(mi.is_invocable_with<clazz&>());
            CHECK_FALSE(mi.is_invocable_with<const clazz&>());
            CHECK(mi.is_invocable_with<clazz&&>());
            CHECK_FALSE(mi.is_invocable_with<const clazz&&>());

            CHECK(mi2.is_invocable_with<clazz&>());
            CHECK_FALSE(mi2.is_invocable_with<const clazz&>());
            CHECK(mi2.is_invocable_with<clazz&&>());
            CHECK_FALSE(mi2.is_invocable_with<const clazz&&>());
        }

        {
            clazz cl;

            CHECK(mi.invoke(cl)->equals(2));
            CHECK_THROWS(mi.invoke(std::as_const(cl)));
            CHECK(mi.invoke(std::move(cl))->equals(2));
            CHECK_THROWS(mi.invoke(std::move(std::as_const(cl))));

            CHECK(mi2.invoke(cl)->equals(2));
            CHECK_THROWS(mi2.invoke(std::as_const(cl)));
            CHECK(mi2.invoke(std::move(cl))->equals(2));
            CHECK_THROWS(mi2.invoke(std::move(std::as_const(cl))));
        }

        static_assert(std::is_invocable_v<decltype(&clazz::non_const_method_noexcept), clazz&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_noexcept), const clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::non_const_method_noexcept), clazz&&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_noexcept), const clazz&&>);

        static_assert(std::is_invocable_v<decltype(&clazz::non_const_method_noexcept_volatile), clazz&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_noexcept_volatile), const clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::non_const_method_noexcept_volatile), clazz&&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_noexcept_volatile), const clazz&&>);
    }

    SUBCASE("const_method") {
        const method_info mi = db.get_method_by_path("clazz::const_method");
        REQUIRE(mi);

        const method_info mi2 = db.get_method_by_path("clazz::const_method_volatile");
        REQUIRE(mi2);

        {
            CHECK(mi.type().arity() == 0);
            CHECK(mi.type().class_type() == type_db::get<clazz>());
            CHECK(mi.type().return_type() == type_db::get<int>());
            CHECK(mi.type().flags() == method_flags::is_const);

            CHECK(mi2.type().arity() == 0);
            CHECK(mi2.type().class_type() == type_db::get<clazz>());
            CHECK(mi2.type().return_type() == type_db::get<int>());
            CHECK(mi2.type().flags() == (method_flags::is_const | method_flags::is_volatile));
        }

        {
            CHECK(mi.is_invocable_with<clazz&>());
            CHECK(mi.is_invocable_with<const clazz&>());
            CHECK(mi.is_invocable_with<clazz&&>());
            CHECK(mi.is_invocable_with<const clazz&&>());

            CHECK(mi2.is_invocable_with<clazz&>());
            CHECK(mi2.is_invocable_with<const clazz&>());
            CHECK(mi2.is_invocable_with<clazz&&>());
            CHECK(mi2.is_invocable_with<const clazz&&>());
        }

        {
            clazz cl;

            CHECK(mi.invoke(cl)->equals(3));
            CHECK(mi.invoke(std::as_const(cl))->equals(3));
            CHECK(mi.invoke(std::move(cl))->equals(3));
            CHECK(mi.invoke(std::move(std::as_const(cl)))->equals(3));

            CHECK(mi2.invoke(cl)->equals(3));
            CHECK(mi2.invoke(std::as_const(cl))->equals(3));
            CHECK(mi2.invoke(std::move(cl))->equals(3));
            CHECK(mi2.invoke(std::move(std::as_const(cl)))->equals(3));
        }

        static_assert(std::is_invocable_v<decltype(&clazz::const_method), clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method), const clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method), clazz&&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method), const clazz&&>);

        static_assert(std::is_invocable_v<decltype(&clazz::const_method_volatile), clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method_volatile), const clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method_volatile), clazz&&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method_volatile), const clazz&&>);
    }

    SUBCASE("const_method_noexcept") {
        const method_info mi = db.get_method_by_path("clazz::const_method_noexcept");
        REQUIRE(mi);

        const method_info mi2 = db.get_method_by_path("clazz::const_method_noexcept_volatile");
        REQUIRE(mi2);

        {
            CHECK(mi.type().arity() == 0);
            CHECK(mi.type().class_type() == type_db::get<clazz>());
            CHECK(mi.type().return_type() == type_db::get<int>());
            CHECK(mi.type().flags() == (method_flags::is_const | method_flags::is_noexcept));

            CHECK(mi2.type().arity() == 0);
            CHECK(mi2.type().class_type() == type_db::get<clazz>());
            CHECK(mi2.type().return_type() == type_db::get<int>());
            CHECK(mi2.type().flags() == (method_flags::is_const | method_flags::is_noexcept | method_flags::is_volatile));
        }

        {
            CHECK(mi.is_invocable_with<clazz&>());
            CHECK(mi.is_invocable_with<const clazz&>());
            CHECK(mi.is_invocable_with<clazz&&>());
            CHECK(mi.is_invocable_with<const clazz&&>());

            CHECK(mi2.is_invocable_with<clazz&>());
            CHECK(mi2.is_invocable_with<const clazz&>());
            CHECK(mi2.is_invocable_with<clazz&&>());
            CHECK(mi2.is_invocable_with<const clazz&&>());
        }

        {
            clazz cl;

            CHECK(mi.invoke(cl)->equals(4));
            CHECK(mi.invoke(std::as_const(cl))->equals(4));
            CHECK(mi.invoke(std::move(cl))->equals(4));
            CHECK(mi.invoke(std::move(std::as_const(cl)))->equals(4));

            CHECK(mi2.invoke(cl)->equals(4));
            CHECK(mi2.invoke(std::as_const(cl))->equals(4));
            CHECK(mi2.invoke(std::move(cl))->equals(4));
            CHECK(mi2.invoke(std::move(std::as_const(cl)))->equals(4));
        }

        static_assert(std::is_invocable_v<decltype(&clazz::const_method_noexcept), clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method_noexcept), const clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method_noexcept), clazz&&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method_noexcept), const clazz&&>);

        static_assert(std::is_invocable_v<decltype(&clazz::const_method_noexcept_volatile), clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method_noexcept_volatile), const clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method_noexcept_volatile), clazz&&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method_noexcept_volatile), const clazz&&>);
    }

    SUBCASE("non_const_method_ref") {
        const method_info mi = db.get_method_by_path("clazz::non_const_method_ref");
        REQUIRE(mi);

        const method_info mi2 = db.get_method_by_path("clazz::non_const_method_ref_volatile");
        REQUIRE(mi2);

        {
            CHECK(mi.type().arity() == 0);
            CHECK(mi.type().class_type() == type_db::get<clazz>());
            CHECK(mi.type().return_type() == type_db::get<int>());
            CHECK(mi.type().flags() == method_flags::is_lvalue_qualified);

            CHECK(mi2.type().arity() == 0);
            CHECK(mi2.type().class_type() == type_db::get<clazz>());
            CHECK(mi2.type().return_type() == type_db::get<int>());
            CHECK(mi2.type().flags() == (method_flags::is_volatile | method_flags::is_lvalue_qualified));
        }

        {
            CHECK(mi.is_invocable_with<clazz&>());
            CHECK_FALSE(mi.is_invocable_with<const clazz&>());
            CHECK_FALSE(mi.is_invocable_with<clazz&&>());
            CHECK_FALSE(mi.is_invocable_with<const clazz&&>());

            CHECK(mi2.is_invocable_with<clazz&>());
            CHECK_FALSE(mi2.is_invocable_with<const clazz&>());
            CHECK_FALSE(mi2.is_invocable_with<clazz&&>());
            CHECK_FALSE(mi2.is_invocable_with<const clazz&&>());
        }

        {
            clazz cl;

            CHECK(mi.invoke(cl)->equals(5));
            CHECK_THROWS(mi.invoke(std::as_const(cl)));
            CHECK_THROWS(mi.invoke(std::move(cl)));
            CHECK_THROWS(mi.invoke(std::move(std::as_const(cl))));

            CHECK(mi2.invoke(cl)->equals(5));
            CHECK_THROWS(mi2.invoke(std::as_const(cl)));
            CHECK_THROWS(mi2.invoke(std::move(cl)));
            CHECK_THROWS(mi2.invoke(std::move(std::as_const(cl))));
        }

        static_assert(std::is_invocable_v<decltype(&clazz::non_const_method_ref), clazz&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_ref), const clazz&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_ref), clazz&&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_ref), const clazz&&>);

        static_assert(std::is_invocable_v<decltype(&clazz::non_const_method_ref_volatile), clazz&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_ref_volatile), const clazz&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_ref_volatile), clazz&&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_ref_volatile), const clazz&&>);
    }

    SUBCASE("non_const_method_noexcept_ref") {
        const method_info mi = db.get_method_by_path("clazz::non_const_method_noexcept_ref");
        REQUIRE(mi);

        const method_info mi2 = db.get_method_by_path("clazz::non_const_method_noexcept_ref_volatile");
        REQUIRE(mi2);

        {
            CHECK(mi.type().arity() == 0);
            CHECK(mi.type().class_type() == type_db::get<clazz>());
            CHECK(mi.type().return_type() == type_db::get<int>());
            CHECK(mi.type().flags() == (method_flags::is_noexcept | method_flags::is_lvalue_qualified));

            CHECK(mi2.type().arity() == 0);
            CHECK(mi2.type().class_type() == type_db::get<clazz>());
            CHECK(mi2.type().return_type() == type_db::get<int>());
            CHECK(mi2.type().flags() == (method_flags::is_noexcept | method_flags::is_volatile | method_flags::is_lvalue_qualified));
        }

        {
            CHECK(mi.is_invocable_with<clazz&>());
            CHECK_FALSE(mi.is_invocable_with<const clazz&>());
            CHECK_FALSE(mi.is_invocable_with<clazz&&>());
            CHECK_FALSE(mi.is_invocable_with<const clazz&&>());

            CHECK(mi2.is_invocable_with<clazz&>());
            CHECK_FALSE(mi2.is_invocable_with<const clazz&>());
            CHECK_FALSE(mi2.is_invocable_with<clazz&&>());
            CHECK_FALSE(mi2.is_invocable_with<const clazz&&>());
        }

        {
            clazz cl;

            CHECK(mi.invoke(cl)->equals(6));
            CHECK_THROWS(mi.invoke(std::as_const(cl)));
            CHECK_THROWS(mi.invoke(std::move(cl)));
            CHECK_THROWS(mi.invoke(std::move(std::as_const(cl))));

            CHECK(mi2.invoke(cl)->equals(6));
            CHECK_THROWS(mi2.invoke(std::as_const(cl)));
            CHECK_THROWS(mi2.invoke(std::move(cl)));
            CHECK_THROWS(mi2.invoke(std::move(std::as_const(cl))));
        }

        static_assert(std::is_invocable_v<decltype(&clazz::non_const_method_noexcept_ref), clazz&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_noexcept_ref), const clazz&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_noexcept_ref), clazz&&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_noexcept_ref), const clazz&&>);

        static_assert(std::is_invocable_v<decltype(&clazz::non_const_method_noexcept_ref_volatile), clazz&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_noexcept_ref_volatile), const clazz&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_noexcept_ref_volatile), clazz&&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_noexcept_ref_volatile), const clazz&&>);
    }

    SUBCASE("const_method_ref") {
        const method_info mi = db.get_method_by_path("clazz::const_method_ref");
        REQUIRE(mi);

        const method_info mi2 = db.get_method_by_path("clazz::const_method_ref_volatile");
        REQUIRE(mi2);

        {
            CHECK(mi.type().arity() == 0);
            CHECK(mi.type().class_type() == type_db::get<clazz>());
            CHECK(mi.type().return_type() == type_db::get<int>());
            CHECK(mi.type().flags() == (method_flags::is_const | method_flags::is_lvalue_qualified));

            CHECK(mi2.type().arity() == 0);
            CHECK(mi2.type().class_type() == type_db::get<clazz>());
            CHECK(mi2.type().return_type() == type_db::get<int>());
            CHECK(mi2.type().flags() == (method_flags::is_const | method_flags::is_volatile | method_flags::is_lvalue_qualified));
        }

        {
            CHECK(mi.is_invocable_with<clazz&>());
            CHECK(mi.is_invocable_with<const clazz&>());
            CHECK_FALSE(mi.is_invocable_with<clazz&&>());
            CHECK_FALSE(mi.is_invocable_with<const clazz&&>());

            CHECK(mi2.is_invocable_with<clazz&>());
            CHECK(mi2.is_invocable_with<const clazz&>());
            CHECK_FALSE(mi2.is_invocable_with<clazz&&>());
            CHECK_FALSE(mi2.is_invocable_with<const clazz&&>());
        }

        {
            clazz cl;

            CHECK(mi.invoke(cl)->equals(7));
            CHECK(mi.invoke(std::as_const(cl))->equals(7));
            CHECK_THROWS(mi.invoke(std::move(cl)));
            CHECK_THROWS(mi.invoke(std::move(std::as_const(cl))));

            CHECK(mi2.invoke(cl)->equals(7));
            CHECK(mi2.invoke(std::as_const(cl))->equals(7));
            CHECK_THROWS(mi2.invoke(std::move(cl)));
            CHECK_THROWS(mi2.invoke(std::move(std::as_const(cl))));
        }

        static_assert(std::is_invocable_v<decltype(&clazz::const_method_ref), clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method_ref), const clazz&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::const_method_ref), clazz&&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::const_method_ref), const clazz&&>);

        static_assert(std::is_invocable_v<decltype(&clazz::const_method_ref_volatile), clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method_ref_volatile), const clazz&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::const_method_ref_volatile), clazz&&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::const_method_ref_volatile), const clazz&&>);
    }

    SUBCASE("const_method_noexcept_ref") {
        const method_info mi = db.get_method_by_path("clazz::const_method_noexcept_ref");
        REQUIRE(mi);

        const method_info mi2 = db.get_method_by_path("clazz::const_method_noexcept_ref_volatile");
        REQUIRE(mi2);

        {
            CHECK(mi.type().arity() == 0);
            CHECK(mi.type().class_type() == type_db::get<clazz>());
            CHECK(mi.type().return_type() == type_db::get<int>());
            CHECK(mi.type().flags() == (method_flags::is_const | method_flags::is_noexcept | method_flags::is_lvalue_qualified));

            CHECK(mi2.type().arity() == 0);
            CHECK(mi2.type().class_type() == type_db::get<clazz>());
            CHECK(mi2.type().return_type() == type_db::get<int>());
            CHECK(mi2.type().flags() == (method_flags::is_const | method_flags::is_noexcept | method_flags::is_volatile | method_flags::is_lvalue_qualified));
        }

        {
            CHECK(mi.is_invocable_with<clazz&>());
            CHECK(mi.is_invocable_with<const clazz&>());
            CHECK_FALSE(mi.is_invocable_with<clazz&&>());
            CHECK_FALSE(mi.is_invocable_with<const clazz&&>());

            CHECK(mi2.is_invocable_with<clazz&>());
            CHECK(mi2.is_invocable_with<const clazz&>());
            CHECK_FALSE(mi2.is_invocable_with<clazz&&>());
            CHECK_FALSE(mi2.is_invocable_with<const clazz&&>());
        }

        {
            clazz cl;

            CHECK(mi.invoke(cl)->equals(8));
            CHECK(mi.invoke(std::as_const(cl))->equals(8));
            CHECK_THROWS(mi.invoke(std::move(cl)));
            CHECK_THROWS(mi.invoke(std::move(std::as_const(cl))));

            CHECK(mi2.invoke(cl)->equals(8));
            CHECK(mi2.invoke(std::as_const(cl))->equals(8));
            CHECK_THROWS(mi2.invoke(std::move(cl)));
            CHECK_THROWS(mi2.invoke(std::move(std::as_const(cl))));
        }

        static_assert(std::is_invocable_v<decltype(&clazz::const_method_noexcept_ref), clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method_noexcept_ref), const clazz&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::const_method_noexcept_ref), clazz&&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::const_method_noexcept_ref), const clazz&&>);

        static_assert(std::is_invocable_v<decltype(&clazz::const_method_noexcept_ref_volatile), clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method_noexcept_ref_volatile), const clazz&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::const_method_noexcept_ref_volatile), clazz&&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::const_method_noexcept_ref_volatile), const clazz&&>);
    }

    SUBCASE("non_const_method_rref") {
        const method_info mi = db.get_method_by_path("clazz::non_const_method_rref");
        REQUIRE(mi);

        const method_info mi2 = db.get_method_by_path("clazz::non_const_method_rref_volatile");
        REQUIRE(mi2);

        {
            CHECK(mi.type().arity() == 0);
            CHECK(mi.type().class_type() == type_db::get<clazz>());
            CHECK(mi.type().return_type() == type_db::get<int>());
            CHECK(mi.type().flags() == method_flags::is_rvalue_qualified);

            CHECK(mi2.type().arity() == 0);
            CHECK(mi2.type().class_type() == type_db::get<clazz>());
            CHECK(mi2.type().return_type() == type_db::get<int>());
            CHECK(mi2.type().flags() == (method_flags::is_volatile | method_flags::is_rvalue_qualified));
        }

        {
            CHECK_FALSE(mi.is_invocable_with<clazz&>());
            CHECK_FALSE(mi.is_invocable_with<const clazz&>());
            CHECK(mi.is_invocable_with<clazz&&>());
            CHECK_FALSE(mi.is_invocable_with<const clazz&&>());

            CHECK_FALSE(mi2.is_invocable_with<clazz&>());
            CHECK_FALSE(mi2.is_invocable_with<const clazz&>());
            CHECK(mi2.is_invocable_with<clazz&&>());
            CHECK_FALSE(mi2.is_invocable_with<const clazz&&>());
        }

        {
            clazz cl;

            CHECK_THROWS(mi.invoke(cl));
            CHECK_THROWS(mi.invoke(std::as_const(cl)));
            CHECK(mi.invoke(std::move(cl))->equals(9));
            CHECK_THROWS(mi.invoke(std::move(std::as_const(cl))));

            CHECK_THROWS(mi2.invoke(cl));
            CHECK_THROWS(mi2.invoke(std::as_const(cl)));
            CHECK(mi2.invoke(std::move(cl))->equals(9));
            CHECK_THROWS(mi2.invoke(std::move(std::as_const(cl))));
        }

        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_rref), clazz&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_rref), const clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::non_const_method_rref), clazz&&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_rref), const clazz&&>);

        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_rref_volatile), clazz&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_rref_volatile), const clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::non_const_method_rref_volatile), clazz&&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_rref_volatile), const clazz&&>);
    }

    SUBCASE("non_const_method_noexcept_rref") {
        const method_info mi = db.get_method_by_path("clazz::non_const_method_noexcept_rref");
        REQUIRE(mi);

        const method_info mi2 = db.get_method_by_path("clazz::non_const_method_noexcept_rref_volatile");
        REQUIRE(mi2);

        {
            CHECK(mi.type().arity() == 0);
            CHECK(mi.type().class_type() == type_db::get<clazz>());
            CHECK(mi.type().return_type() == type_db::get<int>());
            CHECK(mi.type().flags() == (method_flags::is_noexcept | method_flags::is_rvalue_qualified));

            CHECK(mi2.type().arity() == 0);
            CHECK(mi2.type().class_type() == type_db::get<clazz>());
            CHECK(mi2.type().return_type() == type_db::get<int>());
            CHECK(mi2.type().flags() == (method_flags::is_noexcept | method_flags::is_volatile | method_flags::is_rvalue_qualified));
        }

        {
            CHECK_FALSE(mi.is_invocable_with<clazz&>());
            CHECK_FALSE(mi.is_invocable_with<const clazz&>());
            CHECK(mi.is_invocable_with<clazz&&>());
            CHECK_FALSE(mi.is_invocable_with<const clazz&&>());

            CHECK_FALSE(mi2.is_invocable_with<clazz&>());
            CHECK_FALSE(mi2.is_invocable_with<const clazz&>());
            CHECK(mi2.is_invocable_with<clazz&&>());
            CHECK_FALSE(mi2.is_invocable_with<const clazz&&>());
        }

        {
            clazz cl;

            CHECK_THROWS(mi.invoke(cl));
            CHECK_THROWS(mi.invoke(std::as_const(cl)));
            CHECK(mi.invoke(std::move(cl))->equals(10));
            CHECK_THROWS(mi.invoke(std::move(std::as_const(cl))));

            CHECK_THROWS(mi2.invoke(cl));
            CHECK_THROWS(mi2.invoke(std::as_const(cl)));
            CHECK(mi2.invoke(std::move(cl))->equals(10));
            CHECK_THROWS(mi2.invoke(std::move(std::as_const(cl))));
        }

        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_noexcept_rref), clazz&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_noexcept_rref), const clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::non_const_method_noexcept_rref), clazz&&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_noexcept_rref), const clazz&&>);

        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_noexcept_rref_volatile), clazz&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_noexcept_rref_volatile), const clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::non_const_method_noexcept_rref_volatile), clazz&&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::non_const_method_noexcept_rref_volatile), const clazz&&>);
    }

    SUBCASE("const_method_rref") {
        const method_info mi = db.get_method_by_path("clazz::const_method_rref");
        REQUIRE(mi);

        const method_info mi2 = db.get_method_by_path("clazz::const_method_rref_volatile");
        REQUIRE(mi2);

        {
            CHECK(mi.type().arity() == 0);
            CHECK(mi.type().class_type() == type_db::get<clazz>());
            CHECK(mi.type().return_type() == type_db::get<int>());
            CHECK(mi.type().flags() == (method_flags::is_const | method_flags::is_rvalue_qualified));

            CHECK(mi2.type().arity() == 0);
            CHECK(mi2.type().class_type() == type_db::get<clazz>());
            CHECK(mi2.type().return_type() == type_db::get<int>());
            CHECK(mi2.type().flags() == (method_flags::is_const | method_flags::is_volatile | method_flags::is_rvalue_qualified));
        }

        {
            CHECK_FALSE(mi.is_invocable_with<clazz&>());
            CHECK_FALSE(mi.is_invocable_with<const clazz&>());
            CHECK(mi.is_invocable_with<clazz&&>());
            CHECK(mi.is_invocable_with<const clazz&&>());

            CHECK_FALSE(mi2.is_invocable_with<clazz&>());
            CHECK_FALSE(mi2.is_invocable_with<const clazz&>());
            CHECK(mi2.is_invocable_with<clazz&&>());
            CHECK(mi2.is_invocable_with<const clazz&&>());
        }

        {
            clazz cl;

            CHECK_THROWS(mi.invoke(cl));
            CHECK_THROWS(mi.invoke(std::as_const(cl)));
            CHECK(mi.invoke(std::move(cl))->equals(11));
            CHECK(mi.invoke(std::move(std::as_const(cl)))->equals(11));

            CHECK_THROWS(mi2.invoke(cl));
            CHECK_THROWS(mi2.invoke(std::as_const(cl)));
            CHECK(mi2.invoke(std::move(cl))->equals(11));
            CHECK(mi2.invoke(std::move(std::as_const(cl)))->equals(11));
        }

        static_assert(!std::is_invocable_v<decltype(&clazz::const_method_rref), clazz&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::const_method_rref), const clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method_rref), clazz&&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method_rref), const clazz&&>);

        static_assert(!std::is_invocable_v<decltype(&clazz::const_method_rref_volatile), clazz&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::const_method_rref_volatile), const clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method_rref_volatile), clazz&&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method_rref_volatile), const clazz&&>);
    }

    SUBCASE("const_method_noexcept_rref") {
        const method_info mi = db.get_method_by_path("clazz::const_method_noexcept_rref");
        REQUIRE(mi);

        const method_info mi2 = db.get_method_by_path("clazz::const_method_noexcept_rref_volatile");
        REQUIRE(mi2);

        {
            CHECK(mi.type().arity() == 0);
            CHECK(mi.type().class_type() == type_db::get<clazz>());
            CHECK(mi.type().return_type() == type_db::get<int>());
            CHECK(mi.type().flags() == (method_flags::is_const | method_flags::is_noexcept | method_flags::is_rvalue_qualified));

            CHECK(mi2.type().arity() == 0);
            CHECK(mi2.type().class_type() == type_db::get<clazz>());
            CHECK(mi2.type().return_type() == type_db::get<int>());
            CHECK(mi2.type().flags() == (method_flags::is_const | method_flags::is_noexcept | method_flags::is_volatile | method_flags::is_rvalue_qualified));
        }

        {
            CHECK_FALSE(mi.is_invocable_with<clazz&>());
            CHECK_FALSE(mi.is_invocable_with<const clazz&>());
            CHECK(mi.is_invocable_with<clazz&&>());
            CHECK(mi.is_invocable_with<const clazz&&>());

            CHECK_FALSE(mi2.is_invocable_with<clazz&>());
            CHECK_FALSE(mi2.is_invocable_with<const clazz&>());
            CHECK(mi2.is_invocable_with<clazz&&>());
            CHECK(mi2.is_invocable_with<const clazz&&>());
        }

        {
            clazz cl;

            CHECK_THROWS(mi.invoke(cl));
            CHECK_THROWS(mi.invoke(std::as_const(cl)));
            CHECK(mi.invoke(std::move(cl))->equals(12));
            CHECK(mi.invoke(std::move(std::as_const(cl)))->equals(12));

            CHECK_THROWS(mi2.invoke(cl));
            CHECK_THROWS(mi2.invoke(std::as_const(cl)));
            CHECK(mi2.invoke(std::move(cl))->equals(12));
            CHECK(mi2.invoke(std::move(std::as_const(cl)))->equals(12));
        }

        static_assert(!std::is_invocable_v<decltype(&clazz::const_method_noexcept_rref), clazz&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::const_method_noexcept_rref), const clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method_noexcept_rref), clazz&&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method_noexcept_rref), const clazz&&>);

        static_assert(!std::is_invocable_v<decltype(&clazz::const_method_noexcept_rref_volatile), clazz&>);
        static_assert(!std::is_invocable_v<decltype(&clazz::const_method_noexcept_rref_volatile), const clazz&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method_noexcept_rref_volatile), clazz&&>);
        static_assert(std::is_invocable_v<decltype(&clazz::const_method_noexcept_rref_volatile), const clazz&&>);
    }
}
