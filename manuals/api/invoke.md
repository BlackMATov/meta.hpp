- [API Invoke](#api-invoke)
  - [Functions](#functions)
    - [invoke](#invoke)
    - [try\_invoke](#try_invoke)
    - [is\_invocable\_with](#is_invocable_with)
    - [check\_invocable\_error](#check_invocable_error)

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
