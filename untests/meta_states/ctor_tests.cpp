/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "../meta_untests.hpp"

namespace
{
    template < std::size_t N >
    struct clazz {
        int i{};

        clazz(int i)
        : i{i} {
            ++ctor_counter;
        }

        clazz(clazz&& other)
        : i{other.i} {
            other.i = 0;
            ++move_ctor_counter;
        }

        clazz(const clazz& other)
        : i{other.i} {
            ++copy_ctor_counter;
        }

        clazz& operator=(clazz&& other) = delete;
        clazz& operator=(const clazz& other) = delete;

        ~clazz() {
            ++dtor_counter;
        }
    public:
        static int ctor_counter;
        static int dtor_counter;
        static int move_ctor_counter;
        static int copy_ctor_counter;
    };

    template < std::size_t N >
    int clazz<N>::ctor_counter{0};

    template < std::size_t N >
    int clazz<N>::dtor_counter{0};

    template < std::size_t N >
    int clazz<N>::move_ctor_counter{0};

    template < std::size_t N >
    int clazz<N>::copy_ctor_counter{0};
}

TEST_CASE("meta/meta_states/ctor") {
    namespace meta = meta_hpp;

    meta::class_<clazz<1>>()
        .ctor_<int>(meta::ctor_policy::as_object{})
        .dtor_();

    meta::class_<clazz<2>>()
        .ctor_<int>(meta::ctor_policy::as_raw_pointer{})
        .dtor_();

    meta::class_<clazz<3>>()
        .ctor_<int>(meta::ctor_policy::as_shared_pointer{})
        .dtor_();

    clazz<1>::ctor_counter = 0;
    clazz<1>::dtor_counter = 0;
    clazz<1>::move_ctor_counter = 0;
    clazz<1>::copy_ctor_counter = 0;

    clazz<2>::ctor_counter = 0;
    clazz<2>::dtor_counter = 0;
    clazz<2>::move_ctor_counter = 0;
    clazz<2>::copy_ctor_counter = 0;

    clazz<3>::ctor_counter = 0;
    clazz<3>::dtor_counter = 0;
    clazz<3>::move_ctor_counter = 0;
    clazz<3>::copy_ctor_counter = 0;

    SUBCASE("clazz<1>") {
        const meta::class_type clazz_type = meta::resolve_type<clazz<1>>();
        REQUIRE(clazz_type);

        {
            CHECK_FALSE(clazz_type.create(10, 20));
            const meta::value v = clazz_type.create(10);
            CHECK(v.get_type() == meta::resolve_type<clazz<1>>());
            CHECK(v.cast<clazz<1>>().i == 10);

            CHECK(clazz_type.destroy(nullptr));
            CHECK(clazz_type.destroy(meta::value{nullptr}));
        }

        CHECK(clazz<1>::ctor_counter == 1);
        CHECK(clazz<1>::dtor_counter == 2);
        CHECK(clazz<1>::move_ctor_counter == 1);
        CHECK(clazz<1>::copy_ctor_counter == 0);
    }

    SUBCASE("clazz<2>") {
        const meta::class_type clazz_type = meta::resolve_type<clazz<2>>();
        REQUIRE(clazz_type);

        {
            CHECK_FALSE(clazz_type.create(10, 20));
            const meta::value v = clazz_type.create(20);
            CHECK(v.get_type() == meta::resolve_type<clazz<2>*>());
            CHECK(v.cast<clazz<2>*>()->i == 20);
            CHECK(clazz_type.destroy(v));

            CHECK(clazz_type.destroy(nullptr));
            CHECK(clazz_type.destroy(meta::value{nullptr}));
        }

        CHECK(clazz<2>::ctor_counter == 1);
        CHECK(clazz<2>::dtor_counter == 1);
        CHECK(clazz<2>::move_ctor_counter == 0);
        CHECK(clazz<2>::copy_ctor_counter == 0);
    }

    SUBCASE("clazz<3>") {
        const meta::class_type clazz_type = meta::resolve_type<clazz<3>>();
        REQUIRE(clazz_type);

        {
            CHECK_FALSE(clazz_type.create(10, 20));
            const meta::value v = clazz_type.create(30);
            CHECK(v.get_type() == meta::resolve_type<std::shared_ptr<clazz<3>>>());
            CHECK(v.cast<std::shared_ptr<clazz<3>>>()->i == 30);

            CHECK(clazz_type.destroy(nullptr));
            CHECK(clazz_type.destroy(meta::value{nullptr}));
        }

        CHECK(clazz<3>::ctor_counter == 1);
        CHECK(clazz<3>::dtor_counter == 1);
        CHECK(clazz<3>::move_ctor_counter == 0);
        CHECK(clazz<3>::copy_ctor_counter == 0);
    }
}
