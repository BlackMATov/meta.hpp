# Roadmap

## Backlog

- type conversions
- non-linear search of methods/functions/...
- register base types by `META_HPP_ENABLE_POLY_INFO`
- add the library version to sources
- fix all includes to work with the library more flexible
- `try_invoke`/`is_invocable` should return error codes
- test and support shared libraries

## Thoughts

- should `uvalue` dereference operators return `reference_wrapper`?
- `array_view`/`pointer_view` instead `unmap`/`operator[]`/`operator*`?
- can we add move-only uvalue analog to return move-only values?

## Version 1.0

## Version 2.0

- instance mapper
- dynamic binds listener
- static binds listener
- dynamic type visitor
- conan package config
- type names by [nameof](https://github.com/Neargye/nameof)
