/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_base.hpp"
#include "meta_states.hpp"

namespace meta_hpp::detail
{
    template < typename Class, typename... Args >
    concept class_bind_constructor_kind =
        class_kind<Class> &&
        requires(Args&&... args) { { Class{std::forward<Args>(args)...} }; };

    template < typename Class >
    concept class_bind_destructor_kind =
        class_kind<Class> &&
        requires(Class&& inst) { { inst.~Class() }; };

    template < typename Class, typename Base >
    concept class_bind_base_kind =
        class_kind<Class> && class_kind<Base> &&
        stdex::derived_from<Class, Base>;

    template < typename Class, typename Member >
    concept class_bind_member_kind =
        class_kind<Class> && member_kind<Member> &&
        stdex::same_as<Class, typename member_traits<Member>::class_type>;

    template < typename Class, typename Method >
    concept class_bind_method_kind =
        class_kind<Class> && method_kind<Method> &&
        stdex::same_as<Class, typename method_traits<Method>::class_type>;
}

namespace meta_hpp
{
    struct type_opts final {
        metadata_map metadata{};
    };

    struct argument_opts final {
        std::string name{};
        metadata_map metadata{};
    };

    struct constructor_opts final {
        std::vector<argument_opts> arguments{};
        metadata_map metadata{};
    };

    struct destructor_opts final {
        metadata_map metadata{};
    };

    struct evalue_opts final {
        metadata_map metadata{};
    };

    struct function_opts final {
        std::vector<argument_opts> arguments{};
        metadata_map metadata{};
    };

    struct member_opts final {
        metadata_map metadata{};
    };

    struct method_opts final {
        std::vector<argument_opts> arguments{};
        metadata_map metadata{};
    };

    struct scope_opts final {
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
        explicit array_bind(type_opts opts);
        operator array_type() const noexcept;
    private:
        detail::array_type_data_ptr data_;
    };
}

namespace meta_hpp
{
    template < detail::class_kind Class >
    class class_bind final {
    public:
        explicit class_bind(type_opts opts);
        operator class_type() const noexcept;

        // constructor_

        template < typename... Args
                 , constructor_policy_kind Policy = constructor_policy::as_object >
        class_bind& constructor_(Policy = Policy{})
            requires detail::class_bind_constructor_kind<Class, Args...>;

        template < typename... Args
                 , constructor_policy_kind Policy = constructor_policy::as_object >
        class_bind& constructor_(constructor_opts opts, Policy = Policy{})
            requires detail::class_bind_constructor_kind<Class, Args...>;

        // destructor_

        class_bind& destructor_()
            requires detail::class_bind_destructor_kind<Class>;

        class_bind& destructor_(destructor_opts opts)
            requires detail::class_bind_destructor_kind<Class>;

        // base_

        template < detail::class_kind Base >
        class_bind& base_()
            requires detail::class_bind_base_kind<Class, Base>;

        // function_

        template < detail::function_kind Function
                 , function_policy_kind Policy = function_policy::as_copy >
        class_bind& function_(
            std::string name,
            Function function,
            Policy = Policy{});

        template < detail::function_kind Function
                 , function_policy_kind Policy = function_policy::as_copy >
        class_bind& function_(
            std::string name,
            Function function,
            function_opts opts,
            Policy = Policy{});

        template < detail::function_kind Function
                 , function_policy_kind Policy = function_policy::as_copy >
        class_bind& function_(
            std::string name,
            Function function,
            std::initializer_list<std::string_view> arguments,
            Policy = Policy{});

        // member_

        template < detail::member_kind Member
                 , member_policy_kind Policy = member_policy::as_copy >
        class_bind& member_(
            std::string name,
            Member member,
            Policy = Policy{})
            requires detail::class_bind_member_kind<Class, Member>;

