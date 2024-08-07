/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_base.hpp"
#include "meta_details.hpp"
#include "meta_uvalue.hpp"

#include "meta_detail/type_family.hpp"

namespace meta_hpp
{
    class type_id final {
    public:
        type_id() = default;
        ~type_id() = default;

        type_id(type_id&&) = default;
        type_id(const type_id&) = default;

        type_id& operator=(type_id&&) = default;
        type_id& operator=(const type_id&) = default;

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        void swap(type_id& other) noexcept;
        [[nodiscard]] std::size_t get_hash() const noexcept;

        [[nodiscard]] bool operator==(const type_id& other) const noexcept;
        [[nodiscard]] std::strong_ordering operator<=>(const type_id& other) const noexcept;

    private:
        template < type_family T >
        friend class type_base;

        explicit type_id(const detail::type_data_base* data);

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
    template < type_family Type >
    class type_base {
        using data_ptr = typename detail::type_traits<Type>::data_ptr;
        friend data_ptr detail::type_access<Type>(const Type&);

    public:
        using id_type = type_id;

        type_base() = default;

        explicit type_base(data_ptr data);

        type_base(type_base&&) = default;
        type_base(const type_base&) = default;

        type_base& operator=(type_base&&) = default;
        type_base& operator=(const type_base&) = default;

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] std::size_t get_hash() const noexcept;

        [[nodiscard]] id_type get_id() const noexcept;
        [[nodiscard]] type_kind get_kind() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

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

        template < type_family Type >
        any_type(const Type& other) noexcept;

        template < type_family Type >
        [[nodiscard]] bool is() const noexcept;
        [[nodiscard]] bool is(type_kind kind) const noexcept;

        template < type_family Type >
        [[nodiscard]] Type as() const noexcept;

        template < typename F >
        bool match(F&& f) const;

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
        [[nodiscard]] const uvalue& get_argument_value(std::size_t position) const noexcept;
        [[nodiscard]] const any_type_list& get_argument_types() const noexcept;
        [[nodiscard]] const uvalue_list& get_argument_values() const noexcept;

        [[nodiscard]] const class_list& get_base_classes() const noexcept;
        [[nodiscard]] const constructor_list& get_constructors() const noexcept;
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

        template < class_kind Derived >
        [[nodiscard]] bool is_base_of() const;
        [[nodiscard]] bool is_base_of(const class_type& derived) const noexcept;

        template < class_kind Derived >
        [[nodiscard]] bool is_direct_base_of() const;
        [[nodiscard]] bool is_direct_base_of(const class_type& derived) const noexcept;

        template < class_kind Base >
        [[nodiscard]] bool is_derived_from() const;
        [[nodiscard]] bool is_derived_from(const class_type& base) const noexcept;

        template < class_kind Base >
        [[nodiscard]] bool is_direct_derived_from() const;
        [[nodiscard]] bool is_direct_derived_from(const class_type& base) const noexcept;

        [[nodiscard]] function get_function(std::string_view name, bool recursively = true) const noexcept;
        [[nodiscard]] member get_member(std::string_view name, bool recursively = true) const noexcept;
        [[nodiscard]] method get_method(std::string_view name, bool recursively = true) const noexcept;
        [[nodiscard]] any_type get_typedef(std::string_view name, bool recursively = true) const noexcept;
        [[nodiscard]] variable get_variable(std::string_view name, bool recursively = true) const noexcept;

        template < typename... Args >
        [[nodiscard]] constructor get_constructor_with() const;
        template < typename Iter >
        [[nodiscard]] constructor get_constructor_with(Iter first, Iter last) const;
        [[nodiscard]] constructor get_constructor_with(std::span<const any_type> args) const noexcept;
        [[nodiscard]] constructor get_constructor_with(std::initializer_list<any_type> args) const noexcept;

        [[nodiscard]] destructor get_destructor() const noexcept;

        template < typename... Args >
        [[nodiscard]] function get_function_with( //
            std::string_view name,
            bool recursively = true
        ) const;

        template < typename Iter >
        [[nodiscard]] function get_function_with( //
            std::string_view name,
            Iter first,
            Iter last,
            bool recursively = true
        ) const;

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
        ) const;

        template < typename Iter >
        [[nodiscard]] method get_method_with( //
            std::string_view name,
            Iter first,
            Iter last,
            bool recursively = true
        ) const;

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

        template < enum_kind Enum >
        [[nodiscard]] evalue value_to_evalue(Enum value) const;
        [[nodiscard]] evalue value_to_evalue(const uvalue& value) const;
        [[nodiscard]] evalue name_to_evalue(std::string_view name) const noexcept;
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
    template < meta_hpp::type_family Type >
    struct hash<Type> {
        size_t operator()(const Type& type) const noexcept {
            return type.get_hash();
        }
    };
}

