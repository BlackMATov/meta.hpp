/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <vmath.hpp/vmath_all.hpp>

#include <rttr/type>
#include <rttr/registration>

#include <benchmark/benchmark.h>

namespace
{
    namespace meta = meta_hpp;
    namespace vmath = vmath_hpp;

    float static_angle{};
    volatile float static_acc{};

    void static_function_0() {
        static_acc = static_acc + vmath::determinant(vmath::rotate(static_angle, vmath::unit3_x<float>));
        static_angle = static_angle + 0.00001f;
    }

    void static_function_1(float angle) {
        static_acc = static_acc + vmath::determinant(vmath::rotate(angle, vmath::unit3_x<float>));
        static_angle = static_angle + 0.00001f;
    }

    void static_function_2(float angle, const vmath::fvec3& axis) {
        static_acc = static_acc + vmath::determinant(vmath::rotate(angle, axis));
        static_angle = static_angle + 0.00001f;
    }

    void static_function_3(float angle, const vmath::fvec3& axis, float mul) {
        static_acc = static_acc + vmath::determinant(vmath::rotate(angle, axis)) * mul;
        static_angle = static_angle + 0.00001f;
    }

    void static_function_4(float angle, const vmath::fvec3& axis, float mul, const vmath::fmat3& mat) {
        static_acc = static_acc + vmath::determinant(vmath::rotate(angle, axis) * mat) * mul;
        static_angle = static_angle + 0.00001f;
    }

    void static_function_reset(const benchmark::State&) {
        static_acc = 0.f;
        static_angle = 0.f;
    }

    meta::scope meta_bench_scope = meta::local_scope_("")
        .function_("static_function_0", &static_function_0)
        .function_("static_function_1", &static_function_1)
        .function_("static_function_2", &static_function_2)
        .function_("static_function_3", &static_function_3)
        .function_("static_function_4", &static_function_4);
}

RTTR_REGISTRATION
{
    using namespace rttr;

    registration::method("static_function_0", &static_function_0);
    registration::method("static_function_1", &static_function_1);
    registration::method("static_function_2", &static_function_2);
    registration::method("static_function_3", &static_function_3);
    registration::method("static_function_4", &static_function_4);
}

//
// native
//

namespace
{
    [[maybe_unused]]
    void invoke_function_0(benchmark::State &state) {
        for ( auto _ : state ) {
            static_function_0();
        }
    }

    [[maybe_unused]]
    void invoke_function_1(benchmark::State &state) {
        for ( auto _ : state ) {
            static_function_1(static_angle);
        }
    }

    [[maybe_unused]]
    void invoke_function_2(benchmark::State &state) {
        for ( auto _ : state ) {
            static_function_2(static_angle, vmath::unit3_x<float>);
        }
    }

    [[maybe_unused]]
    void invoke_function_3(benchmark::State &state) {
        for ( auto _ : state ) {
            static_function_3(static_angle, vmath::unit3_x<float>, 2.f);
        }
    }

    [[maybe_unused]]
    void invoke_function_4(benchmark::State &state) {
        for ( auto _ : state ) {
            static_function_4(static_angle, vmath::unit3_x<float>, 2.f, vmath::midentity3<float>);
        }
    }
}

//
// meta
//

namespace
{
    [[maybe_unused]]
    void meta_invoke_function_0(benchmark::State &state) {
        meta::function f = meta_bench_scope.get_function("static_function_0");
        for ( auto _ : state ) {
            f();
        }
    }

    [[maybe_unused]]
    void meta_invoke_function_1(benchmark::State &state) {
        meta::function f = meta_bench_scope.get_function("static_function_1");
        META_HPP_ASSERT(f.is_valid());

        for ( auto _ : state ) {
            f(static_angle);
        }
    }

    [[maybe_unused]]
    void meta_invoke_function_2(benchmark::State &state) {
        meta::function f = meta_bench_scope.get_function("static_function_2");
        META_HPP_ASSERT(f.is_valid());

        for ( auto _ : state ) {
            f(static_angle, vmath::unit3_x<float>);
        }
    }

    [[maybe_unused]]
    void meta_invoke_function_3(benchmark::State &state) {
        meta::function f = meta_bench_scope.get_function("static_function_3");
        META_HPP_ASSERT(f.is_valid());

        for ( auto _ : state ) {
            f(static_angle, vmath::unit3_x<float>, 2.f);
        }
    }

    [[maybe_unused]]
    void meta_invoke_function_4(benchmark::State &state) {
        meta::function f = meta_bench_scope.get_function("static_function_4");
        META_HPP_ASSERT(f.is_valid());

        for ( auto _ : state ) {
            f(static_angle, vmath::unit3_x<float>, 2.f, vmath::midentity3<float>);
        }
    }
}

//
// rttr
//

namespace
{
    [[maybe_unused]]
    void rttr_invoke_function_0(benchmark::State &state) {
        rttr::method m = rttr::type::get_global_method("static_function_0");
        META_HPP_ASSERT(m.is_valid());

        for ( auto _ : state ) {
            m.invoke({});
        }
    }

    [[maybe_unused]]
    void rttr_invoke_function_1(benchmark::State &state) {
        rttr::method m = rttr::type::get_global_method("static_function_1");
        META_HPP_ASSERT(m.is_valid());

        for ( auto _ : state ) {
            m.invoke({}, static_angle);
        }
    }

    [[maybe_unused]]
    void rttr_invoke_function_2(benchmark::State &state) {
        rttr::method m = rttr::type::get_global_method("static_function_2");
        META_HPP_ASSERT(m.is_valid());

        for ( auto _ : state ) {
            m.invoke({}, static_angle, vmath::unit3_x<float>);
        }
    }

    [[maybe_unused]]
    void rttr_invoke_function_3(benchmark::State &state) {
        rttr::method m = rttr::type::get_global_method("static_function_3");
        META_HPP_ASSERT(m.is_valid());

        for ( auto _ : state ) {
            m.invoke({}, static_angle, vmath::unit3_x<float>, 2.f);
        }
    }

    [[maybe_unused]]
    void rttr_invoke_function_4(benchmark::State &state) {
        rttr::method m = rttr::type::get_global_method("static_function_4");
        META_HPP_ASSERT(m.is_valid());

        for ( auto _ : state ) {
            m.invoke({}, static_angle, vmath::unit3_x<float>, 2.f, vmath::midentity3<float>);
        }
    }
}

BENCHMARK(invoke_function_0)->Teardown(static_function_reset);
BENCHMARK(meta_invoke_function_0)->Teardown(static_function_reset);
BENCHMARK(rttr_invoke_function_0)->Teardown(static_function_reset);

BENCHMARK(invoke_function_1)->Teardown(static_function_reset);
BENCHMARK(meta_invoke_function_1)->Teardown(static_function_reset);
BENCHMARK(rttr_invoke_function_1)->Teardown(static_function_reset);

BENCHMARK(invoke_function_2)->Teardown(static_function_reset);
BENCHMARK(meta_invoke_function_2)->Teardown(static_function_reset);
BENCHMARK(rttr_invoke_function_2)->Teardown(static_function_reset);

BENCHMARK(invoke_function_3)->Teardown(static_function_reset);
BENCHMARK(meta_invoke_function_3)->Teardown(static_function_reset);
BENCHMARK(rttr_invoke_function_3)->Teardown(static_function_reset);

BENCHMARK(invoke_function_4)->Teardown(static_function_reset);
BENCHMARK(meta_invoke_function_4)->Teardown(static_function_reset);
BENCHMARK(rttr_invoke_function_4)->Teardown(static_function_reset);
