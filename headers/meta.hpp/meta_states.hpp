/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_base.hpp"
#include "meta_indices.hpp"
#include "meta_types.hpp"
#include "meta_uvalue.hpp"

#include "meta_detail/state_family.hpp"

namespace meta_hpp
{
    namespace constructor_policy
    {
        struct as_object_t final {};

        struct as_raw_pointer_t final {};

        struct as_shared_pointer_t final {};

        inline constexpr as_object_t as_object{};
        inline constexpr as_raw_pointer_t as_raw_pointer{};
        inline constexpr as_shared_pointer_t as_shared_pointer{};
    }

    namespace function_policy
    {
        struct as_copy_t final {};

        struct discard_return_t final {};

        struct return_reference_as_pointer_t final {};

        inline constexpr as_copy_t as_copy{};
        inline constexpr discard_return_t discard_return{};
        inline constexpr return_reference_as_pointer_t return_reference_as_pointer{};
    }

    namespace member_policy
    {
        struct as_copy_t final {};

        struct as_pointer_t final {};

        struct as_reference_wrapper_t final {};

        inline constexpr as_copy_t as_copy{};
        inline constexpr as_pointer_t as_pointer{};
        inline constexpr as_reference_wrapper_t as_reference_wrapper{};
    }

    namespace method_policy
    {
        struct as_copy_t final {};

        struct discard_return_t final {};

        struct return_reference_as_pointer_t final {};

        inline constexpr as_copy_t as_copy{};
        inline constexpr discard_return_t discard_return{};
        inline constexpr return_reference_as_pointer_t return_reference_as_pointer{};
    }

    namespace variable_policy
    {
        struct as_copy_t final {};

        struct as_pointer_t final {};

        struct as_reference_wrapper_t final {};

        inline constexpr as_copy_t as_copy{};
        inline constexpr as_pointer_t as_pointer{};
        inline constexpr as_reference_wrapper_t as_reference_wrapper{};
    }

    template < typename Policy >
    concept constructor_policy_kind                                        //
        = std::is_same_v<Policy, constructor_policy::as_object_t>          //
       || std::is_same_v<Policy, constructor_policy::as_raw_pointer_t>     //
       || std::is_same_v<Policy, constructor_policy::as_shared_pointer_t>; //

    template < typename Policy >
    concept function_policy_kind                                                  //
        = std::is_same_v<Policy, function_policy::as_copy_t>                      //
       || std::is_same_v<Policy, function_policy::discard_return_t>               //
       || std::is_same_v<Policy, function_policy::return_reference_as_pointer_t>; //

    template < typename Policy >
    concept member_policy_kind                                           //
        = std::is_same_v<Policy, member_policy::as_copy_t>               //
       || std::is_same_v<Policy, member_policy::as_pointer_t>            //
       || std::is_same_v<Policy, member_policy::as_reference_wrapper_t>; //

    template < typename Policy >
    concept method_policy_kind                                                  //
        = std::is_same_v<Policy, method_policy::as_copy_t>                      //
       || std::is_same_v<Policy, method_policy::discard_return_t>               //
       || std::is_same_v<Policy, method_policy::return_reference_as_pointer_t>; //

    template < typename Policy >
    concept variable_policy_kind                                           //
        = std::is_same_v<Policy, variable_policy::as_copy_t>               //
       || std::is_same_v<Policy, variable_policy::as_pointer_t>            //
       || std::is_same_v<Policy, variable_policy::as_reference_wrapper_t>; //
}

namespace meta_hpp
{
    class argument final {
    public:
        using index_type = argument_index;
        using state_ptr = detail::argument_state_ptr;

        argument() = default;
        explicit argument(state_ptr state) noexcept;

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] const argument_index& get_index() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

        [[nodiscard]] const any_type& get_type() const noexcept;
        [[nodiscard]] std::size_t get_position() const noexcept;

        [[nodiscard]] const std::string& get_name() const noexcept;

