/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_base.hpp"
#include "meta_details.hpp"
#include "meta_indices.hpp"
#include "meta_policies.hpp"
#include "meta_types.hpp"
#include "meta_uresult.hpp"
#include "meta_uvalue.hpp"

#include "meta_detail/state_family.hpp"

namespace meta_hpp
{
    template < state_family State >
    class state_base {
        using state_ptr = typename detail::state_traits<State>::state_ptr;
        friend state_ptr detail::state_access<State>(const State&);

    public:
        using index_type = typename detail::state_traits<State>::index_type;

        state_base() = default;

        explicit state_base(state_ptr state);

        state_base(state_base&&) = default;
        state_base(const state_base&) = default;

        state_base& operator=(state_base&&) = default;
        state_base& operator=(const state_base&) = default;

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] const index_type& get_index() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

    protected:
        ~state_base() = default;

        state_ptr state_{};
    };
}

namespace meta_hpp
{
    class argument final : public state_base<argument> {
    public:
        using state_base<argument>::state_base;

        [[nodiscard]] any_type get_type() const noexcept;
        [[nodiscard]] std::size_t get_position() const noexcept;

        [[nodiscard]] const std::string& get_name() const noexcept;
    };

    class constructor final : public state_base<constructor> {
    public:
        using state_base<constructor>::state_base;

        [[nodiscard]] constructor_type get_type() const noexcept;

        [[nodiscard]] std::size_t get_arity() const noexcept;
        [[nodiscard]] argument get_argument(std::size_t position) const noexcept;
        [[nodiscard]] const argument_list& get_arguments() const noexcept;

        //

        template < typename... Args >
        [[nodiscard]] uvalue create(Args&&... args) const;

        template < typename... Args >
        [[nodiscard]] uresult try_create(Args&&... args) const;

        template < typename... Args >
        uvalue create_at(void* mem, Args&&... args) const;

        template < typename... Args >
        uresult try_create_at(void* mem, Args&&... args) const;

        template < typename... Args >
        [[nodiscard]] bool is_invocable_with() const;

        template < typename... Args >
        [[nodiscard]] bool is_invocable_with(Args&&... args) const;

        template < typename... Args >
        [[nodiscard]] uerror check_invocable_error() const;

        template < typename... Args >
        [[nodiscard]] uerror check_invocable_error(Args&&... args) const;

        //

        template < typename Iter >
        [[nodiscard]] uvalue create_variadic(Iter first, Iter last) const;

        template < typename Iter >
        [[nodiscard]] uresult try_create_variadic(Iter first, Iter last) const;

        template < typename Iter >
        uvalue create_variadic_at(void* mem, Iter first, Iter last) const;

        template < typename Iter >
        uresult try_create_variadic_at(void* mem, Iter first, Iter last) const;

        template < typename Iter >
        [[nodiscard]] bool is_variadic_invocable_with(Iter first, Iter last) const;

        template < typename Iter >
        [[nodiscard]] uerror check_variadic_invocable_error(Iter first, Iter last) const;
    };

    class destructor final : public state_base<destructor> {
    public:
        using state_base<destructor>::state_base;

        [[nodiscard]] destructor_type get_type() const noexcept;

        template < typename Arg >
        void destroy(Arg&& arg) const;

        template < typename Arg >
        uresult try_destroy(Arg&& arg) const;

        void destroy_at(void* mem) const;

        uresult try_destroy_at(void* mem) const;

        template < typename Arg >
        [[nodiscard]] bool is_invocable_with() const;

        template < typename Arg >
        [[nodiscard]] bool is_invocable_with(Arg&& arg) const;

        template < typename Arg >
        [[nodiscard]] uerror check_invocable_error() const;

        template < typename Arg >
        [[nodiscard]] uerror check_invocable_error(Arg&& arg) const;
    };

    class evalue final : public state_base<evalue> {
    public:
        using state_base<evalue>::state_base;

        [[nodiscard]] enum_type get_type() const noexcept;
        [[nodiscard]] const std::string& get_name() const noexcept;

        [[nodiscard]] const uvalue& get_value() const noexcept;
        [[nodiscard]] const uvalue& get_underlying_value() const noexcept;
    };

    class function final : public state_base<function> {
    public:
        using state_base<function>::state_base;

        [[nodiscard]] function_type get_type() const noexcept;
        [[nodiscard]] const std::string& get_name() const noexcept;

        [[nodiscard]] std::size_t get_arity() const noexcept;
        [[nodiscard]] argument get_argument(std::size_t position) const noexcept;
        [[nodiscard]] const argument_list& get_arguments() const noexcept;

