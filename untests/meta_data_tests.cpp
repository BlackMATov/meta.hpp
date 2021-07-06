/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <meta.hpp/meta_class.hpp>
#include <meta.hpp/meta_data.hpp>
#include <meta.hpp/meta_field.hpp>
#include <meta.hpp/meta_function.hpp>
#include <meta.hpp/meta_method.hpp>
#include <meta.hpp/meta_namespace.hpp>
#include <meta.hpp/meta_variable.hpp>

#include "doctest/doctest.hpp"

namespace
{
    struct clazz {
        int field{42};
        static int variable;
        int method() const { return 42; }
        static int function() { return 42; }
    };

    int clazz::variable{42};
}

TEST_CASE("meta/data") {
    namespace meta = meta_hpp;
    using namespace std::string_literals;

    meta::data_ data_("key"s, "value"s);
    const meta::data_info& data_info = data_;

    CHECK(data_info.id() == "key");
    CHECK(data_info.get().to_string() == "value");

    {
        data_(
            meta::data_("key2"s, "value2"s)(
                meta::data_("key3"s, "value3"s)
            )
        );

        CHECK(data_info.get_data("key2"));
        CHECK(data_info.get_data("key2")->get().to_string() == "value2");

        CHECK(data_info.get_data("key2")->get_data("key3"));
        CHECK(data_info.get_data("key2")->get_data("key3")->get().to_string() == "value3");
    }

    {
        data_(
            meta::data_("key2"s, "value2"s)(
                meta::data_("key4"s, "value4"s)
            )
        );

        CHECK(data_info.get_data("key2"));
        CHECK(data_info.get_data("key2")->get().to_string() == "value2");

        CHECK(data_info.get_data("key2")->get_data("key3"));
        CHECK(data_info.get_data("key2")->get_data("key3")->get().to_string() == "value3");

        CHECK(data_info.get_data("key2")->get_data("key4"));
        CHECK(data_info.get_data("key2")->get_data("key4")->get().to_string() == "value4");
    }
}

TEST_CASE("meta/data/class") {
    namespace meta = meta_hpp;
    using namespace std::string_literals;

    const meta::class_info clazz_info = meta::class_<clazz>("clazz")(
        meta::data_("hello"s, "world"s)
    );

    CHECK(clazz_info.get_data("hello"));
    CHECK(clazz_info.get_data("hello")->get().to_string() == "world");
}

TEST_CASE("meta/data/data") {
    namespace meta = meta_hpp;
    using namespace std::string_literals;

    const meta::class_info clazz_info = meta::class_<clazz>("clazz")(
        meta::data_("hello"s, "world"s)(
            meta::data_("hello2"s, "world2"s)
        )
    );

    CHECK(clazz_info.get_data("hello"));
    CHECK(clazz_info.get_data("hello")->get_data("hello2"));
    CHECK(clazz_info.get_data("hello")->get_data("hello2")->get().to_string() == "world2");
}

TEST_CASE("meta/data/field") {
    namespace meta = meta_hpp;
    using namespace std::string_literals;

    const meta::class_info clazz_info = meta::class_<clazz>("clazz")(
        meta::field_("field", &clazz::field)(
            meta::data_("hello"s, "world"s)
        )
    );

    CHECK(clazz_info.get_field("field"));
    CHECK(clazz_info.get_field("field")->get_data("hello"));
    CHECK(clazz_info.get_field("field")->get_data("hello")->get().to_string() == "world");
}

TEST_CASE("meta/data/function") {
    namespace meta = meta_hpp;
    using namespace std::string_literals;

    const meta::class_info clazz_info = meta::class_<clazz>("clazz")(
        meta::function_<&clazz::function>("function")(
            meta::data_("hello"s, "world"s)
        )
    );

    CHECK(clazz_info.get_function("function"));
    CHECK(clazz_info.get_function("function")->get_data("hello"));
    CHECK(clazz_info.get_function("function")->get_data("hello")->get().to_string() == "world");
}

TEST_CASE("meta/data/method") {
    namespace meta = meta_hpp;
    using namespace std::string_literals;

    const meta::class_info clazz_info = meta::class_<clazz>("clazz")(
        meta::method_("method", &clazz::method)(
            meta::data_("hello"s, "world"s)
        )
    );

    CHECK(clazz_info.get_method("method"));
    CHECK(clazz_info.get_method("method")->get_data("hello"));
    CHECK(clazz_info.get_method("method")->get_data("hello")->get().to_string() == "world");
}

TEST_CASE("meta/data/namespace") {
    namespace meta = meta_hpp;
    using namespace std::string_literals;

    const meta::namespace_info ns_info = meta::namespace_("ns")(
        meta::data_("hello"s, "world"s)
    );

    CHECK(ns_info.get_data("hello"));
    CHECK(ns_info.get_data("hello")->get().to_string() == "world");
}

TEST_CASE("meta/data/variable") {
    namespace meta = meta_hpp;
    using namespace std::string_literals;

    const meta::class_info clazz_info = meta::class_<clazz>("clazz")(
        meta::variable_<&clazz::variable>("variable")(
            meta::data_("hello"s, "world"s)
        )
    );

    CHECK(clazz_info.get_variable("variable"));
    CHECK(clazz_info.get_variable("variable")->get_data("hello"));
    CHECK(clazz_info.get_variable("variable")->get_data("hello")->get().to_string() == "world");
}