    private:
        state_ptr state_;
        friend state_ptr detail::state_access<argument>(const argument&);
    };

    class constructor final {
    public:
        using index_type = constructor_index;
        using state_ptr = detail::constructor_state_ptr;

        constructor() = default;
        explicit constructor(state_ptr state) noexcept;

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] const constructor_index& get_index() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

        [[nodiscard]] const constructor_type& get_type() const noexcept;

        template < typename... Args >
        [[nodiscard]] uvalue create(Args&&... args) const;

        template < typename... Args >
        uvalue create_at(void* mem, Args&&... args) const;

        template < typename... Args >
        [[nodiscard]] bool is_invocable_with() const noexcept;

        template < typename... Args >
        [[nodiscard]] bool is_invocable_with(Args&&... args) const noexcept;

        [[nodiscard]] argument get_argument(std::size_t position) const noexcept;
        [[nodiscard]] const argument_list& get_arguments() const noexcept;

    private:
        state_ptr state_;
        friend state_ptr detail::state_access<constructor>(const constructor&);
    };

    class destructor final {
    public:
        using index_type = destructor_index;
        using state_ptr = detail::destructor_state_ptr;

        destructor() = default;
        explicit destructor(state_ptr state) noexcept;

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] const destructor_index& get_index() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

        [[nodiscard]] const destructor_type& get_type() const noexcept;

        template < typename Arg >
        bool destroy(Arg&& arg) const;

        void destroy_at(void* mem) const;

    private:
        state_ptr state_;
        friend state_ptr detail::state_access<destructor>(const destructor&);
    };

    class evalue final {
    public:
        using index_type = evalue_index;
        using state_ptr = detail::evalue_state_ptr;

        evalue() = default;
        explicit evalue(state_ptr state) noexcept;

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] const evalue_index& get_index() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

        [[nodiscard]] const enum_type& get_type() const noexcept;
        [[nodiscard]] const std::string& get_name() const noexcept;

        [[nodiscard]] const uvalue& get_value() const noexcept;
        [[nodiscard]] const uvalue& get_underlying_value() const noexcept;

        template < typename T >
        [[nodiscard]] T get_value_as() const;

        template < typename T >
        [[nodiscard]] T get_underlying_value_as() const;

    private:
        state_ptr state_;
        friend state_ptr detail::state_access<evalue>(const evalue&);
    };

    class function final {
    public:
        using index_type = function_index;
        using state_ptr = detail::function_state_ptr;

        function() = default;
        explicit function(state_ptr state) noexcept;

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
        state_ptr state_;
        friend state_ptr detail::state_access<function>(const function&);
    };

    class member final {
    public:
        using index_type = member_index;
        using state_ptr = detail::member_state_ptr;

        member() = default;
        explicit member(state_ptr state) noexcept;

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] const member_index& get_index() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

        [[nodiscard]] const member_type& get_type() const noexcept;
        [[nodiscard]] const std::string& get_name() const noexcept;

        template < typename Instance >
        [[nodiscard]] uvalue get(Instance&& instance) const;

        template < typename T, typename Instance >
        [[nodiscard]] T get_as(Instance&& instance) const;

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
        state_ptr state_;
        friend state_ptr detail::state_access<member>(const member&);
    };

    class method final {
    public:
        using index_type = method_index;
        using state_ptr = detail::method_state_ptr;

        method() = default;
        explicit method(state_ptr state) noexcept;

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
        state_ptr state_;
        friend state_ptr detail::state_access<method>(const method&);
    };

    class scope final {
    public:
        using index_type = scope_index;
        using state_ptr = detail::scope_state_ptr;

        scope() = default;
        explicit scope(state_ptr state) noexcept;

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] const scope_index& get_index() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

        [[nodiscard]] const std::string& get_name() const noexcept;

        [[nodiscard]] const function_set& get_functions() const noexcept;
        [[nodiscard]] const typedef_map& get_typedefs() const noexcept;
        [[nodiscard]] const variable_set& get_variables() const noexcept;

        [[nodiscard]] function get_function(std::string_view name) const noexcept;
        [[nodiscard]] any_type get_typedef(std::string_view name) const noexcept;
        [[nodiscard]] variable get_variable(std::string_view name) const noexcept;

        template < typename... Args >
        [[nodiscard]] function get_function_with(std::string_view name) const noexcept;
        template < typename Iter >
        [[nodiscard]] function get_function_with(std::string_view name, Iter first, Iter last) const noexcept;
        [[nodiscard]] function get_function_with(std::string_view name, std::span<const any_type> args) const noexcept;
        [[nodiscard]] function get_function_with(std::string_view name, std::initializer_list<any_type> args) const noexcept;

    private:
        state_ptr state_;
        friend state_ptr detail::state_access<scope>(const scope&);
    };

    class variable final {
    public:
        using index_type = variable_index;
        using state_ptr = detail::variable_state_ptr;

        variable() = default;
        explicit variable(state_ptr state) noexcept;

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] const variable_index& get_index() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

        [[nodiscard]] const pointer_type& get_type() const noexcept;
        [[nodiscard]] const std::string& get_name() const noexcept;

        [[nodiscard]] uvalue get() const;

        template < typename T >
        [[nodiscard]] T get_as() const;

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
        state_ptr state_;
        friend state_ptr detail::state_access<variable>(const variable&);
    };
}