        template < detail::member_kind Member
                 , member_policy_kind Policy = member_policy::as_copy >
        class_bind& member_(
            std::string name,
            Member member,
            member_opts opts,
            Policy = Policy{})
            requires detail::class_bind_member_kind<Class, Member>;

        // method_

        template < detail::method_kind Method
                 , method_policy_kind Policy = method_policy::as_copy >
        class_bind& method_(
            std::string name,
            Method method,
            Policy = Policy{})
            requires detail::class_bind_method_kind<Class, Method>;

        template < detail::method_kind Method
                 , method_policy_kind Policy = method_policy::as_copy >
        class_bind& method_(
            std::string name,
            Method method,
            method_opts opts,
            Policy = Policy{})
            requires detail::class_bind_method_kind<Class, Method>;

        template < detail::method_kind Method
                 , method_policy_kind Policy = method_policy::as_copy >
        class_bind& method_(
            std::string name,
            Method method,
            std::initializer_list<std::string_view> arguments,
            Policy = Policy{})
            requires detail::class_bind_method_kind<Class, Method>;

        // variable_

        template < detail::pointer_kind Pointer
                 , variable_policy_kind Policy = variable_policy::as_copy >
        class_bind& variable_(
            std::string name,
            Pointer pointer,
            Policy = Policy{});

        template < detail::pointer_kind Pointer
                 , variable_policy_kind Policy = variable_policy::as_copy >
        class_bind& variable_(
            std::string name,
            Pointer pointer,
            variable_opts opts,
            Policy = Policy{});
    private:
        detail::class_type_data_ptr data_;
    };
}

namespace meta_hpp
{
    template < detail::enum_kind Enum >
    class enum_bind final {
    public:
        explicit enum_bind(type_opts opts);
        operator enum_type() const noexcept;

        enum_bind& evalue_(std::string name, Enum value);
        enum_bind& evalue_(std::string name, Enum value, evalue_opts opts);
    private:
        detail::enum_type_data_ptr data_;
    };
}

namespace meta_hpp
{
    template < detail::function_kind Function >
    class function_bind final {
    public:
        explicit function_bind(type_opts opts);
        operator function_type() const noexcept;
    private:
        detail::function_type_data_ptr data_;
    };
}

namespace meta_hpp
{
    template < detail::member_kind Member >
    class member_bind final {
    public:
        explicit member_bind(type_opts opts);
        operator member_type() const noexcept;
    private:
        detail::member_type_data_ptr data_;
    };
}

namespace meta_hpp
{
    template < detail::method_kind Method >
    class method_bind final {
    public:
        explicit method_bind(type_opts opts);
        operator method_type() const noexcept;
    private:
        detail::method_type_data_ptr data_;
    };
}

namespace meta_hpp
{
    template < detail::nullptr_kind Nullptr >
    class nullptr_bind final {
    public:
        explicit nullptr_bind(type_opts opts);
        operator nullptr_type() const noexcept;
    private:
        detail::nullptr_type_data_ptr data_;
    };
}

namespace meta_hpp
{
    template < detail::number_kind Number >
    class number_bind final {
    public:
        explicit number_bind(type_opts opts);
        operator number_type() const noexcept;
    private:
        detail::number_type_data_ptr data_;
    };
}

namespace meta_hpp
{
    template < detail::pointer_kind Pointer >
    class pointer_bind final {
    public:
        explicit pointer_bind(type_opts opts);
        operator pointer_type() const noexcept;
    private:
        detail::pointer_type_data_ptr data_;
    };
}

namespace meta_hpp
{
    template < detail::reference_kind Reference >
    class reference_bind final {
    public:
        explicit reference_bind(type_opts opts);
        operator reference_type() const noexcept;
    private:
        detail::reference_type_data_ptr data_;
    };
}

namespace meta_hpp
{
    template < detail::void_kind Void >
    class void_bind final {
    public:
        explicit void_bind(type_opts opts);
        operator void_type() const noexcept;
    private:
        detail::void_type_data_ptr data_;
    };
}

