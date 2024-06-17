/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_all.hpp>
#include <doctest/doctest.h>

namespace
{
    struct component {
        component() = default;
        component(const component&) = default;
        virtual ~component() = default;
        META_HPP_ENABLE_POLY_INFO()
    };

    struct position_component : component {
        META_HPP_ENABLE_POLY_INFO()
    public:
        explicit position_component(int nx, int ny) : x{nx}, y{ny} {}

        int x{};
        int y{};
    };
}

TEST_CASE("meta/meta_discussion/88") {
    namespace meta = meta_hpp;

    meta::class_<position_component>()
        .member_("x", &position_component::x, meta::member_policy::as_pointer)
        .member_("y", &position_component::y, meta::member_policy::as_pointer);

    position_component position{21, 42};

    std::vector<component*> components{&position};

    for (component* c : components) {
        // meta_poly_ptr returns a pointer of the most derived type (position_component* in this case)
        meta::uvalue derived_instance_ptr = c->meta_poly_ptr();
        meta::any_type derived_instance_ptr_type = derived_instance_ptr.get_type();
        CHECK(derived_instance_ptr_type == meta::resolve_type<position_component*>());

        // to get all members of the component we should extract a class type from the pointer (position_component)
        meta::any_type derived_instance_type = derived_instance_ptr_type.as_pointer().get_data_type();
        CHECK(derived_instance_type == meta::resolve_type<position_component>());

        // for each member we can show our debug inspector (imgui input for example)
        for (const meta::member& m : derived_instance_type.as_class().get_members()) {
            // we registered members as pointers (member_policy::as_pointer), so the type of the member is a pointer
            CHECK(m.get_type() == meta::resolve_type<int position_component::*>());
            CHECK(m.get_type().get_value_type() == meta::resolve_type<int>());

            // Note:
            // We can register members without any policy then member::get will return copy of this member value,
            // and to change this member we will have to call member::set function.
            // But in this case we will change values in-place by a pointer

            meta::uvalue value = m.get(derived_instance_ptr);
            CHECK(value.get_type() == meta::resolve_type<int*>());

            int* raw_value = value.as<int*>();

            // here we can show the value on the screen or change it
            CHECK((*raw_value == 21 || *raw_value == 42));
            *raw_value = 84;
        }

        CHECK(position.x == 84);
        CHECK(position.y == 84);
    }
}
