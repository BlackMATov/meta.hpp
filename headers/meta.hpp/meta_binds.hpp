/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_base.hpp"
#include "meta_states.hpp"
#include "meta_registry.hpp"

namespace meta_hpp::detail
{
    template < typename Class, typename... Args >
    concept class_bind_constructor_kind =
        class_kind<Class> &&
        std::is_constructible_v<Class, Args...>;

    template < typename Class >
    concept class_bind_destructor_kind =
        class_kind<Class> &&
        std::is_destructible_v<Class>;

    template < typename Class, typename Base >
    concept class_bind_base_kind =
        class_kind<Class> && class_kind<Base> &&
        std::derived_from<Class, Base>;

    template < typename Class, typename Member >
    concept class_bind_member_kind =
        class_kind<Class> && member_kind<Member> &&
        std::is_same_v<Class, typename member_traits<Member>::class_type>;

    template < typename Class, typename Method >
    concept class_bind_method_kind =
        class_kind<Class> && method_kind<Method> &&
        std::is_same_v<Class, typename method_traits<Method>::class_type>;
}

namespace meta_hpp
{
    struct argument_opts final {
        std::string name{};
        metadata_map metadata{};
    };

    using argument_opts_list = std::vector<argument_opts>;

    struct constructor_opts final {
        argument_opts_list arguments{};
        metadata_map metadata{};
    };

    struct destructor_opts final {
        metadata_map metadata{};
    };

    struct evalue_opts final {
        metadata_map metadata{};
    };

    struct function_opts final {
        argument_opts_list arguments{};
        metadata_map metadata{};
    };

    struct member_opts final {
        metadata_map metadata{};
    };

    struct method_opts final {
        argument_opts_list arguments{};
        metadata_map metadata{};
    };

    struct variable_opts final {
        metadata_map metadata{};
    };
}

namespace meta_hpp
{
    template < detail::array_kind Array >
    class array_bind final {
    public:
        explicit array_bind(metadata_map metadata);
        operator array_type() const noexcept;
    private:
        detail::array_type_data* data_{};
        detail::type_registry::locker locker_{};
    };
}

namespace meta_hpp
{
    template < detail::class_kind Class >
    class class_bind final {
    public:
        explicit class_bind(metadata_map metadata);
        operator class_type() const noexcept;

        // base_

        template < detail::class_kind... Bases >
        class_bind& base_()
            requires (... && detail::class_bind_base_kind<Class, Bases>);

        // constructor_

        template < typename... Args
                 , constructor_policy_kind Policy = constructor_policy::as_object_t >
        class_bind& constructor_(Policy = Policy{})
            requires detail::class_bind_constructor_kind<Class, Args...>;

        template < typename... Args
                 , constructor_policy_kind Policy = constructor_policy::as_object_t >
        class_bind& constructor_(constructor_opts opts, Policy = Policy{})
            requires detail::class_bind_constructor_kind<Class, Args...>;

        // destructor_

        class_bind& destructor_()
            requires detail::class_bind_destructor_kind<Class>;

        class_bind& destructor_(destructor_opts opts)
            requires detail::class_bind_destructor_kind<Class>;

        // function_

        template < detail::function_kind Function
                 , function_policy_kind Policy = function_policy::as_copy_t >
        class_bind& function_(
            std::string name,
            Function function,
            Policy = Policy{});

        template < detail::function_kind Function
                 , function_policy_kind Policy = function_policy::as_copy_t >
        class_bind& function_(
            std::string name,
            Function function,
            function_opts opts,
            Policy = Policy{});

        template < detail::function_kind Function
                 , function_policy_kind Policy = function_policy::as_copy_t >
        class_bind& function_(
            std::string name,
            Function function,
            std::initializer_list<std::string_view> arguments,
            Policy = Policy{});

        // member_

        template < detail::member_kind Member
                 , member_policy_kind Policy = member_policy::as_copy_t >
        class_bind& member_(
            std::string name,
            Member member,
            Policy = Policy{})
            requires detail::class_bind_member_kind<Class, Member>;

        template < detail::member_kind Member
                 , member_policy_kind Policy = member_policy::as_copy_t >
        class_bind& member_(
            std::string name,
            Member member,
            member_opts opts,
            Policy = Policy{})
            requires detail::class_bind_member_kind<Class, Member>;

