/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "meta_shared_lib.hpp"

namespace
{
    using namespace meta_shared_lib;

    struct ivec2 {
        int x;
        int y;

        ivec2() = default;
        explicit ivec2(int nv) : x{nv}, y{nv} {}
        ivec2(int nx, int ny) : x{nx}, y{ny} {}

        int length2() const noexcept {
            return x * x + y * y;
        }
    };

    struct another_ivec2 {
        int x;
        int y;
    };

    enum class color {
        red, green, blue,
    };

    void bind_ivec2() {
        meta::class_<ivec2>()
            .constructor_<>()
            .constructor_<int>()
            .constructor_<int, int>()
            .constructor_<const ivec2&>()
            .member_("x", &ivec2::x)
            .member_("y", &ivec2::y)
            .method_("length2", &ivec2::length2);

        meta::class_<another_ivec2>();
    }

    void bind_color() {
        meta::enum_<color>()
            .evalue_("red", color::red)
            .evalue_("green", color::green)
            .evalue_("blue", color::blue);
    }

    const auto library_scope = []() -> meta::scope {
        bind_ivec2();
        bind_color();

        return meta::local_scope_("library_scope")
            .typedef_<int>("int")

            .typedef_<int*>("int*")
            .typedef_<int const*>("int const*")
            .typedef_<int volatile*>("int volatile*")
            .typedef_<int const volatile*>("int const volatile*")

            .typedef_<int&>("int&")
            .typedef_<int const&>("int const&")
            .typedef_<int volatile&>("int volatile&")
            .typedef_<int const volatile&>("int const volatile&")

            .typedef_<int**>("int**")
            .typedef_<int* const*>("int* const*")
            .typedef_<int const**>("int const**")

            .typedef_<void**>("void**")
            .typedef_<void* const*>("void* const*")
            .typedef_<void const**>("void const**")

            .typedef_<void()>("void()")
            .typedef_<int*()>("int*()")
            .typedef_<color(int*)>("color(int*)")
            .typedef_<color(float*)>("color(float*)")
            .typedef_<color(float*) noexcept>("color(float*) noexcept")

            .typedef_<void>("void")
            .typedef_<void*>("void*")
            .typedef_<void const*>("void const*")
            .typedef_<decltype(nullptr)>("nullptr_t")
            .typedef_<decltype(nullptr)*>("nullptr_t*")
            .typedef_<decltype(nullptr) const*>("nullptr_t const*")

            .typedef_<ivec2>("ivec2")
            .typedef_<another_ivec2>("another_ivec2")

            .typedef_<ivec2 const&>("ivec2 const&")
            .typedef_<ivec2 const&&>("ivec2 const&&")
            .typedef_<ivec2[]>("ivec2[]")
            .typedef_<ivec2(&)[]>("ivec2(&)[]")
            .typedef_<ivec2 const(&)[]>("ivec2 const(&)[]")
            .typedef_<ivec2 const*[42]>("ivec2 const*[42]")
            .typedef_<ivec2 const*[21]>("ivec2 const*[21]")

            .typedef_<color>("color")
            .typedef_<color const*>("color const*")

            .typedef_<int ivec2::*>("int ivec2::*")
            .typedef_<int const ivec2::*>("int const ivec2::*")
            .typedef_<int volatile ivec2::*>("int volatile ivec2::*")
            .typedef_<int const volatile ivec2::*>("int const volatile ivec2::*")
            .typedef_<int ivec2::*const*>("int ivec2::*const*")

            .typedef_<int (ivec2::*)()>("int (ivec2::*)()")
            .typedef_<float (ivec2::*)()>("float (ivec2::*)()")
            .typedef_<int (ivec2::*)(float)>("int (ivec2::*)(float)")
            .typedef_<int (ivec2::*)(double)>("int (ivec2::*)(double)")
            .typedef_<int (ivec2::*)(double) noexcept>("int (ivec2::*)(double) noexcept")
            .typedef_<int (ivec2::*const*)()>("int (ivec2::*const*)()")
            ;
    }();
}

META_HPP_DEFINE_SHARED_TYPE(ivec2, "ivec2")
META_HPP_DEFINE_SHARED_TYPE(another_ivec2, "another_ivec2")
META_HPP_DEFINE_SHARED_TYPE(color, "color")

namespace meta_shared_lib
{
    meta::scope get_library_scope() {
        return library_scope;
    }
}
