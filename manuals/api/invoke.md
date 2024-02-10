- [API Invoke](#api-invoke)
  - [Functions](#functions)
    - [invoke](#invoke)
    - [invoke\_variadic](#invoke_variadic)
    - [try\_invoke](#try_invoke)
    - [try\_invoke\_variadic](#try_invoke_variadic)
    - [is\_invocable\_with](#is_invocable_with)
    - [is\_variadic\_invocable\_with](#is_variadic_invocable_with)
    - [check\_invocable\_error](#check_invocable_error)
    - [check\_variadic\_invocable\_error](#check_variadic_invocable_error)

# API Invoke

## Functions

### invoke

```cpp
template < typename... Args >
uvalue invoke(const function& function, Args&&... args);

template < function_pointer_kind Function, typename... Args >
uvalue invoke(Function function_ptr, Args&&... args);

//

template < typename Instance >
uvalue invoke(const member& member, Instance&& instance);

template < member_pointer_kind Member, typename Instance >
uvalue invoke(Member member_ptr, Instance&& instance);

//

template < typename Instance, typename... Args >
uvalue invoke(const method& method, Instance&& instance, Args&&... args);

template < method_pointer_kind Method, typename Instance, typename... Args >
uvalue invoke(Method method_ptr, Instance&& instance, Args&&... args);
```


### invoke_variadic

```cpp
template < typename Iter >
uvalue invoke_variadic(const function& function, Iter first, Iter last);

template < function_pointer_kind Function, typename Iter >
uvalue invoke_variadic(Function function_ptr, Iter first, Iter last);

//

template < typename Instance, typename Iter >
uvalue invoke_variadic(const method& method, Instance&& instance, Iter first, Iter last);

template < method_pointer_kind Method, typename Instance, typename Iter >
uvalue invoke_variadic(Method method_ptr, Instance&& instance, Iter first, Iter last);
```

### try_invoke

```cpp
template < typename... Args >
uresult try_invoke(const function& function, Args&&... args);

template < function_pointer_kind Function, typename... Args >
uresult try_invoke(Function function_ptr, Args&&... args);

//

template < typename Instance >
uresult try_invoke(const member& member, Instance&& instance);

template < member_pointer_kind Member, typename Instance >
uresult try_invoke(Member member_ptr, Instance&& instance);

//

template < typename Instance, typename... Args >
uresult try_invoke(const method& method, Instance&& instance, Args&&... args);

template < method_pointer_kind Method, typename Instance, typename... Args >
uresult try_invoke(Method method_ptr, Instance&& instance, Args&&... args);
```

### try_invoke_variadic

```cpp
template < typename Iter >
uresult try_invoke_variadic(const function& function, Iter first, Iter last);

template < function_pointer_kind Function, typename Iter >
uresult try_invoke_variadic(Function function_ptr, Iter first, Iter last);

//

template < typename Instance, typename Iter >
uresult try_invoke_variadic(const method& method, Instance&& instance, Iter first, Iter last);

template < method_pointer_kind Method, typename Instance, typename Iter >
uresult try_invoke_variadic(Method method_ptr, Instance&& instance, Iter first, Iter last);
```

### is_invocable_with

```cpp
template < typename... Args >
bool is_invocable_with(const function& function) noexcept;

template < typename... Args >
bool is_invocable_with(const function& function, Args&&... args) noexcept;

template < typename... Args, function_pointer_kind Function >
bool is_invocable_with(Function function_ptr) noexcept;

template < typename... Args, function_pointer_kind Function >
bool is_invocable_with(Function function_ptr, Args&&... args) noexcept;

//

template < typename Instance >
bool is_invocable_with(const member& member) noexcept;

template < typename Instance >
bool is_invocable_with(const member& member, Instance&& instance) noexcept;

template < typename Instance, member_pointer_kind Member >
bool is_invocable_with(Member member_ptr) noexcept;

template < typename Instance, member_pointer_kind Member >
bool is_invocable_with(Member member_ptr, Instance&& instance) noexcept;

//

template < typename Instance, typename... Args >
bool is_invocable_with(const method& method) noexcept;

template < typename Instance, typename... Args >
bool is_invocable_with(const method& method, Instance&& instance, Args&&... args) noexcept;

template < typename Instance, typename... Args, method_pointer_kind Method >
bool is_invocable_with(Method method_ptr) noexcept;

template < typename Instance, typename... Args, method_pointer_kind Method >
bool is_invocable_with(Method method_ptr, Instance&& instance, Args&&... args) noexcept;
```

### is_variadic_invocable_with

```cpp
template < typename Iter >
bool is_variadic_invocable_with(const function& function, Iter first, Iter last) noexcept;

template < typename Iter, function_pointer_kind Function >
bool is_variadic_invocable_with(Function function_ptr, Iter first, Iter last) noexcept;

//

template < typename Instance, typename Iter >
bool is_variadic_invocable_with(const method& method, Instance&& instance, Iter first, Iter last) noexcept;

template < typename Instance, typename Iter, method_pointer_kind Method >
bool is_variadic_invocable_with(Method method_ptr, Instance&& instance, Iter first, Iter last) noexcept;
```

### check_invocable_error

```cpp
template < typename... Args >
uerror check_invocable_error(const function& function) noexcept;

template < typename... Args >
uerror check_invocable_error(const function& function, Args&&... args) noexcept;

template < typename... Args, function_pointer_kind Function >
uerror check_invocable_error(Function function_ptr) noexcept;

template < typename... Args, function_pointer_kind Function >
uerror check_invocable_error(Function function_ptr, Args&&... args) noexcept;

//

template < typename Instance >
uerror check_invocable_error(const member& member) noexcept;

template < typename Instance >
uerror check_invocable_error(const member& member, Instance&& instance) noexcept;

template < typename Instance, member_pointer_kind Member >
uerror check_invocable_error(Member member_ptr) noexcept;

template < typename Instance, member_pointer_kind Member >
uerror check_invocable_error(Member member_ptr, Instance&& instance) noexcept;

//

template < typename Instance, typename... Args >
uerror check_invocable_error(const method& method) noexcept;

template < typename Instance, typename... Args >
uerror check_invocable_error(const method& method, Instance&& instance, Args&&... args) noexcept;

template < typename Instance, typename... Args, method_pointer_kind Method >
uerror check_invocable_error(Method method_ptr) noexcept;

template < typename Instance, typename... Args, method_pointer_kind Method >
uerror check_invocable_error(Method method_ptr, Instance&& instance, Args&&... args) noexcept;
```

### check_variadic_invocable_error

```cpp
template < typename Iter >
uerror check_variadic_invocable_error(const function& function, Iter first, Iter last) noexcept;

template < typename Iter, function_pointer_kind Function >
uerror check_variadic_invocable_error(Function function_ptr, Iter first, Iter last) noexcept;

//

template < typename Instance, typename Iter >
uerror check_variadic_invocable_error(const method& method, Instance&& instance, Iter first, Iter last) noexcept;

template < typename Instance, typename Iter, method_pointer_kind Method >
uerror check_variadic_invocable_error(Method method_ptr, Instance&& instance, Iter first, Iter last) noexcept;
```