        // method_

        template < detail::method_kind Method
                 , method_policy_kind Policy = method_policy::as_copy_t >
        class_bind& method_(
            std::string name,
            Method method,
            Policy = Policy{})
            requires detail::class_bind_method_kind<Class, Method>;

        template < detail::method_kind Method
                 , method_policy_kind Policy = method_policy::as_copy_t >
        class_bind& method_(
            std::string name,
            Method method,
            method_opts opts,
            Policy = Policy{})
            requires detail::class_bind_method_kind<Class, Method>;

        template < detail::method_kind Method
                 , method_policy_kind Policy = method_policy::as_copy_t >
        class_bind& method_(
            std::string name,
            Method method,
            std::initializer_list<std::string_view> arguments,
            Policy = Policy{})
            requires detail::class_bind_method_kind<Class, Method>;

        // typdef_

        template < typename Type >
        class_bind& typedef_(std::string name);

        // variable_

        template < detail::pointer_kind Pointer
                 , variable_policy_kind Policy = variable_policy::as_copy_t >
        class_bind& variable_(
            std::string name,
            Pointer pointer,
            Policy = Policy{});

        template < detail::pointer_kind Pointer
                 , variable_policy_kind Policy = variable_policy::as_copy_t >
        class_bind& variable_(
            std::string name,
            Pointer pointer,
            variable_opts opts,
            Policy = Policy{});
    private:
        detail::class_type_data* data_{};
        detail::type_registry::locker locker_{};
    };
}

namespace meta_hpp
{
    template < detail::enum_kind Enum >
    class enum_bind final {
    public:
        explicit enum_bind(metadata_map metadata);
        operator enum_type() const noexcept;

        enum_bind& evalue_(std::string name, Enum value);
        enum_bind& evalue_(std::string name, Enum value, evalue_opts opts);
    private:
        detail::enum_type_data* data_{};
        detail::type_registry::locker locker_{};
    };
}

namespace meta_hpp
{
    template < detail::function_kind Function >
    class function_bind final {
    public:
        explicit function_bind(metadata_map metadata);
        operator function_type() const noexcept;
    private:
        detail::function_type_data* data_{};
        detail::type_registry::locker locker_{};
    };
}

namespace meta_hpp
{
    template < detail::member_kind Member >
    class member_bind final {
    public:
        explicit member_bind(metadata_map metadata);
        operator member_type() const noexcept;
    private:
        detail::member_type_data* data_{};
        detail::type_registry::locker locker_{};
    };
}

namespace meta_hpp
{
    template < detail::method_kind Method >
    class method_bind final {
    public:
        explicit method_bind(metadata_map metadata);
        operator method_type() const noexcept;
    private:
        detail::method_type_data* data_{};
        detail::type_registry::locker locker_{};
    };
}

namespace meta_hpp
{
    template < detail::nullptr_kind Nullptr >
    class nullptr_bind final {
    public:
        explicit nullptr_bind(metadata_map metadata);
        operator nullptr_type() const noexcept;
    private:
        detail::nullptr_type_data* data_{};
        detail::type_registry::locker locker_{};
    };
}

namespace meta_hpp
{
    template < detail::number_kind Number >
    class number_bind final {
    public:
        explicit number_bind(metadata_map metadata);
        operator number_type() const noexcept;
    private:
        detail::number_type_data* data_{};
        detail::type_registry::locker locker_{};
    };
}

namespace meta_hpp
{
    template < detail::pointer_kind Pointer >
    class pointer_bind final {
    public:
        explicit pointer_bind(metadata_map metadata);
        operator pointer_type() const noexcept;
    private:
        detail::pointer_type_data* data_{};
        detail::type_registry::locker locker_{};
    };
}

namespace meta_hpp
{
    template < detail::reference_kind Reference >
    class reference_bind final {
    public:
        explicit reference_bind(metadata_map metadata);
        operator reference_type() const noexcept;
    private:
        detail::reference_type_data* data_{};
        detail::type_registry::locker locker_{};
    };
}

