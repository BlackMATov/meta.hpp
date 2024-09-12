- [API Reference](#api-reference)
  - [Basics](#basics)
    - [Classes](#classes)
    - [Functions](#functions)
  - [Binds](#binds)
    - [Classes](#classes-1)
    - [Functions](#functions-1)
  - [Invoke](#invoke)
    - [Functions](#functions-2)
  - [Policies](#policies)
    - [Namespaces](#namespaces)
  - [Registry](#registry)
    - [Functions](#functions-3)
  - [States](#states)
    - [Classes](#classes-2)
  - [Types](#types)
    - [Classes](#classes-3)
    - [Enumerations](#enumerations)

# API Reference

## Basics

### Classes

|                                    |         |
| ---------------------------------- | ------- |
| [uerror](./api/basics.md#uerror)   | uerror  |
| [uresult](./api/basics.md#uresult) | uresult |
| [uvalue](./api/basics.md#uvalue)   | uvalue  |

### Functions

|                                              |              |
| -------------------------------------------- | ------------ |
| [make_uerror](./api/basics.md#make_uerror)   | make_uerror  |
| [make_uresult](./api/basics.md#make_uresult) | make_uresult |
| [make_uvalue](./api/basics.md#make_uvalue)   | make_uvalue  |
| [ucast](./api/basics.md#ucast)               | ucast        |

## Binds

### Classes

|                                                 |                |
| ----------------------------------------------- | -------------- |
| [array_bind](./api/binds.md#array_bind)         | array_bind     |
| [class_bind](./api/binds.md#class_bind)         | class_bind     |
| [enum_bind](./api/binds.md#enum_bind)           | enum_bind      |
| [function_bind](./api/binds.md#function_bind)   | function_bind  |
| [member_bind](./api/binds.md#member_bind)       | member_bind    |
| [method_bind](./api/binds.md#method_bind)       | method_bind    |
| [nullptr_bind](./api/binds.md#nullptr_bind)     | nullptr_bind   |
| [number_bind](./api/binds.md#number_bind)       | number_bind    |
| [pointer_bind](./api/binds.md#pointer_bind)     | pointer_bind   |
| [reference_bind](./api/binds.md#reference_bind) | reference_bind |
| [void_bind](./api/binds.md#void_bind)           | void_bind      |
| [scope_bind](./api/binds.md#scope_bind)         | scope_bind     |
| [arguments_bind](./api/binds.md#arguments_bind) | arguments_bind |
| [metadata_bind](./api/binds.md#metadata_bind)   | metadata_bind  |

### Functions

|                                               |               |
| --------------------------------------------- | ------------- |
| [bind_](./api/binds.md#bind_)                 | bind_         |
| [array_](./api/binds.md#array_)               | array_        |
| [class_](./api/binds.md#class_)               | class_        |
| [enum_](./api/binds.md#enum_)                 | enum_         |
| [function_](./api/binds.md#function_)         | function_     |
| [member_](./api/binds.md#member_)             | member_       |
| [method_](./api/binds.md#method_)             | method_       |
| [nullptr_](./api/binds.md#nullptr_)           | nullptr_      |
| [number_](./api/binds.md#number_)             | number_       |
| [pointer_](./api/binds.md#pointer_)           | pointer_      |
| [reference_](./api/binds.md#reference_)       | reference_    |
| [void_](./api/binds.md#void_)                 | void_         |
| [local_scope_](./api/binds.md#local_scope_)   | local_scope_  |
| [static_scope_](./api/binds.md#static_scope_) | static_scope_ |
| [extend_scope_](./api/binds.md#extend_scope_) | extend_scope_ |
| [arguments_](./api/binds.md#arguments_)       | arguments_    |
| [metadata_](./api/binds.md#metadata_)         | metadata_     |

## Invoke

### Functions

|                                                                                  |                                |
| -------------------------------------------------------------------------------- | ------------------------------ |
| [invoke](./api/invoke.md#invoke)                                                 | invoke                         |
| [invoke_variadic](./api/invoke.md#invoke_variadic)                               | invoke_variadic                |
| [try_invoke](./api/invoke.md#try_invoke)                                         | try_invoke                     |
| [try_invoke_variadic](./api/invoke.md#try_invoke_variadic)                       | try_invoke_variadic            |
| [is_invocable_with](./api/invoke.md#is_invocable_with)                           | is_invocable_with              |
| [is_variadic_invocable_with](./api/invoke.md#is_variadic_invocable_with)         | is_variadic_invocable_with     |
| [check_invocable_error](./api/invoke.md#check_invocable_error)                   | check_invocable_error          |
| [check_variadic_invocable_error](./api/invoke.md#check_variadic_invocable_error) | check_variadic_invocable_error |


## Policies

### Namespaces

|                                                            |                    |
| ---------------------------------------------------------- | ------------------ |
| [constructor_policy](./api/policies.md#constructor_policy) | constructor_policy |
| [function_policy](./api/policies.md#function_policy)       | function_policy    |
| [member_policy](./api/policies.md#member_policy)           | member_policy      |
| [method_policy](./api/policies.md#method_policy)           | method_policy      |
| [variable_policy](./api/policies.md#variable_policy)       | variable_policy    |

## Registry

### Functions

|                                                    |                |
| -------------------------------------------------- | -------------- |
| [for_each_type](./api/registry.md#for_each_type)   | for_each_type  |
| [resolve_type](./api/registry.md#resolve_type)     | resole_type    |
| [for_each_scope](./api/registry.md#for_each_scope) | for_each_scope |
| [resolve_scope](./api/registry.md#resolve_scope)   | resolve_scope  |

## States

### Classes

|                                            |             |
| ------------------------------------------ | ----------- |
| [state_base](./api/states.md#state_base)   | state_base  |
| [argument](./api/states.md#argument)       | argument    |
| [constructor](./api/states.md#constructor) | constructor |
| [destructor](./api/states.md#destructor)   | destructor  |
| [evalue](./api/states.md#evalue)           | evalue      |
| [function](./api/states.md#function)       | function    |
| [member](./api/states.md#member)           | member      |
| [method](./api/states.md#method)           | method      |
| [scope](./api/states.md#scope)             | scope       |
| [variable](./api/states.md#variable)       | variable    |

## Types

### Classes

|                                                     |                  |
| --------------------------------------------------- | ---------------- |
| [type_id](./api/types.md#type_id)                   | type_id          |
| [type_base](./api/types.md#type_base)               | type_base        |
| [any_type](./api/types.md#any_type)                 | any_type         |
| [array_type](./api/types.md#array_type)             | array_type       |
| [class_type](./api/types.md#class_type)             | class_type       |
| [constructor_type](./api/types.md#constructor_type) | constructor_type |
| [destructor_type](./api/types.md#destructor_type)   | destructor_type  |
| [enum_type](./api/types.md#enum_type)               | enum_type        |
| [function_type](./api/types.md#function_type)       | function_type    |
| [member_type](./api/types.md#member_type)           | member_type      |
| [method_type](./api/types.md#method_type)           | method_type      |
| [nullptr_type](./api/types.md#nullptr_type)         | nullptr_type     |
| [number_type](./api/types.md#number_type)           | number_type      |
| [pointer_type](./api/types.md#pointer_type)         | pointer_type     |
| [reference_type](./api/types.md#reference_type)     | reference_type   |
| [void_type](./api/types.md#void_type)               | void_type        |

### Enumerations

|                                                       |                   |
| ----------------------------------------------------- | ----------------- |
| [array_flags](./api/types.md#array_flags)             | array_flags       |
| [class_flags](./api/types.md#class_flags)             | class_flags       |
| [constructor_flags](./api/types.md#constructor_flags) | constructor_flags |
| [destructor_flags](./api/types.md#destructor_flags)   | destructor_flags  |
| [enum_flags](./api/types.md#enum_flags)               | enum_flags        |
| [function_flags](./api/types.md#function_flags)       | function_flags    |
| [member_flags](./api/types.md#member_flags)           | member_flags      |
| [method_flags](./api/types.md#method_flags)           | method_flags      |
| [number_flags](./api/types.md#number_flags)           | number_flags      |
| [pointer_flags](./api/types.md#pointer_flags)         | pointer_flags     |
| [reference_flags](./api/types.md#reference_flags)     | reference_flags   |
