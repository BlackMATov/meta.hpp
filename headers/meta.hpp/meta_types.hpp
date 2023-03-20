/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_base.hpp"

#include "meta_detail/type_family.hpp"

namespace meta_hpp
{
    using detail::array_bitflags;
    using detail::array_flags;

    using detail::class_bitflags;
    using detail::class_flags;

    using detail::constructor_bitflags;
    using detail::constructor_flags;

    using detail::destructor_bitflags;
    using detail::destructor_flags;

    using detail::enum_bitflags;
    using detail::enum_flags;

    using detail::function_bitflags;
    using detail::function_flags;

    using detail::member_bitflags;
    using detail::member_flags;

    using detail::method_bitflags;
    using detail::method_flags;

    using detail::number_bitflags;
    using detail::number_flags;

    using detail::pointer_bitflags;
    using detail::pointer_flags;

    using detail::reference_bitflags;
    using detail::reference_flags;
}

namespace meta_hpp
{
    class type_id final {
    public:
        type_id() = default;

        [[nodiscard]] bool is_valid() const noexcept {
            return data_ != nullptr;
        }

        [[nodiscard]] explicit operator bool() const noexcept {
            return is_valid();
        }

        void swap(type_id& other) noexcept {
            std::swap(data_, other.data_);
        }

        [[nodiscard]] std::size_t get_hash() const noexcept {
            return data_ != nullptr ? detail::hash_combiner{}(data_) : 0;
        }

        [[nodiscard]] std::strong_ordering operator<=>(const type_id& other) const = default;

    private:
        template < detail::type_family T >
        friend class type_base;

        explicit type_id(const detail::type_data_base* data)
        : data_{data} {}

    private:
        const detail::type_data_base* data_{};
    };
}

namespace std
{
    template <>
    struct hash<meta_hpp::type_id> {
        size_t operator()(const meta_hpp::type_id& id) const noexcept {
            return id.get_hash();
        }
    };
}

namespace meta_hpp
{
    template < detail::type_family Type >
    class type_base {
        using data_ptr = typename detail::type_traits<Type>::data_ptr;
        friend data_ptr detail::type_access<Type>(const Type&);

    public:
        using id_type = type_id;

        type_base() = default;

        explicit type_base(data_ptr data)
        : data_{data} {}

        type_base(type_base&&) noexcept = default;
        type_base(const type_base&) = default;

        type_base& operator=(type_base&&) noexcept = default;
        type_base& operator=(const type_base&) = default;

        [[nodiscard]] bool is_valid() const noexcept {
            return data_ != nullptr;
        }

        [[nodiscard]] explicit operator bool() const noexcept {
            return is_valid();
        }

        [[nodiscard]] id_type get_id() const noexcept {
            return id_type{data_};
        }

        [[nodiscard]] type_kind get_kind() const noexcept {
            return data_->kind;
        }

        [[nodiscard]] const metadata_map& get_metadata() const noexcept {
            return data_->metadata;
        }

    protected:
        ~type_base() = default;

        data_ptr data_{};
    };
}

namespace meta_hpp
{
    class any_type final : public type_base<any_type> {
    public:
        using type_base<any_type>::type_base;

        template < detail::type_family Type >
        any_type(const Type& other) noexcept;

        template < detail::type_family Type >
        [[nodiscard]] bool is() const noexcept;
        [[nodiscard]] bool is(type_kind kind) const noexcept;

        template < detail::type_family Type >
        [[nodiscard]] Type as() const noexcept;

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
    };

    class array_type final : public type_base<array_type> {
    public:
        using type_base<array_type>::type_base;
        [[nodiscard]] array_bitflags get_flags() const noexcept;

        [[nodiscard]] std::size_t get_extent() const noexcept;
        [[nodiscard]] any_type get_data_type() const noexcept;
    };

    class class_type final : public type_base<class_type> {
    public:
        using type_base<class_type>::type_base;
        [[nodiscard]] class_bitflags get_flags() const noexcept;

        [[nodiscard]] std::size_t get_size() const noexcept;
        [[nodiscard]] std::size_t get_align() const noexcept;

