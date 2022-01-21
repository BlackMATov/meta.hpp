/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_base.hpp"
#include "meta_traits.hpp"

namespace meta_hpp
{
    class type_id final {
    public:
        template < typename T >
        // NOLINTNEXTLINE(readability-named-parameter)
        explicit type_id(detail::type_list<T>) noexcept
        : id_{type_to_id<T>()} {}

        type_id(type_id&&) = default;
        type_id(const type_id&) = default;

        type_id& operator=(type_id&&) = default;
        type_id& operator=(const type_id&) = default;

        ~type_id() = default;

        [[nodiscard]] std::size_t get_hash() const noexcept {
            return std::hash<underlying_type>{}(id_);
        }

        [[nodiscard]] friend bool operator<(type_id l, type_id r) noexcept {
            return l.id_ < r.id_;
        }

        [[nodiscard]] friend bool operator==(type_id l, type_id r) noexcept {
            return l.id_ == r.id_;
        }

        [[nodiscard]] friend bool operator!=(type_id l, type_id r) noexcept {
            return l.id_ != r.id_;
        }
    private:
        using underlying_type = std::uint32_t;
        underlying_type id_{};
    private:
        [[nodiscard]] static underlying_type next() noexcept {
            static std::atomic<underlying_type> id{};
            return ++id;
        }

        template < typename T >
        [[nodiscard]] static underlying_type type_to_id() noexcept {
            static const underlying_type id{next()};
            return id;
        }
    };
}

namespace meta_hpp
{
    namespace detail
    {
        template < typename T >
        concept type_family =
            std::is_same_v<T, any_type> ||
            std::is_same_v<T, array_type> ||
            std::is_same_v<T, class_type> ||
            std::is_same_v<T, ctor_type> ||
            std::is_same_v<T, dtor_type> ||
            std::is_same_v<T, enum_type> ||
            std::is_same_v<T, function_type> ||
            std::is_same_v<T, member_type> ||
            std::is_same_v<T, method_type> ||
            std::is_same_v<T, nullptr_type> ||
            std::is_same_v<T, number_type> ||
            std::is_same_v<T, pointer_type> ||
            std::is_same_v<T, reference_type> ||
            std::is_same_v<T, void_type>;

        template < type_family T >
        [[nodiscard]] auto data_access(const T& type) {
            return type.data_;
        }
    }

    template < detail::type_family T, detail::type_family U >
    [[nodiscard]] bool operator<(const T& l, const U& r) noexcept {
        if ( !static_cast<bool>(r) ) {
            return false;
        }

        if ( !static_cast<bool>(l) ) {
            return true;
        }

        return l.get_id() < r.get_id();
    }

    template < detail::type_family T, detail::type_family U >
    [[nodiscard]] bool operator==(const T& l, const U& r) noexcept {
        if ( static_cast<bool>(l) != static_cast<bool>(r) ) {
            return false;
        }

        if ( !static_cast<bool>(l) ) {
            return true;
        }

        return l.get_id() == r.get_id();
    }

    template < detail::type_family T, detail::type_family U >
    [[nodiscard]] bool operator!=(const T& l, const U& r) noexcept {
        return !(l == r);
    }
}

namespace meta_hpp
{
    class any_type final {
    public:
        explicit any_type() = default;
        explicit any_type(detail::type_data_base_ptr data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] type_kind get_kind() const noexcept;

        any_type(const array_type& other) noexcept;
        any_type(const class_type& other) noexcept;
        any_type(const ctor_type& other) noexcept;
        any_type(const dtor_type& other) noexcept;
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
        [[nodiscard]] bool is_ctor() const noexcept;
        [[nodiscard]] bool is_dtor() const noexcept;
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
        [[nodiscard]] ctor_type as_ctor() const noexcept;
        [[nodiscard]] dtor_type as_dtor() const noexcept;
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
        detail::type_data_base_ptr data_;
        friend auto detail::data_access<any_type>(const any_type&);
    };

    class array_type final {
    public:
        array_type() = default;
        array_type(detail::array_type_data_ptr data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] bitflags<array_flags> get_flags() const noexcept;

        [[nodiscard]] std::size_t get_extent() const noexcept;
        [[nodiscard]] any_type get_data_type() const noexcept;
    private:
        detail::array_type_data_ptr data_;
        friend auto detail::data_access<array_type>(const array_type&);
    };

    class class_type final {
    public:
        class_type() = default;
        class_type(detail::class_type_data_ptr data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] bitflags<class_flags> get_flags() const noexcept;

