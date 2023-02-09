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

    struct clazz {
        static float static_function() {
            return vmath::determinant(vmath::inverse(vmath::fmat4{}));
        }
    };

    const meta::function dynamic_function{[](){
        meta::class_<clazz>().function_("static_function", &clazz::static_function);
        return meta::resolve_type<clazz>().get_function("static_function");
    }()};

    void invoke_without_args_native(benchmark::State &state) {
        for ( auto _ : state ) {
            const auto r{clazz::static_function()};
            benchmark::DoNotOptimize(r);
            benchmark::ClobberMemory();
        }
    }

    void invoke_without_args_dynamic1(benchmark::State &state) {
        for ( auto _ : state ) {
            const auto r{meta::invoke(&clazz::static_function)};
            benchmark::DoNotOptimize(r);
            benchmark::ClobberMemory();
        }
    }

    void invoke_without_args_dynamic2(benchmark::State &state) {
        for ( auto _ : state ) {
            const auto r{meta::invoke(dynamic_function)};
            benchmark::DoNotOptimize(r);
            benchmark::ClobberMemory();
        }
    }
}

BENCHMARK(invoke_without_args_native);
BENCHMARK(invoke_without_args_dynamic1);
BENCHMARK(invoke_without_args_dynamic2);
