/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "../meta_untests.hpp"

namespace
{
}

TEST_CASE("meta/meta_base/memory_buffer") {
    namespace meta = meta_hpp;
    using meta::detail::memory_buffer;

    SUBCASE("ctor/0") {
        memory_buffer buf;

        CHECK(!buf);
        CHECK(!buf.is_valid());

        CHECK(buf.get_size() == 0);
        CHECK(buf.get_align() == std::align_val_t{});

        CHECK(buf.get_memory() == nullptr);
        CHECK(std::as_const(buf).get_memory() == nullptr);
    }

    SUBCASE("ctor/1") {
        memory_buffer buf1{10, std::align_val_t{32}};
        const void* buf1_memory{buf1.get_memory()};

        memory_buffer buf2{std::move(buf1)};

        {
            CHECK(!buf1);
            CHECK(!buf1.is_valid());

            CHECK(buf1.get_size() == 0);
            CHECK(buf1.get_align() == std::align_val_t{});

            CHECK(buf1.get_memory() == nullptr);
            CHECK(std::as_const(buf1).get_memory() == nullptr);
        }

        {
            CHECK(buf2);
            CHECK(buf2.is_valid());

            CHECK(buf2.get_size() == 10);
            CHECK(buf2.get_align() == std::align_val_t{32});

            CHECK(buf2.get_memory() == buf1_memory);
            CHECK(std::as_const(buf2).get_memory() == buf1_memory);
        }
    }

    SUBCASE("ctor/2") {
        memory_buffer buf{10, std::align_val_t{32}};

        CHECK(buf);
        CHECK(buf.is_valid());

        CHECK(buf.get_size() == 10);
        CHECK(buf.get_align() == std::align_val_t{32});

        CHECK(buf.get_memory());
        CHECK(std::as_const(buf).get_memory());

        {
            void* aligned_ptr{buf.get_memory()};
            std::size_t aligned_size{buf.get_size()};
            CHECK(std::align(
                meta::detail::to_underlying(buf.get_align()), buf.get_size(),
                aligned_ptr, aligned_size) == buf.get_memory());
        }
    }

    SUBCASE("operator=/0") {
        memory_buffer buf1{10, std::align_val_t{32}};
        const void* buf1_memory{buf1.get_memory()};

        memory_buffer buf2;
        buf2 = std::move(buf1);

        {
            CHECK(!buf1);
            CHECK(!buf1.is_valid());

            CHECK(buf1.get_size() == 0);
            CHECK(buf1.get_align() == std::align_val_t{});

            CHECK(buf1.get_memory() == nullptr);
            CHECK(std::as_const(buf1).get_memory() == nullptr);
        }

        {
            CHECK(buf2);
            CHECK(buf2.is_valid());

            CHECK(buf2.get_size() == 10);
            CHECK(buf2.get_align() == std::align_val_t{32});

            CHECK(buf2.get_memory() == buf1_memory);
            CHECK(std::as_const(buf2).get_memory() == buf1_memory);
        }
    }

    SUBCASE("operator=/1") {
        memory_buffer buf1{10, std::align_val_t{32}};
        const void* buf1_memory{buf1.get_memory()};

        memory_buffer buf2{20, std::align_val_t{16}};
        buf2 = std::move(buf1);

        {
            CHECK(!buf1);
            CHECK(!buf1.is_valid());

            CHECK(buf1.get_size() == 0);
            CHECK(buf1.get_align() == std::align_val_t{});

            CHECK(buf1.get_memory() == nullptr);
            CHECK(std::as_const(buf1).get_memory() == nullptr);
        }

        {
            CHECK(buf2);
            CHECK(buf2.is_valid());

            CHECK(buf2.get_size() == 10);
            CHECK(buf2.get_align() == std::align_val_t{32});

            CHECK(buf2.get_memory() == buf1_memory);
            CHECK(std::as_const(buf2).get_memory() == buf1_memory);
        }
    }

    SUBCASE("reset") {
        memory_buffer buf{10, std::align_val_t{32}};
        buf.reset();

        CHECK(!buf);
        CHECK(!buf.is_valid());

        CHECK(buf.get_size() == 0);
        CHECK(buf.get_align() == std::align_val_t{});

        CHECK(buf.get_memory() == nullptr);
        CHECK(std::as_const(buf).get_memory() == nullptr);
    }

    SUBCASE("swap") {
        memory_buffer buf1{10, std::align_val_t{32}};
        memory_buffer buf2{15, std::align_val_t{16}};

        const void* buf1_memory{buf1.get_memory()};
        const void* buf2_memory{buf2.get_memory()};

        meta::detail::swap(buf1, buf2);

        CHECK(buf1.get_size() == 15);
        CHECK(buf1.get_align() == std::align_val_t{16});
        CHECK(buf1.get_memory() == buf2_memory);

        CHECK(buf2.get_size() == 10);
        CHECK(buf2.get_align() == std::align_val_t{32});
        CHECK(buf2.get_memory() == buf1_memory);
    }
}
