- [API Registry](#api-registry)
  - [Functions](#functions)
    - [for\_each\_type](#for_each_type)
    - [resolve\_type](#resolve_type)
    - [for\_each\_scope](#for_each_scope)
    - [resolve\_scope](#resolve_scope)

# API Registry

## Functions

### for_each_type

```cpp
template < type_family Type = any_type, typename F >
void for_each_type(F&& f);
```

### resolve_type

```cpp
template < typename T >
auto resolve_type();

template < typename T >
auto resolve_type(T&& from);
```

### for_each_scope

```cpp
template < typename F >
void for_each_scope(F&& f);
```

### resolve_scope

```cpp
scope resolve_scope(std::string_view name);
```
