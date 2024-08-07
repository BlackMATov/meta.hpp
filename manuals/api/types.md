- [API Types](#api-types)
  - [Classes](#classes)
    - [type\_id](#type_id)
    - [type\_base](#type_base)
    - [any\_type](#any_type)
    - [array\_type](#array_type)
    - [class\_type](#class_type)
    - [constructor\_type](#constructor_type)
    - [destructor\_type](#destructor_type)
    - [enum\_type](#enum_type)
    - [function\_type](#function_type)
    - [member\_type](#member_type)
    - [method\_type](#method_type)
    - [nullptr\_type](#nullptr_type)
    - [number\_type](#number_type)
    - [pointer\_type](#pointer_type)
    - [reference\_type](#reference_type)
    - [void\_type](#void_type)
  - [Enumerations](#enumerations)
    - [array\_flags](#array_flags)
    - [class\_flags](#class_flags)
    - [constructor\_flags](#constructor_flags)
    - [destructor\_flags](#destructor_flags)
    - [enum\_flags](#enum_flags)
    - [function\_flags](#function_flags)
    - [member\_flags](#member_flags)
    - [method\_flags](#method_flags)
    - [number\_flags](#number_flags)
    - [pointer\_flags](#pointer_flags)
    - [reference\_flags](#reference_flags)

# API Types

## Classes

### type_id

```cpp
class type_id final {
public:
    type_id() = default;
    ~type_id() = default;

    type_id(type_id&&) = default;
    type_id(const type_id&) = default;

    type_id& operator=(type_id&&) = default;
    type_id& operator=(const type_id&) = default;

    bool is_valid() const noexcept;
    explicit operator bool() const noexcept;

    void swap(type_id& other) noexcept;
    std::size_t get_hash() const noexcept;

    bool operator==(const type_id& other) const noexcept;
    std::strong_ordering operator<=>(const type_id& other) const noexcept;
};
```

### type_base

```cpp
template < type_family Type >
class type_base {
public:
    using id_type = type_id;

    type_base() = default;

    explicit type_base(data_ptr data);

    type_base(type_base&&) = default;
    type_base(const type_base&) = default;

    type_base& operator=(type_base&&) = default;
    type_base& operator=(const type_base&) = default;

    bool is_valid() const noexcept;
    explicit operator bool() const noexcept;

    std::size_t get_hash() const noexcept;

    id_type get_id() const noexcept;
    type_kind get_kind() const noexcept;
    const metadata_map& get_metadata() const noexcept;
};
```

### any_type

```cpp
class any_type final : public type_base<any_type> {
public:
    using type_base<any_type>::type_base;

    template < type_family Type >
    any_type(const Type& other) noexcept;

    template < type_family Type >
    bool is() const noexcept;
    bool is(type_kind kind) const noexcept;

    template < type_family Type >
    Type as() const noexcept;

    template < typename F >
    bool match(F&& f) const;

    bool is_array() const noexcept;
    bool is_class() const noexcept;
    bool is_constructor() const noexcept;
    bool is_destructor() const noexcept;
    bool is_enum() const noexcept;
    bool is_function() const noexcept;
    bool is_member() const noexcept;
    bool is_method() const noexcept;
    bool is_nullptr() const noexcept;
    bool is_number() const noexcept;
    bool is_pointer() const noexcept;
    bool is_reference() const noexcept;
    bool is_void() const noexcept;

    array_type as_array() const noexcept;
    class_type as_class() const noexcept;
    constructor_type as_constructor() const noexcept;
    destructor_type as_destructor() const noexcept;
    enum_type as_enum() const noexcept;
    function_type as_function() const noexcept;
    member_type as_member() const noexcept;
    method_type as_method() const noexcept;
    nullptr_type as_nullptr() const noexcept;
    number_type as_number() const noexcept;
    pointer_type as_pointer() const noexcept;
    reference_type as_reference() const noexcept;
    void_type as_void() const noexcept;
};
```

### array_type

```cpp
class array_type final : public type_base<array_type> {
public:
    using type_base<array_type>::type_base;
    array_bitflags get_flags() const noexcept;

    std::size_t get_extent() const noexcept;
    any_type get_data_type() const noexcept;
};
```

### class_type

```cpp
class class_type final : public type_base<class_type> {
public:
    using type_base<class_type>::type_base;
    class_bitflags get_flags() const noexcept;

    std::size_t get_size() const noexcept;
    std::size_t get_align() const noexcept;

    std::size_t get_arity() const noexcept;
    any_type get_argument_type(std::size_t position) const noexcept;
    const uvalue& get_argument_value(std::size_t position) const noexcept;
    const any_type_list& get_argument_types() const noexcept;
    const uvalue_list& get_argument_values() const noexcept;

    const class_list& get_base_classes() const noexcept;
    const constructor_list& get_constructors() const noexcept;
    const function_list& get_functions() const noexcept;
    const member_list& get_members() const noexcept;
    const method_list& get_methods() const noexcept;
    const typedef_map& get_typedefs() const noexcept;
    const variable_list& get_variables() const noexcept;

    template < typename... Args >
    uvalue create(Args&&... args) const;
    template < typename... Args >
    uvalue create_at(void* mem, Args&&... args) const;

    template < typename Arg >
    bool destroy(Arg&& arg) const;
    bool destroy_at(void* mem) const;

    template < class_kind Derived >
    bool is_base_of() const;
    bool is_base_of(const class_type& derived) const noexcept;

    template < class_kind Derived >
    bool is_direct_base_of() const;
    bool is_direct_base_of(const class_type& derived) const noexcept;

    template < class_kind Base >
    bool is_derived_from() const;
    bool is_derived_from(const class_type& base) const noexcept;

    template < class_kind Base >
    bool is_direct_derived_from() const;
    bool is_direct_derived_from(const class_type& base) const noexcept;

    function get_function(std::string_view name, bool recursively = true) const noexcept;
    member get_member(std::string_view name, bool recursively = true) const noexcept;
    method get_method(std::string_view name, bool recursively = true) const noexcept;
    any_type get_typedef(std::string_view name, bool recursively = true) const noexcept;
    variable get_variable(std::string_view name, bool recursively = true) const noexcept;

    template < typename... Args >
    constructor get_constructor_with() const;
    template < typename Iter >
    constructor get_constructor_with(Iter first, Iter last) const;
    constructor get_constructor_with(std::span<const any_type> args) const noexcept;
    constructor get_constructor_with(std::initializer_list<any_type> args) const noexcept;

    destructor get_destructor() const noexcept;

    template < typename... Args >
    function get_function_with(
        std::string_view name,
        bool recursively = true
    ) const;

    template < typename Iter >
    function get_function_with(
        std::string_view name,
        Iter first,
        Iter last,
        bool recursively = true
    ) const;

    function get_function_with(
        std::string_view name,
        std::span<const any_type> args,
        bool recursively = true
    ) const noexcept;

    function get_function_with(
        std::string_view name,
        std::initializer_list<any_type> args,
        bool recursively = true
    ) const noexcept;

    template < typename... Args >
    method get_method_with(
        std::string_view name,
        bool recursively = true
    ) const;

    template < typename Iter >
    method get_method_with(
        std::string_view name,
        Iter first,
        Iter last,
        bool recursively = true
    ) const;

    method get_method_with(
        std::string_view name,
        std::span<const any_type> args,
        bool recursively = true
    ) const noexcept;

    method get_method_with(
        std::string_view name,
        std::initializer_list<any_type> args,
        bool recursively = true
    ) const noexcept;
};
```

### constructor_type

```cpp
class constructor_type final : public type_base<constructor_type> {
public:
    using type_base<constructor_type>::type_base;
    constructor_bitflags get_flags() const noexcept;

    std::size_t get_arity() const noexcept;
    class_type get_owner_type() const noexcept;
    any_type get_argument_type(std::size_t position) const noexcept;
    const any_type_list& get_argument_types() const noexcept;
};
```

### destructor_type

```cpp
class destructor_type final : public type_base<destructor_type> {
public:
    using type_base<destructor_type>::type_base;
    destructor_bitflags get_flags() const noexcept;

    class_type get_owner_type() const noexcept;
};
```

### enum_type

```cpp
class enum_type final : public type_base<enum_type> {
public:
    using type_base<enum_type>::type_base;
    enum_bitflags get_flags() const noexcept;

    number_type get_underlying_type() const noexcept;

    const evalue_list& get_evalues() const noexcept;

    template < enum_kind Enum >
    evalue value_to_evalue(Enum value) const;
    evalue value_to_evalue(const uvalue& value) const;
    evalue name_to_evalue(std::string_view name) const noexcept;
};
```

### function_type

```cpp
class function_type final : public type_base<function_type> {
public:
    using type_base<function_type>::type_base;
    function_bitflags get_flags() const noexcept;

    std::size_t get_arity() const noexcept;
    any_type get_return_type() const noexcept;
    any_type get_argument_type(std::size_t position) const noexcept;
    const any_type_list& get_argument_types() const noexcept;
};
```

### member_type

```cpp
class member_type final : public type_base<member_type> {
public:
    using type_base<member_type>::type_base;
    member_bitflags get_flags() const noexcept;

    class_type get_owner_type() const noexcept;
    any_type get_value_type() const noexcept;
};
```

### method_type

```cpp
class method_type final : public type_base<method_type> {
public:
    using type_base<method_type>::type_base;
    method_bitflags get_flags() const noexcept;

    std::size_t get_arity() const noexcept;
    class_type get_owner_type() const noexcept;
    any_type get_return_type() const noexcept;
    any_type get_argument_type(std::size_t position) const noexcept;
    const any_type_list& get_argument_types() const noexcept;
};
```

### nullptr_type

```cpp
class nullptr_type final : public type_base<nullptr_type> {
public:
    using type_base<nullptr_type>::type_base;
};
```

### number_type

```cpp
class number_type final : public type_base<number_type> {
public:
    using type_base<number_type>::type_base;
    number_bitflags get_flags() const noexcept;

    std::size_t get_size() const noexcept;
    std::size_t get_align() const noexcept;
};
```

### pointer_type

```cpp
class pointer_type final : public type_base<pointer_type> {
public:
    using type_base<pointer_type>::type_base;
    pointer_bitflags get_flags() const noexcept;

    any_type get_data_type() const noexcept;
};
```

### reference_type

```cpp
class reference_type final : public type_base<reference_type> {
public:
    using type_base<reference_type>::type_base;
    reference_bitflags get_flags() const noexcept;

    any_type get_data_type() const noexcept;
};
```

### void_type

```cpp
class void_type final : public type_base<void_type> {
public:
    using type_base<void_type>::type_base;
};
```

## Enumerations

### array_flags

```cpp
enum class array_flags : std::uint8_t {
    is_readonly = 1 << 0,
    is_volatile = 1 << 1,
    is_bounded = 1 << 2,
    is_unbounded = 1 << 3,
};

using array_bitflags = bitflags<array_flags>;
```

### class_flags

```cpp
enum class class_flags : std::uint8_t {
    is_empty = 1 << 0,
    is_final = 1 << 1,
    is_abstract = 1 << 2,
    is_polymorphic = 1 << 3,
    is_template_instantiation = 1 << 4,
};

using class_bitflags = bitflags<class_flags>;
```

### constructor_flags

```cpp
enum class constructor_flags : std::uint8_t {
    is_noexcept = 1 << 0,
};

using constructor_bitflags = bitflags<constructor_flags>;
```

### destructor_flags

```cpp
enum class destructor_flags : std::uint8_t {
    is_noexcept = 1 << 0,
    is_virtual = 1 << 1,
};

using destructor_bitflags = bitflags<destructor_flags>;
```

### enum_flags

```cpp
enum class enum_flags : std::uint8_t {
    is_scoped = 1 << 0,
};

using enum_bitflags = bitflags<enum_flags>;
```

### function_flags

```cpp
enum class function_flags : std::uint8_t {
    is_noexcept = 1 << 0,
};

using function_bitflags = bitflags<function_flags>;
```

### member_flags

```cpp
enum class member_flags : std::uint8_t {
    is_readonly = 1 << 0,
    is_volatile = 1 << 1,
};

using member_bitflags = bitflags<member_flags>;
```

### method_flags

```cpp
enum class method_flags : std::uint8_t {
    is_const = 1 << 0,
    is_noexcept = 1 << 1,
    is_lvalue_qualified = 1 << 2,
    is_rvalue_qualified = 1 << 3,
};

using method_bitflags = bitflags<method_flags>;
```

### number_flags

```cpp
enum class number_flags : std::uint8_t {
    is_signed = 1 << 0,
    is_unsigned = 1 << 1,
    is_integral = 1 << 2,
    is_floating_point = 1 << 3,
};

using number_bitflags = bitflags<number_flags>;
```

### pointer_flags

```cpp
enum class pointer_flags : std::uint8_t {
    is_readonly = 1 << 0,
    is_volatile = 1 << 1,
};

using pointer_bitflags = bitflags<pointer_flags>;
```

### reference_flags

```cpp
enum class reference_flags : std::uint8_t {
    is_readonly = 1 << 0,
    is_volatile = 1 << 1,
    is_lvalue = 1 << 2,
    is_rvalue = 1 << 3,
};

using reference_bitflags = bitflags<reference_flags>;
```