        //

        template < typename... Args >
        uvalue invoke(Args&&... args) const;

        template < typename... Args >
        uresult try_invoke(Args&&... args) const;

        template < typename... Args >
        uvalue operator()(Args&&... args) const;

        template < typename... Args >
        [[nodiscard]] bool is_invocable_with() const;

        template < typename... Args >
        [[nodiscard]] bool is_invocable_with(Args&&... args) const;

        template < typename... Args >
        [[nodiscard]] uerror check_invocable_error() const;

        template < typename... Args >
        [[nodiscard]] uerror check_invocable_error(Args&&... args) const;

        //

        template < typename Iter >
        uvalue invoke_variadic(Iter first, Iter last) const;

        template < typename Iter >
        uresult try_invoke_variadic(Iter first, Iter last) const;

        template < typename Iter >
        [[nodiscard]] bool is_variadic_invocable_with(Iter first, Iter last) const;

        template < typename Iter >
        [[nodiscard]] uerror check_variadic_invocable_error(Iter first, Iter last) const;
    };

    class member final : public state_base<member> {
    public:
        using state_base<member>::state_base;

        [[nodiscard]] member_type get_type() const noexcept;
        [[nodiscard]] const std::string& get_name() const noexcept;

        template < typename Instance >
        [[nodiscard]] uvalue get(Instance&& instance) const;

        template < typename Instance >
        [[nodiscard]] uresult try_get(Instance&& instance) const;

        template < typename Instance >
        [[nodiscard]] uvalue operator()(Instance&& instance) const;

        template < typename Instance, typename Value >
        void set(Instance&& instance, Value&& value) const;

        template < typename Instance, typename Value >
        uresult try_set(Instance&& instance, Value&& value) const;

        template < typename Instance, typename Value >
        void operator()(Instance&& instance, Value&& value) const;

        template < typename Instance >
        [[nodiscard]] bool is_gettable_with() const;

        template < typename Instance >
        [[nodiscard]] bool is_gettable_with(Instance&& instance) const;

        template < typename Instance, typename Value >
        [[nodiscard]] bool is_settable_with() const;

        template < typename Instance, typename Value >
        [[nodiscard]] bool is_settable_with(Instance&& instance, Value&& value) const;

        template < typename Instance >
        [[nodiscard]] uerror check_gettable_error() const;

        template < typename Instance >
        [[nodiscard]] uerror check_gettable_error(Instance&& instance) const;

        template < typename Instance, typename Value >
        [[nodiscard]] uerror check_settable_error() const;

        template < typename Instance, typename Value >
        [[nodiscard]] uerror check_settable_error(Instance&& instance, Value&& value) const;
    };

    class method final : public state_base<method> {
    public:
        using state_base<method>::state_base;

        [[nodiscard]] method_type get_type() const noexcept;
        [[nodiscard]] const std::string& get_name() const noexcept;

        [[nodiscard]] std::size_t get_arity() const noexcept;
        [[nodiscard]] argument get_argument(std::size_t position) const noexcept;
        [[nodiscard]] const argument_list& get_arguments() const noexcept;

        //

        template < typename Instance, typename... Args >
        uvalue invoke(Instance&& instance, Args&&... args) const;

        template < typename Instance, typename... Args >
        uresult try_invoke(Instance&& instance, Args&&... args) const;

        template < typename Instance, typename... Args >
        uvalue operator()(Instance&& instance, Args&&... args) const;

        template < typename Instance, typename... Args >
        [[nodiscard]] bool is_invocable_with() const;

        template < typename Instance, typename... Args >
        [[nodiscard]] bool is_invocable_with(Instance&& instance, Args&&... args) const;

        template < typename Instance, typename... Args >
        [[nodiscard]] uerror check_invocable_error() const;

        template < typename Instance, typename... Args >
        [[nodiscard]] uerror check_invocable_error(Instance&& instance, Args&&... args) const;

        //

        template < typename Instance, typename Iter >
        uvalue invoke_variadic(Instance&& instance, Iter first, Iter last) const;

        template < typename Instance, typename Iter >
        uresult try_invoke_variadic(Instance&& instance, Iter first, Iter last) const;

        template < typename Instance, typename Iter >
        [[nodiscard]] bool is_variadic_invocable_with(Instance&& instance, Iter first, Iter last) const;

        template < typename Instance, typename Iter >
        [[nodiscard]] uerror check_variadic_invocable_error(Instance&& instance, Iter first, Iter last) const;
    };

