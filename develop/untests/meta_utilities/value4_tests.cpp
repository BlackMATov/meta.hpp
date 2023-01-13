/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "../meta_untests.hpp"

namespace
{
    struct clazz_throw_dtor {
        int i{};

        clazz_throw_dtor() {
            ++constructor_counter;
        }

        clazz_throw_dtor(int ni) : i{ni} {
            ++constructor_counter;
        }

        clazz_throw_dtor(clazz_throw_dtor&& other)
        : i{other.i} {
            other.i = 0;
            ++move_constructor_counter;
        }

        clazz_throw_dtor(const clazz_throw_dtor& other)
        : i{other.i} {
            ++copy_constructor_counter;
        }

        ~clazz_throw_dtor() noexcept(false) {
            ++destructor_counter;
        }

        static clazz_throw_dtor make(int ni) {
            return {ni};
        }

        inline static int destructor_counter{};
        inline static int constructor_counter{};
        inline static int move_constructor_counter{};
        inline static int copy_constructor_counter{};
    };
}

TEST_CASE("meta/meta_utilities/value5") {
    namespace meta = meta_hpp;

    meta::class_<clazz_throw_dtor>()
        .function_("make", &clazz_throw_dtor::make);
}

TEST_CASE("meta/meta_utilities/value5/throw_dtor") {
    namespace meta = meta_hpp;

    SUBCASE("value") {
        meta::uvalue v{clazz_throw_dtor{42}};

        CHECK(v.get_type() == meta::resolve_type<clazz_throw_dtor>());
        CHECK(v.get_as<clazz_throw_dtor>().i == 42);
    }

    SUBCASE("ptr_deref") {
        clazz_throw_dtor obj{42};

        meta::uvalue v_ptr{&obj};
        CHECK(v_ptr.get_type() == meta::resolve_type<clazz_throw_dtor*>());
        CHECK(v_ptr.get_as<clazz_throw_dtor*>() == &obj);

        meta::uvalue v = *v_ptr;
        CHECK(v.get_type() == meta::resolve_type<clazz_throw_dtor>());
        CHECK(v.get_as<clazz_throw_dtor>().i == 42);
    }

    SUBCASE("array_index") {
        clazz_throw_dtor objs[2]{42, 21};

        meta::uvalue v_ptr{objs};
        CHECK(v_ptr.get_type() == meta::resolve_type<clazz_throw_dtor*>());
        CHECK(v_ptr.get_as<clazz_throw_dtor*>() == objs);

        meta::uvalue v = v_ptr[1];
        CHECK(v.get_type() == meta::resolve_type<clazz_throw_dtor>());
        CHECK(v.get_as<clazz_throw_dtor>().i == 21);
    }

    SUBCASE("as_return_value") {
        meta::class_type clazz_throw_dtor_type = meta::resolve_type<clazz_throw_dtor>();
        meta::function clazz_throw_dtor_make_function = clazz_throw_dtor_type.get_function("make");

        meta::uvalue v{clazz_throw_dtor_make_function(42)};
        CHECK(v.get_type() == meta::resolve_type<clazz_throw_dtor>());
        CHECK(v.get_as<clazz_throw_dtor>().i == 42);
    }
}

TEST_CASE("meta/meta_utilities/value5/inplace") {
    namespace meta = meta_hpp;

    clazz_throw_dtor::destructor_counter = 0;
    clazz_throw_dtor::constructor_counter = 0;
    clazz_throw_dtor::move_constructor_counter = 0;
    clazz_throw_dtor::copy_constructor_counter = 0;

    SUBCASE("def") {
        meta::uvalue v = meta::make_uvalue<clazz_throw_dtor>();
        CHECK(v.get_type() == meta::resolve_type<clazz_throw_dtor>());
        CHECK(v.get_as<clazz_throw_dtor>().i == 0);

        CHECK(clazz_throw_dtor::destructor_counter == 0);
        CHECK(clazz_throw_dtor::constructor_counter == 1);
        CHECK(clazz_throw_dtor::move_constructor_counter == 0);
        CHECK(clazz_throw_dtor::copy_constructor_counter == 0);
    }

    SUBCASE("args") {
        meta::uvalue v = meta::make_uvalue<std::vector<clazz_throw_dtor>>(2, 42);
        CHECK(v.get_type() == meta::resolve_type<std::vector<clazz_throw_dtor>>());

        CHECK(v[0].get_type() == meta::resolve_type<clazz_throw_dtor>());
        CHECK(v[0].get_as<clazz_throw_dtor>().i == 42);

        CHECK(v[1].get_type() == meta::resolve_type<clazz_throw_dtor>());
        CHECK(v[1].get_as<clazz_throw_dtor>().i == 42);
    }

    SUBCASE("args/counters") {
        {
            meta::uvalue v = meta::make_uvalue<std::vector<clazz_throw_dtor>>(2, 42);
            CHECK(v.get_type() == meta::resolve_type<std::vector<clazz_throw_dtor>>());
        }
        CHECK(clazz_throw_dtor::destructor_counter == 3);
        CHECK(clazz_throw_dtor::constructor_counter == 1);
        CHECK(clazz_throw_dtor::move_constructor_counter == 0);
        CHECK(clazz_throw_dtor::copy_constructor_counter == 2);
    }

    SUBCASE("ilist/1") {
        meta::uvalue v = meta::make_uvalue<std::vector<int>>({21, 42});
        CHECK(v.get_type() == meta::resolve_type<std::vector<int>>());

        CHECK(v[0].get_type() == meta::resolve_type<int>());
        CHECK(v[0].get_as<int>() == 21);

        CHECK(v[1].get_type() == meta::resolve_type<int>());
        CHECK(v[1].get_as<int>() == 42);
    }

    SUBCASE("ilist/2") {
        meta::uvalue v = meta::make_uvalue<std::vector<int>>({21, 42}, std::allocator<int>{});
        CHECK(v.get_type() == meta::resolve_type<std::vector<int, std::allocator<int>>>());

        CHECK(v[0].get_type() == meta::resolve_type<int>());
        CHECK(v[0].get_as<int>() == 21);

        CHECK(v[1].get_type() == meta::resolve_type<int>());
        CHECK(v[1].get_as<int>() == 42);
    }

    SUBCASE("ilist/counters") {
        {
            meta::uvalue v = meta::make_uvalue<std::vector<clazz_throw_dtor>>({
                clazz_throw_dtor{21},
                clazz_throw_dtor{42}});
            CHECK(v.get_type() == meta::resolve_type<std::vector<clazz_throw_dtor>>());
        }
        CHECK(clazz_throw_dtor::destructor_counter == 4);
        CHECK(clazz_throw_dtor::constructor_counter == 2);
        CHECK(clazz_throw_dtor::move_constructor_counter == 0);
        CHECK(clazz_throw_dtor::copy_constructor_counter == 2);
    }
}

