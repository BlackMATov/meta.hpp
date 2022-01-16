/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_base.hpp"
#include "meta_states.hpp"

namespace meta_hpp::detail
{
    template < typename Class, typename... Args >
    concept class_bind_ctor_kind =
        class_kind<Class> &&
        requires(Args&&... args) { { Class{std::forward<Args>(args)...} }; };

    template < typename Class, typename Base >
    concept class_bind_base_kind =
        class_kind<Class> && class_kind<Base> &&
        stdex::derived_from<Class, Base>;

    template < typename Class, typename Function >
    concept class_bind_function_kind =
        class_kind<Class> && function_kind<Function>;

    template < typename Class, typename Member >
    concept class_bind_member_kind =
        class_kind<Class> && member_kind<Member> &&
        stdex::same_as<Class, typename member_traits<Member>::class_type>;

    template < typename Class, typename Method >
    concept class_bind_method_kind =
        class_kind<Class> && method_kind<Method> &&
        stdex::same_as<Class, typename method_traits<Method>::class_type>;

    template < typename Class, typename Pointer >
    concept class_bind_variable_kind =
        class_kind<Class> && pointer_kind<Pointer>;
}

namespace meta_hpp
{
    template < detail::class_kind Class >
    class class_bind final {
    public:
        explicit class_bind();
        operator class_type() const noexcept;

        template < typename... Args >
            requires detail::class_bind_ctor_kind<Class, Args...>
        class_bind& ctor_();

        template < detail::class_kind Base >
            requires detail::class_bind_base_kind<Class, Base>
        class_bind& base_();

        template < detail::function_kind Function >
            requires detail::class_bind_function_kind<Class, Function>
        class_bind& function_(std::string name, Function function);

        template < detail::member_kind Member >
            requires detail::class_bind_member_kind<Class, Member>
        class_bind& member_(std::string name, Member member);

        template < detail::method_kind Method >
            requires detail::class_bind_method_kind<Class, Method>
        class_bind& method_(std::string name, Method method);

        template < detail::pointer_kind Pointer >
            requires detail::class_bind_variable_kind<Class, Pointer>
        class_bind& variable_(std::string name, Pointer pointer);
    private:
        detail::class_type_data_ptr data_;
    };
}

namespace meta_hpp
{
    template < detail::enum_kind Enum >
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
        struct local_tag {};
        struct static_tag {};

        explicit scope_bind(std::string name, local_tag);
        explicit scope_bind(std::string_view name, static_tag);
        operator scope() const noexcept;

        template < detail::class_kind Class >
        scope_bind& class_(std::string name);

        template < detail::enum_kind Enum >
        scope_bind& enum_(std::string name);

        template < detail::function_kind Function >
        scope_bind& function_(std::string name, Function function);

        template < detail::pointer_kind Pointer >
        scope_bind& variable_(std::string name, Pointer pointer);
    private:
        detail::scope_state_ptr state_;
    };
}

namespace meta_hpp
{
    template < detail::class_kind Class >
    class_bind<Class> class_() {
        return class_bind<Class>{};
    }

    template < detail::enum_kind Enum >
    enum_bind<Enum> enum_() {
        return enum_bind<Enum>{};
    }

    inline scope_bind local_scope_(std::string name) {
        return scope_bind{std::move(name), scope_bind::local_tag()};
    }

    inline scope_bind static_scope_(std::string_view name) {
        return scope_bind{name, scope_bind::static_tag()};
    }
}