namespace meta_hpp
{
    template < detail::state_family T, detail::state_family U >
    [[nodiscard]] bool operator==(const T& l, const U& r) noexcept {
        return l.is_valid() == r.is_valid() && (!l.is_valid() || l.get_index() == r.get_index());
    }

    template < detail::state_family T, detail::state_family U >
    [[nodiscard]] std::strong_ordering operator<=>(const T& l, const U& r) noexcept {
        if ( const std::strong_ordering cmp{l.is_valid() <=> r.is_valid()}; cmp != std::strong_ordering::equal ) {
            return cmp;
        }
        return l.is_valid() ? l.get_index() <=> r.get_index() : std::strong_ordering::equal;
    }
}

namespace meta_hpp
{
    template < detail::state_family T, typename U >
        requires std::is_same_v<U, typename T::index_type>
    [[nodiscard]] bool operator==(const T& l, const U& r) noexcept {
        return l.is_valid() && l.get_index() == r;
    }

    template < detail::state_family T, typename U >
        requires std::is_same_v<U, typename T::index_type>
    [[nodiscard]] std::strong_ordering operator<=>(const T& l, const U& r) noexcept {
        return l.is_valid() ? l.get_index() <=> r : std::strong_ordering::less;
    }
}

namespace meta_hpp::detail
{
    struct argument_state final : intrusive_ref_counter<argument_state> {
        argument_index index;
        metadata_map metadata;

        std::string name{};

        template < typename Argument >
        [[nodiscard]] static argument_state_ptr make(std::size_t position, metadata_map metadata);
        explicit argument_state(argument_index index, metadata_map metadata);
    };

    struct constructor_state final : intrusive_ref_counter<constructor_state> {
        using create_impl = fixed_function<uvalue(std::span<const uarg>)>;
        using create_at_impl = fixed_function<uvalue(void*, std::span<const uarg>)>;
        using is_invocable_with_impl = fixed_function<bool(std::span<const uarg_base>)>;

        constructor_index index;
        metadata_map metadata;

        create_impl create{};
        create_at_impl create_at{};
        is_invocable_with_impl is_invocable_with{};
        argument_list arguments{};

        template < constructor_policy_kind Policy, class_kind Class, typename... Args >
        [[nodiscard]] static constructor_state_ptr make(metadata_map metadata);
        explicit constructor_state(constructor_index index, metadata_map metadata);
    };

    struct destructor_state final : intrusive_ref_counter<destructor_state> {
        using destroy_impl = fixed_function<bool(const uarg&)>;
        using destroy_at_impl = fixed_function<void(void*)>;