        [[nodiscard]] std::size_t get_size() const noexcept;

        [[nodiscard]] std::size_t get_arity() const noexcept;
        [[nodiscard]] any_type get_argument_type(std::size_t index) const noexcept;
        [[nodiscard]] const std::vector<any_type>& get_argument_types() const noexcept;

        [[nodiscard]] const ctor_map& get_ctors() const noexcept;
        [[nodiscard]] const dtor_map& get_dtors() const noexcept;
        [[nodiscard]] const class_set& get_bases() const noexcept;
        [[nodiscard]] const function_map& get_functions() const noexcept;
        [[nodiscard]] const member_map& get_members() const noexcept;
        [[nodiscard]] const method_map& get_methods() const noexcept;
        [[nodiscard]] const variable_map& get_variables() const noexcept;

        template < typename... Args >
        [[nodiscard]] std::optional<value> create(Args&&... args) const;

        template < typename... Args >
        [[nodiscard]] std::optional<value> operator()(Args&&... args) const;

        template < typename Arg >
        bool destroy(Arg&& ptr) const;

        template < detail::class_kind Derived >
        [[nodiscard]] bool is_base_of() const noexcept;
        [[nodiscard]] bool is_base_of(const class_type& derived) const noexcept;

        template < detail::class_kind Base >
        [[nodiscard]] bool is_derived_from() const noexcept;
        [[nodiscard]] bool is_derived_from(const class_type& base) const noexcept;

        [[nodiscard]] function get_function(std::string_view name) const noexcept;
        [[nodiscard]] member get_member(std::string_view name) const noexcept;
        [[nodiscard]] method get_method(std::string_view name) const noexcept;
        [[nodiscard]] variable get_variable(std::string_view name) const noexcept;

        template < typename... Args >
        [[nodiscard]] ctor get_ctor_with() const noexcept;
        [[nodiscard]] ctor get_ctor_with(const std::vector<any_type>& args) const noexcept;
        [[nodiscard]] ctor get_ctor_with(std::initializer_list<any_type> args) const noexcept;

        template < typename... Args >
        [[nodiscard]] function get_function_with(std::string_view name) const noexcept;
        [[nodiscard]] function get_function_with(std::string_view name, const std::vector<any_type>& args) const noexcept;
        [[nodiscard]] function get_function_with(std::string_view name, std::initializer_list<any_type> args) const noexcept;

        template < typename... Args >
        [[nodiscard]] method get_method_with(std::string_view name) const noexcept;
        [[nodiscard]] method get_method_with(std::string_view name, const std::vector<any_type>& args) const noexcept;
        [[nodiscard]] method get_method_with(std::string_view name, std::initializer_list<any_type> args) const noexcept;
    private:
        detail::class_type_data_ptr data_;
        friend auto detail::data_access<class_type>(const class_type&);
    };

    class ctor_type final {
    public:
        ctor_type() = default;
        ctor_type(detail::ctor_type_data_ptr data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] bitflags<ctor_flags> get_flags() const noexcept;

        [[nodiscard]] std::size_t get_arity() const noexcept;
        [[nodiscard]] any_type get_class_type() const noexcept;
        [[nodiscard]] any_type get_argument_type(std::size_t index) const noexcept;
        [[nodiscard]] const std::vector<any_type>& get_argument_types() const noexcept;
    private:
        detail::ctor_type_data_ptr data_;
        friend auto detail::data_access<ctor_type>(const ctor_type&);
    };

    class dtor_type final {
    public:
        dtor_type() = default;
        dtor_type(detail::dtor_type_data_ptr data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] bitflags<dtor_flags> get_flags() const noexcept;

        [[nodiscard]] any_type get_class_type() const noexcept;
    private:
        detail::dtor_type_data_ptr data_;
        friend auto detail::data_access<dtor_type>(const dtor_type&);
    };

    class enum_type final {
    public:
        enum_type() = default;
        enum_type(detail::enum_type_data_ptr data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] bitflags<enum_flags> get_flags() const noexcept;

        [[nodiscard]] number_type get_underlying_type() const noexcept;

        [[nodiscard]] const evalue_map& get_evalues() const noexcept;

        [[nodiscard]] evalue get_evalue(std::string_view name) const noexcept;

        template < typename Value >
        [[nodiscard]] std::optional<std::string> value_to_name(Value&& value) const noexcept;
        [[nodiscard]] std::optional<value> name_to_value(std::string_view name) const noexcept;
    private:
        detail::enum_type_data_ptr data_;
        friend auto detail::data_access<enum_type>(const enum_type&);
    };

