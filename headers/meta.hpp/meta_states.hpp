/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_base.hpp"
#include "meta_indices.hpp"
#include "meta_types.hpp"
#include "meta_value.hpp"

#include "meta_detail/state_family.hpp"

namespace meta_hpp
{
    namespace ctor_policy
    {
        struct as_object final {};
        struct as_raw_pointer final {};
        struct as_shared_pointer final {};
    }

    namespace function_policy
    {
        struct as_copy final {};
        struct discard_return final {};
        struct return_reference_as_pointer final {};
    }

    namespace member_policy
    {
        struct as_copy final {};
        struct as_pointer final {};
        struct as_reference_wrapper final {};
    }

    namespace method_policy
    {
        struct as_copy final {};
        struct discard_return final {};
        struct return_reference_as_pointer final {};
    }

    namespace variable_policy
    {
        struct as_copy final {};
        struct as_pointer final {};
        struct as_reference_wrapper final {};
    }

    template < typename Policy >
    concept ctor_policy_kind =
        stdex::same_as<Policy, ctor_policy::as_object> ||
        stdex::same_as<Policy, ctor_policy::as_raw_pointer> ||
        stdex::same_as<Policy, ctor_policy::as_shared_pointer>;

    template < typename Policy >
    concept function_policy_kind =
        stdex::same_as<Policy, function_policy::as_copy> ||
        stdex::same_as<Policy, function_policy::discard_return> ||
        stdex::same_as<Policy, function_policy::return_reference_as_pointer>;

    template < typename Policy >
    concept member_policy_kind =
        stdex::same_as<Policy, member_policy::as_copy> ||
        stdex::same_as<Policy, member_policy::as_pointer> ||
        stdex::same_as<Policy, member_policy::as_reference_wrapper>;

    template < typename Policy >
    concept method_policy_kind =
        stdex::same_as<Policy, method_policy::as_copy> ||
        stdex::same_as<Policy, method_policy::discard_return> ||
        stdex::same_as<Policy, method_policy::return_reference_as_pointer>;

    template < typename Policy >
    concept variable_policy_kind =
        stdex::same_as<Policy, variable_policy::as_copy> ||
        stdex::same_as<Policy, variable_policy::as_pointer> ||
        stdex::same_as<Policy, variable_policy::as_reference_wrapper>;
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
        friend auto detail::state_access<ctor>(const ctor&);
    };

    class dtor final {
    public:
        explicit dtor() = default;
        explicit dtor(detail::dtor_state_ptr state);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] const dtor_index& get_index() const noexcept;
        [[nodiscard]] const dtor_type& get_type() const noexcept;

        template < typename Arg >
        void invoke(Arg&& ptr) const;

        template < typename Arg >
        void operator()(Arg&& ptr) const;

        template < typename Arg >
        [[nodiscard]] bool is_invocable_with() const noexcept;

        template < typename Arg >
        [[nodiscard]] bool is_invocable_with(Arg&& ptr) const noexcept;
    private:
        detail::dtor_state_ptr state_;
        friend auto detail::state_access<dtor>(const dtor&);
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
        friend auto detail::state_access<evalue>(const evalue&);
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
        value invoke(Args&&... args) const;

        template < typename... Args >
        value operator()(Args&&... args) const;

        template < typename... Args >
        [[nodiscard]] bool is_invocable_with() const noexcept;

        template < typename... Args >
        [[nodiscard]] bool is_invocable_with(Args&&... args) const noexcept;
    private:
        detail::function_state_ptr state_;
        friend auto detail::state_access<function>(const function&);
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

        template < typename Instance >
        [[nodiscard]] value operator()(Instance&& instance) const;

        template < typename Instance, typename Value >
        void operator()(Instance&& instance, Value&& value) const;

        template < typename Instance >
        [[nodiscard]] bool is_gettable_with() const noexcept;

        template < typename Instance >
        [[nodiscard]] bool is_gettable_with(Instance&& instance) const noexcept;

        template < typename Instance, typename Value >
        [[nodiscard]] bool is_settable_with() const noexcept;

        template < typename Instance, typename Value >
        [[nodiscard]] bool is_settable_with(Instance&& instance, Value&& value) const noexcept;
    private:
        detail::member_state_ptr state_;
        friend auto detail::state_access<member>(const member&);
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
        value invoke(Instance&& instance, Args&&... args) const;

        template < typename Instance, typename... Args >
        value operator()(Instance&& instance, Args&&... args) const;

        template < typename Instance, typename... Args >
        [[nodiscard]] bool is_invocable_with() const noexcept;

        template < typename Instance, typename... Args >
        [[nodiscard]] bool is_invocable_with(Instance&& instance, Args&&... args) const noexcept;
    private:
        detail::method_state_ptr state_;
        friend auto detail::state_access<method>(const method&);
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
        friend auto detail::state_access<scope>(const scope&);
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

        [[nodiscard]] value operator()() const;

        template < typename Value >
        void operator()(Value&& value) const;

        template < typename Value >
        [[nodiscard]] bool is_settable_with() const noexcept;

        template < typename Value >
        [[nodiscard]] bool is_settable_with(Value&& value) const noexcept;
    private:
        detail::variable_state_ptr state_;
        friend auto detail::state_access<variable>(const variable&);
    };
}