        destructor_index index;
        metadata_map metadata;

        destroy_impl destroy{};
        destroy_at_impl destroy_at{};

        template < class_kind Class >
        [[nodiscard]] static destructor_state_ptr make(metadata_map metadata);
        explicit destructor_state(destructor_index index, metadata_map metadata);
    };

    struct evalue_state final : intrusive_ref_counter<evalue_state> {
        evalue_index index;
        metadata_map metadata;

        uvalue enum_value{};
        uvalue underlying_value{};

        template < enum_kind Enum >
        [[nodiscard]] static evalue_state_ptr make(std::string name, Enum evalue, metadata_map metadata);
        explicit evalue_state(evalue_index index, metadata_map metadata);
    };

    struct function_state final : intrusive_ref_counter<function_state> {
        using invoke_impl = fixed_function<uvalue(std::span<const uarg>)>;
        using is_invocable_with_impl = fixed_function<bool(std::span<const uarg_base>)>;

        function_index index;
        metadata_map metadata;

        invoke_impl invoke{};
        is_invocable_with_impl is_invocable_with{};
        argument_list arguments{};

        template < function_policy_kind Policy, function_pointer_kind Function >
        [[nodiscard]] static function_state_ptr make(std::string name, Function function_ptr, metadata_map metadata);
        explicit function_state(function_index index, metadata_map metadata);
    };

    struct member_state final : intrusive_ref_counter<member_state> {
        using getter_impl = fixed_function<uvalue(const uinst&)>;
        using setter_impl = fixed_function<void(const uinst&, const uarg&)>;

        using is_gettable_with_impl = fixed_function<bool(const uinst_base&)>;
        using is_settable_with_impl = fixed_function<bool(const uinst_base&, const uarg_base&)>;

        member_index index;
        metadata_map metadata;

        getter_impl getter{};
        setter_impl setter{};
        is_gettable_with_impl is_gettable_with{};
        is_settable_with_impl is_settable_with{};

        template < member_policy_kind Policy, member_pointer_kind Member >
        [[nodiscard]] static member_state_ptr make(std::string name, Member member_ptr, metadata_map metadata);
        explicit member_state(member_index index, metadata_map metadata);
    };

    struct method_state final : intrusive_ref_counter<method_state> {
        using invoke_impl = fixed_function<uvalue(const uinst&, std::span<const uarg>)>;
        using is_invocable_with_impl = fixed_function<bool(const uinst_base&, std::span<const uarg_base>)>;

        method_index index;
        metadata_map metadata;

        invoke_impl invoke{};
        is_invocable_with_impl is_invocable_with{};
        argument_list arguments{};

        template < method_policy_kind Policy, method_pointer_kind Method >
        [[nodiscard]] static method_state_ptr make(std::string name, Method method_ptr, metadata_map metadata);
        explicit method_state(method_index index, metadata_map metadata);
    };

    struct scope_state final : intrusive_ref_counter<scope_state> {
        scope_index index;
        metadata_map metadata;

        function_set functions{};
        typedef_map typedefs{};
        variable_set variables{};

        [[nodiscard]] static scope_state_ptr make(std::string name, metadata_map metadata);
        explicit scope_state(scope_index index, metadata_map metadata);
    };

    struct variable_state final : intrusive_ref_counter<variable_state> {
        using getter_impl = fixed_function<uvalue()>;
        using setter_impl = fixed_function<void(const uarg&)>;
        using is_settable_with_impl = fixed_function<bool(const uarg_base&)>;

        variable_index index;
        metadata_map metadata;

        getter_impl getter{};
        setter_impl setter{};
        is_settable_with_impl is_settable_with{};

        template < variable_policy_kind Policy, pointer_kind Pointer >
        [[nodiscard]] static variable_state_ptr make(std::string name, Pointer variable_ptr, metadata_map metadata);
        explicit variable_state(variable_index index, metadata_map metadata);
    };
}