    class scope final : public state_base<scope> {
    public:
        using state_base<scope>::state_base;

        [[nodiscard]] const std::string& get_name() const noexcept;

        [[nodiscard]] const function_list& get_functions() const noexcept;
        [[nodiscard]] const typedef_map& get_typedefs() const noexcept;
        [[nodiscard]] const variable_list& get_variables() const noexcept;

        [[nodiscard]] function get_function(std::string_view name) const noexcept;
        [[nodiscard]] any_type get_typedef(std::string_view name) const noexcept;
        [[nodiscard]] variable get_variable(std::string_view name) const noexcept;

        template < typename... Args >
        [[nodiscard]] function get_function_with( //
            std::string_view name
        ) const;

        template < typename Iter >
        [[nodiscard]] function get_function_with( //
            std::string_view name,
            Iter first,
            Iter last
        ) const;

        [[nodiscard]] function get_function_with( //
            std::string_view name,
            std::span<const any_type> args
        ) const noexcept;

        [[nodiscard]] function get_function_with( //
            std::string_view name,
            std::initializer_list<any_type> args
        ) const noexcept;
    };

    class variable final : public state_base<variable> {
    public:
        using state_base<variable>::state_base;

        [[nodiscard]] pointer_type get_type() const noexcept;
        [[nodiscard]] const std::string& get_name() const noexcept;

        [[nodiscard]] uvalue get() const;
        [[nodiscard]] uresult try_get() const;

        [[nodiscard]] uvalue operator()() const;

        template < typename Value >
        void set(Value&& value) const;

        template < typename Value >
        uresult try_set(Value&& value) const;

        template < typename Value >
        void operator()(Value&& value) const;

        template < typename Value >
        [[nodiscard]] bool is_settable_with() const;

        template < typename Value >
        [[nodiscard]] bool is_settable_with(Value&& value) const;

        template < typename Value >
        [[nodiscard]] uerror check_settable_error() const;

        template < typename Value >
        [[nodiscard]] uerror check_settable_error(Value&& value) const;
    };
}

namespace std
{
    template < meta_hpp::state_family State >
    struct hash<State> {
        size_t operator()(const State& state) const noexcept {
            return state.is_valid() ? state.get_index().get_hash() : 0;
        }
    };
}

namespace meta_hpp
{
    template < state_family State >
    [[nodiscard]] bool operator==(const State& l, const State& r) noexcept {
        return l.is_valid() == r.is_valid() && (!l.is_valid() || l.get_index() == r.get_index());
    }

    template < state_family State >
    [[nodiscard]] std::strong_ordering operator<=>(const State& l, const State& r) noexcept {
        if ( const std::strong_ordering cmp{l.is_valid() <=> r.is_valid()}; cmp != std::strong_ordering::equal ) {
            return cmp;
        }
        return l.is_valid() ? l.get_index() <=> r.get_index() : std::strong_ordering::equal;
    }
}

namespace meta_hpp
{
    template < state_family State >
    [[nodiscard]] bool operator==(const State& l, const typename State::index_type& r) noexcept {
        return l.is_valid() && l.get_index() == r;
    }

    template < state_family State >
    [[nodiscard]] std::strong_ordering operator<=>(const State& l, const typename State::index_type& r) noexcept {
        return l.is_valid() ? l.get_index() <=> r : std::strong_ordering::less;
    }
}

namespace meta_hpp::detail
{
    struct argument_state final : private state_traits<argument> {
        argument_index index;
        metadata_map metadata;

        std::string name{};

        template < typename Argument >
        [[nodiscard]] static state_ptr make(std::size_t position, metadata_map metadata);
        explicit argument_state(argument_index index, metadata_map metadata);
    };

    struct constructor_state final : private state_traits<constructor> {
        using create_impl = fixed_function<uvalue(std::span<const uarg>)>;
        using create_at_impl = fixed_function<uvalue(void*, std::span<const uarg>)>;
        using create_error_impl = fixed_function<uerror(std::span<const uarg_base>)>;

        constructor_index index;
        metadata_map metadata;

        create_impl create{};
        create_at_impl create_at{};
        create_error_impl create_error{};
        argument_list arguments{};

        template < constructor_policy_family Policy, class_kind Class, typename... Args >
        [[nodiscard]] static state_ptr make(metadata_map metadata);
        explicit constructor_state(constructor_index index, metadata_map metadata);
    };

