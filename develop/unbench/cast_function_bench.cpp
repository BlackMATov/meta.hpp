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

    volatile unsigned static_acc{};

    struct base1 {
        unsigned b1{1};

        base1() = default;
        virtual ~base1() = default;

        base1(const base1&) = default;
        base1& operator=(const base1&) = default;

        RTTR_ENABLE()
    };

    struct base2 : base1 {
        unsigned b2{2};
        RTTR_ENABLE(base1)
    };

    struct base3 : base2 {
        unsigned b3{3};
        RTTR_ENABLE(base2)
    };

    struct base4 : base3 {
        unsigned b4{4};
        RTTR_ENABLE(base3)
    };

    unsigned static_function_1(base1* b1) {
        unsigned r = b1->b1;
        static_acc = static_acc + r;
        return r;
    }

    unsigned static_function_2(base1* b1, base2* b2) {
        unsigned r = b1->b1 + b2->b2;
        static_acc = static_acc + r;
        return r;
    }

    unsigned static_function_3(base1* b1, base2* b2, base3* b3) {
        unsigned r = b1->b1 + b2->b2 + b3->b3;
        static_acc = static_acc + r;
        return r;
    }

    unsigned static_function_4(base1* b1, base2* b2, base3* b3, base4* b4) {
        unsigned r = b1->b1 + b2->b2 + b3->b3 + b4->b4;
        static_acc = static_acc + r;
        return r;
    }

    [[maybe_unused]]
    void static_function_reset(const benchmark::State&) {
        static_acc = 0;
    }

    const bool registered = [](){
        meta::class_<base1>();
        meta::class_<base2>().base_<base1>();
        meta::class_<base3>().base_<base2>();
        meta::class_<base4>().base_<base3>();
        return true;
    }();

    meta::scope meta_bench_scope = meta::local_scope_("")
        .function_("cast_function_1", &static_function_1)
        .function_("cast_function_2", &static_function_2)
        .function_("cast_function_3", &static_function_3)
        .function_("cast_function_4", &static_function_4);
}

RTTR_REGISTRATION
{
    using namespace rttr;

    registration::method("cast_function_1", &static_function_1);
    registration::method("cast_function_2", &static_function_2);
    registration::method("cast_function_3", &static_function_3);
    registration::method("cast_function_4", &static_function_4);
}

//
// native
//

namespace
{
    [[maybe_unused]]
    void cast_function_1(benchmark::State &state) {
        base4 b4;
        for ( auto _ : state ) {
            unsigned r = static_function_1(&b4);
            META_HPP_ASSERT(r == 1);
            benchmark::DoNotOptimize(r);
        }
    }

    [[maybe_unused]]
    void cast_function_2(benchmark::State &state) {
        base4 b4;
        for ( auto _ : state ) {
            unsigned r = static_function_2(&b4, &b4);
            META_HPP_ASSERT(r == 3);
            benchmark::DoNotOptimize(r);
        }
    }

    [[maybe_unused]]
    void cast_function_3(benchmark::State &state) {
        base4 b4;
        for ( auto _ : state ) {
            unsigned r = static_function_3(&b4, &b4, &b4);
            META_HPP_ASSERT(r == 6);
            benchmark::DoNotOptimize(r);
        }
    }

    [[maybe_unused]]
    void cast_function_4(benchmark::State &state) {
        base4 b4;
        for ( auto _ : state ) {
            unsigned r = static_function_4(&b4, &b4, &b4, &b4);
            META_HPP_ASSERT(r == 10);
            benchmark::DoNotOptimize(r);
        }
    }
}

//
// meta
//

namespace
{
    [[maybe_unused]]
    void meta_cast_function_1(benchmark::State &state) {
        meta::function f = meta_bench_scope.get_function("cast_function_1");
        META_HPP_ASSERT(f.is_valid());

        base4 b4;
        for ( auto _ : state ) {
            unsigned r = f(&b4).as<unsigned>();
            META_HPP_ASSERT(r == 1);
            benchmark::DoNotOptimize(r);
        }
    }

    [[maybe_unused]]
    void meta_cast_function_2(benchmark::State &state) {
        meta::function f = meta_bench_scope.get_function("cast_function_2");
        META_HPP_ASSERT(f.is_valid());

        base4 b4;
        for ( auto _ : state ) {
            unsigned r = f(&b4, &b4).as<unsigned>();
            META_HPP_ASSERT(r == 3);
            benchmark::DoNotOptimize(r);
        }
    }