namespace meta_hpp
{
    template < type_family TypeL, type_family TypeR >
    [[nodiscard]] bool operator==(const TypeL& l, const TypeR& r) noexcept {
        return l.is_valid() == r.is_valid() && (!l.is_valid() || l.get_id() == r.get_id());
    }

    template < type_family TypeL, type_family TypeR >
    [[nodiscard]] std::strong_ordering operator<=>(const TypeL& l, const TypeR& r) noexcept {
        if ( const std::strong_ordering cmp{l.is_valid() <=> r.is_valid()}; cmp != std::strong_ordering::equal ) {
            return cmp;
        }
        return l.is_valid() ? l.get_id() <=> r.get_id() : std::strong_ordering::equal;
    }
}

namespace meta_hpp
{
    template < type_family Type >
    [[nodiscard]] bool operator==(const Type& l, const typename Type::id_type& r) noexcept {
        return l.is_valid() && l.get_id() == r;
    }

    template < type_family Type >
    [[nodiscard]] std::strong_ordering operator<=>(const Type& l, const typename Type::id_type& r) noexcept {
        return l.is_valid() ? l.get_id() <=> r : std::strong_ordering::less;
    }
}

namespace meta_hpp::detail
{
    struct type_data_base {
        // NOLINTBEGIN(*-avoid-const-or-ref-data-members)
        const type_kind kind;
        const std::size_t shared;
        // NOLINTEND(*-avoid-const-or-ref-data-members)

        metadata_map metadata;

        explicit type_data_base(type_kind nkind, std::size_t nshared)
        : kind{nkind}
        , shared{nshared} {}

        type_data_base(type_data_base&&) = delete;
        type_data_base(const type_data_base&) = delete;
        type_data_base& operator=(type_data_base&&) = delete;
        type_data_base& operator=(const type_data_base&) = delete;

    protected:
        ~type_data_base() = default;
    };

    struct array_type_data final : type_data_base {
        // NOLINTBEGIN(*-avoid-const-or-ref-data-members)
        const array_bitflags flags;
        const std::size_t extent;
        const any_type data_type;
        // NOLINTEND(*-avoid-const-or-ref-data-members)

        template < array_kind Array >
        explicit array_type_data(array_traits<Array>);
    };

    struct class_type_data final : type_data_base {
        // NOLINTBEGIN(*-avoid-const-or-ref-data-members)
        const class_bitflags flags;
        const std::size_t size;
        const std::size_t align;
        const any_type_list argument_types;
        const uvalue_list argument_values;
        // NOLINTEND(*-avoid-const-or-ref-data-members)

        class_list base_classes;
        constructor_list constructors;
        destructor_list destructors;
        function_list functions;
        member_list members;
        method_list methods;
        typedef_map typedefs;
        variable_list variables;

        struct upcast_func_t final {
            using upcast_t = void* (*)(void*);

            type_id target{};
            upcast_t upcast{};

            [[nodiscard]] void* apply(void* ptr) const noexcept;
            [[nodiscard]] const void* apply(const void* ptr) const noexcept;
        };

        using deep_upcasts_t = std::vector<upcast_func_t>;
        deep_upcasts_t deep_upcasts;

        template < class_kind Class >
        explicit class_type_data(class_traits<Class>);
    };

    struct constructor_type_data final : type_data_base {
        // NOLINTBEGIN(*-avoid-const-or-ref-data-members)
        const constructor_bitflags flags;
        const class_type owner_type;
        const any_type_list argument_types;
        // NOLINTEND(*-avoid-const-or-ref-data-members)

        template < class_kind Class, typename... Args >
        explicit constructor_type_data(constructor_traits<Class, Args...>);
    };

    struct destructor_type_data final : type_data_base {
        // NOLINTBEGIN(*-avoid-const-or-ref-data-members)
        const destructor_bitflags flags;
        const class_type owner_type;
        // NOLINTEND(*-avoid-const-or-ref-data-members)

        template < class_kind Class >
        explicit destructor_type_data(destructor_traits<Class>);
    };

    struct enum_type_data final : type_data_base {
        // NOLINTBEGIN(*-avoid-const-or-ref-data-members)
        const enum_bitflags flags;
        const number_type underlying_type;
        // NOLINTEND(*-avoid-const-or-ref-data-members)

        evalue_list evalues;

        template < enum_kind Enum >
        explicit enum_type_data(enum_traits<Enum>);
    };

