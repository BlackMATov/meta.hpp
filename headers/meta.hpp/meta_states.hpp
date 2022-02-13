/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_base.hpp"
#include "meta_indices.hpp"
#include "meta_types.hpp"
#include "meta_value.hpp"

#include "meta_detail/state_family.hpp"

namespace meta_hpp
{
    namespace constructor_policy
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
    inline constexpr bool is_constructor_policy_v =
        std::is_same_v<Policy, constructor_policy::as_object> ||
        std::is_same_v<Policy, constructor_policy::as_raw_pointer> ||
        std::is_same_v<Policy, constructor_policy::as_shared_pointer>;

    template < typename Policy >
    inline constexpr bool is_function_policy_v =
        std::is_same_v<Policy, function_policy::as_copy> ||
        std::is_same_v<Policy, function_policy::discard_return> ||
        std::is_same_v<Policy, function_policy::return_reference_as_pointer>;

    template < typename Policy >
    inline constexpr bool is_member_policy_v =
        std::is_same_v<Policy, member_policy::as_copy> ||
        std::is_same_v<Policy, member_policy::as_pointer> ||
        std::is_same_v<Policy, member_policy::as_reference_wrapper>;

    template < typename Policy >
    inline constexpr bool is_method_policy_v =
        std::is_same_v<Policy, method_policy::as_copy> ||
        std::is_same_v<Policy, method_policy::discard_return> ||
        std::is_same_v<Policy, method_policy::return_reference_as_pointer>;

    template < typename Policy >
    inline constexpr bool is_variable_policy_v =
        std::is_same_v<Policy, variable_policy::as_copy> ||
        std::is_same_v<Policy, variable_policy::as_pointer> ||
        std::is_same_v<Policy, variable_policy::as_reference_wrapper>;

    template < typename Policy >
    concept constructor_policy_kind = is_constructor_policy_v<Policy>;

    template < typename Policy >
    concept function_policy_kind = is_function_policy_v<Policy>;

    template < typename Policy >
    concept member_policy_kind = is_member_policy_v<Policy>;

    template < typename Policy >
    concept method_policy_kind = is_method_policy_v<Policy>;

    template < typename Policy >
    concept variable_policy_kind = is_variable_policy_v<Policy>;
}

namespace meta_hpp
{
    class argument final {
    public:
        explicit argument() = default;
        explicit argument(detail::argument_state_ptr state) noexcept;
        argument& operator=(detail::argument_state_ptr state) noexcept;

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] const argument_index& get_index() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

        [[nodiscard]] const any_type& get_type() const noexcept;
        [[nodiscard]] std::size_t get_position() const noexcept;