    [[maybe_unused]]
    void meta_cast_function_3(benchmark::State &state) {
        meta::function f = meta_bench_scope.get_function("cast_function_3");
        META_HPP_ASSERT(f.is_valid());

        base4 b4;
        for ( auto _ : state ) {
            unsigned r = f(&b4, &b4, &b4).as<unsigned>();
            META_HPP_ASSERT(r == 6);
            benchmark::DoNotOptimize(r);
        }
    }

    [[maybe_unused]]
    void meta_cast_function_4(benchmark::State &state) {
        meta::function f = meta_bench_scope.get_function("cast_function_4");
        META_HPP_ASSERT(f.is_valid());

        base4 b4;
        for ( auto _ : state ) {
            unsigned r = f(&b4, &b4, &b4, &b4).as<unsigned>();
            META_HPP_ASSERT(r == 10);
            benchmark::DoNotOptimize(r);
        }
    }
}

//
// rttr
//

namespace
{
    [[maybe_unused]]
    void rttr_cast_function_1(benchmark::State &state) {
        rttr::method m = rttr::type::get_global_method("cast_function_1");
        META_HPP_ASSERT(m.is_valid());

        base4 b4;
        for ( auto _ : state ) {
            unsigned r = m.invoke({},
                rttr::rttr_cast<base1*>(&b4)).get_value<unsigned>();
            META_HPP_ASSERT(r == 1);
            benchmark::DoNotOptimize(r);
        }
    }

    [[maybe_unused]]
    void rttr_cast_function_2(benchmark::State &state) {
        rttr::method m = rttr::type::get_global_method("cast_function_2");
        META_HPP_ASSERT(m.is_valid());

        base4 b4;
        for ( auto _ : state ) {
            unsigned r = m.invoke({},
                rttr::rttr_cast<base1*>(&b4),
                rttr::rttr_cast<base2*>(&b4)).get_value<unsigned>();
            META_HPP_ASSERT(r == 3);
            benchmark::DoNotOptimize(r);
        }
    }

    [[maybe_unused]]
    void rttr_cast_function_3(benchmark::State &state) {
        rttr::method m = rttr::type::get_global_method("cast_function_3");
        META_HPP_ASSERT(m.is_valid());

        base4 b4;
        for ( auto _ : state ) {
            unsigned r = m.invoke({},
                rttr::rttr_cast<base1*>(&b4),
                rttr::rttr_cast<base2*>(&b4),
                rttr::rttr_cast<base3*>(&b4)).get_value<unsigned>();
            META_HPP_ASSERT(r == 6);
            benchmark::DoNotOptimize(r);
        }
    }

    [[maybe_unused]]
    void rttr_cast_function_4(benchmark::State &state) {
        rttr::method m = rttr::type::get_global_method("cast_function_4");
        META_HPP_ASSERT(m.is_valid());

        base4 b4;
        for ( auto _ : state ) {
            unsigned r = m.invoke({},
                rttr::rttr_cast<base1*>(&b4),
                rttr::rttr_cast<base2*>(&b4),
                rttr::rttr_cast<base3*>(&b4),
                rttr::rttr_cast<base4*>(&b4)).get_value<unsigned>();
            META_HPP_ASSERT(r == 10);
            benchmark::DoNotOptimize(r);
        }
    }
}

BENCHMARK(cast_function_1)->Teardown(static_function_reset);
BENCHMARK(meta_cast_function_1)->Teardown(static_function_reset);
BENCHMARK(rttr_cast_function_1)->Teardown(static_function_reset);

BENCHMARK(cast_function_2)->Teardown(static_function_reset);
BENCHMARK(meta_cast_function_2)->Teardown(static_function_reset);
BENCHMARK(rttr_cast_function_2)->Teardown(static_function_reset);

BENCHMARK(cast_function_3)->Teardown(static_function_reset);
BENCHMARK(meta_cast_function_3)->Teardown(static_function_reset);
BENCHMARK(rttr_cast_function_3)->Teardown(static_function_reset);

BENCHMARK(cast_function_4)->Teardown(static_function_reset);
BENCHMARK(meta_cast_function_4)->Teardown(static_function_reset);
BENCHMARK(rttr_cast_function_4)->Teardown(static_function_reset);