namespace meta_hpp
{
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

namespace meta_hpp::detail
{
    struct ctor_state final {
        using invoke_impl = std::function<value(std::span<const arg>)>;
        using is_invocable_with_impl = std::function<bool(std::span<const arg_base>)>;

        const ctor_index index;
        const invoke_impl invoke;
        const is_invocable_with_impl is_invocable_with;

        template < ctor_policy_kind Policy, class_kind Class, typename... Args >
        [[nodiscard]] static ctor_state_ptr make();
    };

    struct dtor_state final {
        using invoke_impl = std::function<void(const arg&)>;
        using is_invocable_with_impl = std::function<bool(const arg_base&)>;

        const dtor_index index;
        const invoke_impl invoke;
        const is_invocable_with_impl is_invocable_with;

        template < class_kind Class >
        [[nodiscard]] static dtor_state_ptr make();
    };

    struct evalue_state final {
        const evalue_index index;
        const value enum_value;
        const value underlying_value;

        template < enum_kind Enum >
        [[nodiscard]] static evalue_state_ptr make(std::string name, Enum evalue);
    };

    struct function_state final {
        using invoke_impl = std::function<value(std::span<const arg>)>;
        using is_invocable_with_impl = std::function<bool(std::span<const arg_base>)>;

        const function_index index;
        const invoke_impl invoke;
        const is_invocable_with_impl is_invocable_with;

        template < function_policy_kind Policy, function_kind Function >
        [[nodiscard]] static function_state_ptr make(std::string name, Function function);
    };

    struct member_state final {
        using getter_impl = std::function<value(const inst&)>;
        using setter_impl = std::function<void(const inst&, const arg&)>;

        using is_gettable_with_impl = std::function<bool(const inst_base&)>;
        using is_settable_with_impl = std::function<bool(const inst_base&, const arg_base&)>;

        const member_index index;
        const getter_impl getter;
        const setter_impl setter;
        const is_gettable_with_impl is_gettable_with;
        const is_settable_with_impl is_settable_with;

        template < member_policy_kind Policy, member_kind Member >
        [[nodiscard]] static member_state_ptr make(std::string name, Member member);
    };

    struct method_state final {
        using invoke_impl = std::function<value(const inst&, std::span<const arg>)>;
        using is_invocable_with_impl = std::function<bool(const inst_base&, std::span<const arg_base>)>;

        const method_index index;
        const invoke_impl invoke;
        const is_invocable_with_impl is_invocable_with;

        template < method_policy_kind Policy, method_kind Method >
        [[nodiscard]] static method_state_ptr make(std::string name, Method method);
    };

    struct scope_state final {
        const scope_index index;

        class_map classes{};
        enum_map enums{};
        function_map functions{};
        variable_map variables{};

        [[nodiscard]] static scope_state_ptr make(std::string name);
    };

    struct variable_state final {
        using getter_impl = std::function<value()>;
        using setter_impl = std::function<void(const arg&)>;
        using is_settable_with_impl = std::function<bool(const arg_base&)>;

        const variable_index index;
        const getter_impl getter;
        const setter_impl setter;
        const is_settable_with_impl is_settable_with;

        template < variable_policy_kind Policy, pointer_kind Pointer >
        [[nodiscard]] static variable_state_ptr make(std::string name, Pointer pointer);
    };
}
