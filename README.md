# meta.hpp

> C++20 tiny dynamic reflection library

[![linux][badge.linux]][linux]
[![darwin][badge.darwin]][darwin]
[![windows][badge.windows]][windows]
[![language][badge.language]][language]
[![license][badge.license]][license]

[badge.darwin]: https://img.shields.io/github/actions/workflow/status/BlackMATov/meta.hpp/.github/workflows/darwin.yml?label=Xcode&logo=xcode
[badge.linux]: https://img.shields.io/github/actions/workflow/status/BlackMATov/meta.hpp/.github/workflows/linux.yml?label=GCC%2FClang&logo=linux
[badge.windows]: https://img.shields.io/github/actions/workflow/status/BlackMATov/meta.hpp/.github/workflows/windows.yml?label=Visual%20Studio&logo=visual-studio
[badge.language]: https://img.shields.io/badge/language-C%2B%2B20-orange
[badge.license]: https://img.shields.io/badge/license-MIT-blue

[darwin]: https://github.com/BlackMATov/meta.hpp/actions?query=workflow%3Adarwin
[linux]: https://github.com/BlackMATov/meta.hpp/actions?query=workflow%3Alinux
[windows]: https://github.com/BlackMATov/meta.hpp/actions?query=workflow%3Awindows
[language]: https://en.wikipedia.org/wiki/C%2B%2B20
[license]: https://en.wikipedia.org/wiki/MIT_License

[meta]: https://github.com/BlackMATov/meta.hpp

## Requirements

- [clang](https://clang.llvm.org/) **>= 10**
- [gcc](https://www.gnu.org/software/gcc/) **>= 10**
- [msvc](https://visualstudio.microsoft.com/) **>= 2019**
- [xcode](https://developer.apple.com/xcode/) **>= 13.0**

## Installation

[meta.hpp][meta] is a header-only library. All you need to do is copy the headers files from `headers` directory into your project and include them:

```cpp
#include "meta.hpp/meta_all.hpp"
```

Also, you can add the root repository directory to your [cmake](https://cmake.org) project:

```cmake
add_subdirectory(external/meta.hpp)
target_link_libraries(your_project_target meta.hpp)
```

## Examples

- [Class](./manuals/meta_examples/class_example.cpp)
- [Enum](./manuals/meta_examples/enum_example.cpp)
- [Function](./manuals/meta_examples/function_example.cpp)
- [Member](./manuals/meta_examples/member_example.cpp)
- [Method](./manuals/meta_examples/method_example.cpp)
- [Scopes](./manuals/meta_examples/scopes_example.cpp)
- [UValue](./manuals/meta_examples/uvalue_example.cpp)
- [Variable](./manuals/meta_examples/variable_example.cpp)

## Features

- can reflect:
    - arrays
    - classes
    - constructors
    - destructors
    - enums
    - functions
    - member
    - methods
    - numbers
    - pointers
    - references
    - typedefs
    - variables
- has different behaviour bind policies
- supports multiple and virtual inheritance
- allows add additional metadata to all types
- calls functions/methods with runtime variables
- doesn't contain 3rd-party dependencies
- can be used without rtti and exceptions

## API

> coming soon

## Alternatives

- [billyquith/ponder](https://github.com/billyquith/ponder)
- [rttrorg/rttr](https://github.com/rttrorg/rttr)
- [skypjack/meta](https://github.com/skypjack/meta)

## [License (MIT)](./LICENSE.md)