    class function_type final {
    public:
        function_type() = default;
        function_type(detail::function_type_data_ptr data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] bitflags<function_flags> get_flags() const noexcept;

        [[nodiscard]] std::size_t get_arity() const noexcept;
        [[nodiscard]] any_type get_return_type() const noexcept;
        [[nodiscard]] any_type get_argument_type(std::size_t index) const noexcept;
        [[nodiscard]] const std::vector<any_type>& get_argument_types() const noexcept;
    private:
        detail::function_type_data_ptr data_;
        friend auto detail::data_access<function_type>(const function_type&);
    };

    class member_type final {
    public:
        member_type() = default;
        member_type(detail::member_type_data_ptr data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] bitflags<member_flags> get_flags() const noexcept;

        [[nodiscard]] class_type get_owner_type() const noexcept;
        [[nodiscard]] any_type get_value_type() const noexcept;
    private:
        detail::member_type_data_ptr data_;
        friend auto detail::data_access<member_type>(const member_type&);
    };

    class method_type final {
    public:
        method_type() = default;
        method_type(detail::method_type_data_ptr data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] bitflags<method_flags> get_flags() const noexcept;

        [[nodiscard]] std::size_t get_arity() const noexcept;
        [[nodiscard]] class_type get_owner_type() const noexcept;
        [[nodiscard]] any_type get_return_type() const noexcept;
        [[nodiscard]] any_type get_argument_type(std::size_t index) const noexcept;
        [[nodiscard]] const std::vector<any_type>& get_argument_types() const noexcept;
    private:
        detail::method_type_data_ptr data_;
        friend auto detail::data_access<method_type>(const method_type&);
    };

    class nullptr_type final {
    public:
        nullptr_type() = default;
        nullptr_type(detail::nullptr_type_data_ptr data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
    private:
        detail::nullptr_type_data_ptr data_;
        friend auto detail::data_access<nullptr_type>(const nullptr_type&);
    };

    class number_type final {
    public:
        number_type() = default;
        number_type(detail::number_type_data_ptr data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] bitflags<number_flags> get_flags() const noexcept;

        [[nodiscard]] std::size_t get_size() const noexcept;
    private:
        detail::number_type_data_ptr data_;
        friend auto detail::data_access<number_type>(const number_type&);
    };

    class pointer_type final {
    public:
        pointer_type() = default;
        pointer_type(detail::pointer_type_data_ptr data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] bitflags<pointer_flags> get_flags() const noexcept;

        [[nodiscard]] any_type get_data_type() const noexcept;
    private:
        detail::pointer_type_data_ptr data_;
        friend auto detail::data_access<pointer_type>(const pointer_type&);
    };

    class reference_type final {
    public:
        reference_type() = default;
        reference_type(detail::reference_type_data_ptr data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] bitflags<reference_flags> get_flags() const noexcept;

        [[nodiscard]] any_type get_data_type() const noexcept;
    private:
        detail::reference_type_data_ptr data_;
        friend auto detail::data_access<reference_type>(const reference_type&);
    };

    class void_type final {
    public:
        void_type() = default;
        void_type(detail::void_type_data_ptr data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
    private:
        detail::void_type_data_ptr data_;
        friend auto detail::data_access<void_type>(const void_type&);
    };
}

namespace meta_hpp::detail
{
    struct type_data_base {
        const type_id id;
        const type_kind kind;

        explicit type_data_base(type_id id, type_kind kind)
        : id{id}
        , kind{kind} {}
    };

    struct array_type_data final : type_data_base {
        const bitflags<array_flags> flags;
        const std::size_t extent;
        const any_type data_type;

        template < array_kind Array >
        explicit array_type_data(type_list<Array>);

        template < array_kind Array >
        [[nodiscard]] static array_type_data_ptr get_static();
    };

    struct class_type_data final : type_data_base {
        const bitflags<class_flags> flags;
        const std::size_t size;
        const std::vector<any_type> argument_types;

        ctor_map ctors;
        dtor_map dtors;
        class_set bases;
        function_map functions;
        member_map members;
        method_map methods;
        variable_map variables;

        struct base_info final {
            using upcast_fptr = void*(*)(void*);
            const upcast_fptr upcast;
        };

        using base_info_map = std::map<class_type, base_info, std::less<>>;
        base_info_map bases_info;

        template < class_kind Class >
        explicit class_type_data(type_list<Class>);

        template < class_kind Class >
        [[nodiscard]] static class_type_data_ptr get_static();
    };

