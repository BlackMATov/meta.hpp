/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

namespace
{
    namespace meta = meta_hpp;
    using meta::detail::intrusive_ptr;
    using meta::detail::intrusive_ref_counter;

    struct obj_t final : intrusive_ref_counter<obj_t> {
        int v{42};

        obj_t() { ++ctor_counter; }
        obj_t(int nv) : v{nv} { ++ctor_counter; }
        ~obj_t() { ++dtor_counter; }

        inline static std::size_t ctor_counter{};
        inline static std::size_t dtor_counter{};
    };
}

TEST_CASE("meta/meta_base/intrusive_ptr") {
    obj_t::ctor_counter = 0;
    obj_t::dtor_counter = 0;

    SUBCASE("ctor/0") {
        intrusive_ptr<obj_t> ptr;
        CHECK_FALSE(ptr);
    }

    SUBCASE("copy_ctor/0") {
        intrusive_ptr<obj_t> ptr;
        intrusive_ptr<obj_t> ptr2{ptr};
        CHECK_FALSE(ptr2);
    }

    SUBCASE("move_ctor/0") {
        intrusive_ptr<obj_t> ptr;
        intrusive_ptr<obj_t> ptr2{std::move(ptr)};
        CHECK_FALSE(ptr2);
    }

    SUBCASE("ctor/1") {
        {
            intrusive_ptr<obj_t> ptr{std::make_unique<obj_t>().release()};
            CHECK(ptr);
            CHECK(ptr->get_use_count() == 1);
            CHECK(ptr->v == 42);
            CHECK(obj_t::ctor_counter == 1);
            CHECK(obj_t::dtor_counter == 0);
        }
        CHECK(obj_t::ctor_counter == 1);
        CHECK(obj_t::dtor_counter == 1);
    }

    SUBCASE("copy_ctor/1") {
        {
            intrusive_ptr<obj_t> ptr{std::make_unique<obj_t>().release()};
            {
                intrusive_ptr<obj_t> ptr2{ptr};
                CHECK(ptr);
                CHECK(ptr2);
                CHECK(ptr2->get_use_count() == 2);
                CHECK(ptr2->v == 42);
            }
            CHECK(obj_t::ctor_counter == 1);
            CHECK(obj_t::dtor_counter == 0);
        }
        CHECK(obj_t::ctor_counter == 1);
        CHECK(obj_t::dtor_counter == 1);
    }

    SUBCASE("move_ctor/1") {
        {
            intrusive_ptr<obj_t> ptr{std::make_unique<obj_t>().release()};
            {
                intrusive_ptr<obj_t> ptr2{std::move(ptr)};
                CHECK_FALSE(ptr);
                CHECK(ptr2);
                CHECK(ptr2->get_use_count() == 1);
                CHECK(ptr2->v == 42);
            }
            CHECK(obj_t::ctor_counter == 1);
            CHECK(obj_t::dtor_counter == 1);
        }
        CHECK(obj_t::ctor_counter == 1);
        CHECK(obj_t::dtor_counter == 1);
    }

    SUBCASE("operator=/ptr") {
        {
            intrusive_ptr<obj_t> ptr1{std::make_unique<obj_t>().release()};
            ptr1 = std::make_unique<obj_t>().release();
            CHECK(obj_t::ctor_counter == 2);
            CHECK(obj_t::dtor_counter == 1);
        }
        CHECK(obj_t::ctor_counter == 2);
        CHECK(obj_t::dtor_counter == 2);
    }

    SUBCASE("operator=/copy") {
        {
            intrusive_ptr<obj_t> ptr1{std::make_unique<obj_t>().release()};
            intrusive_ptr<obj_t> ptr2{std::make_unique<obj_t>().release()};
            ptr1 = ptr2;
            CHECK(ptr1);
            CHECK(ptr2);
            CHECK(obj_t::ctor_counter == 2);
            CHECK(obj_t::dtor_counter == 1);
        }
        CHECK(obj_t::ctor_counter == 2);
        CHECK(obj_t::dtor_counter == 2);
    }

    SUBCASE("operator=/move") {
        {
            intrusive_ptr<obj_t> ptr1{std::make_unique<obj_t>().release()};
            intrusive_ptr<obj_t> ptr2{std::make_unique<obj_t>().release()};
            ptr1 = std::move(ptr2);
            CHECK(ptr1);
            CHECK_FALSE(ptr2);
            CHECK(obj_t::ctor_counter == 2);
            CHECK(obj_t::dtor_counter == 1);
        }
        CHECK(obj_t::ctor_counter == 2);
        CHECK(obj_t::dtor_counter == 2);
    }

    SUBCASE("reset/0") {
        intrusive_ptr<obj_t> ptr{std::make_unique<obj_t>().release()};
        ptr.reset();
        CHECK(obj_t::ctor_counter == 1);
        CHECK(obj_t::dtor_counter == 1);
    }

    SUBCASE("reset/1") {
        {
            intrusive_ptr<obj_t> ptr{std::make_unique<obj_t>().release()};
            ptr.reset(std::make_unique<obj_t>().release());
            CHECK(obj_t::ctor_counter == 2);
            CHECK(obj_t::dtor_counter == 1);
        }
        CHECK(obj_t::ctor_counter == 2);
        CHECK(obj_t::dtor_counter == 2);
    }

    SUBCASE("get") {
        intrusive_ptr<obj_t> ptr{std::make_unique<obj_t>().release()};
        CHECK(ptr.get()->v == 42);
        CHECK(ptr->v == 42);
        CHECK((*ptr).v == 42);
        ptr.reset();
        CHECK_FALSE(ptr.get());
    }

    SUBCASE("release") {
        obj_t* obj{};
        {
            intrusive_ptr<obj_t> ptr{std::make_unique<obj_t>().release()};
            obj = ptr.release();
            CHECK_FALSE(ptr);
        }
        CHECK(obj_t::ctor_counter == 1);
        CHECK(obj_t::dtor_counter == 0);
        {
            intrusive_ptr<obj_t> ptr2{obj, false};
            CHECK(ptr2->get_use_count() == 1);
        }
        CHECK(obj_t::ctor_counter == 1);
        CHECK(obj_t::dtor_counter == 1);
    }

    SUBCASE("swap") {
        intrusive_ptr<obj_t> ptr1{std::make_unique<obj_t>(42).release()};
        intrusive_ptr<obj_t> ptr2{std::make_unique<obj_t>(21).release()};
        swap(ptr1, ptr2);
        CHECK(ptr1->v == 21);
        CHECK(ptr2->v == 42);
    }

    SUBCASE("operator==/!=") {
        intrusive_ptr<obj_t> ptr1{std::make_unique<obj_t>(42).release()};
        intrusive_ptr<obj_t> ptr1_copy{ptr1};
        intrusive_ptr<obj_t> ptr2{std::make_unique<obj_t>(42).release()};
        intrusive_ptr<obj_t> ptr3;

        CHECK(ptr1 == ptr1_copy);
        CHECK(ptr1 == ptr1_copy.get());
        CHECK(ptr1_copy.get() == ptr1);
        CHECK_FALSE(ptr1 == ptr2);
        CHECK_FALSE(ptr1 == ptr3);

        CHECK_FALSE(ptr1 != ptr1_copy);
        CHECK_FALSE(ptr1 != ptr1_copy.get());
        CHECK_FALSE(ptr1_copy.get() != ptr1);
        CHECK(ptr1 != ptr2);
        CHECK(ptr1 != ptr3);

        CHECK_FALSE(ptr1 == nullptr);
        CHECK_FALSE(nullptr == ptr1);
        CHECK(ptr1 != nullptr);
        CHECK(nullptr != ptr1);

        CHECK(ptr3 == nullptr);
        CHECK(nullptr == ptr3);
        CHECK_FALSE(ptr3 != nullptr);
        CHECK_FALSE(nullptr != ptr3);
    }
}