    struct function_type_data final : type_data_base {
        // NOLINTBEGIN(*-avoid-const-or-ref-data-members)
        const function_bitflags flags;
        const any_type return_type;
        const any_type_list argument_types;
        // NOLINTEND(*-avoid-const-or-ref-data-members)

        template < function_kind Function >
        explicit function_type_data(function_traits<Function>);
    };

    struct member_type_data final : type_data_base {
        // NOLINTBEGIN(*-avoid-const-or-ref-data-members)
        const member_bitflags flags;
        const class_type owner_type;
        const any_type value_type;
        // NOLINTEND(*-avoid-const-or-ref-data-members)

        template < member_pointer_kind Member >
        explicit member_type_data(member_traits<Member>);
    };

    struct method_type_data final : type_data_base {
        // NOLINTBEGIN(*-avoid-const-or-ref-data-members)
        const method_bitflags flags;
        const class_type owner_type;
        const any_type return_type;
        const any_type_list argument_types;
        // NOLINTEND(*-avoid-const-or-ref-data-members)

        template < method_pointer_kind Method >
        explicit method_type_data(method_traits<Method>);
    };

    struct nullptr_type_data final : type_data_base {
        template < nullptr_kind Nullptr >
        explicit nullptr_type_data(nullptr_traits<Nullptr>);
    };

    struct number_type_data final : type_data_base {
        // NOLINTBEGIN(*-avoid-const-or-ref-data-members)
        const number_bitflags flags;
        const std::size_t size;
        const std::size_t align;
        // NOLINTEND(*-avoid-const-or-ref-data-members)

        template < number_kind Number >
        explicit number_type_data(number_traits<Number>);
    };

    struct pointer_type_data final : type_data_base {
        // NOLINTBEGIN(*-avoid-const-or-ref-data-members)
        const pointer_bitflags flags;
        const any_type data_type;
        // NOLINTEND(*-avoid-const-or-ref-data-members)

        template < pointer_kind Pointer >
        explicit pointer_type_data(pointer_traits<Pointer>);
    };

    struct reference_type_data final : type_data_base {
        // NOLINTBEGIN(*-avoid-const-or-ref-data-members)
        const reference_bitflags flags;
        const any_type data_type;
        // NOLINTEND(*-avoid-const-or-ref-data-members)

        template < reference_kind Reference >
        explicit reference_type_data(reference_traits<Reference>);
    };

    struct void_type_data final : type_data_base {
        template < void_kind Void >
        explicit void_type_data(void_traits<Void>);
    };
}

namespace meta_hpp
{
    inline type_id::type_id(const detail::type_data_base* data)
    : data_{data} {}

    inline bool type_id::is_valid() const noexcept {
        return data_ != nullptr;
    }

    inline type_id::operator bool() const noexcept {
        return is_valid();
    }

    inline void type_id::swap(type_id& other) noexcept {
        std::swap(data_, other.data_);
    }

    inline std::size_t type_id::get_hash() const noexcept {
        return data_ != nullptr ? data_->shared : 0;
    }

    inline bool type_id::operator==(const type_id& other) const noexcept {
        if ( data_ == other.data_ ) {
            return true;
        }

        if ( is_valid() != other.is_valid() ) {
            return false;
        }

        return data_->shared == other.data_->shared;
    }

    inline std::strong_ordering type_id::operator<=>(const type_id& other) const noexcept {
        if ( data_ == other.data_ ) {
            return std::strong_ordering::equal;
        }

        // NOLINTNEXTLINE(*-bool-conversion)
        if ( const std::strong_ordering cmp{is_valid() <=> other.is_valid()}; cmp != std::strong_ordering::equal ) {
            return cmp;
        }

        return data_->shared <=> other.data_->shared;
    }
}

namespace meta_hpp
{
    template < type_family Type >
    type_base<Type>::type_base(data_ptr data)
    : data_{std::move(data)} {}

    template < type_family Type >
    bool type_base<Type>::is_valid() const noexcept {
        return data_ != nullptr;
    }

    template < type_family Type >
    type_base<Type>::operator bool() const noexcept {
        return is_valid();
    }

    template < type_family Type >
    std::size_t type_base<Type>::get_hash() const noexcept {
        return get_id().get_hash();
    }

    template < type_family Type >
    typename type_base<Type>::id_type type_base<Type>::get_id() const noexcept {
        return id_type{data_};
    }

    template < type_family Type >
    type_kind type_base<Type>::get_kind() const noexcept {
        return data_->kind;
    }

    template < type_family Type >
    const metadata_map& type_base<Type>::get_metadata() const noexcept {
        return data_->metadata;
    }
}
