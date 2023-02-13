/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>

#include <benchmark/benchmark.h>
#include <rttr/registration>
#include <vmath.hpp/vmath_all.hpp>

namespace
{
    namespace meta = meta_hpp;
    namespace vmath = vmath_hpp;

    float static_angle{};
    volatile float static_acc{};

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

    // native

    void invoke_function_0(benchmark::State &state) {
        for ( auto _ : state ) {
            clazz::static_function0();
        }
    }

    void invoke_function_1(benchmark::State &state) {
        for ( auto _ : state ) {
            clazz::static_function1(static_angle);
        }
    }

    void invoke_function_2(benchmark::State &state) {
        for ( auto _ : state ) {
            clazz::static_function2(static_angle, vmath::unit3_x<float>);
        }
    }

    void invoke_function_3(benchmark::State &state) {
        for ( auto _ : state ) {
            clazz::static_function3(static_angle, vmath::unit3_x<float>, 2.f);
        }
    }

    // meta

    void meta_invoke_function_0(benchmark::State &state) {
        for ( auto _ : state ) {
            meta::invoke(&clazz::static_function0);
        }
    }

    void meta_invoke_function_1(benchmark::State &state) {
        for ( auto _ : state ) {
            meta::invoke(&clazz::static_function1, static_angle);
        }
    }

    void meta_invoke_function_2(benchmark::State &state) {
        for ( auto _ : state ) {
            meta::invoke(&clazz::static_function2, static_angle, vmath::unit3_x<float>);
        }
    }

    void meta_invoke_function_3(benchmark::State &state) {
        for ( auto _ : state ) {
            meta::invoke(&clazz::static_function3, static_angle, vmath::unit3_x<float>, 2.f);
        }
    }

    // rttr

    void rttr_invoke_function_0(benchmark::State &state) {
        rttr::method m = rttr::type::get_global_method("rttr_invoke_function_0");
        META_HPP_ASSERT(m.is_valid());

        for ( auto _ : state ) {
            m.invoke({});
        }
    }

    void rttr_invoke_function_1(benchmark::State &state) {
        rttr::method m = rttr::type::get_global_method("rttr_invoke_function_1");
        META_HPP_ASSERT(m.is_valid());

        for ( auto _ : state ) {
            m.invoke({}, static_angle);
        }
    }

    void rttr_invoke_function_2(benchmark::State &state) {
        rttr::method m = rttr::type::get_global_method("rttr_invoke_function_2");
        META_HPP_ASSERT(m.is_valid());

        for ( auto _ : state ) {
            m.invoke({}, static_angle, vmath::unit3_x<float>);
        }
    }

    void rttr_invoke_function_3(benchmark::State &state) {
        rttr::method m = rttr::type::get_global_method("rttr_invoke_function_3");
        META_HPP_ASSERT(m.is_valid());

        for ( auto _ : state ) {
            m.invoke({}, static_angle, vmath::unit3_x<float>, 2.f);
        }
    }
}

RTTR_REGISTRATION
{
    using namespace rttr;
    registration::method("rttr_invoke_function_0", &clazz::static_function0);
    registration::method("rttr_invoke_function_1", &clazz::static_function1);
    registration::method("rttr_invoke_function_2", &clazz::static_function2);
    registration::method("rttr_invoke_function_3", &clazz::static_function3);
}

BENCHMARK(invoke_function_0)->Teardown(invoke_function_reset);
BENCHMARK(meta_invoke_function_0)->Teardown(invoke_function_reset);
BENCHMARK(rttr_invoke_function_0)->Teardown(invoke_function_reset);

BENCHMARK(invoke_function_1)->Teardown(invoke_function_reset);
BENCHMARK(meta_invoke_function_1)->Teardown(invoke_function_reset);
BENCHMARK(rttr_invoke_function_1)->Teardown(invoke_function_reset);

BENCHMARK(invoke_function_2)->Teardown(invoke_function_reset);
BENCHMARK(meta_invoke_function_2)->Teardown(invoke_function_reset);
BENCHMARK(rttr_invoke_function_2)->Teardown(invoke_function_reset);

BENCHMARK(invoke_function_3)->Teardown(invoke_function_reset);
BENCHMARK(meta_invoke_function_3)->Teardown(invoke_function_reset);
BENCHMARK(rttr_invoke_function_3)->Teardown(invoke_function_reset);
