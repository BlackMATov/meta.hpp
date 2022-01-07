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
    namespace detail
    {
        template < typename T >
        concept state_family =
            std::is_same_v<T, ctor> ||
            std::is_same_v<T, evalue> ||
            std::is_same_v<T, function> ||
            std::is_same_v<T, member> ||
            std::is_same_v<T, method> ||
            std::is_same_v<T, scope> ||
            std::is_same_v<T, variable>;
    }

    template < detail::state_family T, detail::state_family U >
    [[nodiscard]] bool operator<(const T& l, const U& r) noexcept {
        if ( !static_cast<bool>(r) ) {
            return false;
        }

        if ( !static_cast<bool>(l) ) {
            return true;
        }

        return l.get_index() < r.get_index();
    }

    template < detail::state_family T, detail::state_family U >
    [[nodiscard]] bool operator==(const T& l, const U& r) noexcept {
        if ( static_cast<bool>(l) != static_cast<bool>(r) ) {
            return false;
        }

        if ( !static_cast<bool>(l) ) {
            return true;
        }

        return l.get_index() == r.get_index();
    }

    template < detail::state_family T, detail::state_family U >
    [[nodiscard]] bool operator!=(const T& l, const U& r) noexcept {
        return !(l == r);
    }
}

namespace meta_hpp
{
    class ctor final {
    public:
        explicit ctor() = default;
        explicit ctor(detail::ctor_state_ptr state);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] const ctor_index& get_index() const noexcept;
        [[nodiscard]] const ctor_type& get_type() const noexcept;

        template < typename... Args >
        value invoke(Args&&... args) const;

        template < typename... Args >
        value operator()(Args&&... args) const;

        template < typename... Args >
        [[nodiscard]] bool is_invocable_with() const noexcept;

        template < typename... Args >
        [[nodiscard]] bool is_invocable_with(Args&&... args) const noexcept;
    private:
        detail::ctor_state_ptr state_;
    };

    class evalue final {
    public:
        explicit evalue() = default;
        explicit evalue(detail::evalue_state_ptr state);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] const evalue_index& get_index() const noexcept;
        [[nodiscard]] const enum_type& get_type() const noexcept;
        [[nodiscard]] const std::string& get_name() const noexcept;

        [[nodiscard]] const value& get_value() const noexcept;
        [[nodiscard]] const value& get_underlying_value() const noexcept;
    private:
        detail::evalue_state_ptr state_;
    };

    class function final {
    public:
        explicit function() = default;
        explicit function(detail::function_state_ptr state);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] const function_index& get_index() const noexcept;
        [[nodiscard]] const function_type& get_type() const noexcept;
        [[nodiscard]] const std::string& get_name() const noexcept;

        template < typename... Args >
        std::optional<value> invoke(Args&&... args) const;

        template < typename... Args >
        std::optional<value> operator()(Args&&... args) const;

        template < typename... Args >
        [[nodiscard]] bool is_invocable_with() const noexcept;

        template < typename... Args >
        [[nodiscard]] bool is_invocable_with(Args&&... args) const noexcept;
    private:
        detail::function_state_ptr state_;
    };

    class member final {
    public:
        explicit member() = default;
        explicit member(detail::member_state_ptr state);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] const member_index& get_index() const noexcept;
        [[nodiscard]] const member_type& get_type() const noexcept;
        [[nodiscard]] const std::string& get_name() const noexcept;

        template < typename Instance >
        [[nodiscard]] value get(Instance&& instance) const;

        template < typename Instance, typename Value >
        void set(Instance&& instance, Value&& value) const;
    private:
        detail::member_state_ptr state_;
    };

    class method final {
    public:
        explicit method() = default;
        explicit method(detail::method_state_ptr state);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] const method_index& get_index() const noexcept;
        [[nodiscard]] const method_type& get_type() const noexcept;
        [[nodiscard]] const std::string& get_name() const noexcept;

        template < typename Instance, typename... Args >
        std::optional<value> invoke(Instance&& instance, Args&&... args) const;

        template < typename Instance, typename... Args >
        std::optional<value> operator()(Instance&& instance, Args&&... args) const;

        template < typename Instance, typename... Args >
        [[nodiscard]] bool is_invocable_with() const noexcept;

        template < typename Instance, typename... Args >
        [[nodiscard]] bool is_invocable_with(Instance&& instance, Args&&... args) const noexcept;
    private:
        detail::method_state_ptr state_;
    };

    class scope final {
    public:
        explicit scope() = default;
        explicit scope(detail::scope_state_ptr state);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] const scope_index& get_index() const noexcept;
        [[nodiscard]] const std::string& get_name() const noexcept;

        [[nodiscard]] const class_map& get_classes() const noexcept;
        [[nodiscard]] const enum_map& get_enums() const noexcept;
        [[nodiscard]] const function_map& get_functions() const noexcept;
        [[nodiscard]] const variable_map& get_variables() const noexcept;

        [[nodiscard]] class_type get_class(std::string_view name) const noexcept;
        [[nodiscard]] enum_type get_enum(std::string_view name) const noexcept;
        [[nodiscard]] function get_function(std::string_view name) const noexcept;
        [[nodiscard]] variable get_variable(std::string_view name) const noexcept;

        template < typename... Args >
        [[nodiscard]] function get_function_with(std::string_view name) const noexcept;
        [[nodiscard]] function get_function_with(std::string_view name, const std::vector<any_type>& args) const noexcept;
        [[nodiscard]] function get_function_with(std::string_view name, std::initializer_list<any_type> args) const noexcept;
    private:
        detail::scope_state_ptr state_;
    };

    class variable final {
    public:
        explicit variable() = default;
        explicit variable(detail::variable_state_ptr state);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] const variable_index& get_index() const noexcept;
        [[nodiscard]] const pointer_type& get_type() const noexcept;
        [[nodiscard]] const std::string& get_name() const noexcept;

        [[nodiscard]] value get() const;

        template < typename Value >
        void set(Value&& value) const;
    private:
        detail::variable_state_ptr state_;
    };
}

