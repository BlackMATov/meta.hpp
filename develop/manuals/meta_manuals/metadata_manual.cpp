/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>

#include <doctest/doctest.h>
#include <fmt/core.h>

namespace
{
    struct ivec3 {
        int x{};
        int y{};
        int z{};
    };

    ivec3 cross(const ivec3& a, const ivec3& b) {
        return {
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x, };
    }
}

TEST_CASE("meta/meta_manuals/metadata") {
    namespace meta = meta_hpp;
    using namespace std::literals;

    // you can add additional key-value metadata to any types and their properties:
    //   key: std::string
    //   value: meta::uvalue

    meta::class_<ivec3>({
        {"tooltip", "3D Vector"s} // for class type
    })
    .member_("x", &ivec3::x, {
        .metadata { {"tooltip", "X-Coordinate"s} } // for class members
    })
    .member_("y", &ivec3::y, {
        .metadata { {"tooltip", "Y-Coordinate"s} }
    })
    .member_("z", &ivec3::z, {
        .metadata { {"tooltip", "Z-Coordinate"s} }
    });

    const meta::scope math_scope = meta::local_scope_("math")
        .typedef_<ivec3>("ivec3")
        .function_("cross", &cross, {
            .arguments {{
                .name = "first vector",
                .metadata {} // even function arguments can have metadata
            },{
                .name = "second vector"
            }},
            .metadata { {"tooltip", "Cross product of vectors"s} } // for functions in a scope
        });

    // after binding, you can use it as you wish
    // in this example, I'm just going to print all of them to stdout

    for ( const auto& [type_name, type] : math_scope.get_typedefs() ) {
        if ( !type.is_class() ) {
            continue;
        }

        const meta::class_type& class_type = type.as_class();
        const meta::metadata_map& class_metadata = class_type.get_metadata();

        fmt::print("{} ({})\n",
            type_name,
            class_metadata.at("tooltip").get_as<std::string>());

        for ( const meta::member& member : class_type.get_members() ) {
            const meta::metadata_map& member_metadata = member.get_metadata();

            fmt::print("  - {} ({})\n",
                member.get_name(),
                member_metadata.at("tooltip").get_as<std::string>());
        }
    }

    for ( const meta::function& function : math_scope.get_functions() ) {
        const meta::metadata_map& function_metadata = function.get_metadata();

        fmt::print("  + {}/{} ({})\n",
            function.get_name(),
            function.get_type().get_arity(),
            function_metadata.at("tooltip").get_as<std::string>());

        for ( const meta::argument& argument : function.get_arguments() ) {
            fmt::print("    ({}) : {}\n",
                argument.get_position(),
                argument.get_name());
        }
    }

    // Output:
    // ivec3 (3D Vector)
    // - x (X-Coordinate)
    // - y (Y-Coordinate)
    // - z (Z-Coordinate)
    // + cross/2 (Cross product of vectors)
    //     (0) : first vector
    //     (1) : second vector
}