    struct destructor_state final : private state_traits<destructor> {
        using destroy_impl = fixed_function<void(const uarg&)>;
        using destroy_at_impl = fixed_function<void(void*)>;
        using destroy_error_impl = fixed_function<uerror(const uarg_base&)>;

        destructor_index index;
        metadata_map metadata;

        destroy_impl destroy{};
        destroy_at_impl destroy_at{};
        destroy_error_impl destroy_error{};

        template < class_kind Class >
        [[nodiscard]] static state_ptr make(metadata_map metadata);
        explicit destructor_state(destructor_index index, metadata_map metadata);
    };

    struct evalue_state final : private state_traits<evalue> {
        evalue_index index;
        metadata_map metadata;

        uvalue enum_value{};
        uvalue underlying_value{};

        template < enum_kind Enum >
        [[nodiscard]] static state_ptr make(std::string name, Enum evalue, metadata_map metadata);
        explicit evalue_state(evalue_index index, metadata_map metadata);
    };

    struct function_state final : private state_traits<function> {
        using invoke_impl = fixed_function<uvalue(std::span<const uarg>)>;
        using invoke_error_impl = fixed_function<uerror(std::span<const uarg_base>)>;

        function_index index;
        metadata_map metadata;

        invoke_impl invoke{};
        invoke_error_impl invoke_error{};
        argument_list arguments{};

        template < function_policy_family Policy, function_pointer_kind Function >
        [[nodiscard]] static state_ptr make(std::string name, Function function_ptr, metadata_map metadata);
        explicit function_state(function_index index, metadata_map metadata);
    };

    struct member_state final : private state_traits<member> {
        using getter_impl = fixed_function<uvalue(const uinst&)>;
        using setter_impl = fixed_function<void(const uinst&, const uarg&)>;

        using getter_error_impl = fixed_function<uerror(const uinst_base&)>;
        using setter_error_impl = fixed_function<uerror(const uinst_base&, const uarg_base&)>;

        member_index index;
        metadata_map metadata;

        getter_impl getter{};
        setter_impl setter{};
        getter_error_impl getter_error{};
        setter_error_impl setter_error{};

        template < member_policy_family Policy, member_pointer_kind Member >
        [[nodiscard]] static state_ptr make(std::string name, Member member_ptr, metadata_map metadata);
        explicit member_state(member_index index, metadata_map metadata);
    };

    struct method_state final : private state_traits<method> {
        using invoke_impl = fixed_function<uvalue(const uinst&, std::span<const uarg>)>;
        using invoke_error_impl = fixed_function<uerror(const uinst_base&, std::span<const uarg_base>)>;

        method_index index;
        metadata_map metadata;

        invoke_impl invoke{};
        invoke_error_impl invoke_error{};
        argument_list arguments{};

        template < method_policy_family Policy, method_pointer_kind Method >
        [[nodiscard]] static state_ptr make(std::string name, Method method_ptr, metadata_map metadata);
        explicit method_state(method_index index, metadata_map metadata);
    };

    struct scope_state final : private state_traits<scope> {
        scope_index index;
        metadata_map metadata;

        function_list functions{};
        typedef_map typedefs{};
        variable_list variables{};

        [[nodiscard]] static state_ptr make(std::string name, metadata_map metadata);
        explicit scope_state(scope_index index, metadata_map metadata);
    };

    struct variable_state final : private state_traits<variable> {
        using getter_impl = fixed_function<uvalue()>;
        using setter_impl = fixed_function<void(const uarg&)>;
        using setter_error_impl = fixed_function<uerror(const uarg_base&)>;

        variable_index index;
        metadata_map metadata;

        getter_impl getter{};
        setter_impl setter{};
        setter_error_impl setter_error{};

        template < variable_policy_family Policy, pointer_kind Pointer >
        [[nodiscard]] static state_ptr make(std::string name, Pointer variable_ptr, metadata_map metadata);
        explicit variable_state(variable_index index, metadata_map metadata);
    };
}

namespace meta_hpp
{
    template < state_family State >
    state_base<State>::state_base(state_ptr state)
    : state_{std::move(state)} {}

    template < state_family State >
    bool state_base<State>::is_valid() const noexcept {
        return state_ != nullptr;
    }

    template < state_family State >
    state_base<State>::operator bool() const noexcept {
        return is_valid();
    }

    template < state_family State >
    const typename state_base<State>::index_type& state_base<State>::get_index() const noexcept {
        return state_->index;
    }

    template < state_family State >
    const metadata_map& state_base<State>::get_metadata() const noexcept {
        return state_->metadata;
    }
}
