- [API Policies](#api-policies)
  - [Namespaces](#namespaces)
    - [constructor\_policy](#constructor_policy)
    - [function\_policy](#function_policy)
    - [member\_policy](#member_policy)
    - [method\_policy](#method_policy)
    - [variable\_policy](#variable_policy)

# API Policies

## Namespaces

### constructor_policy

```cpp
namespace constructor_policy
{
    inline constexpr struct as_object_t final {
    } as_object{};

    inline constexpr struct as_raw_pointer_t final {
    } as_raw_pointer{};

    inline constexpr struct as_shared_pointer_t final {
    } as_shared_pointer{};

    inline constexpr struct as_unique_pointer_t final {
    } as_unique_pointer{};
}
```

### function_policy

```cpp
namespace function_policy
{
    inline constexpr struct as_copy_t final {
    } as_copy{};

    inline constexpr struct discard_return_t final {
    } discard_return{};

    inline constexpr struct return_reference_as_pointer_t final {
    } return_reference_as_pointer{};
}
```

### member_policy

```cpp
namespace member_policy
{
    inline constexpr struct as_copy_t final {
    } as_copy{};

    inline constexpr struct as_pointer_t final {
    } as_pointer{};

    inline constexpr struct as_reference_wrapper_t final {
    } as_reference_wrapper{};
}
```

### method_policy

```cpp
namespace method_policy
{
    inline constexpr struct as_copy_t final {
    } as_copy{};

    inline constexpr struct discard_return_t final {
    } discard_return{};

    inline constexpr struct return_reference_as_pointer_t final {
    } return_reference_as_pointer{};
}
```

### variable_policy

```cpp
namespace variable_policy
{
    inline constexpr struct as_copy_t final {
    } as_copy{};

    inline constexpr struct as_pointer_t final {
    } as_pointer{};

    inline constexpr struct as_reference_wrapper_t final {
    } as_reference_wrapper{};
}
```
