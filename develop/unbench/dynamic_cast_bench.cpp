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

    struct base1 {
        unsigned b1{1};

        base1() = default;
        virtual ~base1() = default;

        base1(const base1&) = default;
        base1& operator=(const base1&) = default;

        RTTR_ENABLE()
        META_HPP_ENABLE_POLYMORPHIC_CAST()
    };

    struct base2 : base1 {
        unsigned b2{2};
        RTTR_ENABLE(base1)
        META_HPP_ENABLE_POLYMORPHIC_CAST()
    };

    struct base3 : base2 {
        unsigned b3{3};
        RTTR_ENABLE(base2)
        META_HPP_ENABLE_POLYMORPHIC_CAST()
    };

    struct base4 : base3 {
        unsigned b4{4};
        RTTR_ENABLE(base3)
        META_HPP_ENABLE_POLYMORPHIC_CAST()
    };

    base4* dynamic_cast_1(base1* b1) {
        return dynamic_cast<base4*>(b1);
    }

    base4* dynamic_cast_2(base2* b2) {
        return dynamic_cast<base4*>(b2);
    }

    base4* dynamic_cast_3(base3* b3) {
        return dynamic_cast<base4*>(b3);
    }

    base4* dynamic_cast_4(base4* b4) {
        return dynamic_cast<base4*>(b4);
    }

    const bool registered = [](){
        meta::class_<base1>();
        meta::class_<base2>().base_<base1>();
        meta::class_<base3>().base_<base2>();
        meta::class_<base4>().base_<base3>();
        return true;
    }();
}

//
// native
//

namespace
{
    [[maybe_unused]]
    void dynamic_cast_1(benchmark::State &state) {
        base4 b4;
        for ( auto _ : state ) {
            base4* r = dynamic_cast_1(&b4);
            benchmark::DoNotOptimize(r);
        }
    }

    [[maybe_unused]]
    void dynamic_cast_2(benchmark::State &state) {
        base4 b4;
        for ( auto _ : state ) {
            base4* r = dynamic_cast_2(&b4);
            benchmark::DoNotOptimize(r);
        }
    }

    [[maybe_unused]]
    void dynamic_cast_3(benchmark::State &state) {
        base4 b4;
        for ( auto _ : state ) {
            base4* r = dynamic_cast_3(&b4);
            benchmark::DoNotOptimize(r);
        }
    }

    [[maybe_unused]]
    void dynamic_cast_4(benchmark::State &state) {
        base4 b4;
        for ( auto _ : state ) {
            base4* r = dynamic_cast_4(&b4);
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
    void meta_dynamic_cast_1(benchmark::State &state) {
        base4 b4;
        for ( auto _ : state ) {
            base4* r = meta::ucast<base4*>(static_cast<base1*>(&b4));
            benchmark::DoNotOptimize(r);
        }
    }

    [[maybe_unused]]
    void meta_dynamic_cast_2(benchmark::State &state) {
        base4 b4;
        for ( auto _ : state ) {
            base4* r = meta::ucast<base4*>(static_cast<base2*>(&b4));
            benchmark::DoNotOptimize(r);
        }
    }

    [[maybe_unused]]
    void meta_dynamic_cast_3(benchmark::State &state) {
        base4 b4;
        for ( auto _ : state ) {
            base4* r = meta::ucast<base4*>(static_cast<base3*>(&b4));
            benchmark::DoNotOptimize(r);
        }
    }

    [[maybe_unused]]
    void meta_dynamic_cast_4(benchmark::State &state) {
        base4 b4;
        for ( auto _ : state ) {
            base4* r = meta::ucast<base4*>(static_cast<base4*>(&b4));
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
    void rttr_dynamic_cast_1(benchmark::State &state) {
        base4 b4;
        for ( auto _ : state ) {
            base4* r = rttr::rttr_cast<base4*>(static_cast<base1*>(&b4));
            benchmark::DoNotOptimize(r);
        }
    }

    [[maybe_unused]]
    void rttr_dynamic_cast_2(benchmark::State &state) {
        base4 b4;
        for ( auto _ : state ) {
            base4* r = rttr::rttr_cast<base4*>(static_cast<base2*>(&b4));
            benchmark::DoNotOptimize(r);
        }
    }

    [[maybe_unused]]
    void rttr_dynamic_cast_3(benchmark::State &state) {
        base4 b4;
        for ( auto _ : state ) {
            base4* r = rttr::rttr_cast<base4*>(static_cast<base3*>(&b4));
            benchmark::DoNotOptimize(r);
        }
    }

    [[maybe_unused]]
    void rttr_dynamic_cast_4(benchmark::State &state) {
        base4 b4;
        for ( auto _ : state ) {
            base4* r = rttr::rttr_cast<base4*>(static_cast<base4*>(&b4));
            benchmark::DoNotOptimize(r);
        }
    }
}

BENCHMARK(dynamic_cast_1);
BENCHMARK(meta_dynamic_cast_1);
BENCHMARK(rttr_dynamic_cast_1);

BENCHMARK(dynamic_cast_2);
BENCHMARK(meta_dynamic_cast_2);
BENCHMARK(rttr_dynamic_cast_2);

BENCHMARK(dynamic_cast_3);
BENCHMARK(meta_dynamic_cast_3);
BENCHMARK(rttr_dynamic_cast_3);

BENCHMARK(dynamic_cast_4);
BENCHMARK(meta_dynamic_cast_4);
BENCHMARK(rttr_dynamic_cast_4);
