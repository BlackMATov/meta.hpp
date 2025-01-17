/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2025, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <vmath.hpp/vmath_all.hpp>

#include <benchmark/benchmark.h>

namespace
{
    namespace meta = meta_hpp;
    namespace vmath = vmath_hpp;

    template < typename To, typename From >
    To* dyn_cast(From* from) {
        return dynamic_cast<To*>(from);
    }
}

namespace
{
    struct A1 {
        META_HPP_ENABLE_POLY_INFO()
    };

    struct B1 : A1 {
        META_HPP_ENABLE_POLY_INFO(A1)
    };

    struct C1 : B1 {
        META_HPP_ENABLE_POLY_INFO(B1)
    };

    // A1 < B1 < C1

    [[maybe_unused]]
    void dynamic_cast_1a(benchmark::State &state) {
        C1 c;
        for ( auto _ : state ) {
            A1* a = &c;
            C1* cc = dyn_cast<C1>(a);
            benchmark::DoNotOptimize(cc);
        }
    }

    [[maybe_unused]]
    void meta_dynamic_cast_1a(benchmark::State &state) {
        C1 c;
        for ( auto _ : state ) {
            A1* a = &c;
            C1* cc = meta::ucast<C1*>(a);
            benchmark::DoNotOptimize(cc);
        }
    }

    [[maybe_unused]]
    void dynamic_cast_1b(benchmark::State &state) {
        C1 c;
        for ( auto _ : state ) {
            A1* a = &c;
            B1* b = dyn_cast<B1>(a);
            benchmark::DoNotOptimize(b);
        }
    }

    [[maybe_unused]]
    void meta_dynamic_cast_1b(benchmark::State &state) {
        C1 c;
        for ( auto _ : state ) {
            A1* a = &c;
            B1* b = meta::ucast<B1*>(a);
            benchmark::DoNotOptimize(b);
        }
    }
}

BENCHMARK(dynamic_cast_1a);
BENCHMARK(meta_dynamic_cast_1a);
BENCHMARK(dynamic_cast_1b);
BENCHMARK(meta_dynamic_cast_1b);

namespace
{
    struct A2 {
        META_HPP_ENABLE_POLY_INFO()
    };

    struct B2 {
        META_HPP_ENABLE_POLY_INFO()
    };

    struct C2 : A2, B2 {
        META_HPP_ENABLE_POLY_INFO(A2, B2)
    };

    struct D2 : C2 {
        META_HPP_ENABLE_POLY_INFO(C2)
    };

    // A2
    //   < C2 < D2
    // B2

    [[maybe_unused]]
    void dynamic_cast_2a(benchmark::State &state) {
        D2 d;
        for ( auto _ : state ) {
            A2* a = &d;
            D2* dd = dyn_cast<D2>(a);
            benchmark::DoNotOptimize(dd);
        }
    }

    [[maybe_unused]]
    void meta_dynamic_cast_2a(benchmark::State &state) {
        D2 d;
        for ( auto _ : state ) {
            A2* a = &d;
            D2* dd = meta::ucast<D2*>(a);
            benchmark::DoNotOptimize(dd);
        }
    }

    [[maybe_unused]]
    void dynamic_cast_2b(benchmark::State &state) {
        D2 d;
        for ( auto _ : state ) {
            B2* b = &d;
            C2* c = dyn_cast<C2>(b);
            benchmark::DoNotOptimize(c);
        }
    }

    [[maybe_unused]]
    void meta_dynamic_cast_2b(benchmark::State &state) {
        D2 d;
        for ( auto _ : state ) {
            B2* b = &d;
            C2* c = meta::ucast<C2*>(b);
            benchmark::DoNotOptimize(c);
        }
    }
}

BENCHMARK(dynamic_cast_2a);
BENCHMARK(meta_dynamic_cast_2a);
BENCHMARK(dynamic_cast_2b);
BENCHMARK(meta_dynamic_cast_2b);

namespace
{
    struct A3 {
        META_HPP_ENABLE_POLY_INFO()
    };

    struct B3 : virtual A3 {
        META_HPP_ENABLE_POLY_INFO(A3)
    };

    struct C3 : virtual A3 {
        META_HPP_ENABLE_POLY_INFO(A3)
    };

    struct D3 : B3, C3 {
        META_HPP_ENABLE_POLY_INFO(B3, C3)
    };

    // A3 <= B3
    //         < D3
    // A3 <= C3

    [[maybe_unused]]
    void dynamic_cast_3a(benchmark::State &state) {
        D3 d;
        for ( auto _ : state ) {
            A3* a = &d;
            D3* dd = dyn_cast<D3>(a);
            benchmark::DoNotOptimize(dd);
        }
    }

    [[maybe_unused]]
    void meta_dynamic_cast_3a(benchmark::State &state) {
        D3 d;
        for ( auto _ : state ) {
            A3* a = &d;
            D3* dd = meta::ucast<D3*>(a);
            benchmark::DoNotOptimize(dd);
        }
    }

    [[maybe_unused]]
    void dynamic_cast_3b(benchmark::State &state) {
        D3 d;
        for ( auto _ : state ) {
            B3* b = &d;
            C3* c = dyn_cast<C3>(b);
            benchmark::DoNotOptimize(c);
        }
    }

    [[maybe_unused]]
    void meta_dynamic_cast_3b(benchmark::State &state) {
        D3 d;
        for ( auto _ : state ) {
            B3* b = &d;
            C3* c = meta::ucast<C3*>(b);
            benchmark::DoNotOptimize(c);
        }
    }

    [[maybe_unused]]
    void dynamic_cast_3c(benchmark::State &state) {
        B3 b;
        for ( auto _ : state ) {
            A3* a = &b;
            C3* c = dyn_cast<C3>(a);
            benchmark::DoNotOptimize(c);
        }
    }

    [[maybe_unused]]
    void meta_dynamic_cast_3c(benchmark::State &state) {
        B3 b;
        for ( auto _ : state ) {
            A3* a = &b;
            C3* c = meta::ucast<C3*>(a);
            benchmark::DoNotOptimize(c);
        }
    }
}

BENCHMARK(dynamic_cast_3a);
BENCHMARK(meta_dynamic_cast_3a);
BENCHMARK(dynamic_cast_3b);
BENCHMARK(meta_dynamic_cast_3b);
BENCHMARK(dynamic_cast_3c);
BENCHMARK(meta_dynamic_cast_3c);
