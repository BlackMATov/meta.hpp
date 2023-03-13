# Roadmap

## Backlog

- instance mapper
- type conversions
- dynamic binds listener
- static binds listener
- dynamic type visitor
- type names by [nameof](https://github.com/Neargye/nameof)
- non-linear search of methods/functions/...
- register base types by `META_HPP_ENABLE_POLY_INFO`
- distinguish between function types and function pointer types
- add the library version to sources
- fix all includes to work with the library more flexible
- build and test on CI with additional flags like no-exception and no-rtti
- `try_invoke`/`is_invocable` should return error codes
- conan package config
- test and support shared libraries
- add `for_each_type` for specific kind of types

## Thoughts

- should `uvalue` dereference operators return `reference_wrapper`?
- `array_view`/`pointer_view` instead `unmap`/`operator[]`/`operator*`?
- can we add move-only uvalue analog to return move-only values?
