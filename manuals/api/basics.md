- [API Basics](#api-basics)
  - [Classes](#classes)
    - [uerror](#uerror)
    - [uresult](#uresult)
    - [uvalue](#uvalue)
  - [Functions](#functions)
    - [make\_uerror](#make_uerror)
    - [make\_uresult](#make_uresult)
    - [make\_uvalue](#make_uvalue)
    - [ucast](#ucast)

# API Basics

## Classes

### uerror

```cpp
class uerror final {
public:
    uerror() = default;
    ~uerror() = default;

    uerror(uerror&&) = default;
    uerror& operator=(uerror&&) = default;

    uerror(const uerror&) = default;
    uerror& operator=(const uerror&) = default;

    explicit uerror(error_code error) noexcept;
    uerror& operator=(error_code error) noexcept;

    bool has_error() const noexcept;
    explicit operator bool() const noexcept;

    error_code operator*() const noexcept;
    error_code get_error() const noexcept;

    void reset() noexcept;
    void swap(uerror& other) noexcept;

    std::size_t get_hash() const noexcept;
    std::strong_ordering operator<=>(const uerror& other) const = default;

private:
    error_code error_{error_code::no_error};
};
```

### uresult

```cpp
class uresult final {
public:
    uresult() = default;
    ~uresult() = default;

    uresult(uresult&&) = default;
    uresult& operator=(uresult&&) = default;

    uresult(const uresult&) = delete;
    uresult& operator=(const uresult&) = delete;

    explicit(false) uresult(uerror error) noexcept;
    explicit(false) uresult(uvalue value) noexcept;

    uresult& operator=(uerror error) noexcept;
    uresult& operator=(uvalue value) noexcept;

    template <
        typename T,
        typename = std::enable_if_t<
            !uvalue_family<std::decay_t<T>> &&
            !detail::is_in_place_type_v<std::decay_t<T>> &&
            std::is_constructible_v<std::decay_t<T>, T>>>
    uresult(T&& val);

    template <
        typename T,
        typename = std::enable_if_t<
            !uvalue_family<std::decay_t<T>> &&
            std::is_constructible_v<std::decay_t<T>, T>>>
    uresult& operator=(T&& val);

    template < typename T, typename... Args >
        requires std::is_constructible_v<std::decay_t<T>, Args...>
    explicit uresult(std::in_place_type_t<T>, Args&&... args);

    template < typename T, typename U, typename... Args >
        requires std::is_constructible_v<std::decay_t<T>, std::initializer_list<U>&, Args...>
    explicit uresult(std::in_place_type_t<T>, std::initializer_list<U> ilist, Args&&... args);

    template < typename T, typename... Args >
        requires std::is_constructible_v<std::decay_t<T>, Args...>
    std::decay_t<T>& emplace(Args&&... args);

    template < typename T, typename U, typename... Args >
        requires std::is_constructible_v<std::decay_t<T>, std::initializer_list<U>&, Args...>
    std::decay_t<T>& emplace(std::initializer_list<U> ilist, Args&&... args);

    bool has_error() const noexcept;
    bool has_value() const noexcept;
    explicit operator bool() const noexcept;

    uvalue* operator->() noexcept;
    const uvalue* operator->() const noexcept;

    uvalue& operator*() & noexcept;
    uvalue&& operator*() && noexcept;
    const uvalue& operator*() const& noexcept;
    const uvalue&& operator*() const&& noexcept;

    uvalue& get_value() & noexcept;
    uvalue&& get_value() && noexcept;
    const uvalue& get_value() const& noexcept;
    const uvalue&& get_value() const&& noexcept;

    error_code get_error() const noexcept;

    void reset() noexcept;
    void swap(uresult& other) noexcept;
};
```

### uvalue

```cpp
class uvalue final {
public:
    uvalue() = default;
    ~uvalue() noexcept;

    uvalue(uvalue&& other) noexcept;
    uvalue& operator=(uvalue&& other) noexcept;

    uvalue(const uvalue& other) = delete;
    uvalue& operator=(const uvalue& other) = delete;

    template <
        typename T,
        typename = std::enable_if_t<
            !uvalue_family<std::decay_t<T>> &&
            !detail::is_in_place_type_v<std::decay_t<T>> &&
            std::is_constructible_v<std::decay_t<T>, T>>>
    uvalue(T&& val);

    template <
        typename T,
        typename = std::enable_if_t<
            !uvalue_family<std::decay_t<T>> &&
            std::is_constructible_v<std::decay_t<T>, T>>>
    uvalue& operator=(T&& val);

    template < typename T, typename... Args >
        requires std::is_constructible_v<std::decay_t<T>, Args...>
    explicit uvalue(std::in_place_type_t<T>, Args&&... args);

    template < typename T, typename U, typename... Args >
        requires std::is_constructible_v<std::decay_t<T>, std::initializer_list<U>&, Args...>
    explicit uvalue(std::in_place_type_t<T>, std::initializer_list<U> ilist, Args&&... args);

    template < typename T, typename... Args >
        requires std::is_constructible_v<std::decay_t<T>, Args...>
    std::decay_t<T>& emplace(Args&&... args);

    template < typename T, typename U, typename... Args >
        requires std::is_constructible_v<std::decay_t<T>, std::initializer_list<U>&, Args...>
    std::decay_t<T>& emplace(std::initializer_list<U> ilist, Args&&... args);

    bool has_value() const noexcept;
    explicit operator bool() const noexcept;

    void reset() noexcept;
    void swap(uvalue& other) noexcept;

    any_type get_type() const noexcept;

    void* get_data() noexcept;
    const void* get_data() const noexcept;
    const void* get_cdata() const noexcept;

    uvalue operator*() const;
    bool has_deref_op() const noexcept;

    uvalue operator[](std::size_t index) const;
    bool has_index_op() const noexcept;

    uvalue copy() const;
    bool has_copy_op() const noexcept;

    uvalue unmap() const;
    bool has_unmap_op() const noexcept;

    template < typename T >
    bool is() const noexcept;

    template < any_pointer_family T >
    T as();
    template < any_pointer_family T >
    T as() const;

    template < not_any_pointer_family T >
    T& as() &;
    template < not_any_pointer_family T >
    const T& as() const&;
    template < not_any_pointer_family T >
    T as() &&;
    template < not_any_pointer_family T >
    const T&& as() const&&;

    template < any_pointer_family T >
    T try_as() noexcept;
    template < any_pointer_family T >
    T try_as() const noexcept;

    template < not_any_pointer_family T >
    T* try_as() & noexcept;
    template < not_any_pointer_family T >
    const T* try_as() const& noexcept;
    template < not_any_pointer_family T >
    void try_as() && = delete;
    template < not_any_pointer_family T >
    void try_as() const&& = delete;
};
```

## Functions

### make_uerror

```cpp
uerror make_uerror(error_code error);
```

### make_uresult

```cpp
template < typename T, typename... Args >
uresult make_uresult(Args&&... args);

template < typename T, typename U, typename... Args >
uresult make_uresult(std::initializer_list<U> ilist, Args&&... args);
```

### make_uvalue

```cpp
template < typename T, typename... Args >
uvalue make_uvalue(Args&&... args);

template < typename T, typename U, typename... Args >
uvalue make_uvalue(std::initializer_list<U> ilist, Args&&... args);
```

### ucast

```cpp
template < typename To, typename From >
    requires detail::ucast_as_pointers<To, From>
To ucast(From from);

template < typename To, typename From >
    requires detail::ucast_as_references<To, From>
To ucast(From&& from);
```