        [[nodiscard]] std::size_t get_arity() const noexcept;
        [[nodiscard]] any_type get_argument_type(std::size_t position) const noexcept;
        [[nodiscard]] const any_type_list& get_argument_types() const noexcept;

        [[nodiscard]] const class_list& get_base_classes() const noexcept;
        [[nodiscard]] const class_list& get_derived_classes() const noexcept;
        [[nodiscard]] const constructor_list& get_constructors() const noexcept;
        [[nodiscard]] const destructor_list& get_destructors() const noexcept;
        [[nodiscard]] const function_list& get_functions() const noexcept;
        [[nodiscard]] const member_list& get_members() const noexcept;
        [[nodiscard]] const method_list& get_methods() const noexcept;
        [[nodiscard]] const typedef_map& get_typedefs() const noexcept;
        [[nodiscard]] const variable_list& get_variables() const noexcept;

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

        template < detail::class_kind Derived >
        [[nodiscard]] bool is_direct_base_of() const noexcept;
        [[nodiscard]] bool is_direct_base_of(const class_type& derived) const noexcept;

        template < detail::class_kind Derived >
        [[nodiscard]] bool is_virtual_base_of() const noexcept;
        [[nodiscard]] bool is_virtual_base_of(const class_type& derived) const noexcept;

        template < detail::class_kind Base >
        [[nodiscard]] bool is_derived_from() const noexcept;
        [[nodiscard]] bool is_derived_from(const class_type& base) const noexcept;

        template < detail::class_kind Base >
        [[nodiscard]] bool is_direct_derived_from() const noexcept;
        [[nodiscard]] bool is_direct_derived_from(const class_type& base) const noexcept;

        template < detail::class_kind Base >
        [[nodiscard]] bool is_virtual_derived_from() const noexcept;
        [[nodiscard]] bool is_virtual_derived_from(const class_type& base) const noexcept;

        [[nodiscard]] function get_function(std::string_view name, bool recursively = true) const noexcept;
        [[nodiscard]] member get_member(std::string_view name, bool recursively = true) const noexcept;
        [[nodiscard]] method get_method(std::string_view name, bool recursively = true) const noexcept;
        [[nodiscard]] any_type get_typedef(std::string_view name, bool recursively = true) const noexcept;
        [[nodiscard]] variable get_variable(std::string_view name, bool recursively = true) const noexcept;

        template < typename... Args >
        [[nodiscard]] constructor get_constructor_with() const noexcept;
        template < typename Iter >
        [[nodiscard]] constructor get_constructor_with(Iter first, Iter last) const noexcept;
        [[nodiscard]] constructor get_constructor_with(std::span<const any_type> args) const noexcept;
        [[nodiscard]] constructor get_constructor_with(std::initializer_list<any_type> args) const noexcept;

        [[nodiscard]] destructor get_destructor() const noexcept;

        template < typename... Args >
        [[nodiscard]] function get_function_with( //
            std::string_view name,
            bool recursively = true
        ) const noexcept;

        template < typename Iter >
        [[nodiscard]] function get_function_with( //
            std::string_view name,
            Iter first,
            Iter last,
            bool recursively = true
        ) const noexcept;

        [[nodiscard]] function get_function_with( //
            std::string_view name,
            std::span<const any_type> args,
            bool recursively = true
        ) const noexcept;

        [[nodiscard]] function get_function_with( //
            std::string_view name,
            std::initializer_list<any_type> args,
            bool recursively = true
        ) const noexcept;

        template < typename... Args >
        [[nodiscard]] method get_method_with( //
            std::string_view name,
            bool recursively = true
        ) const noexcept;

        template < typename Iter >
        [[nodiscard]] method get_method_with( //
            std::string_view name,
            Iter first,
            Iter last,
            bool recursively = true
        ) const noexcept;

        [[nodiscard]] method get_method_with( //
            std::string_view name,
            std::span<const any_type> args,
            bool recursively = true
        ) const noexcept;

        [[nodiscard]] method get_method_with( //
            std::string_view name,
            std::initializer_list<any_type> args,
            bool recursively = true
        ) const noexcept;
    };

    class constructor_type final : public type_base<constructor_type> {
    public:
        using type_base<constructor_type>::type_base;
        [[nodiscard]] constructor_bitflags get_flags() const noexcept;

