/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta_shared_lib.hpp>

#include <doctest/doctest.h>

namespace
{
    namespace meta = meta_hpp;
    namespace shared = meta_shared_lib;

    struct ivec2 {
        int x{}, y{};

        explicit ivec2(int nv) : x{nv}, y{nv} {}
        ivec2(int nx, int ny) : x{nx}, y{ny} {}
    };

    struct another_ivec2 {
        int x;
        int y;
    };

    struct unshared_ivec2 {
        int x{}, y{};
    };

    enum class color {};
    enum class unshared_color {};
}

META_HPP_DEFINE_SHARED_TYPE(ivec2, "ivec2")
META_HPP_DEFINE_SHARED_TYPE(another_ivec2, "another_ivec2")
META_HPP_DEFINE_SHARED_TYPE(color, "color")

TEST_CASE("meta/meta_shared/tests") {
    const meta::scope library_scope = shared::get_library_scope();
    REQUIRE(library_scope);

    SUBCASE("0") {
        static_assert(meta::detail::shared_type_hash<meta::type_kind::array_, ivec2[42]>{}());
        static_assert(meta::detail::shared_type_hash<meta::type_kind::class_, ivec2>{}());
        static_assert(meta::detail::shared_type_hash<meta::type_kind::constructor_, ivec2, int>{}());
        static_assert(meta::detail::shared_type_hash<meta::type_kind::destructor_, ivec2>{}());
        static_assert(meta::detail::shared_type_hash<meta::type_kind::enum_, color>{}());
        static_assert(meta::detail::shared_type_hash<meta::type_kind::function_, color(ivec2)>{}());
        static_assert(meta::detail::shared_type_hash<meta::type_kind::member_, color ivec2::*>{}());
        static_assert(meta::detail::shared_type_hash<meta::type_kind::method_, color (ivec2::*)(int const*)>{}());
        static_assert(meta::detail::shared_type_hash<meta::type_kind::nullptr_, std::nullptr_t>{}());
        static_assert(meta::detail::shared_type_hash<meta::type_kind::number_, int>{}());
        static_assert(meta::detail::shared_type_hash<meta::type_kind::pointer_, int*>{}());
        static_assert(meta::detail::shared_type_hash<meta::type_kind::reference_, int&>{}());
    }

    SUBCASE("1") {
        const meta::class_type ivec2_type = library_scope.get_typedef("ivec2").as_class();
        const meta::class_type another_ivec2_type = library_scope.get_typedef("another_ivec2").as_class();

        REQUIRE(ivec2_type);
        REQUIRE(another_ivec2_type);

        CHECK(ivec2_type == meta::resolve_type<ivec2>());
        CHECK(another_ivec2_type == meta::resolve_type<another_ivec2>());

        const meta::constructor ivec2_ctor0 = ivec2_type.get_constructor_with<const ivec2&>();
        const meta::constructor ivec2_ctor1 = ivec2_type.get_constructor_with<int>();
        const meta::constructor ivec2_ctor2 = ivec2_type.get_constructor_with<int, int>();
        const meta::destructor ivec2_dtor = ivec2_type.get_destructor();
        const meta::destructor another_ivec2_dtor = another_ivec2_type.get_destructor();

        REQUIRE(ivec2_ctor0);
        REQUIRE(ivec2_ctor1);
        REQUIRE(ivec2_ctor2);
        REQUIRE(ivec2_dtor);
        REQUIRE(another_ivec2_dtor);

        CHECK(ivec2_ctor0.get_type() == meta::resolve_constructor_type<ivec2, const ivec2&>());
        CHECK(ivec2_ctor1.get_type() == meta::resolve_constructor_type<ivec2, int>());
        CHECK(ivec2_ctor2.get_type() == meta::resolve_constructor_type<ivec2, int, int>());
        CHECK(ivec2_dtor.get_type() == meta::resolve_destructor_type<ivec2>());
        CHECK(another_ivec2_dtor.get_type() == meta::resolve_destructor_type<another_ivec2>());

        CHECK(ivec2_ctor0 != ivec2_ctor1);
        CHECK(ivec2_ctor1 != ivec2_ctor2);

        CHECK(ivec2_dtor.get_type() != another_ivec2_dtor.get_type());
    }

    SUBCASE("2") {
        const meta::any_type color_type = library_scope.get_typedef("color");
        const meta::any_type color_const_ptr_type = library_scope.get_typedef("color const*");

        REQUIRE(color_type);
        REQUIRE(color_const_ptr_type);

        CHECK(color_type == meta::resolve_type<color>());
        CHECK(color_const_ptr_type == meta::resolve_type<color const*>());
    }

    SUBCASE("3") {
        const meta::any_type int_ptr_ptr_type = library_scope.get_typedef("int**");
        const meta::any_type int_ptr_const_ptr_type = library_scope.get_typedef("int* const*");
        const meta::any_type int_const_ptr_ptr_type = library_scope.get_typedef("int const**");

        REQUIRE(int_ptr_ptr_type);
        REQUIRE(int_ptr_const_ptr_type);
        REQUIRE(int_const_ptr_ptr_type);

        CHECK(int_ptr_ptr_type == meta::resolve_type<int**>());
        CHECK(int_ptr_const_ptr_type == meta::resolve_type<int* const*>());
        CHECK(int_const_ptr_ptr_type == meta::resolve_type<int const**>());

        CHECK(int_ptr_ptr_type != int_ptr_const_ptr_type);
        CHECK(int_ptr_const_ptr_type != int_const_ptr_ptr_type);
    }

    SUBCASE("4") {
        const meta::any_type void_ptr_ptr_type = library_scope.get_typedef("void**");
        const meta::any_type void_ptr_const_ptr_type = library_scope.get_typedef("void* const*");
        const meta::any_type void_const_ptr_ptr_type = library_scope.get_typedef("void const**");

        REQUIRE(void_ptr_ptr_type);
        REQUIRE(void_ptr_const_ptr_type);
        REQUIRE(void_const_ptr_ptr_type);

        CHECK(void_ptr_ptr_type == meta::resolve_type<void**>());
        CHECK(void_ptr_const_ptr_type == meta::resolve_type<void* const*>());
        CHECK(void_const_ptr_ptr_type == meta::resolve_type<void const**>());

        CHECK(void_ptr_ptr_type != void_ptr_const_ptr_type);
        CHECK(void_ptr_const_ptr_type != void_const_ptr_ptr_type);
    }

    SUBCASE("5") {
        const meta::any_type void_func_type = library_scope.get_typedef("void()");
        const meta::any_type int_ptr_func_type = library_scope.get_typedef("int*()");
        const meta::any_type color_int_ptr_func_type = library_scope.get_typedef("color(int*)");
        const meta::any_type color_float_ptr_func_type = library_scope.get_typedef("color(float*)");
        const meta::any_type color_float_ptr_noexcept_func_type = library_scope.get_typedef("color(float*) noexcept");

        REQUIRE(void_func_type);
        REQUIRE(int_ptr_func_type);
        REQUIRE(color_int_ptr_func_type);
        REQUIRE(color_float_ptr_func_type);
        REQUIRE(color_float_ptr_noexcept_func_type);

        CHECK(void_func_type == meta::resolve_type<void()>());
        CHECK(int_ptr_func_type == meta::resolve_type<int*()>());
        CHECK(color_int_ptr_func_type == meta::resolve_type<color(int*)>());
        CHECK(color_float_ptr_func_type == meta::resolve_type<color(float*)>());
        CHECK(color_float_ptr_noexcept_func_type == meta::resolve_type<color(float*) noexcept>());

        CHECK(void_func_type != int_ptr_func_type);
        CHECK(int_ptr_func_type != color_int_ptr_func_type);
        CHECK(color_int_ptr_func_type != color_float_ptr_func_type);
        CHECK(color_float_ptr_func_type != color_float_ptr_noexcept_func_type);
    }

    SUBCASE("6") {
        const meta::any_type void_type = library_scope.get_typedef("void");
        const meta::any_type void_ptr_type = library_scope.get_typedef("void*");
        const meta::any_type void_const_ptr_type = library_scope.get_typedef("void const*");
        const meta::any_type nullptr_type = library_scope.get_typedef("nullptr_t");
        const meta::any_type nullptr_ptr_type = library_scope.get_typedef("nullptr_t*");
        const meta::any_type nullptr_const_ptr_type = library_scope.get_typedef("nullptr_t const*");

        REQUIRE(void_type);
        REQUIRE(void_ptr_type);
        REQUIRE(void_const_ptr_type);
        REQUIRE(nullptr_type);
        REQUIRE(nullptr_ptr_type);
        REQUIRE(nullptr_const_ptr_type);

        CHECK(void_type == meta::resolve_type<void>());
        CHECK(void_ptr_type == meta::resolve_type<void*>());
        CHECK(void_const_ptr_type == meta::resolve_type<void const*>());
        CHECK(nullptr_type == meta::resolve_type<std::nullptr_t>());
        CHECK(nullptr_ptr_type == meta::resolve_type<std::nullptr_t*>());
        CHECK(nullptr_const_ptr_type == meta::resolve_type<std::nullptr_t const*>());

        CHECK(void_type != void_ptr_type);
        CHECK(void_ptr_type != void_const_ptr_type);
        CHECK(void_const_ptr_type != nullptr_type);
        CHECK(nullptr_type != nullptr_ptr_type);
        CHECK(nullptr_ptr_type != nullptr_const_ptr_type);
    }

    SUBCASE("7") {
        const meta::any_type int_type = library_scope.get_typedef("int");
        const meta::any_type int_ptr_type = library_scope.get_typedef("int*");
        const meta::any_type int_const_ptr_type = library_scope.get_typedef("int const*");
        const meta::any_type int_volatile_ptr_type = library_scope.get_typedef("int volatile*");
        const meta::any_type int_const_volatile_ptr_type = library_scope.get_typedef("int const volatile*");
        const meta::any_type int_ref_type = library_scope.get_typedef("int&");
        const meta::any_type int_const_ref_type = library_scope.get_typedef("int const&");
        const meta::any_type int_volatile_ref_type = library_scope.get_typedef("int volatile&");
        const meta::any_type int_const_volatile_ref_type = library_scope.get_typedef("int const volatile&");

        REQUIRE(int_type);
        REQUIRE(int_ptr_type);
        REQUIRE(int_const_ptr_type);
        REQUIRE(int_volatile_ptr_type);
        REQUIRE(int_const_volatile_ptr_type);
        REQUIRE(int_ref_type);
        REQUIRE(int_const_ref_type);
        REQUIRE(int_volatile_ref_type);
        REQUIRE(int_const_volatile_ref_type);

        CHECK(int_type == meta::resolve_type<int>());
        CHECK(int_ptr_type == meta::resolve_type<int*>());
        CHECK(int_const_ptr_type == meta::resolve_type<int const*>());
        CHECK(int_volatile_ptr_type == meta::resolve_type<int volatile*>());
        CHECK(int_const_volatile_ptr_type == meta::resolve_type<int const volatile*>());
        CHECK(int_ref_type == meta::resolve_type<int&>());
        CHECK(int_const_ref_type == meta::resolve_type<int const&>());
        CHECK(int_volatile_ref_type == meta::resolve_type<int volatile&>());
        CHECK(int_const_volatile_ref_type == meta::resolve_type<int const volatile&>());

        CHECK(int_type != int_ptr_type);
        CHECK(int_type != int_ref_type);

        CHECK(int_ptr_type != int_const_ptr_type);
        CHECK(int_ptr_type != int_volatile_ptr_type);
        CHECK(int_ptr_type != int_const_volatile_ptr_type);
        CHECK(int_ptr_type != int_const_ref_type);
        CHECK(int_ptr_type != int_volatile_ref_type);
        CHECK(int_ptr_type != int_const_volatile_ref_type);

        CHECK(int_const_ptr_type != int_volatile_ptr_type);
        CHECK(int_const_ptr_type != int_const_volatile_ptr_type);
        CHECK(int_const_ptr_type != int_volatile_ref_type);
        CHECK(int_const_ptr_type != int_const_volatile_ref_type);

        CHECK(int_volatile_ptr_type != int_const_volatile_ptr_type);
        CHECK(int_volatile_ptr_type != int_const_volatile_ref_type);

        CHECK(int_ref_type != int_const_ref_type);
        CHECK(int_ref_type != int_volatile_ref_type);
        CHECK(int_ref_type != int_const_volatile_ref_type);
        CHECK(int_const_ref_type != int_volatile_ref_type);
        CHECK(int_const_ref_type != int_const_volatile_ref_type);
        CHECK(int_volatile_ref_type != int_const_volatile_ref_type);
    }

    SUBCASE("8") {
        const meta::any_type ivec2_const_lref_type = library_scope.get_typedef("ivec2 const&");
        const meta::any_type ivec2_const_rref_type = library_scope.get_typedef("ivec2 const&&");
        const meta::any_type ivec2_unbounded_array_type = library_scope.get_typedef("ivec2[]");
        const meta::any_type ivec2_unbounded_array_ref_type = library_scope.get_typedef("ivec2(&)[]");
        const meta::any_type ivec2_const_unbounded_array_ref_type = library_scope.get_typedef("ivec2 const(&)[]");
        const meta::any_type ivec2_const_ptr_bounded42_array_type = library_scope.get_typedef("ivec2 const*[42]");
        const meta::any_type ivec2_const_ptr_bounded21_array_type = library_scope.get_typedef("ivec2 const*[21]");

        REQUIRE(ivec2_const_lref_type);
        REQUIRE(ivec2_const_rref_type);
        REQUIRE(ivec2_unbounded_array_type);
        REQUIRE(ivec2_unbounded_array_ref_type);
        REQUIRE(ivec2_const_unbounded_array_ref_type);
        REQUIRE(ivec2_const_ptr_bounded42_array_type);
        REQUIRE(ivec2_const_ptr_bounded21_array_type);

        CHECK(ivec2_const_lref_type == meta::resolve_type<ivec2 const&>());
        CHECK(ivec2_const_rref_type == meta::resolve_type<ivec2 const&&>());
        CHECK(ivec2_unbounded_array_type == meta::resolve_type<ivec2[]>());
        CHECK(ivec2_unbounded_array_ref_type == meta::resolve_type<ivec2(&)[]>());
        CHECK(ivec2_const_unbounded_array_ref_type == meta::resolve_type<ivec2 const(&)[]>());
        CHECK(ivec2_const_ptr_bounded42_array_type == meta::resolve_type<ivec2 const*[42]>());
        CHECK(ivec2_const_ptr_bounded21_array_type == meta::resolve_type<ivec2 const*[21]>());

        CHECK(ivec2_const_lref_type != ivec2_const_rref_type);
        CHECK(ivec2_const_rref_type != ivec2_unbounded_array_type);
        CHECK(ivec2_unbounded_array_type != ivec2_unbounded_array_ref_type);
        CHECK(ivec2_unbounded_array_ref_type != ivec2_const_unbounded_array_ref_type);
        CHECK(ivec2_const_unbounded_array_ref_type != ivec2_const_ptr_bounded42_array_type);
        CHECK(ivec2_const_ptr_bounded42_array_type != ivec2_const_ptr_bounded21_array_type);
    }

    SUBCASE("9") {
        using meta::detail::type_access;
        using meta::detail::hash_composer;

        CHECK_FALSE(meta::resolve_type<ivec2[]>().get_id().get_hash() == hash_composer{} << type_access(meta::resolve_type<ivec2[]>()));
        CHECK_FALSE(meta::resolve_type<ivec2>().get_id().get_hash() == hash_composer{} << type_access(meta::resolve_type<ivec2>()));
        CHECK_FALSE(meta::resolve_type<color>().get_id().get_hash() == hash_composer{} << type_access(meta::resolve_type<color>()));
        CHECK_FALSE(meta::resolve_type<ivec2()>().get_id().get_hash() == hash_composer{} << type_access(meta::resolve_type<ivec2()>()));
        CHECK_FALSE(meta::resolve_type<void(ivec2)>().get_id().get_hash() == hash_composer{} << type_access(meta::resolve_type<void(ivec2)>()));
        CHECK_FALSE(meta::resolve_type<int ivec2::*>().get_id().get_hash() == hash_composer{} << type_access(meta::resolve_type<int ivec2::*>()));
        CHECK_FALSE(meta::resolve_type<int (ivec2::*)()>().get_id().get_hash() == hash_composer{} << type_access(meta::resolve_type<int (ivec2::*)()>()));
        CHECK_FALSE(meta::resolve_type<ivec2*>().get_id().get_hash() == hash_composer{} << type_access(meta::resolve_type<ivec2*>()));
        CHECK_FALSE(meta::resolve_type<ivec2 const*>().get_id().get_hash() == hash_composer{} << type_access(meta::resolve_type<ivec2 const*>()));
        CHECK_FALSE(meta::resolve_type<ivec2&>().get_id().get_hash() == hash_composer{} << type_access(meta::resolve_type<ivec2&>()));
        CHECK_FALSE(meta::resolve_type<ivec2 const&>().get_id().get_hash() == hash_composer{} << type_access(meta::resolve_type<ivec2 const&>()));

        CHECK(meta::resolve_type<unshared_ivec2[]>().get_id().get_hash() == hash_composer{} << type_access(meta::resolve_type<unshared_ivec2[]>()));
        CHECK(meta::resolve_type<unshared_ivec2>().get_id().get_hash() == hash_composer{} << type_access(meta::resolve_type<unshared_ivec2>()));
        CHECK(meta::resolve_type<unshared_color>().get_id().get_hash() == hash_composer{} << type_access(meta::resolve_type<unshared_color>()));
        CHECK(meta::resolve_type<unshared_ivec2()>().get_id().get_hash() == hash_composer{} << type_access(meta::resolve_type<unshared_ivec2()>()));
        CHECK(meta::resolve_type<void(unshared_ivec2)>().get_id().get_hash() == hash_composer{} << type_access(meta::resolve_type<void(unshared_ivec2)>()));
        CHECK(meta::resolve_type<unshared_ivec2 ivec2::*>().get_id().get_hash() == hash_composer{} << type_access(meta::resolve_type<unshared_ivec2 ivec2::*>()));
        CHECK(meta::resolve_type<int unshared_ivec2::*>().get_id().get_hash() == hash_composer{} << type_access(meta::resolve_type<int unshared_ivec2::*>()));
        CHECK(meta::resolve_type<unshared_ivec2 (ivec2::*)()>().get_id().get_hash() == hash_composer{} << type_access(meta::resolve_type<unshared_ivec2 (ivec2::*)()>()));
        CHECK(meta::resolve_type<int (ivec2::*)(unshared_ivec2)>().get_id().get_hash() == hash_composer{} << type_access(meta::resolve_type<int (ivec2::*)(unshared_ivec2)>()));
        CHECK(meta::resolve_type<int (unshared_ivec2::*)()>().get_id().get_hash() == hash_composer{} << type_access(meta::resolve_type<int (unshared_ivec2::*)()>()));
        CHECK(meta::resolve_type<unshared_ivec2*>().get_id().get_hash() == hash_composer{} << type_access(meta::resolve_type<unshared_ivec2*>()));
        CHECK(meta::resolve_type<unshared_ivec2 const*>().get_id().get_hash() == hash_composer{} << type_access(meta::resolve_type<unshared_ivec2 const*>()));
        CHECK(meta::resolve_type<unshared_ivec2&>().get_id().get_hash() == hash_composer{} << type_access(meta::resolve_type<unshared_ivec2&>()));
        CHECK(meta::resolve_type<unshared_ivec2 const&>().get_id().get_hash() == hash_composer{} << type_access(meta::resolve_type<unshared_ivec2 const&>()));
    }

    SUBCASE("10") {
        const meta::any_type int_ivec2_type = library_scope.get_typedef("int ivec2::*");
        const meta::any_type int_const_ivec2_type = library_scope.get_typedef("int const ivec2::*");
        const meta::any_type int_volatile_ivec2_type = library_scope.get_typedef("int volatile ivec2::*");
        const meta::any_type int_const_volatile_ivec2_type = library_scope.get_typedef("int const volatile ivec2::*");
        const meta::any_type int_ivec2_const_ptr = library_scope.get_typedef("int ivec2::*const*");

        REQUIRE(int_ivec2_type);
        REQUIRE(int_const_ivec2_type);
        REQUIRE(int_volatile_ivec2_type);
        REQUIRE(int_const_volatile_ivec2_type);
        REQUIRE(int_ivec2_const_ptr);

        CHECK(int_ivec2_type != int_const_ivec2_type);
        CHECK(int_const_ivec2_type != int_volatile_ivec2_type);
        CHECK(int_const_ivec2_type != int_const_volatile_ivec2_type);
        CHECK(int_volatile_ivec2_type != int_const_volatile_ivec2_type);
    }

    SUBCASE("11") {
        const meta::any_type int_ivec2_type = library_scope.get_typedef("int (ivec2::*)()");
        const meta::any_type float_ivec2_type = library_scope.get_typedef("float (ivec2::*)()");
        const meta::any_type int_ivec2_float_type = library_scope.get_typedef("int (ivec2::*)(float)");
        const meta::any_type int_ivec2_double_type = library_scope.get_typedef("int (ivec2::*)(double)");
        const meta::any_type int_ivec2_double_noexcept_type = library_scope.get_typedef("int (ivec2::*)(double) noexcept");
        const meta::any_type int_ivec2_const_ptr = library_scope.get_typedef("int (ivec2::*const*)()");

        REQUIRE(int_ivec2_type);
        REQUIRE(float_ivec2_type);
        REQUIRE(int_ivec2_float_type);
        REQUIRE(int_ivec2_double_type);
        REQUIRE(int_ivec2_double_noexcept_type);
        REQUIRE(int_ivec2_const_ptr);

        CHECK(int_ivec2_type == meta::resolve_type<int (ivec2::*)()>());
        CHECK(float_ivec2_type == meta::resolve_type<float (ivec2::*)()>());
        CHECK(int_ivec2_float_type == meta::resolve_type<int (ivec2::*)(float)>());
        CHECK(int_ivec2_double_type == meta::resolve_type<int (ivec2::*)(double)>());
        CHECK(int_ivec2_double_noexcept_type == meta::resolve_type<int (ivec2::*)(double) noexcept>());

        CHECK(int_ivec2_type != float_ivec2_type);
        CHECK(int_ivec2_type != int_ivec2_float_type);
        CHECK(int_ivec2_type != int_ivec2_double_type);
        CHECK(int_ivec2_type != int_ivec2_double_noexcept_type);

        CHECK(float_ivec2_type != int_ivec2_float_type);
        CHECK(int_ivec2_float_type != int_ivec2_double_type);
        CHECK(int_ivec2_double_type != int_ivec2_double_noexcept_type);

        CHECK(int_ivec2_float_type != int_ivec2_double_type);
        CHECK(int_ivec2_double_type != int_ivec2_double_noexcept_type);
    }
}
