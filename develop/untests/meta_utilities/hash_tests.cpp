/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include "../meta_untests.hpp"

namespace
{
    const double pi_v{3.1415926536};

    enum class color : unsigned {
        red = 0xFF0000,
        green = 0x00FF00,
        blue = 0x0000FF,
    };

    struct ivec2 {
        int x{};
        int y{};

        [[maybe_unused]] explicit ivec2(int nv) : x{nv}, y{nv} {}
        [[maybe_unused]] ivec2(int nx, int ny) : x{nx}, y{ny} {}

        ivec2& add(const ivec2& other) noexcept {
            x += other.x;
            y += other.y;
            return *this;
        }

        static ivec2 iadd(const ivec2& l, const ivec2& r) noexcept {
            return {l.x + r.x, l.y + r.y};
        }
    };
}

TEST_CASE("meta/meta_utilities/hash") {
    namespace meta = meta_hpp;

    meta::enum_<color>()
        .evalue_("red", color::red)
        .evalue_("green", color::green)
        .evalue_("blue", color::blue);

    meta::class_<ivec2>()
        .constructor_<int>()
        .constructor_<int, int>()
        .destructor_()
        .member_("x", &ivec2::x)
        .member_("y", &ivec2::y)
        .method_("add", &ivec2::add)
        .function_("iadd", &ivec2::iadd);

    const meta::class_type ivec2_type = meta::resolve_type<ivec2>();
    const meta::constructor ivec2_ctor = ivec2_type.get_constructor_with<int>();
    const meta::destructor ivec2_dtor = *ivec2_type.get_destructors().begin();
    const meta::function ivec2_function = ivec2_type.get_function("iadd");
    const meta::argument ivec2_function_arg = ivec2_function.get_argument(0);
    const meta::member ivec2_member = ivec2_type.get_member("x");
    const meta::method ivec2_method = ivec2_type.get_method("add");

    const meta::enum_type color_type = meta::resolve_type<color>();
    const meta::evalue red_color = color_type.get_evalue("red");

    const meta::scope local_scope = meta::local_scope_("local-scope")
        .variable_("pi_v", &pi_v);

    const meta::variable pi_variable = local_scope.get_variable("pi_v");

    SUBCASE("index_family") {
        std::hash<meta::argument_index>{}(ivec2_function_arg.get_index());
        std::hash<meta::constructor_index>{}(ivec2_ctor.get_index());
        std::hash<meta::destructor_index>{}(ivec2_dtor.get_index());
        std::hash<meta::evalue_index>{}(red_color.get_index());
        std::hash<meta::function_index>{}(ivec2_function.get_index());
        std::hash<meta::member_index>{}(ivec2_member.get_index());
        std::hash<meta::method_index>{}(ivec2_method.get_index());
        std::hash<meta::scope_index>{}(local_scope.get_index());
        std::hash<meta::variable_index>{}(pi_variable.get_index());
    }

    SUBCASE("type_family") {
        std::hash<meta::any_type>{}(meta::resolve_type<ivec2>());
        std::hash<meta::array_type>{}(meta::resolve_type<int[]>());
        std::hash<meta::class_type>{}(meta::resolve_type<ivec2>());
        std::hash<meta::constructor_type>{}(ivec2_ctor.get_type());
        std::hash<meta::destructor_type>{}(ivec2_dtor.get_type());
        std::hash<meta::enum_type>{}(red_color.get_type());
        std::hash<meta::function_type>{}(ivec2_function.get_type());
        std::hash<meta::member_type>{}(ivec2_member.get_type());
        std::hash<meta::method_type>{}(ivec2_method.get_type());
        std::hash<meta::nullptr_type>{}(meta::resolve_type<std::nullptr_t>());
        std::hash<meta::number_type>{}(meta::resolve_type<int>());
        std::hash<meta::pointer_type>{}(meta::resolve_type<int*>());
        std::hash<meta::reference_type>{}(meta::resolve_type<int&>());
        std::hash<meta::void_type>{}(meta::resolve_type<void>());
    }
}
