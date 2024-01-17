/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

TEST_CASE("meta/meta_base/memory_buffer") {
    namespace meta = meta_hpp;
    using meta::detail::memory_buffer;

    SUBCASE("ctor/0") {
        memory_buffer buf;

        CHECK(!buf);
        CHECK_FALSE(buf.is_valid());

        CHECK(buf.get_size() == 0);
        CHECK(buf.get_align() == std::align_val_t{});

        CHECK(buf.get_data() == nullptr);
        CHECK(std::as_const(buf).get_data() == nullptr);
    }

    SUBCASE("ctor/1") {
        memory_buffer buf1{10, std::align_val_t{32}};
        const void* buf1_memory{buf1.get_data()};

        memory_buffer buf2{std::move(buf1)};

        {
            CHECK(!buf1);
            CHECK_FALSE(buf1.is_valid());

            CHECK(buf1.get_size() == 0);
            CHECK(buf1.get_align() == std::align_val_t{});

            CHECK(buf1.get_data() == nullptr);
            CHECK(std::as_const(buf1).get_data() == nullptr);
        }

        {
            CHECK(buf2);
            CHECK(buf2.is_valid());

            CHECK(buf2.get_size() == 10);
            CHECK(buf2.get_align() == std::align_val_t{32});

            CHECK(buf2.get_data() == buf1_memory);
            CHECK(std::as_const(buf2).get_data() == buf1_memory);
        }
    }

    SUBCASE("ctor/2") {
        memory_buffer buf{10, std::align_val_t{32}};

        CHECK(buf);
        CHECK(buf.is_valid());

        CHECK(buf.get_size() == 10);
        CHECK(buf.get_align() == std::align_val_t{32});

        CHECK(buf.get_data());
        CHECK(std::as_const(buf).get_data());

        {
            void* aligned_ptr{buf.get_data()};
            std::size_t aligned_size{buf.get_size()};
            std::size_t align{meta::detail::to_underlying(buf.get_align())};
            CHECK(std::align(align, buf.get_size(), aligned_ptr, aligned_size) == buf.get_data());
        }
    }

    SUBCASE("operator=/0") {
        memory_buffer buf1{10, std::align_val_t{32}};
        const void* buf1_memory{buf1.get_data()};

        memory_buffer buf2;
        buf2 = std::move(buf1);

        {
            CHECK(!buf1);
            CHECK_FALSE(buf1.is_valid());

            CHECK(buf1.get_size() == 0);
            CHECK(buf1.get_align() == std::align_val_t{});

            CHECK(buf1.get_data() == nullptr);
            CHECK(std::as_const(buf1).get_data() == nullptr);
        }

        {
            CHECK(buf2);
            CHECK(buf2.is_valid());

            CHECK(buf2.get_size() == 10);
            CHECK(buf2.get_align() == std::align_val_t{32});

            CHECK(buf2.get_data() == buf1_memory);
            CHECK(std::as_const(buf2).get_data() == buf1_memory);
        }
    }

    SUBCASE("operator=/1") {
        memory_buffer buf1{10, std::align_val_t{32}};
        const void* buf1_memory{buf1.get_data()};

        memory_buffer buf2{20, std::align_val_t{16}};
        buf2 = std::move(buf1);

        {
            CHECK(!buf1);
            CHECK_FALSE(buf1.is_valid());

            CHECK(buf1.get_size() == 0);
            CHECK(buf1.get_align() == std::align_val_t{});

            CHECK(buf1.get_data() == nullptr);
            CHECK(std::as_const(buf1).get_data() == nullptr);
        }

        {
            CHECK(buf2);
            CHECK(buf2.is_valid());

            CHECK(buf2.get_size() == 10);
            CHECK(buf2.get_align() == std::align_val_t{32});

            CHECK(buf2.get_data() == buf1_memory);
            CHECK(std::as_const(buf2).get_data() == buf1_memory);
        }
    }

    SUBCASE("reset") {
        memory_buffer buf{10, std::align_val_t{32}};
        buf.reset();

        CHECK(!buf);
        CHECK_FALSE(buf.is_valid());

        CHECK(buf.get_size() == 0);
        CHECK(buf.get_align() == std::align_val_t{});

        CHECK(buf.get_data() == nullptr);
        CHECK(std::as_const(buf).get_data() == nullptr);
    }

    SUBCASE("swap") {
        memory_buffer buf1{10, std::align_val_t{32}};
        memory_buffer buf2{15, std::align_val_t{16}};

        const void* buf1_memory{buf1.get_data()};
        const void* buf2_memory{buf2.get_data()};

        meta::detail::swap(buf1, buf2);

        CHECK(buf1.get_size() == 15);
        CHECK(buf1.get_align() == std::align_val_t{16});
        CHECK(buf1.get_data() == buf2_memory);

        CHECK(buf2.get_size() == 10);
        CHECK(buf2.get_align() == std::align_val_t{32});
        CHECK(buf2.get_data() == buf1_memory);
    }

    SUBCASE("operator<") {
        memory_buffer buf1{5, std::align_val_t{1}};
        memory_buffer buf2{5, std::align_val_t{1}};
        memory_buffer buf3{6, std::align_val_t{1}};
        memory_buffer buf4{6, std::align_val_t{1}};

        std::memcpy(buf1.get_data(), "hello", 5);
        std::memcpy(buf2.get_data(), "world", 5);
        std::memcpy(buf3.get_data(), "hello2", 6);
        std::memcpy(buf4.get_data(), "hello2", 6);

        CHECK_FALSE(buf1 < buf1);

        CHECK(buf1 < buf2);
        CHECK_FALSE(buf2 < buf1);

        CHECK(buf1 < buf3);
        CHECK_FALSE(buf3 < buf1);

        CHECK_FALSE(buf3 < buf4);
        CHECK_FALSE(buf4 < buf3);
    }

    SUBCASE("operator==") {
        memory_buffer buf1{5, std::align_val_t{1}};
        memory_buffer buf2{5, std::align_val_t{1}};
        memory_buffer buf3{6, std::align_val_t{1}};
        memory_buffer buf4{6, std::align_val_t{1}};

        std::memcpy(buf1.get_data(), "hello", 5);
        std::memcpy(buf2.get_data(), "world", 5);
        std::memcpy(buf3.get_data(), "hello2", 6);
        std::memcpy(buf4.get_data(), "hello2", 6);

        CHECK(buf1 == buf1);

        CHECK_FALSE(buf1 == buf2);
        CHECK_FALSE(buf1 == buf3);

        CHECK(buf3 == buf4);

        CHECK_FALSE(memory_buffer{} == buf1);
        CHECK(memory_buffer{} == memory_buffer{});
    }

    SUBCASE("operator!=") {
        memory_buffer buf1{5, std::align_val_t{1}};
        memory_buffer buf2{5, std::align_val_t{1}};
        memory_buffer buf3{6, std::align_val_t{1}};
        memory_buffer buf4{6, std::align_val_t{1}};

        std::memcpy(buf1.get_data(), "hello", 5);
        std::memcpy(buf2.get_data(), "world", 5);
        std::memcpy(buf3.get_data(), "hello2", 6);
        std::memcpy(buf4.get_data(), "hello2", 6);

        CHECK_FALSE(buf1 != buf1);

        CHECK(buf1 != buf2);
        CHECK(buf1 != buf3);

        CHECK_FALSE(buf3 != buf4);
    }

    SUBCASE("hash") {
        memory_buffer buf1{"hello", 5, std::align_val_t{1}};
        memory_buffer buf2{"hello", 5, std::align_val_t{1}};
        memory_buffer buf3{"world", 5, std::align_val_t{1}};
        memory_buffer buf4{"worlX", 5, std::align_val_t{1}};
        memory_buffer buf5{"worlX2", 6, std::align_val_t{1}};

        CHECK(std::hash<memory_buffer>{}(buf1) == std::hash<memory_buffer>{}(buf2));
        CHECK(std::hash<memory_buffer>{}(buf2) != std::hash<memory_buffer>{}(buf3));
        CHECK(std::hash<memory_buffer>{}(buf3) != std::hash<memory_buffer>{}(buf4));
        CHECK(std::hash<memory_buffer>{}(buf4) != std::hash<memory_buffer>{}(buf5));
    }
}
