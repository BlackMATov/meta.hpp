/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_base.hpp"
#include "meta_types.hpp"
#include "meta_utilities.hpp"

namespace meta_hpp
{
    template < typename T >
    concept states_family =
        std::same_as<T, ctor> ||
        std::same_as<T, evalue> ||
        std::same_as<T, function> ||
        std::same_as<T, member> ||
        std::same_as<T, method> ||
        std::same_as<T, scope> ||
        std::same_as<T, variable>;

    template < states_family T, states_family U >
    bool operator<(const T& l, const U& r) noexcept {
        if ( !static_cast<bool>(r) ) {
            return false;
        }

        if ( !static_cast<bool>(l) ) {
            return true;
        }

        return l.get_index() < r.get_index();
    }

    template < states_family T, states_family U >
    bool operator==(const T& l, const U& r) noexcept {
        if ( static_cast<bool>(l) != static_cast<bool>(r) ) {
            return false;
        }

        if ( !static_cast<bool>(l) ) {
            return true;
        }

        return l.get_index() == r.get_index();
    }

    template < states_family T, states_family U >
    bool operator!=(const T& l, const U& r) noexcept {
        return !(l == r);
    }
}

namespace meta_hpp
{
    class ctor final {
    public:
        explicit ctor() = default;
        explicit ctor(detail::ctor_state_ptr state);

        bool is_valid() const noexcept;
        explicit operator bool() const noexcept;

        const ctor_index& get_index() const noexcept;
        const ctor_type& get_type() const noexcept;

        template < typename... Args >
        value invoke(Args&&... args) const;

        template < typename... Args >
        bool is_invocable_with() const noexcept;
    private:
        detail::ctor_state_ptr state_;
    };

    class evalue final {
    public:
        explicit evalue() = default;
        explicit evalue(detail::evalue_state_ptr state);

        bool is_valid() const noexcept;
        explicit operator bool() const noexcept;

        const evalue_index& get_index() const noexcept;
        const enum_type& get_type() const noexcept;
        const std::string& get_name() const noexcept;

        const value& get_value() const noexcept;
    private:
        detail::evalue_state_ptr state_;
    };

    class function final {
    public:
        explicit function() = default;
        explicit function(detail::function_state_ptr state);

        bool is_valid() const noexcept;
        explicit operator bool() const noexcept;

        const function_index& get_index() const noexcept;
        const function_type& get_type() const noexcept;
        const std::string& get_name() const noexcept;

        template < typename... Args >
        std::optional<value> invoke(Args&&... args) const;

        template < typename... Args >
        bool is_invocable_with() const noexcept;
    private:
        detail::function_state_ptr state_;
    };

    class member final {
    public:
        explicit member() = default;
        explicit member(detail::member_state_ptr state);

        bool is_valid() const noexcept;
        explicit operator bool() const noexcept;

        const member_index& get_index() const noexcept;
        const member_type& get_type() const noexcept;
        const std::string& get_name() const noexcept;

        template < typename Instance >
        value get(Instance&& instance) const;

        template < typename Instance, typename Value >
        void set(Instance&& instance, Value&& value) const;
    private:
        detail::member_state_ptr state_;
    };

    class method final {
    public:
        explicit method() = default;
        explicit method(detail::method_state_ptr state);

        bool is_valid() const noexcept;
        explicit operator bool() const noexcept;

        const method_index& get_index() const noexcept;
        const method_type& get_type() const noexcept;
        const std::string& get_name() const noexcept;

        template < typename Instance, typename... Args >
        std::optional<value> invoke(Instance&& instance, Args&&... args) const;

        template < typename Inst, typename... Args >
        bool is_invocable_with() const noexcept;
    private:
        detail::method_state_ptr state_;
    };

    class scope final {
    public:
        explicit scope() = default;
        explicit scope(detail::scope_state_ptr state);

        bool is_valid() const noexcept;
        explicit operator bool() const noexcept;

        const scope_index& get_index() const noexcept;
        const std::string& get_name() const noexcept;

        const class_map& get_classes() const noexcept;
        const enum_map& get_enums() const noexcept;
        const function_map& get_functions() const noexcept;
        const variable_map& get_variables() const noexcept;

