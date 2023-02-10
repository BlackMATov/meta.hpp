/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>

#include <benchmark/benchmark.h>
#include <vmath.hpp/vmath_all.hpp>

namespace
{
    namespace meta = meta_hpp;
    namespace vmath = vmath_hpp;

    volatile float static_acc{};
    volatile float static_angle{};

    struct clazz {
        static void static_function0() {
            static_acc = static_acc + vmath::determinant(vmath::rotate(static_angle, vmath::unit3_x<float>));
            static_angle = static_angle + 0.00001f;
        }

        static void static_function1(float angle) {
            static_acc = static_acc + vmath::determinant(vmath::rotate(angle, vmath::unit3_x<float>));
            static_angle = static_angle + 0.00001f;
        }

        static void static_function2(float angle, const vmath::fvec3& axis) {
            static_acc = static_acc + vmath::determinant(vmath::rotate(angle, axis));
            static_angle = static_angle + 0.00001f;
        }

        static void static_function3(float angle, const vmath::fvec3& axis, float mul) {
            static_acc = static_acc + vmath::determinant(vmath::rotate(angle, axis)) * mul;
            static_angle = static_angle + 0.00001f;
        }
    };

    void invoke_function_reset(const benchmark::State&) {
        static_acc = 0.f;
        static_angle = 0.f;
    }

    void invoke_function_static0(benchmark::State &state) {
        for ( auto _ : state ) {
            clazz::static_function0();
        }
    }

    void invoke_function_static1(benchmark::State &state) {
        for ( auto _ : state ) {
            clazz::static_function1(static_angle);
        }
    }

    void invoke_function_static2(benchmark::State &state) {
        for ( auto _ : state ) {
            clazz::static_function2(static_angle, vmath::unit3_x<float>);
        }
    }

    void invoke_function_static3(benchmark::State &state) {
        for ( auto _ : state ) {
            clazz::static_function3(static_angle, vmath::unit3_x<float>, 2.f);
        }
    }

    void invoke_function_dynamic0(benchmark::State &state) {
        for ( auto _ : state ) {
            meta::invoke(&clazz::static_function0);
        }
    }

    void invoke_function_dynamic1(benchmark::State &state) {
        for ( auto _ : state ) {
            meta::invoke(&clazz::static_function1, static_angle);
        }
    }

    void invoke_function_dynamic2(benchmark::State &state) {
        for ( auto _ : state ) {
            meta::invoke(&clazz::static_function2, static_angle, vmath::unit3_x<float>);
        }
    }

    void invoke_function_dynamic3(benchmark::State &state) {
        for ( auto _ : state ) {
            meta::invoke(&clazz::static_function3, static_angle, vmath::unit3_x<float>, 2.f);
        }
    }
}

BENCHMARK(invoke_function_static0)->Teardown(invoke_function_reset);
BENCHMARK(invoke_function_dynamic0)->Teardown(invoke_function_reset);

BENCHMARK(invoke_function_static1)->Teardown(invoke_function_reset);
BENCHMARK(invoke_function_dynamic1)->Teardown(invoke_function_reset);

BENCHMARK(invoke_function_static2)->Teardown(invoke_function_reset);
BENCHMARK(invoke_function_dynamic2)->Teardown(invoke_function_reset);

BENCHMARK(invoke_function_static3)->Teardown(invoke_function_reset);
BENCHMARK(invoke_function_dynamic3)->Teardown(invoke_function_reset);