        [[nodiscard]] std::size_t get_arity() const noexcept;
        [[nodiscard]] class_type get_owner_type() const noexcept;
        [[nodiscard]] any_type get_argument_type(std::size_t position) const noexcept;
        [[nodiscard]] const any_type_list& get_argument_types() const noexcept;
    };

    class destructor_type final : public type_base<destructor_type> {
    public:
        using type_base<destructor_type>::type_base;
        [[nodiscard]] destructor_bitflags get_flags() const noexcept;

        [[nodiscard]] class_type get_owner_type() const noexcept;
    };

    class enum_type final : public type_base<enum_type> {
    public:
        using type_base<enum_type>::type_base;
        [[nodiscard]] enum_bitflags get_flags() const noexcept;

        [[nodiscard]] number_type get_underlying_type() const noexcept;

        [[nodiscard]] const evalue_list& get_evalues() const noexcept;

        [[nodiscard]] evalue get_evalue(std::string_view name) const noexcept;

        template < detail::enum_kind Enum >
        [[nodiscard]] std::string_view value_to_name(Enum value) const noexcept;
        [[nodiscard]] uvalue name_to_value(std::string_view name) const noexcept;
    };

    class function_type final : public type_base<function_type> {
    public:
        using type_base<function_type>::type_base;
        [[nodiscard]] function_bitflags get_flags() const noexcept;

        [[nodiscard]] std::size_t get_arity() const noexcept;
        [[nodiscard]] any_type get_return_type() const noexcept;
        [[nodiscard]] any_type get_argument_type(std::size_t position) const noexcept;
        [[nodiscard]] const any_type_list& get_argument_types() const noexcept;
    };

    class member_type final : public type_base<member_type> {
    public:
        using type_base<member_type>::type_base;
        [[nodiscard]] member_bitflags get_flags() const noexcept;

        [[nodiscard]] class_type get_owner_type() const noexcept;
        [[nodiscard]] any_type get_value_type() const noexcept;
    };

    class method_type final : public type_base<method_type> {
    public:
        using type_base<method_type>::type_base;
        [[nodiscard]] method_bitflags get_flags() const noexcept;

        [[nodiscard]] std::size_t get_arity() const noexcept;
        [[nodiscard]] class_type get_owner_type() const noexcept;
        [[nodiscard]] any_type get_return_type() const noexcept;
        [[nodiscard]] any_type get_argument_type(std::size_t position) const noexcept;
        [[nodiscard]] const any_type_list& get_argument_types() const noexcept;
    };

    class nullptr_type final : public type_base<nullptr_type> {
    public:
        using type_base<nullptr_type>::type_base;
    };

    class number_type final : public type_base<number_type> {
    public:
        using type_base<number_type>::type_base;
        [[nodiscard]] number_bitflags get_flags() const noexcept;

        [[nodiscard]] std::size_t get_size() const noexcept;
        [[nodiscard]] std::size_t get_align() const noexcept;
    };

    class pointer_type final : public type_base<pointer_type> {
    public:
        using type_base<pointer_type>::type_base;
        [[nodiscard]] pointer_bitflags get_flags() const noexcept;

        [[nodiscard]] any_type get_data_type() const noexcept;
    };

    class reference_type final : public type_base<reference_type> {
    public:
        using type_base<reference_type>::type_base;
        [[nodiscard]] reference_bitflags get_flags() const noexcept;

        [[nodiscard]] any_type get_data_type() const noexcept;
    };

    class void_type final : public type_base<void_type> {
    public:
        using type_base<void_type>::type_base;
    };
}

namespace std
{
    template < meta_hpp::detail::type_family Type >
    struct hash<Type> {
        size_t operator()(const Type& type) const noexcept {
            return type.is_valid() ? type.get_id().get_hash() : 0;
        }
    };
}

namespace meta_hpp
{
    template < detail::type_family TypeL, detail::type_family TypeR >
    [[nodiscard]] bool operator==(const TypeL& l, const TypeR& r) noexcept {
        return l.is_valid() == r.is_valid() && (!l.is_valid() || l.get_id() == r.get_id());
    }