        class_type get_class(std::string_view name) const noexcept;
        enum_type get_enum(std::string_view name) const noexcept;
        function get_function(std::string_view name) const noexcept;
        variable get_variable(std::string_view name) const noexcept;

        template < typename... Args >
        function get_function_with(std::string_view name) const noexcept;
        function get_function_with(std::string_view name, std::vector<any_type> args) const noexcept;
        function get_function_with(std::string_view name, std::initializer_list<any_type> args) const noexcept;
    private:
        detail::scope_state_ptr state_;
    };

    class variable final {
    public:
        explicit variable() = default;
        explicit variable(detail::variable_state_ptr state);

        bool is_valid() const noexcept;
        explicit operator bool() const noexcept;

        const variable_index& get_index() const noexcept;
        const pointer_type& get_type() const noexcept;
        const std::string& get_name() const noexcept;

        value get() const;

        template < typename Value >
        void set(Value&& value) const;
    private:
        detail::variable_state_ptr state_;
    };
}

namespace meta_hpp::detail
{
    struct ctor_state final {
        using invoke_impl = std::function<value(const arg*, std::size_t)>;
        using is_invocable_with_impl = std::function<bool(const arg_base*, std::size_t)>;

        ctor_index index;

        invoke_impl invoke;
        is_invocable_with_impl is_invocable_with;

        template < class_kind Class, typename... Args >
        static ctor_state_ptr make();

        template < class_kind Class, typename... Args >
        explicit ctor_state(ctor_index index, type_list<Class>, type_list<Args...>);
    };

    struct evalue_state final {
        evalue_index index;

        class value value;

        template < enum_kind Enum >
        static evalue_state_ptr make(std::string name, Enum value);

        template < enum_kind Enum >
        explicit evalue_state(evalue_index index, Enum value);
    };

    struct function_state final {
        using invoke_impl = std::function<std::optional<value>(const arg*, std::size_t)>;
        using is_invocable_with_impl = std::function<bool(const arg_base*, std::size_t)>;

        function_index index;

        invoke_impl invoke;
        is_invocable_with_impl is_invocable_with;

        template < function_kind Function >
        static function_state_ptr make(std::string name, Function function);

        template < function_kind Function >
        explicit function_state(function_index index, Function function);
    };

    struct member_state final {
        using getter_impl = std::function<value(const inst&)>;
        using setter_impl = std::function<void(const inst&, const arg&)>;

        member_index index;

        getter_impl getter;
        setter_impl setter;

        template < member_kind Member >
        static member_state_ptr make(std::string name, Member member);

        template < member_kind Member >
        explicit member_state(member_index index, Member member);
    };

    struct method_state final {
        using invoke_impl = std::function<std::optional<value>(const inst&, const arg*, std::size_t)>;
        using is_invocable_with_impl = std::function<bool(const inst_base&, const arg_base*, std::size_t)>;

        method_index index;

        invoke_impl invoke;
        is_invocable_with_impl is_invocable_with;

        template < method_kind Method >
        static method_state_ptr make(std::string name, Method method);

        template < method_kind Method >
        explicit method_state(method_index index, Method method);
    };

    struct scope_state final {
        scope_index index;

        class_map classes;
        enum_map enums;
        function_map functions;
        variable_map variables;

        static scope_state_ptr get(std::string_view name);

        explicit scope_state(scope_index index);
    };

    struct variable_state final {
        using getter_impl = std::function<value()>;
        using setter_impl = std::function<void(const arg&)>;

        variable_index index;

        getter_impl getter;
        setter_impl setter;

        template < pointer_kind Pointer >
        static variable_state_ptr make(std::string name, Pointer pointer);

        template < pointer_kind Pointer >
        explicit variable_state(variable_index index, Pointer pointer);
    };
}

namespace meta_hpp
{
    namespace detail
    {
        inline scope_state_ptr get_scope_state(std::string_view name) {
            return scope_state::get(name);
        }
    }

    inline scope resolve_scope(std::string_view name) {
        return scope{detail::get_scope_state(name)};
    }
}
