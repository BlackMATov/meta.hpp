/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_base.hpp"

#include "meta_detail/type_family.hpp"

#include "meta_detail/type_traits/array_traits.hpp"
#include "meta_detail/type_traits/class_traits.hpp"
#include "meta_detail/type_traits/constructor_traits.hpp"
#include "meta_detail/type_traits/destructor_traits.hpp"
#include "meta_detail/type_traits/enum_traits.hpp"
#include "meta_detail/type_traits/function_traits.hpp"
#include "meta_detail/type_traits/member_traits.hpp"
#include "meta_detail/type_traits/method_traits.hpp"
#include "meta_detail/type_traits/number_traits.hpp"
#include "meta_detail/type_traits/pointer_traits.hpp"
#include "meta_detail/type_traits/reference_traits.hpp"

namespace meta_hpp
{
    using array_flags = detail::array_flags;
    using array_bitflags = detail::array_bitflags;

    using class_flags = detail::class_flags;
    using class_bitflags = detail::class_bitflags;

    using constructor_flags = detail::constructor_flags;
    using constructor_bitflags = detail::constructor_bitflags;

    using destructor_flags = detail::destructor_flags;
    using destructor_bitflags = detail::destructor_bitflags;

    using enum_flags = detail::enum_flags;
    using enum_bitflags = detail::enum_bitflags;

    using function_flags = detail::function_flags;
    using function_bitflags = detail::function_bitflags;

    using member_flags = detail::member_flags;
    using member_bitflags = detail::member_bitflags;

    using method_flags = detail::method_flags;
    using method_bitflags = detail::method_bitflags;

    using number_flags = detail::number_flags;
    using number_bitflags = detail::number_bitflags;

    using pointer_flags = detail::pointer_flags;
    using pointer_bitflags = detail::pointer_bitflags;

    using reference_flags = detail::reference_flags;
    using reference_bitflags = detail::reference_bitflags;
}

namespace meta_hpp
{
    class any_type final {
    public:
        using data_ptr = detail::type_data_base*;

        any_type() = default;

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] type_kind get_kind() const noexcept;

        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

        any_type(const array_type& other) noexcept;
        any_type(const class_type& other) noexcept;
        any_type(const constructor_type& other) noexcept;
        any_type(const destructor_type& other) noexcept;
        any_type(const enum_type& other) noexcept;
        any_type(const function_type& other) noexcept;
        any_type(const member_type& other) noexcept;
        any_type(const method_type& other) noexcept;
        any_type(const nullptr_type& other) noexcept;
        any_type(const number_type& other) noexcept;
        any_type(const pointer_type& other) noexcept;
        any_type(const reference_type& other) noexcept;
        any_type(const void_type& other) noexcept;

        [[nodiscard]] bool is_array() const noexcept;
        [[nodiscard]] bool is_class() const noexcept;
        [[nodiscard]] bool is_constructor() const noexcept;
        [[nodiscard]] bool is_destructor() const noexcept;
        [[nodiscard]] bool is_enum() const noexcept;
        [[nodiscard]] bool is_function() const noexcept;
        [[nodiscard]] bool is_member() const noexcept;
        [[nodiscard]] bool is_method() const noexcept;
        [[nodiscard]] bool is_nullptr() const noexcept;
        [[nodiscard]] bool is_number() const noexcept;
        [[nodiscard]] bool is_pointer() const noexcept;
        [[nodiscard]] bool is_reference() const noexcept;
        [[nodiscard]] bool is_void() const noexcept;

        [[nodiscard]] array_type as_array() const noexcept;
        [[nodiscard]] class_type as_class() const noexcept;
        [[nodiscard]] constructor_type as_constructor() const noexcept;
        [[nodiscard]] destructor_type as_destructor() const noexcept;
        [[nodiscard]] enum_type as_enum() const noexcept;
        [[nodiscard]] function_type as_function() const noexcept;
        [[nodiscard]] member_type as_member() const noexcept;
        [[nodiscard]] method_type as_method() const noexcept;
        [[nodiscard]] nullptr_type as_nullptr() const noexcept;
        [[nodiscard]] number_type as_number() const noexcept;
        [[nodiscard]] pointer_type as_pointer() const noexcept;
        [[nodiscard]] reference_type as_reference() const noexcept;
        [[nodiscard]] void_type as_void() const noexcept;