namespace meta_hpp::detail
{
    struct ctor_state final {
        using invoke_impl = std::function<value(std::span<arg>)>;
        using is_invocable_with_impl = std::function<bool(std::span<arg_base>)>;

        const ctor_index index;
        const invoke_impl invoke;
        const is_invocable_with_impl is_invocable_with;

        template < class_kind Class, typename... Args >
        explicit ctor_state(ctor_index index, type_list<Class>, type_list<Args...>);

        template < class_kind Class, typename... Args >
        [[nodiscard]] static ctor_state_ptr make();
    };

    struct evalue_state final {
        const evalue_index index;
        const value enum_value;
        const value underlying_value;

        template < enum_kind Enum >
        explicit evalue_state(evalue_index index, Enum value);

        template < enum_kind Enum >
        [[nodiscard]] static evalue_state_ptr make(std::string name, Enum value);
    };

    struct function_state final {
        using invoke_impl = std::function<std::optional<value>(std::span<arg>)>;
        using is_invocable_with_impl = std::function<bool(std::span<arg_base>)>;

        const function_index index;
        const invoke_impl invoke;
        const is_invocable_with_impl is_invocable_with;

        template < function_kind Function >
        explicit function_state(function_index index, Function function);

        template < function_kind Function >
        [[nodiscard]] static function_state_ptr make(std::string name, Function function);
    };

    struct member_state final {
        using getter_impl = std::function<value(const inst&)>;
        using setter_impl = std::function<void(const inst&, const arg&)>;

        const member_index index;
        const getter_impl getter;
        const setter_impl setter;

        template < member_kind Member >
        explicit member_state(member_index index, Member member);

        template < member_kind Member >
        [[nodiscard]] static member_state_ptr make(std::string name, Member member);
    };

    struct method_state final {
        using invoke_impl = std::function<std::optional<value>(const inst&, std::span<arg>)>;
        using is_invocable_with_impl = std::function<bool(const inst_base&, std::span<arg_base>)>;

        const method_index index;
        const invoke_impl invoke;
        const is_invocable_with_impl is_invocable_with;

        template < method_kind Method >
        explicit method_state(method_index index, Method method);

        template < method_kind Method >
        [[nodiscard]] static method_state_ptr make(std::string name, Method method);
    };

    struct scope_state final {
        const scope_index index;

        class_map classes;
        enum_map enums;
        function_map functions;
        variable_map variables;

        explicit scope_state(scope_index index);

        [[nodiscard]] static scope_state_ptr make(std::string name);
        [[nodiscard]] static scope_state_ptr get_static(std::string_view name);
    };

    struct variable_state final {
        using getter_impl = std::function<value()>;
        using setter_impl = std::function<void(const arg&)>;

        const variable_index index;
        const getter_impl getter;
        const setter_impl setter;

        template < pointer_kind Pointer >
        explicit variable_state(variable_index index, Pointer pointer);

        template < pointer_kind Pointer >
        [[nodiscard]] static variable_state_ptr make(std::string name, Pointer pointer);
    };
}

namespace meta_hpp
{
    [[nodiscard]] inline scope resolve_scope(std::string_view name) {
        return scope{detail::scope_state::get_static(name)};
    }
}
