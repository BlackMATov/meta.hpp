- [API States](#api-states)
  - [Classes](#classes)
    - [state\_base](#state_base)
    - [argument](#argument)
    - [constructor](#constructor)
    - [destructor](#destructor)
    - [evalue](#evalue)
    - [function](#function)
    - [member](#member)
    - [method](#method)
    - [scope](#scope)
    - [variable](#variable)

# API States

## Classes

### state_base

```cpp
template < state_family State >
class state_base {
public:
    using index_type = typename detail::state_traits<State>::index_type;

    state_base() = default;

    explicit state_base(state_ptr state);

    state_base(state_base&&) = default;
    state_base(const state_base&) = default;

    state_base& operator=(state_base&&) = default;
    state_base& operator=(const state_base&) = default;

    bool is_valid() const noexcept;
    explicit operator bool() const noexcept;

    const index_type& get_index() const noexcept;
    const metadata_map& get_metadata() const noexcept;
};
```

### argument

```cpp
class argument final : public state_base<argument> {
public:
    using state_base<argument>::state_base;

    any_type get_type() const noexcept;
    std::size_t get_position() const noexcept;

    const std::string& get_name() const noexcept;
};
```

### constructor

```cpp
class constructor final : public state_base<constructor> {
public:
    using state_base<constructor>::state_base;

    constructor_type get_type() const noexcept;

    std::size_t get_arity() const noexcept;
    argument get_argument(std::size_t position) const noexcept;
    const argument_list& get_arguments() const noexcept;

    //

    template < typename... Args >
    uvalue create(Args&&... args) const;

    template < typename... Args >
    uresult try_create(Args&&... args) const;

    template < typename... Args >
    uvalue create_at(void* mem, Args&&... args) const;

    template < typename... Args >
    uresult try_create_at(void* mem, Args&&... args) const;

    template < typename... Args >
    bool is_invocable_with() const noexcept;

    template < typename... Args >
    bool is_invocable_with(Args&&... args) const noexcept;

    template < typename... Args >
    uerror check_invocable_error() const noexcept;

    template < typename... Args >
    uerror check_invocable_error(Args&&... args) const noexcept;

    //

    template < typename Iter >
    uvalue create_variadic(Iter first, Iter last) const;

    template < typename Iter >
    uresult try_create_variadic(Iter first, Iter last) const;

    template < typename Iter >
    uvalue create_variadic_at(void* mem, Iter first, Iter last) const;

    template < typename Iter >
    uresult try_create_variadic_at(void* mem, Iter first, Iter last) const;

    template < typename Iter >
    bool is_variadic_invocable_with(Iter first, Iter last) const;

    template < typename Iter >
    uerror check_variadic_invocable_error(Iter first, Iter last) const;
};
```

### destructor

```cpp
class destructor final : public state_base<destructor> {
public:
    using state_base<destructor>::state_base;

    destructor_type get_type() const noexcept;

    template < typename Arg >
    void destroy(Arg&& arg) const;

    template < typename Arg >
    uresult try_destroy(Arg&& arg) const;

    void destroy_at(void* mem) const;

    uresult try_destroy_at(void* mem) const;

    template < typename Arg >
    bool is_invocable_with() const noexcept;

    template < typename Arg >
    bool is_invocable_with(Arg&& arg) const noexcept;

    template < typename Arg >
    uerror check_invocable_error() const noexcept;

    template < typename Arg >
    uerror check_invocable_error(Arg&& arg) const noexcept;
};
```

### evalue

```cpp
class evalue final : public state_base<evalue> {
public:
    using state_base<evalue>::state_base;

    enum_type get_type() const noexcept;
    const std::string& get_name() const noexcept;

    const uvalue& get_value() const noexcept;
    const uvalue& get_underlying_value() const noexcept;
};
```

### function

```cpp
class function final : public state_base<function> {
public:
    using state_base<function>::state_base;

    function_type get_type() const noexcept;
    const std::string& get_name() const noexcept;

    std::size_t get_arity() const noexcept;
    argument get_argument(std::size_t position) const noexcept;
    const argument_list& get_arguments() const noexcept;

    //

    template < typename... Args >
    uvalue invoke(Args&&... args) const;

    template < typename... Args >
    uresult try_invoke(Args&&... args) const;

    template < typename... Args >
    uvalue operator()(Args&&... args) const;

    template < typename... Args >
    bool is_invocable_with() const noexcept;

    template < typename... Args >
    bool is_invocable_with(Args&&... args) const noexcept;

    template < typename... Args >
    uerror check_invocable_error() const noexcept;

    template < typename... Args >
    uerror check_invocable_error(Args&&... args) const noexcept;

    //

    template < typename Iter >
    uvalue invoke_variadic(Iter first, Iter last) const;

    template < typename Iter >
    uresult try_invoke_variadic(Iter first, Iter last) const;

    template < typename Iter >
    bool is_variadic_invocable_with(Iter first, Iter last) const;

    template < typename Iter >
    uerror check_variadic_invocable_error(Iter first, Iter last) const;
};
```

### member

```cpp
class member final : public state_base<member> {
public:
    using state_base<member>::state_base;

    member_type get_type() const noexcept;
    const std::string& get_name() const noexcept;

    template < typename Instance >
    uvalue get(Instance&& instance) const;

    template < typename Instance >
    uresult try_get(Instance&& instance) const;

    template < typename Instance >
    uvalue operator()(Instance&& instance) const;

    template < typename Instance, typename Value >
    void set(Instance&& instance, Value&& value) const;

    template < typename Instance, typename Value >
    uresult try_set(Instance&& instance, Value&& value) const;

    template < typename Instance, typename Value >
    void operator()(Instance&& instance, Value&& value) const;

    template < typename Instance >
    bool is_gettable_with() const noexcept;

    template < typename Instance >
    bool is_gettable_with(Instance&& instance) const noexcept;

    template < typename Instance, typename Value >
    bool is_settable_with() const noexcept;

    template < typename Instance, typename Value >
    bool is_settable_with(Instance&& instance, Value&& value) const noexcept;

    template < typename Instance >
    uerror check_gettable_error() const noexcept;

    template < typename Instance >
    uerror check_gettable_error(Instance&& instance) const noexcept;

    template < typename Instance, typename Value >
    uerror check_settable_error() const noexcept;

    template < typename Instance, typename Value >
    uerror check_settable_error(Instance&& instance, Value&& value) const noexcept;
};
```

### method

```cpp
class method final : public state_base<method> {
public:
    using state_base<method>::state_base;

    method_type get_type() const noexcept;
    const std::string& get_name() const noexcept;

    std::size_t get_arity() const noexcept;
    argument get_argument(std::size_t position) const noexcept;
    const argument_list& get_arguments() const noexcept;

    //

    template < typename Instance, typename... Args >
    uvalue invoke(Instance&& instance, Args&&... args) const;

    template < typename Instance, typename... Args >
    uresult try_invoke(Instance&& instance, Args&&... args) const;

    template < typename Instance, typename... Args >
    uvalue operator()(Instance&& instance, Args&&... args) const;

    template < typename Instance, typename... Args >
    bool is_invocable_with() const noexcept;

    template < typename Instance, typename... Args >
    bool is_invocable_with(Instance&& instance, Args&&... args) const noexcept;

    template < typename Instance, typename... Args >
    uerror check_invocable_error() const noexcept;

    template < typename Instance, typename... Args >
    uerror check_invocable_error(Instance&& instance, Args&&... args) const noexcept;

    //

    template < typename Instance, typename Iter >
    uvalue invoke_variadic(Instance&& instance, Iter first, Iter last) const;

    template < typename Instance, typename Iter >
    uresult try_invoke_variadic(Instance&& instance, Iter first, Iter last) const;

    template < typename Instance, typename Iter >
    bool is_variadic_invocable_with(Instance&& instance, Iter first, Iter last) const;

    template < typename Instance, typename Iter >
    uerror check_variadic_invocable_error(Instance&& instance, Iter first, Iter last) const;
};
```

### scope

```cpp
class scope final : public state_base<scope> {
public:
    using state_base<scope>::state_base;

    const std::string& get_name() const noexcept;

    const function_list& get_functions() const noexcept;
    const typedef_map& get_typedefs() const noexcept;
    const variable_list& get_variables() const noexcept;

    function get_function(std::string_view name) const noexcept;
    any_type get_typedef(std::string_view name) const noexcept;
    variable get_variable(std::string_view name) const noexcept;

    template < typename... Args >
    function get_function_with(
        std::string_view name
    ) const noexcept;

    template < typename Iter >
    function get_function_with(
        std::string_view name,
        Iter first,
        Iter last
    ) const noexcept;

    function get_function_with(
        std::string_view name,
        std::span<const any_type> args
    ) const noexcept;

    function get_function_with(
        std::string_view name,
        std::initializer_list<any_type> args
    ) const noexcept;
};
```

### variable

```cpp
class variable final : public state_base<variable> {
public:
    using state_base<variable>::state_base;

    pointer_type get_type() const noexcept;
    const std::string& get_name() const noexcept;

    uvalue get() const;
    uresult try_get() const;

    uvalue operator()() const;

    template < typename Value >
    void set(Value&& value) const;

    template < typename Value >
    uresult try_set(Value&& value) const;

    template < typename Value >
    void operator()(Value&& value) const;

    template < typename Value >
    bool is_settable_with() const noexcept;

    template < typename Value >
    bool is_settable_with(Value&& value) const noexcept;

    template < typename Value >
    uerror check_settable_error() const noexcept;

    template < typename Value >
    uerror check_settable_error(Value&& value) const noexcept;
};
```