namespace meta_hpp
{
    class scope_bind final {
    public:
        struct local_tag {};
        struct static_tag {};

        explicit scope_bind(std::string name, scope_opts opts, local_tag);
        explicit scope_bind(std::string_view name, scope_opts opts, static_tag);
        operator scope() const noexcept;

        template < detail::class_kind Class >
        scope_bind& class_(std::string name);

        template < detail::enum_kind Enum >
        scope_bind& enum_(std::string name);

        // function_

        template < detail::function_kind Function
                 , function_policy_kind Policy = function_policy::as_copy >
        scope_bind& function_(
            std::string name,
            Function function,
            Policy = Policy{});

        template < detail::function_kind Function
                 , function_policy_kind Policy = function_policy::as_copy >
        scope_bind& function_(
            std::string name,
            Function function,
            function_opts opts,
            Policy = Policy{});

        template < detail::function_kind Function
                 , function_policy_kind Policy = function_policy::as_copy >
        scope_bind& function_(
            std::string name,
            Function function,
            std::initializer_list<std::string_view> arguments,
            Policy = Policy{});

        // variable_

        template < detail::pointer_kind Pointer
                 , variable_policy_kind Policy = variable_policy::as_copy >
        scope_bind& variable_(
            std::string name,
            Pointer pointer,
            Policy = Policy{});

        template < detail::pointer_kind Pointer
                 , variable_policy_kind Policy = variable_policy::as_copy >
        scope_bind& variable_(
            std::string name,
            Pointer pointer,
            variable_opts opts,
            Policy = Policy{});
    private:
        detail::scope_state_ptr state_;
    };
}

namespace meta_hpp
{
    template < detail::array_kind Array >
    array_bind<Array> array_(type_opts opts = {}) {
        return array_bind<Array>{std::move(opts)};
    }

    template < detail::class_kind Class >
    class_bind<Class> class_(type_opts opts = {}) {
        return class_bind<Class>{std::move(opts)};
    }

    template < detail::enum_kind Enum >
    enum_bind<Enum> enum_(type_opts opts = {}) {
        return enum_bind<Enum>{std::move(opts)};
    }

    template < detail::function_kind Function >
    function_bind<Function> function_(type_opts opts = {}) {
        return function_bind<Function>{std::move(opts)};
    }

    template < detail::member_kind Member >
    member_bind<Member> member_(type_opts opts = {}) {
        return member_bind<Member>{std::move(opts)};
    }

    template < detail::method_kind Method >
    method_bind<Method> method_(type_opts opts = {}) {
        return method_bind<Method>{std::move(opts)};
    }

    template < detail::nullptr_kind Nullptr >
    nullptr_bind<Nullptr> nullptr_(type_opts opts = {}) {
        return nullptr_bind<Nullptr>{std::move(opts)};
    }

    template < detail::number_kind Number >
    number_bind<Number> number_(type_opts opts = {}) {
        return number_bind<Number>{std::move(opts)};
    }

    template < detail::pointer_kind Pointer >
    pointer_bind<Pointer> pointer_(type_opts opts = {}) {
        return pointer_bind<Pointer>{std::move(opts)};
    }

    template < detail::reference_kind Reference >
    reference_bind<Reference> reference_(type_opts opts = {}) {
        return reference_bind<Reference>{std::move(opts)};
    }

    template < detail::void_kind Void >
    void_bind<Void> void_(type_opts opts = {}) {
        return void_bind<Void>{std::move(opts)};
    }
}

namespace meta_hpp
{
    inline scope_bind local_scope_(std::string name, scope_opts opts = {}) {
        return scope_bind{std::move(name), std::move(opts), scope_bind::local_tag()};
    }

    inline scope_bind static_scope_(std::string_view name, scope_opts opts = {}) {
        return scope_bind{name, std::move(opts), scope_bind::static_tag()};
    }
}
