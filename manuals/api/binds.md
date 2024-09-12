- [API Binds](#api-binds)
  - [Classes](#classes)
    - [array\_bind](#array_bind)
    - [class\_bind](#class_bind)
    - [enum\_bind](#enum_bind)
    - [function\_bind](#function_bind)
    - [member\_bind](#member_bind)
    - [method\_bind](#method_bind)
    - [nullptr\_bind](#nullptr_bind)
    - [number\_bind](#number_bind)
    - [pointer\_bind](#pointer_bind)
    - [reference\_bind](#reference_bind)
    - [void\_bind](#void_bind)
    - [scope\_bind](#scope_bind)
    - [arguments\_bind](#arguments_bind)
    - [metadata\_bind](#metadata_bind)
  - [Functions](#functions)
    - [bind\_](#bind_)
    - [array\_](#array_)
    - [class\_](#class_)
    - [enum\_](#enum_)
    - [function\_](#function_)
    - [member\_](#member_)
    - [method\_](#method_)
    - [nullptr\_](#nullptr_)
    - [number\_](#number_)
    - [pointer\_](#pointer_)
    - [reference\_](#reference_)
    - [void\_](#void_)
    - [local\_scope\_](#local_scope_)
    - [static\_scope\_](#static_scope_)
    - [extend\_scope\_](#extend_scope_)
    - [arguments\_](#arguments_)
    - [metadata\_](#metadata_)

# API Binds

## Classes

### array_bind

```cpp
template < array_kind Array >
class array_bind final : public type_bind_base<array_type> {
public:
    explicit array_bind(metadata_map metadata);
};
```

### class_bind

```cpp
template < class_kind Class >
class class_bind final : public type_bind_base<class_type> {
public:
    explicit class_bind(metadata_map metadata);

    template < typename... Args, typename... Opts >
        requires class_constructor_kind<Class, Args...>
    class_bind& constructor_(Opts&&... opts);

    template < typename... Opts >
        requires class_destructor_kind<Class>
    class_bind& destructor_(Opts&&... opts);

    template < function_pointer_kind Function, typename... Opts >
    class_bind& function_(std::string name, Function function_ptr, Opts&&... opts);

    template < class_member_pointer_kind<Class> Member, typename... Opts >
    class_bind& member_(std::string name, Member member_ptr, Opts&&... opts);

    template < class_method_pointer_kind<Class> Method, typename... Opts >
    class_bind& method_(std::string name, Method method_ptr, Opts&&... opts);

    template < typename Type >
    class_bind& typedef_(std::string name);

    template < pointer_kind Pointer, typename... Opts >
    class_bind& variable_(std::string name, Pointer variable_ptr, Opts&&... opts);
};
```

### enum_bind

```cpp
template < enum_kind Enum >
class enum_bind final : public type_bind_base<enum_type> {
public:
    explicit enum_bind(metadata_map metadata);

    template < typename... Opts >
    enum_bind& evalue_(std::string name, Enum value, Opts&&... opts);
};
```

### function_bind

```cpp
template < function_kind Function >
class function_bind final : public type_bind_base<function_type> {
public:
    explicit function_bind(metadata_map metadata);
};
```

### member_bind

```cpp
template < member_pointer_kind Member >
class member_bind final : public type_bind_base<member_type> {
public:
    explicit member_bind(metadata_map metadata);
};
```

### method_bind

```cpp
template < method_pointer_kind Method >
class method_bind final : public type_bind_base<method_type> {
public:
    explicit method_bind(metadata_map metadata);
};
```

### nullptr_bind

```cpp
template < nullptr_kind Nullptr >
class nullptr_bind final : public type_bind_base<nullptr_type> {
public:
    explicit nullptr_bind(metadata_map metadata);
};
```

### number_bind

```cpp
template < number_kind Number >
class number_bind final : public type_bind_base<number_type> {
public:
    explicit number_bind(metadata_map metadata);
};
```

### pointer_bind

```cpp
template < pointer_kind Pointer >
class pointer_bind final : public type_bind_base<pointer_type> {
public:
    explicit pointer_bind(metadata_map metadata);
};
```

### reference_bind

```cpp
template < reference_kind Reference >
class reference_bind final : public type_bind_base<reference_type> {
public:
    explicit reference_bind(metadata_map metadata);
};
```

### void_bind

```cpp
template < void_kind Void >
class void_bind final : public type_bind_base<void_type> {
public:
    explicit void_bind(metadata_map metadata);
};
```

### scope_bind

```cpp
class scope_bind final : public state_bind_base<scope> {
public:
    explicit scope_bind(const scope& scope, metadata_map metadata);

    template < function_pointer_kind Function, typename... Opts >
    scope_bind& function_(std::string name, Function function_ptr, Opts&&... opts);

    template < typename Type >
    scope_bind& typedef_(std::string name);

    template < pointer_kind Pointer, typename... Opts >
    scope_bind& variable_(std::string name, Pointer variable_ptr, Opts&&... opts);
};
```

### arguments_bind

```cpp
class arguments_bind final {
public:
    arguments_bind() = default;
    ~arguments_bind() = default;

    arguments_bind(arguments_bind&&) = default;
    arguments_bind& operator=(arguments_bind&&) = default;

    arguments_bind(const arguments_bind&) = delete;
    arguments_bind& operator=(const arguments_bind&) = delete;

    arguments_bind& operator()(std::string name) &;
    arguments_bind operator()(std::string name) &&;

    arguments_bind& operator()(std::string name, metadata_map metadata) &;
    arguments_bind operator()(std::string name, metadata_map metadata) &&;

    arguments_bind& operator()(arguments_bind bind) &;
    arguments_bind operator()(arguments_bind bind) &&;

private:
    values_t values_;
};
```

### metadata_bind

```cpp
class metadata_bind final {
public:
    metadata_bind() = default;
    ~metadata_bind() = default;

    metadata_bind(metadata_bind&&) = default;
    metadata_bind& operator=(metadata_bind&&) = default;

    metadata_bind(const metadata_bind&) = delete;
    metadata_bind& operator=(const metadata_bind&) = delete;

    metadata_bind& operator()(std::string name, uvalue value) &;
    metadata_bind operator()(std::string name, uvalue value) &&;

    metadata_bind& operator()(metadata_bind bind) &;
    metadata_bind operator()(metadata_bind bind) &&;
};
```

## Functions

### bind_

```cpp
template < typename T >
auto bind_(metadata_map metadata = {});
```

### array_

```cpp
template < array_kind Array >
array_bind<Array> array_(metadata_map metadata = {});
```

### class_

```cpp
template < class_kind Class >
class_bind<Class> class_(metadata_map metadata = {});
```

### enum_

```cpp
template < enum_kind Enum >
enum_bind<Enum> enum_(metadata_map metadata = {});
```

### function_

```cpp
template < function_kind Function >
function_bind<Function> function_(metadata_map metadata = {});
```

### member_

```cpp
template < member_pointer_kind Member >
member_bind<Member> member_(metadata_map metadata = {});
```

### method_

```cpp
template < method_pointer_kind Method >
method_bind<Method> method_(metadata_map metadata = {});
```

### nullptr_

```cpp
template < nullptr_kind Nullptr >
nullptr_bind<Nullptr> nullptr_(metadata_map metadata = {});
```

### number_

```cpp
template < number_kind Number >
number_bind<Number> number_(metadata_map metadata = {});
```

### pointer_

```cpp
template < pointer_kind Pointer >
pointer_bind<Pointer> pointer_(metadata_map metadata = {});
```

### reference_

```cpp
template < reference_kind Reference >
reference_bind<Reference> reference_(metadata_map metadata = {});
```

### void_

```cpp
template < void_kind Void >
void_bind<Void> void_(metadata_map metadata = {});
```

### local_scope_

```cpp
scope_bind local_scope_(std::string name, metadata_map metadata = {});
```

### static_scope_

```cpp
scope_bind static_scope_(std::string_view name, metadata_map metadata = {});
```

### extend_scope_

```cpp
scope_bind extend_scope_(const scope& scope, metadata_map metadata = {});
```

### arguments_

```cpp
arguments_bind arguments_();
```

### metadata_

```cpp
metadata_bind metadata_();
```