TEST_CASE("meta/meta_utilities/value5/emplace") {
    namespace meta = meta_hpp;

    clazz_throw_dtor::destructor_counter = 0;
    clazz_throw_dtor::constructor_counter = 0;
    clazz_throw_dtor::move_constructor_counter = 0;
    clazz_throw_dtor::copy_constructor_counter = 0;

    SUBCASE("def") {
        {
            meta::uvalue v = meta::make_uvalue<clazz_throw_dtor>(21);
            CHECK(v.emplace<clazz_throw_dtor>().i == 0);
            CHECK(v.get_type() == meta::resolve_type<clazz_throw_dtor>());
            CHECK(v.get_as<clazz_throw_dtor>().i == 0);
        }
        CHECK(clazz_throw_dtor::destructor_counter == 2);
        CHECK(clazz_throw_dtor::constructor_counter == 2);
        CHECK(clazz_throw_dtor::move_constructor_counter == 0);
        CHECK(clazz_throw_dtor::copy_constructor_counter == 0);
    }

    SUBCASE("args") {
        meta::uvalue v = meta::make_uvalue<clazz_throw_dtor>(21);
        v.emplace<std::vector<clazz_throw_dtor>>(2, 42);
        CHECK(v.get_type() == meta::resolve_type<std::vector<clazz_throw_dtor>>());

        CHECK(v[0].get_type() == meta::resolve_type<clazz_throw_dtor>());
        CHECK(v[0].get_as<clazz_throw_dtor>().i == 42);

        CHECK(v[1].get_type() == meta::resolve_type<clazz_throw_dtor>());
        CHECK(v[1].get_as<clazz_throw_dtor>().i == 42);
    }

    SUBCASE("args/counters") {
        {
            meta::uvalue v = meta::make_uvalue<clazz_throw_dtor>(21);
            v.emplace<std::vector<clazz_throw_dtor>>(2, 42);
            CHECK(v.get_type() == meta::resolve_type<std::vector<clazz_throw_dtor>>());
        }
        CHECK(clazz_throw_dtor::destructor_counter == 4);
        CHECK(clazz_throw_dtor::constructor_counter == 2);
        CHECK(clazz_throw_dtor::move_constructor_counter == 0);
        CHECK(clazz_throw_dtor::copy_constructor_counter == 2);
    }

    SUBCASE("ilist/1") {
        meta::uvalue v = meta::make_uvalue<clazz_throw_dtor>(84);
        v.emplace<std::vector<int>>({21, 42});
        CHECK(v.get_type() == meta::resolve_type<std::vector<int>>());

        CHECK(v[0].get_type() == meta::resolve_type<int>());
        CHECK(v[0].get_as<int>() == 21);

        CHECK(v[1].get_type() == meta::resolve_type<int>());
        CHECK(v[1].get_as<int>() == 42);
    }

    SUBCASE("ilist/2") {
        meta::uvalue v = meta::make_uvalue<clazz_throw_dtor>(84);
        v.emplace<std::vector<int>>({21, 42}, std::allocator<int>{});
        CHECK(v.get_type() == meta::resolve_type<std::vector<int, std::allocator<int>>>());

        CHECK(v[0].get_type() == meta::resolve_type<int>());
        CHECK(v[0].get_as<int>() == 21);

        CHECK(v[1].get_type() == meta::resolve_type<int>());
        CHECK(v[1].get_as<int>() == 42);
    }

    SUBCASE("ilist/counters") {
        {
            meta::uvalue v = meta::make_uvalue<clazz_throw_dtor>(84);
            v.emplace<std::vector<clazz_throw_dtor>>({
                clazz_throw_dtor{21},
                clazz_throw_dtor{42}});
            CHECK(v.get_type() == meta::resolve_type<std::vector<clazz_throw_dtor>>());
        }
        CHECK(clazz_throw_dtor::destructor_counter == 5);
        CHECK(clazz_throw_dtor::constructor_counter == 3);
        CHECK(clazz_throw_dtor::move_constructor_counter == 0);
        CHECK(clazz_throw_dtor::copy_constructor_counter == 2);
    }
}
