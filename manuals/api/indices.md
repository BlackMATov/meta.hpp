- [API Indices](#api-indices)
  - [Classes](#classes)
    - [argument\_index](#argument_index)
    - [constructor\_index](#constructor_index)
    - [destructor\_index](#destructor_index)
    - [evalue\_index](#evalue_index)
    - [function\_index](#function_index)
    - [member\_index](#member_index)
    - [method\_index](#method_index)
    - [scope\_index](#scope_index)
    - [variable\_index](#variable_index)

# API Indices

## Classes

### argument_index

```cpp
class argument_index final {
public:
    argument_index() = delete;
    explicit argument_index(any_type type, std::size_t position);

    any_type get_type() const noexcept;
    std::size_t get_position() const noexcept;

    void swap(argument_index& other) noexcept;
    std::size_t get_hash() const noexcept;
    std::strong_ordering operator<=>(const argument_index&) const = default;
};
```

### constructor_index

```cpp
class constructor_index final {
public:
    constructor_index() = delete;
    explicit constructor_index(constructor_type type);

    constructor_type get_type() const noexcept;

    void swap(constructor_index& other) noexcept;
    std::size_t get_hash() const noexcept;
    std::strong_ordering operator<=>(const constructor_index&) const = default;
};
```

### destructor_index

```cpp
class destructor_index final {
public:
    destructor_index() = delete;
    explicit destructor_index(destructor_type type);

    destructor_type get_type() const noexcept;

    void swap(destructor_index& other) noexcept;
    std::size_t get_hash() const noexcept;
    std::strong_ordering operator<=>(const destructor_index&) const = default;
};
```

### evalue_index

```cpp
class evalue_index final {
public:
    evalue_index() = delete;
    explicit evalue_index(enum_type type, std::string name);

    enum_type get_type() const noexcept;

    std::string get_name() && noexcept;
    const std::string& get_name() const& noexcept;

    void swap(evalue_index& other) noexcept;
    std::size_t get_hash() const noexcept;
    std::strong_ordering operator<=>(const evalue_index&) const = default;
};
```

### function_index

```cpp
class function_index final {
public:
    function_index() = delete;
    explicit function_index(function_type type, std::string name);

    function_type get_type() const noexcept;

    std::string get_name() && noexcept;
    const std::string& get_name() const& noexcept;

    void swap(function_index& other) noexcept;
    std::size_t get_hash() const noexcept;
    std::strong_ordering operator<=>(const function_index&) const = default;
};
```

### member_index

```cpp
class member_index final {
public:
    member_index() = delete;
    explicit member_index(member_type type, std::string name);

    member_type get_type() const noexcept;

    std::string get_name() && noexcept;
    const std::string& get_name() const& noexcept;

    void swap(member_index& other) noexcept;
    std::size_t get_hash() const noexcept;
    std::strong_ordering operator<=>(const member_index&) const = default;
};
```

### method_index

```cpp
class method_index final {
public:
    method_index() = delete;
    explicit method_index(method_type type, std::string name);

    method_type get_type() const noexcept;

    std::string get_name() && noexcept;
    const std::string& get_name() const& noexcept;

    void swap(method_index& other) noexcept;
    std::size_t get_hash() const noexcept;
    std::strong_ordering operator<=>(const method_index&) const = default;
};
```

### scope_index

```cpp
class scope_index final {
public:
    scope_index() = delete;
    explicit scope_index(std::string name);

    std::string get_name() && noexcept;
    const std::string& get_name() const& noexcept;

    void swap(scope_index& other) noexcept;
    std::size_t get_hash() const noexcept;
    std::strong_ordering operator<=>(const scope_index&) const = default;
};
```

### variable_index

```cpp
class variable_index final {
public:
    variable_index() = delete;
    explicit variable_index(pointer_type type, std::string name);

    pointer_type get_type() const noexcept;

    std::string get_name() && noexcept;
    const std::string& get_name() const& noexcept;

    void swap(variable_index& other) noexcept;
    std::size_t get_hash() const noexcept;
    std::strong_ordering operator<=>(const variable_index&) const = default;
};
```