    struct ctor_type_data final : type_data_base {
        const bitflags<ctor_flags> flags;
        const any_type class_type;
        const std::vector<any_type> argument_types;

        template < class_kind Class, typename... Args >
        explicit ctor_type_data(type_list<Class>, type_list<Args...>);

        template < class_kind Class, typename... Args >
        [[nodiscard]] static ctor_type_data_ptr get_static();
    };

    struct dtor_type_data final : type_data_base {
        const bitflags<dtor_flags> flags;
        const any_type class_type;

        template < class_kind Class >
        explicit dtor_type_data(type_list<Class>);

        template < class_kind Class >
        [[nodiscard]] static dtor_type_data_ptr get_static();
    };

    struct enum_type_data final : type_data_base {
        const bitflags<enum_flags> flags;
        const number_type underlying_type;

        evalue_map evalues;

        template < enum_kind Enum >
        explicit enum_type_data(type_list<Enum>);

        template < enum_kind Enum >
        [[nodiscard]] static enum_type_data_ptr get_static();
    };

    struct function_type_data final : type_data_base {
        const bitflags<function_flags> flags;
        const any_type return_type;
        const std::vector<any_type> argument_types;

        template < function_kind Function >
        explicit function_type_data(type_list<Function>);

        template < function_kind Function >
        [[nodiscard]] static function_type_data_ptr get_static();
    };

    struct member_type_data final : type_data_base {
        const bitflags<member_flags> flags;
        const class_type owner_type;
        const any_type value_type;

        template < member_kind Member >
        explicit member_type_data(type_list<Member>);

        template < member_kind Member >
        [[nodiscard]] static member_type_data_ptr get_static();
    };

    struct method_type_data final : type_data_base {
        const bitflags<method_flags> flags;
        const class_type owner_type;
        const any_type return_type;
        const std::vector<any_type> argument_types;

        template < method_kind Method >
        explicit method_type_data(type_list<Method>);

        template < method_kind Method >
        [[nodiscard]] static method_type_data_ptr get_static();
    };

    struct nullptr_type_data final : type_data_base {
        template < nullptr_kind Nullptr >
        explicit nullptr_type_data(type_list<Nullptr>);

        template < nullptr_kind Nullptr >
        [[nodiscard]] static nullptr_type_data_ptr get_static();
    };

    struct number_type_data final : type_data_base {
        const bitflags<number_flags> flags;
        const std::size_t size;

        template < number_kind Number >
        explicit number_type_data(type_list<Number>);

        template < number_kind Number >
        [[nodiscard]] static number_type_data_ptr get_static();
    };

    struct pointer_type_data final : type_data_base {
        const bitflags<pointer_flags> flags;
        const any_type data_type;

        template < pointer_kind Pointer >
        explicit pointer_type_data(type_list<Pointer>);

        template < pointer_kind Pointer >
        [[nodiscard]] static pointer_type_data_ptr get_static();
    };

    struct reference_type_data final : type_data_base {
        const bitflags<reference_flags> flags;
        const any_type data_type;

        template < reference_kind Reference >
        explicit reference_type_data(type_list<Reference>);

        template < reference_kind Reference >
        [[nodiscard]] static reference_type_data_ptr get_static();
    };

    struct void_type_data final : type_data_base {
        template < void_kind Void >
        explicit void_type_data(type_list<Void>);

        template < void_kind Void >
        [[nodiscard]] static void_type_data_ptr get_static();
    };
}

namespace meta_hpp
{
    struct ctor_index final {
        const ctor_type type;

        explicit ctor_index(ctor_type type)
        : type{std::move(type)} {}

        [[nodiscard]] friend bool operator<(const ctor_index& l, const ctor_index& r) noexcept {
            return l.type < r.type;
        }

        [[nodiscard]] friend bool operator==(const ctor_index& l, const ctor_index& r) noexcept {
            return l.type == r.type;
        }
    };

    struct dtor_index final {
        const dtor_type type;

        explicit dtor_index(dtor_type type)
        : type{std::move(type)} {}

        [[nodiscard]] friend bool operator<(const dtor_index& l, const dtor_index& r) noexcept {
            return l.type < r.type;
        }

        [[nodiscard]] friend bool operator==(const dtor_index& l, const dtor_index& r) noexcept {
            return l.type == r.type;
        }
    };

    struct evalue_index final {
        const enum_type type;
        const std::string name;

        explicit evalue_index(enum_type type, std::string name)
        : type{std::move(type)}
        , name{std::move(name)} {}