    private:
        data_ptr data_{};
        friend data_ptr detail::type_access<any_type>(const any_type&);
    };

    class array_type final {
    public:
        using data_ptr = detail::array_type_data*;

        array_type() = default;
        array_type(data_ptr data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] array_bitflags get_flags() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

        [[nodiscard]] std::size_t get_extent() const noexcept;
        [[nodiscard]] any_type get_data_type() const noexcept;

    private:
        data_ptr data_{};
        friend data_ptr detail::type_access<array_type>(const array_type&);
    };

    class class_type final {
    public:
        using data_ptr = detail::class_type_data*;

        class_type() = default;
        class_type(data_ptr data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] class_bitflags get_flags() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

        [[nodiscard]] std::size_t get_size() const noexcept;
        [[nodiscard]] std::size_t get_align() const noexcept;

        [[nodiscard]] std::size_t get_arity() const noexcept;
        [[nodiscard]] any_type get_argument_type(std::size_t position) const noexcept;
        [[nodiscard]] const any_type_list& get_argument_types() const noexcept;

        [[nodiscard]] const class_set& get_bases() const noexcept;
        [[nodiscard]] const constructor_set& get_constructors() const noexcept;
        [[nodiscard]] const destructor_set& get_destructors() const noexcept;
        [[nodiscard]] const function_set& get_functions() const noexcept;
        [[nodiscard]] const member_set& get_members() const noexcept;
        [[nodiscard]] const method_set& get_methods() const noexcept;
        [[nodiscard]] const typedef_map& get_typedefs() const noexcept;
        [[nodiscard]] const variable_set& get_variables() const noexcept;

        template < typename... Args >
        [[nodiscard]] uvalue create(Args&&... args) const;
        template < typename... Args >
        uvalue create_at(void* mem, Args&&... args) const;

        template < typename Arg >
        bool destroy(Arg&& arg) const;
        bool destroy_at(void* mem) const;

        template < detail::class_kind Derived >
        [[nodiscard]] bool is_base_of() const noexcept;
        [[nodiscard]] bool is_base_of(const class_type& derived) const noexcept;

        template < detail::class_kind Base >
        [[nodiscard]] bool is_derived_from() const noexcept;
        [[nodiscard]] bool is_derived_from(const class_type& base) const noexcept;

        [[nodiscard]] function get_function(std::string_view name) const noexcept;
        [[nodiscard]] member get_member(std::string_view name) const noexcept;
        [[nodiscard]] method get_method(std::string_view name) const noexcept;
        [[nodiscard]] any_type get_typedef(std::string_view name) const noexcept;
        [[nodiscard]] variable get_variable(std::string_view name) const noexcept;

        template < typename... Args >
        [[nodiscard]] constructor get_constructor_with() const noexcept;
        template < typename Iter >
        [[nodiscard]] constructor get_constructor_with(Iter first, Iter last) const noexcept;
        [[nodiscard]] constructor get_constructor_with(std::span<const any_type> args) const noexcept;
        [[nodiscard]] constructor get_constructor_with(std::initializer_list<any_type> args) const noexcept;

        [[nodiscard]] destructor get_destructor() const noexcept;

        template < typename... Args >
        [[nodiscard]] function get_function_with(std::string_view name) const noexcept;
        template < typename Iter >
        [[nodiscard]] function get_function_with(std::string_view name, Iter first, Iter last) const noexcept;
        [[nodiscard]] function get_function_with(std::string_view name, std::span<const any_type> args) const noexcept;
        [[nodiscard]] function get_function_with(std::string_view name, std::initializer_list<any_type> args) const noexcept;

        template < typename... Args >
        [[nodiscard]] method get_method_with(std::string_view name) const noexcept;
        template < typename Iter >
        [[nodiscard]] method get_method_with(std::string_view name, Iter first, Iter last) const noexcept;
        [[nodiscard]] method get_method_with(std::string_view name, std::span<const any_type> args) const noexcept;
        [[nodiscard]] method get_method_with(std::string_view name, std::initializer_list<any_type> args) const noexcept;

    private:
        data_ptr data_{};
        friend data_ptr detail::type_access<class_type>(const class_type&);
    };

    class constructor_type final {
    public:
        using data_ptr = detail::constructor_type_data*;

        constructor_type() = default;
        constructor_type(data_ptr data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] constructor_bitflags get_flags() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

        [[nodiscard]] std::size_t get_arity() const noexcept;
        [[nodiscard]] any_type get_class_type() const noexcept;
        [[nodiscard]] any_type get_argument_type(std::size_t position) const noexcept;
        [[nodiscard]] const any_type_list& get_argument_types() const noexcept;

    private:
        data_ptr data_{};
        friend data_ptr detail::type_access<constructor_type>(const constructor_type&);
    };

    class destructor_type final {
    public:
        using data_ptr = detail::destructor_type_data*;

        destructor_type() = default;
        destructor_type(data_ptr data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] destructor_bitflags get_flags() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

        [[nodiscard]] any_type get_class_type() const noexcept;

    private:
        data_ptr data_{};
        friend data_ptr detail::type_access<destructor_type>(const destructor_type&);
    };

    class enum_type final {
    public:
        using data_ptr = detail::enum_type_data*;

        enum_type() = default;
        enum_type(data_ptr data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] enum_bitflags get_flags() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

        [[nodiscard]] number_type get_underlying_type() const noexcept;

        [[nodiscard]] const evalue_set& get_evalues() const noexcept;

        [[nodiscard]] evalue get_evalue(std::string_view name) const noexcept;

        template < detail::enum_kind Enum >
        [[nodiscard]] std::string_view value_to_name(Enum value) const noexcept;
        [[nodiscard]] uvalue name_to_value(std::string_view name) const noexcept;

        template < typename T >
        [[nodiscard]] T name_to_value_as(std::string_view name) const;

    private:
        data_ptr data_{};
        friend data_ptr detail::type_access<enum_type>(const enum_type&);
    };

    class function_type final {
    public:
        using data_ptr = detail::function_type_data*;

        function_type() = default;
        function_type(data_ptr data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] function_bitflags get_flags() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

        [[nodiscard]] std::size_t get_arity() const noexcept;
        [[nodiscard]] any_type get_return_type() const noexcept;
        [[nodiscard]] any_type get_argument_type(std::size_t position) const noexcept;
        [[nodiscard]] const any_type_list& get_argument_types() const noexcept;

    private:
        data_ptr data_{};
        friend data_ptr detail::type_access<function_type>(const function_type&);
    };

    class member_type final {
    public:
        using data_ptr = detail::member_type_data*;

        member_type() = default;
        member_type(data_ptr data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] member_bitflags get_flags() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

        [[nodiscard]] class_type get_owner_type() const noexcept;
        [[nodiscard]] any_type get_value_type() const noexcept;

    private:
        data_ptr data_{};
        friend data_ptr detail::type_access<member_type>(const member_type&);
    };

    class method_type final {
    public:
        using data_ptr = detail::method_type_data*;

        method_type() = default;
        method_type(data_ptr data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] method_bitflags get_flags() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

        [[nodiscard]] std::size_t get_arity() const noexcept;
        [[nodiscard]] class_type get_owner_type() const noexcept;
        [[nodiscard]] any_type get_return_type() const noexcept;
        [[nodiscard]] any_type get_argument_type(std::size_t position) const noexcept;
        [[nodiscard]] const any_type_list& get_argument_types() const noexcept;

    private:
        data_ptr data_{};
        friend data_ptr detail::type_access<method_type>(const method_type&);
    };

    class nullptr_type final {
    public:
        using data_ptr = detail::nullptr_type_data*;

        nullptr_type() = default;
        nullptr_type(data_ptr data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

    private:
        data_ptr data_{};
        friend data_ptr detail::type_access<nullptr_type>(const nullptr_type&);
    };

    class number_type final {
    public:
        using data_ptr = detail::number_type_data*;

        number_type() = default;
        number_type(data_ptr data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] number_bitflags get_flags() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

        [[nodiscard]] std::size_t get_size() const noexcept;
        [[nodiscard]] std::size_t get_align() const noexcept;

    private:
        data_ptr data_{};
        friend data_ptr detail::type_access<number_type>(const number_type&);
    };

    class pointer_type final {
    public:
        using data_ptr = detail::pointer_type_data*;

        pointer_type() = default;
        pointer_type(data_ptr data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] pointer_bitflags get_flags() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

        [[nodiscard]] any_type get_data_type() const noexcept;

    private:
        data_ptr data_{};
        friend data_ptr detail::type_access<pointer_type>(const pointer_type&);
    };

    class reference_type final {
    public:
        using data_ptr = detail::reference_type_data*;

        reference_type() = default;
        reference_type(data_ptr data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] reference_bitflags get_flags() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

        [[nodiscard]] any_type get_data_type() const noexcept;

    private:
        data_ptr data_{};
        friend data_ptr detail::type_access<reference_type>(const reference_type&);
    };

    class void_type final {
    public:
        using data_ptr = detail::void_type_data*;

        void_type() = default;
        void_type(data_ptr data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

    private:
        data_ptr data_{};
        friend data_ptr detail::type_access<void_type>(const void_type&);
    };
}

namespace std
{
    template < meta_hpp::detail::type_family T >
    struct hash<T> {
        size_t operator()(const T& t) const noexcept {
            return meta_hpp::detail::hash_combiner{}(t.get_id());
        }
    };
}

namespace meta_hpp
{
    template < detail::type_family T, detail::type_family U >
    [[nodiscard]] bool operator==(const T& l, const U& r) noexcept {
        return l.is_valid() == r.is_valid() && (!l.is_valid() || l.get_id() == r.get_id());
    }

    template < detail::type_family T, detail::type_family U >
    [[nodiscard]] std::strong_ordering operator<=>(const T& l, const U& r) noexcept {
        if ( const std::strong_ordering cmp{l.is_valid() <=> r.is_valid()}; cmp != std::strong_ordering::equal ) {
            return cmp;
        }
        return l.is_valid() ? l.get_id() <=> r.get_id() : std::strong_ordering::equal;
    }
}

namespace meta_hpp
{
    template < detail::type_family T >
    [[nodiscard]] bool operator==(const T& l, type_id r) noexcept {
        return l.is_valid() && l.get_id() == r;
    }

    template < detail::type_family T >
    [[nodiscard]] std::strong_ordering operator<=>(const T& l, type_id r) noexcept {
        return l.is_valid() ? l.get_id() <=> r : std::strong_ordering::less;
    }
}

namespace meta_hpp::detail
{
    struct type_data_base {
        const type_id id;
        const type_kind kind;

        metadata_map metadata{};

        explicit type_data_base(type_id nid, type_kind nkind)
        : id{nid}
        , kind{nkind} {}

        type_data_base(type_data_base&&) = delete;
        type_data_base(const type_data_base&) = delete;
        type_data_base& operator=(type_data_base&&) = delete;
        type_data_base& operator=(const type_data_base&) = delete;

    protected:
        ~type_data_base() = default;
    };

    struct array_type_data final : type_data_base {
        const array_bitflags flags;
        const std::size_t extent;
        const any_type data_type;

        template < array_kind Array >
        explicit array_type_data(type_list<Array>);
    };

    struct class_type_data final : type_data_base {
        const class_bitflags flags;
        const std::size_t size;
        const std::size_t align;
        const any_type_list argument_types;

        class_set bases;
        constructor_set constructors;
        destructor_set destructors;
        function_set functions;
        member_set members;
        method_set methods;
        typedef_map typedefs;
        variable_set variables;

        struct base_info final {
            using upcast_fptr = void* (*)(void*);
            const upcast_fptr upcast;
        };

        using base_info_map = std::map<class_type, base_info, std::less<>>;
        base_info_map bases_info;

        template < class_kind Class >
        explicit class_type_data(type_list<Class>);
    };

    struct constructor_type_data final : type_data_base {
        const constructor_bitflags flags;
        const any_type class_type;
        const any_type_list argument_types;

        template < class_kind Class, typename... Args >
        explicit constructor_type_data(type_list<Class>, type_list<Args...>);
    };

    struct destructor_type_data final : type_data_base {
        const destructor_bitflags flags;
        const any_type class_type;

        template < class_kind Class >
        explicit destructor_type_data(type_list<Class>);
    };

    struct enum_type_data final : type_data_base {
        const enum_bitflags flags;
        const number_type underlying_type;

        evalue_set evalues;

        template < enum_kind Enum >
        explicit enum_type_data(type_list<Enum>);
    };

    struct function_type_data final : type_data_base {
        const function_bitflags flags;
        const any_type return_type;
        const any_type_list argument_types;

        template < function_kind Function >
        explicit function_type_data(type_list<Function>);
    };

    struct member_type_data final : type_data_base {
        const member_bitflags flags;
        const class_type owner_type;
        const any_type value_type;

        template < member_kind Member >
        explicit member_type_data(type_list<Member>);
    };

    struct method_type_data final : type_data_base {
        const method_bitflags flags;
        const class_type owner_type;
        const any_type return_type;
        const any_type_list argument_types;

        template < method_kind Method >
        explicit method_type_data(type_list<Method>);
    };

    struct nullptr_type_data final : type_data_base {
        template < nullptr_kind Nullptr >
        explicit nullptr_type_data(type_list<Nullptr>);
    };

    struct number_type_data final : type_data_base {
        const number_bitflags flags;
        const std::size_t size;
        const std::size_t align;

        template < number_kind Number >
        explicit number_type_data(type_list<Number>);
    };

    struct pointer_type_data final : type_data_base {
        const pointer_bitflags flags;
        const any_type data_type;

        template < pointer_kind Pointer >
        explicit pointer_type_data(type_list<Pointer>);
    };

    struct reference_type_data final : type_data_base {
        const reference_bitflags flags;
        const any_type data_type;

        template < reference_kind Reference >
        explicit reference_type_data(type_list<Reference>);
    };

    struct void_type_data final : type_data_base {
        template < void_kind Void >
        explicit void_type_data(type_list<Void>);
    };
}
