/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "../meta_untests.hpp"

namespace
{
}

TEST_CASE("meta/meta_base/fixed_function") {
    namespace meta = meta_hpp;
    using meta::detail::fixed_function;

    SUBCASE("is_valid") {
        {
            fixed_function<void()> ff;
            CHECK_FALSE(ff);
            CHECK_FALSE(ff.is_valid());
        }
        {
            fixed_function<void()> ff = []{};
            CHECK(ff);
            CHECK(ff.is_valid());
        }
    }

    SUBCASE("ctor") {
        {
            auto f1 = []{return 1;};
            fixed_function<int()> ff{std::move(f1)};
            CHECK(ff() == 1);

            auto f2 = fixed_function{[]{return 2;}};
            ff = std::move(f2);
            CHECK(ff() == 2);
        }
        {
            auto f1 = []() noexcept {return 1;};
            fixed_function ff{f1};
            CHECK(ff() == 1);
        }
    }

    SUBCASE("reset") {
        fixed_function ff = []{return 0;};

        ff.reset();
        CHECK_FALSE(ff);
        CHECK_FALSE(ff.is_valid());

        ff = []{return 1;};
        CHECK(ff);
        CHECK(ff.is_valid());

        CHECK(ff() == 1);
    }

    SUBCASE("move") {
        fixed_function ff = []{return 1;};
        fixed_function ff2 = std::move(ff);
        CHECK(ff2() == 1);
        CHECK_FALSE(ff);
    }

    SUBCASE("operator=") {
        {
            fixed_function<int()> ff;
            ff = []{return 0;};
            CHECK(ff() == 0);
            ff = []{return 1;};
            CHECK(ff() == 1);

            fixed_function<int()> ff2 = []{return 2;};
            ff = std::move(ff2);
            CHECK(ff() == 2);
            CHECK_FALSE(ff2);
        }
    }

    SUBCASE("swap") {
        {
            fixed_function<void()> ff1;
            fixed_function<void()> ff2;
            ff1.swap(ff2);
            CHECK_FALSE(ff1);
            CHECK_FALSE(ff2);
        }
        {
            fixed_function ff1 = []{return 1;};
            fixed_function ff2 = []{return 2;};
            ff1.swap(ff2);
            CHECK(ff1() == 2);
            CHECK(ff2() == 1);
        }
        {
            fixed_function<int()> ff1;
            fixed_function ff2 = []{return 2;};

            ff1.swap(ff2);
            CHECK(ff1() == 2);
            CHECK_FALSE(ff2);

            ff1.swap(ff2);
            CHECK_FALSE(ff1);
            CHECK(ff2() == 2);
        }
    }

    SUBCASE("-> int") {
        auto f = [
            s = std::make_unique<int>(10)
        ](){ return 0; };
        fixed_function ff = std::move(f);
        CHECK(ff() == 0);
    }

    SUBCASE("-> void") {
        auto f = [
            s = std::make_unique<int>(10)
        ](){};
        fixed_function ff = std::move(f);
        CHECK_NOTHROW(ff());
    }
}