        [[nodiscard]] friend bool operator<(const evalue_index& l, const evalue_index& r) noexcept {
            return (l.type < r.type) || (l.type == r.type && l.name < r.name);
        }

        [[nodiscard]] friend bool operator==(const evalue_index& l, const evalue_index& r) noexcept {
            return l.type == r.type && l.name == r.name;
        }
    };

    struct function_index final {
        const function_type type;
        const std::string name;

        explicit function_index(function_type type, std::string name)
        : type{std::move(type)}
        , name{std::move(name)} {}

        [[nodiscard]] friend bool operator<(const function_index& l, const function_index& r) noexcept {
            return (l.type < r.type) || (l.type == r.type && l.name < r.name);
        }

        [[nodiscard]] friend bool operator==(const function_index& l, const function_index& r) noexcept {
            return l.type == r.type && l.name == r.name;
        }
    };

    struct member_index final {
        const member_type type;
        const std::string name;

        explicit member_index(member_type type, std::string name)
        : type{std::move(type)}
        , name{std::move(name)} {}

        [[nodiscard]] friend bool operator<(const member_index& l, const member_index& r) noexcept {
            return (l.type < r.type) || (l.type == r.type && l.name < r.name);
        }

        [[nodiscard]] friend bool operator==(const member_index& l, const member_index& r) noexcept {
            return l.type == r.type && l.name == r.name;
        }
    };

    struct method_index final {
        const method_type type;
        const std::string name;

        explicit method_index(method_type type, std::string name)
        : type{std::move(type)}
        , name{std::move(name)} {}

        [[nodiscard]] friend bool operator<(const method_index& l, const method_index& r) noexcept {
            return (l.type < r.type) || (l.type == r.type && l.name < r.name);
        }

        [[nodiscard]] friend bool operator==(const method_index& l, const method_index& r) noexcept {
            return l.type == r.type && l.name == r.name;
        }
    };

    struct scope_index final {
        const std::string name;

        explicit scope_index(std::string name)
        : name{std::move(name)} {}

        [[nodiscard]] friend bool operator<(const scope_index& l, const scope_index& r) noexcept {
            return l.name < r.name;
        }

        [[nodiscard]] friend bool operator==(const scope_index& l, const scope_index& r) noexcept {
            return l.name == r.name;
        }
    };

    struct variable_index final {
        const pointer_type type;
        const std::string name;

        explicit variable_index(pointer_type type, std::string name)
        : type{std::move(type)}
        , name{std::move(name)} {}

        [[nodiscard]] friend bool operator<(const variable_index& l, const variable_index& r) noexcept {
            return (l.type < r.type) || (l.type == r.type && l.name < r.name);
        }

        [[nodiscard]] friend bool operator==(const variable_index& l, const variable_index& r) noexcept {
            return l.type == r.type && l.name == r.name;
        }
    };
}

namespace meta_hpp
{
    template < typename T >
    [[nodiscard]] auto resolve_type() {
        using raw_type = std::remove_cv_t<T>;

        using kind_type = detail::kind_type<raw_type>;
        using kind_type_data = detail::kind_type_data<raw_type>;

        return kind_type{kind_type_data::template get_static<raw_type>()};
    }

    template < typename T >
    // NOLINTNEXTLINE(readability-named-parameter)
    [[nodiscard]] auto resolve_type(const T&) {
        return resolve_type<std::remove_reference_t<T>>();
    }
}

namespace meta_hpp
{
    namespace detail
    {
        [[nodiscard]] inline void* pointer_upcast(void* ptr, const class_type& from, const class_type& to) {
            if ( nullptr == ptr || !from || !to ) {
                return nullptr;
            }

            if ( from == to ) {
                return ptr;
            }

            for ( auto&& [base, base_info] : data_access(from)->bases_info ) {
                if ( base == to ) {
                    return base_info.upcast(ptr);
                }

                if ( base.is_derived_from(to) ) {
                    return pointer_upcast(base_info.upcast(ptr), base, to);
                }
            }

            return nullptr;
        }
    }

    template < detail::class_kind Base, detail::class_kind Derived >
    [[nodiscard]] Base* pointer_upcast(Derived* ptr) {
        const class_type& base = resolve_type<Base>();
        const class_type& derived = resolve_type<Derived>();
        return static_cast<Base*>(detail::pointer_upcast(ptr, derived, base));
    }

    template < detail::class_kind Base, detail::class_kind Derived >
    [[nodiscard]] const Base* pointer_upcast(const Derived* ptr) {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
        return pointer_upcast<Base>(const_cast<Derived*>(ptr));
    }
}