    template < detail::type_family TypeL, detail::type_family TypeR >
    [[nodiscard]] std::strong_ordering operator<=>(const TypeL& l, const TypeR& r) noexcept {
        if ( const std::strong_ordering cmp{l.is_valid() <=> r.is_valid()}; cmp != std::strong_ordering::equal ) {
            return cmp;
        }
        return l.is_valid() ? l.get_id() <=> r.get_id() : std::strong_ordering::equal;
    }
}

namespace meta_hpp
{
    template < detail::type_family Type >
    [[nodiscard]] bool operator==(const Type& l, const typename Type::id_type& r) noexcept {
        return l.is_valid() && l.get_id() == r;
    }

    template < detail::type_family Type >
    [[nodiscard]] std::strong_ordering operator<=>(const Type& l, const typename Type::id_type& r) noexcept {
        return l.is_valid() ? l.get_id() <=> r : std::strong_ordering::less;
    }
}

namespace meta_hpp::detail
{
    struct type_data_base {
        const type_kind kind;

        metadata_map metadata{};

        explicit type_data_base(type_kind nkind)
        : kind{nkind} {}

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

        class_list base_classes;
        class_list derived_classes;
        constructor_list constructors;
        destructor_list destructors;
        function_list functions;
        member_list members;
        method_list methods;
        typedef_map typedefs;
        variable_list variables;

        struct upcast_func_t final {
            using upcast_t = void* (*)(void*);

            upcast_t upcast{};
            class_type target{};
            bool is_virtual{};

            template < typename Derived, typename Base >
                requires std::is_base_of_v<Base, Derived>
            upcast_func_t(std::in_place_type_t<Derived>, std::in_place_type_t<Base>);

            [[nodiscard]] void* apply(void* ptr) const noexcept;
            [[nodiscard]] const void* apply(const void* ptr) const noexcept;
        };

        struct upcast_func_list_t final {
            using upcasts_t = std::vector<upcast_func_t>;
            using vbases_t = std::set<class_type, std::less<>>;

            upcasts_t upcasts{};
            vbases_t vbases{};
            bool is_ambiguous{};

            upcast_func_list_t(const upcast_func_t& _upcast);
            upcast_func_list_t(upcasts_t _upcasts, vbases_t _vbases);

            [[nodiscard]] void* apply(void* ptr) const noexcept;
            [[nodiscard]] const void* apply(const void* ptr) const noexcept;

            friend upcast_func_list_t operator+(const upcast_func_list_t& l, const upcast_func_list_t& r);
        };

        using base_upcasts_t = std::map<class_type, upcast_func_t, std::less<>>;
        using deep_upcasts_t = std::map<class_type, upcast_func_list_t, std::less<>>;

        base_upcasts_t base_upcasts;
        deep_upcasts_t deep_upcasts;

        template < class_kind Class >
        explicit class_type_data(type_list<Class>);
    };

    struct constructor_type_data final : type_data_base {
        const constructor_bitflags flags;
        const class_type owner_type;
        const any_type_list argument_types;

        template < class_kind Class, typename... Args >
        explicit constructor_type_data(type_list<Class>, type_list<Args...>);
    };

    struct destructor_type_data final : type_data_base {
        const destructor_bitflags flags;
        const class_type owner_type;

        template < class_kind Class >
        explicit destructor_type_data(type_list<Class>);
    };

    struct enum_type_data final : type_data_base {
        const enum_bitflags flags;
        const number_type underlying_type;

        evalue_list evalues;

        template < enum_kind Enum >
        explicit enum_type_data(type_list<Enum>);
    };

    struct function_type_data final : type_data_base {
        const function_bitflags flags;
        const any_type return_type;
        const any_type_list argument_types;

        template < function_pointer_kind Function >
        explicit function_type_data(type_list<Function>);
    };

    struct member_type_data final : type_data_base {
        const member_bitflags flags;
        const class_type owner_type;
        const any_type value_type;

        template < member_pointer_kind Member >
        explicit member_type_data(type_list<Member>);
    };

    struct method_type_data final : type_data_base {
        const method_bitflags flags;
        const class_type owner_type;
        const any_type return_type;
        const any_type_list argument_types;

        template < method_pointer_kind Method >
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
