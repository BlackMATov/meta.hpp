/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_base.hpp"
#include "meta_states.hpp"

namespace meta_hpp
{
    template < class_kind Class >
    class class_bind final {
    public:
        explicit class_bind();
        operator class_type() const noexcept;

        template < typename... Args >
        class_bind& ctor_();

        template < class_kind Base >
        class_bind& base_();

        template < function_kind Function >
        class_bind& function_(std::string name, Function function);

        template < member_kind Member >
        class_bind& member_(std::string name, Member member);

        template < method_kind Method >
        class_bind& method_(std::string name, Method method);

        template < pointer_kind Pointer >
        class_bind& variable_(std::string name, Pointer pointer);
    private:
        detail::class_type_data_ptr data_;
    };
}

namespace meta_hpp
{
    template < enum_kind Enum >
    class enum_bind final {
    public:
        explicit enum_bind();
        operator enum_type() const noexcept;

        enum_bind& evalue_(std::string name, Enum value);
    private:
        detail::enum_type_data_ptr data_;
    };
}

namespace meta_hpp
{
    class scope_bind final {
    public:
        explicit scope_bind(std::string_view name);
        operator scope() const noexcept;

        template < class_kind Class >
        scope_bind& class_(std::string name);

        template < enum_kind Enum >
        scope_bind& enum_(std::string name);

        template < function_kind Function >
        scope_bind& function_(std::string name, Function function);

        template < pointer_kind Pointer >
        scope_bind& variable_(std::string name, Pointer pointer);
    private:
        detail::scope_state_ptr state_;
    };
}

namespace meta_hpp
{
    template < class_kind Class >
    class_bind<Class> class_() {
        return class_bind<Class>{};
    }

    template < enum_kind Enum >
    enum_bind<Enum> enum_() {
        return enum_bind<Enum>{};
    }

    inline scope_bind scope_(std::string name) {
        return scope_bind{std::move(name)};
    }
}