namespace meta_hpp
{
    template < detail::void_kind Void >
    class void_bind final {
    public:
        explicit void_bind(metadata_map metadata);
        operator void_type() const noexcept;
    private:
        detail::void_type_data* data_{};
        detail::type_registry::locker locker_{};
    };
}

namespace meta_hpp
{
    class scope_bind final {
    public:
        explicit scope_bind(const scope& scope, metadata_map metadata);
        operator scope() const noexcept;

        // function_

        template < detail::function_kind Function
                 , function_policy_kind Policy = function_policy::as_copy_t >
        scope_bind& function_(
            std::string name,
            Function function,
            Policy = Policy{});

        template < detail::function_kind Function
                 , function_policy_kind Policy = function_policy::as_copy_t >
        scope_bind& function_(
            std::string name,
            Function function,
            function_opts opts,
            Policy = Policy{});

        template < detail::function_kind Function
                 , function_policy_kind Policy = function_policy::as_copy_t >
        scope_bind& function_(
            std::string name,
            Function function,
            std::initializer_list<std::string_view> arguments,
            Policy = Policy{});

        // typedef_

        template < typename Type >
        scope_bind& typedef_(std::string name);

        // variable_

        template < detail::pointer_kind Pointer
                 , variable_policy_kind Policy = variable_policy::as_copy_t >
        scope_bind& variable_(
            std::string name,
            Pointer pointer,
            Policy = Policy{});

        template < detail::pointer_kind Pointer
                 , variable_policy_kind Policy = variable_policy::as_copy_t >
        scope_bind& variable_(
            std::string name,
            Pointer pointer,
            variable_opts opts,
            Policy = Policy{});
    private:
        detail::scope_state_ptr state_;
        detail::state_registry::locker locker_{};
    };
}

namespace meta_hpp
{
    template < detail::array_kind Array >
    array_bind<Array> array_(metadata_map metadata = {}) {
        return array_bind<Array>{std::move(metadata)};
    }

    template < detail::class_kind Class >
    class_bind<Class> class_(metadata_map metadata = {}) {
        return class_bind<Class>{std::move(metadata)};
    }

    template < detail::enum_kind Enum >
    enum_bind<Enum> enum_(metadata_map metadata = {}) {
        return enum_bind<Enum>{std::move(metadata)};
    }

    template < detail::function_kind Function >
    function_bind<Function> function_(metadata_map metadata = {}) {
        return function_bind<Function>{std::move(metadata)};
    }

    template < detail::member_kind Member >
    member_bind<Member> member_(metadata_map metadata = {}) {
        return member_bind<Member>{std::move(metadata)};
    }

    template < detail::method_kind Method >
    method_bind<Method> method_(metadata_map metadata = {}) {
        return method_bind<Method>{std::move(metadata)};
    }

    template < detail::nullptr_kind Nullptr >
    nullptr_bind<Nullptr> nullptr_(metadata_map metadata = {}) {
        return nullptr_bind<Nullptr>{std::move(metadata)};
    }

    template < detail::number_kind Number >
    number_bind<Number> number_(metadata_map metadata = {}) {
        return number_bind<Number>{std::move(metadata)};
    }

    template < detail::pointer_kind Pointer >
    pointer_bind<Pointer> pointer_(metadata_map metadata = {}) {
        return pointer_bind<Pointer>{std::move(metadata)};
    }

    template < detail::reference_kind Reference >
    reference_bind<Reference> reference_(metadata_map metadata = {}) {
        return reference_bind<Reference>{std::move(metadata)};
    }

    template < detail::void_kind Void >
    void_bind<Void> void_(metadata_map metadata = {}) {
        return void_bind<Void>{std::move(metadata)};
    }
}

namespace meta_hpp
{
    inline scope_bind local_scope_(std::string name, metadata_map metadata = {}) {
        scope local_scope{detail::scope_state::make(std::move(name))};
        return scope_bind{local_scope, std::move(metadata)};
    }

    inline scope_bind static_scope_(std::string_view name, metadata_map metadata = {}) {
        scope static_scope{resolve_scope(name)};
        return scope_bind{static_scope, std::move(metadata)};
    }

    inline scope_bind extend_scope_(const scope& scope, metadata_map metadata = {}) {
        return scope_bind{scope, std::move(metadata)};
    }
}