        [[nodiscard]] const std::string& get_name() const noexcept;
    private:
        detail::argument_state_ptr state_;
        friend auto detail::state_access<argument>(const argument&);
    };

    class constructor final {
    public:
        explicit constructor() = default;
        explicit constructor(detail::constructor_state_ptr state) noexcept;
        constructor& operator=(detail::constructor_state_ptr state) noexcept;

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] const constructor_index& get_index() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

        [[nodiscard]] const constructor_type& get_type() const noexcept;

        template < typename... Args >
        uvalue invoke(Args&&... args) const;

        template < typename... Args >
        uvalue operator()(Args&&... args) const;

        template < typename... Args >
        [[nodiscard]] bool is_invocable_with() const noexcept;

        template < typename... Args >
        [[nodiscard]] bool is_invocable_with(Args&&... args) const noexcept;

        [[nodiscard]] argument get_argument(std::size_t position) const noexcept;
        [[nodiscard]] const argument_list& get_arguments() const noexcept;
    private:
        detail::constructor_state_ptr state_;
        friend auto detail::state_access<constructor>(const constructor&);
    };

    class destructor final {
    public:
        explicit destructor() = default;
        explicit destructor(detail::destructor_state_ptr state) noexcept;
        destructor& operator=(detail::destructor_state_ptr state) noexcept;

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] const destructor_index& get_index() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

        [[nodiscard]] const destructor_type& get_type() const noexcept;

        template < typename Arg >
        void invoke(Arg&& ptr) const;

        template < typename Arg >
        void operator()(Arg&& ptr) const;

        template < typename Arg >
        [[nodiscard]] bool is_invocable_with() const noexcept;

        template < typename Arg >
        [[nodiscard]] bool is_invocable_with(Arg&& ptr) const noexcept;
    private:
        detail::destructor_state_ptr state_;
        friend auto detail::state_access<destructor>(const destructor&);
    };

    class evalue final {
    public:
        explicit evalue() = default;
        explicit evalue(detail::evalue_state_ptr state) noexcept;
        evalue& operator=(detail::evalue_state_ptr state) noexcept;

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] const evalue_index& get_index() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

        [[nodiscard]] const enum_type& get_type() const noexcept;
        [[nodiscard]] const std::string& get_name() const noexcept;

        [[nodiscard]] const uvalue& get_value() const noexcept;
        [[nodiscard]] const uvalue& get_underlying_value() const noexcept;
    private:
        detail::evalue_state_ptr state_;
        friend auto detail::state_access<evalue>(const evalue&);
    };

    class function final {
    public:
        explicit function() = default;
        explicit function(detail::function_state_ptr state) noexcept;
        function& operator=(detail::function_state_ptr state) noexcept;

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] const function_index& get_index() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

        [[nodiscard]] const function_type& get_type() const noexcept;
        [[nodiscard]] const std::string& get_name() const noexcept;

        template < typename... Args >
        uvalue invoke(Args&&... args) const;

        template < typename... Args >
        uvalue operator()(Args&&... args) const;

        template < typename... Args >
        [[nodiscard]] bool is_invocable_with() const noexcept;

        template < typename... Args >
        [[nodiscard]] bool is_invocable_with(Args&&... args) const noexcept;

        [[nodiscard]] argument get_argument(std::size_t position) const noexcept;
        [[nodiscard]] const argument_list& get_arguments() const noexcept;
    private:
        detail::function_state_ptr state_;
        friend auto detail::state_access<function>(const function&);
    };

    class member final {
    public:
        explicit member() = default;
        explicit member(detail::member_state_ptr state) noexcept;
        member& operator=(detail::member_state_ptr state) noexcept;

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] const member_index& get_index() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

        [[nodiscard]] const member_type& get_type() const noexcept;
        [[nodiscard]] const std::string& get_name() const noexcept;

        template < typename Instance >
        [[nodiscard]] uvalue get(Instance&& instance) const;

        template < typename Instance, typename Value >
        void set(Instance&& instance, Value&& value) const;

        template < typename Instance >
        [[nodiscard]] uvalue operator()(Instance&& instance) const;

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
        explicit method(detail::method_state_ptr state) noexcept;
        method& operator=(detail::method_state_ptr state) noexcept;

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] const method_index& get_index() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

        [[nodiscard]] const method_type& get_type() const noexcept;
        [[nodiscard]] const std::string& get_name() const noexcept;

        template < typename Instance, typename... Args >
        uvalue invoke(Instance&& instance, Args&&... args) const;

        template < typename Instance, typename... Args >
        uvalue operator()(Instance&& instance, Args&&... args) const;

        template < typename Instance, typename... Args >
        [[nodiscard]] bool is_invocable_with() const noexcept;

        template < typename Instance, typename... Args >
        [[nodiscard]] bool is_invocable_with(Instance&& instance, Args&&... args) const noexcept;

        [[nodiscard]] argument get_argument(std::size_t position) const noexcept;
        [[nodiscard]] const argument_list& get_arguments() const noexcept;
    private:
        detail::method_state_ptr state_;
        friend auto detail::state_access<method>(const method&);
    };

    class scope final {
    public:
        explicit scope() = default;
        explicit scope(detail::scope_state_ptr state) noexcept;
        scope& operator=(detail::scope_state_ptr state) noexcept;

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] const scope_index& get_index() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

        [[nodiscard]] const std::string& get_name() const noexcept;

        [[nodiscard]] const function_map& get_functions() const noexcept;
        [[nodiscard]] const typedef_map& get_typedefs() const noexcept;
        [[nodiscard]] const variable_map& get_variables() const noexcept;

        [[nodiscard]] function get_function(std::string_view name) const noexcept;
        [[nodiscard]] any_type get_typedef(std::string_view name) const noexcept;
        [[nodiscard]] variable get_variable(std::string_view name) const noexcept;

        template < typename... Args >
        [[nodiscard]] function get_function_with(std::string_view name) const noexcept;
        template < typename Iter >
        [[nodiscard]] function get_function_with(std::string_view name, Iter first, Iter last) const noexcept;
        [[nodiscard]] function get_function_with(std::string_view name, const std::vector<any_type>& args) const noexcept;
        [[nodiscard]] function get_function_with(std::string_view name, std::initializer_list<any_type> args) const noexcept;
    private:
        detail::scope_state_ptr state_;
        friend auto detail::state_access<scope>(const scope&);
    };

    class variable final {
    public:
        explicit variable() = default;
        explicit variable(detail::variable_state_ptr state) noexcept;
        variable& operator=(detail::variable_state_ptr state) noexcept;

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] const variable_index& get_index() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

        [[nodiscard]] const pointer_type& get_type() const noexcept;
        [[nodiscard]] const std::string& get_name() const noexcept;

        [[nodiscard]] uvalue get() const;

        template < typename Value >
        void set(Value&& value) const;

        [[nodiscard]] uvalue operator()() const;

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
    struct argument_state final {
        argument_index index;
        metadata_map metadata;

        std::string name{};

        template < typename Argument >
        [[nodiscard]] static argument_state_ptr make(std::size_t position, metadata_map metadata);
    };

    struct constructor_state final {
        using invoke_impl = fixed_function<uvalue(std::span<const uarg>)>;
        using is_invocable_with_impl = fixed_function<bool(std::span<const uarg_base>)>;

        constructor_index index;
        metadata_map metadata;

        invoke_impl invoke;
        is_invocable_with_impl is_invocable_with;

        argument_list arguments;

        template < constructor_policy_kind Policy, class_kind Class, typename... Args >
        [[nodiscard]] static constructor_state_ptr make(metadata_map metadata);
    };

    struct destructor_state final {
        using invoke_impl = fixed_function<void(const uarg&)>;
        using is_invocable_with_impl = fixed_function<bool(const uarg_base&)>;

        destructor_index index;
        metadata_map metadata;

        invoke_impl invoke;
        is_invocable_with_impl is_invocable_with;

        template < class_kind Class >
        [[nodiscard]] static destructor_state_ptr make(metadata_map metadata);
    };

    struct evalue_state final {
        evalue_index index;
        metadata_map metadata;

        uvalue enum_value;
        uvalue underlying_value;

        template < enum_kind Enum >
        [[nodiscard]] static evalue_state_ptr make(std::string name, Enum evalue, metadata_map metadata);
    };

    struct function_state final {
        using invoke_impl = fixed_function<uvalue(std::span<const uarg>)>;
        using is_invocable_with_impl = fixed_function<bool(std::span<const uarg_base>)>;

        function_index index;
        metadata_map metadata;

        invoke_impl invoke;
        is_invocable_with_impl is_invocable_with;

        argument_list arguments;

        template < function_policy_kind Policy, function_kind Function >
        [[nodiscard]] static function_state_ptr make(std::string name, Function function, metadata_map metadata);
    };

    struct member_state final {
        using getter_impl = fixed_function<uvalue(const uinst&)>;
        using setter_impl = fixed_function<void(const uinst&, const uarg&)>;

        using is_gettable_with_impl = fixed_function<bool(const uinst_base&)>;
        using is_settable_with_impl = fixed_function<bool(const uinst_base&, const uarg_base&)>;

        member_index index;
        metadata_map metadata;

        getter_impl getter;
        setter_impl setter;
        is_gettable_with_impl is_gettable_with;
        is_settable_with_impl is_settable_with;


        template < member_policy_kind Policy, member_kind Member >
        [[nodiscard]] static member_state_ptr make(std::string name, Member member, metadata_map metadata);
    };

    struct method_state final {
        using invoke_impl = fixed_function<uvalue(const uinst&, std::span<const uarg>)>;
        using is_invocable_with_impl = fixed_function<bool(const uinst_base&, std::span<const uarg_base>)>;

        method_index index;
        metadata_map metadata;

        invoke_impl invoke;
        is_invocable_with_impl is_invocable_with;

        argument_list arguments;

        template < method_policy_kind Policy, method_kind Method >
        [[nodiscard]] static method_state_ptr make(std::string name, Method method, metadata_map metadata);
    };

    struct scope_state final {
        scope_index index;
        metadata_map metadata;

        function_map functions{};
        typedef_map typedefs{};
        variable_map variables{};

        [[nodiscard]] static scope_state_ptr make(std::string name, metadata_map metadata);
    };

    struct variable_state final {
        using getter_impl = fixed_function<uvalue()>;
        using setter_impl = fixed_function<void(const uarg&)>;
        using is_settable_with_impl = fixed_function<bool(const uarg_base&)>;

        variable_index index;
        metadata_map metadata;

        getter_impl getter;
        setter_impl setter;
        is_settable_with_impl is_settable_with;

        template < variable_policy_kind Policy, pointer_kind Pointer >
        [[nodiscard]] static variable_state_ptr make(std::string name, Pointer pointer, metadata_map metadata);
    };
}
