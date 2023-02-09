/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_base.hpp"
#include "meta_registry.hpp"
#include "meta_states.hpp"

namespace meta_hpp::detail
{
    template < typename Class, typename... Args >
    concept class_bind_constructor_kind            //
        = class_kind<Class>                        //
       && std::is_constructible_v<Class, Args...>; //

    template < typename Class >
    concept class_bind_destructor_kind                        //
        = class_kind<Class> && std::is_destructible_v<Class>; //

    template < typename Class, typename Base >
    concept class_bind_base_kind                //
        = class_kind<Class> && class_kind<Base> //
       && std::derived_from<Class, Base>;       //

    template < typename Class, typename Member >
    concept class_bind_member_kind                                           //
        = class_kind<Class> && member_pointer_kind<Member>                   //
       && std::is_same_v<Class, typename member_traits<Member>::class_type>; //

    template < typename Class, typename Method >
    concept class_bind_method_kind                                           //
        = class_kind<Class> && method_pointer_kind<Method>                   //
       && std::is_same_v<Class, typename method_traits<Method>::class_type>; //
}

namespace meta_hpp
{
    struct argument_opts final {
        std::string name{};
        metadata_map metadata{};
    };

    using argument_opts_list = std::vector<argument_opts>;

    struct constructor_opts final {
        argument_opts_list arguments{};
        metadata_map metadata{};
    };

    struct destructor_opts final {
        metadata_map metadata{};
    };

    struct evalue_opts final {
        metadata_map metadata{};
    };

    struct function_opts final {
        argument_opts_list arguments{};
        metadata_map metadata{};
    };

    struct member_opts final {
        metadata_map metadata{};
    };

    struct method_opts final {
        argument_opts_list arguments;
        metadata_map metadata{};
    };

    struct variable_opts final {
        metadata_map metadata{};
    };
}

namespace meta_hpp
{
    template < detail::type_family Type >
    class type_bind_base {
    public:
        explicit type_bind_base(const Type& type, metadata_map metadata)
        : data_{detail::type_access(type)} {
            detail::insert_or_assign(data_->metadata, std::move(metadata));
        }

        operator Type() const noexcept {
            return Type{data_};
        }

    protected:
        using data_ptr = typename detail::type_traits<Type>::data_ptr;
        using data_ref = decltype(*std::declval<data_ptr>());

        [[nodiscard]] data_ref get_data() noexcept {
            return *data_;
        }

    private:
        data_ptr data_;
        detail::type_registry::locker locker_;
    };

    template < detail::state_family State >
    class state_bind_base {
    public:
        explicit state_bind_base(const State& state, metadata_map metadata)
        : state_{detail::state_access(state)} {
            detail::insert_or_assign(state_->metadata, std::move(metadata));
        }

        operator State() const noexcept {
            return State{state_};
        }

    protected:
        using state_ptr = typename detail::state_traits<State>::state_ptr;
        using state_ref = decltype(*std::declval<state_ptr>());

        [[nodiscard]] state_ref get_state() noexcept {
            return *state_;
        }

    private:
        state_ptr state_;
        detail::state_registry::locker locker_;
    };
}

namespace meta_hpp
{
    template < detail::array_kind Array >
    class array_bind final : public type_bind_base<array_type> {
    public:
        explicit array_bind(metadata_map metadata);
    };
}

namespace meta_hpp
{
    template < detail::class_kind Class >
    class class_bind final : public type_bind_base<class_type> {
    public:
        explicit class_bind(metadata_map metadata);

        // base_

        template < detail::class_kind... Bases >
            requires(... && detail::class_bind_base_kind<Class, Bases>)
        class_bind& base_();

        // constructor_

        template < typename... Args, constructor_policy_kind Policy = constructor_policy::as_object_t >
        class_bind& constructor_(Policy = {})
            requires detail::class_bind_constructor_kind<Class, Args...>;

        template < typename... Args, constructor_policy_kind Policy = constructor_policy::as_object_t >
        class_bind& constructor_(constructor_opts opts, Policy = {})
            requires detail::class_bind_constructor_kind<Class, Args...>;

        // destructor_

        class_bind& destructor_()
            requires detail::class_bind_destructor_kind<Class>;

        class_bind& destructor_(destructor_opts opts)
            requires detail::class_bind_destructor_kind<Class>;

        // function_

        template < detail::function_pointer_kind Function, function_policy_kind Policy = function_policy::as_copy_t >
        class_bind& function_(std::string name, Function function_ptr, Policy = {});

        template < detail::function_pointer_kind Function, function_policy_kind Policy = function_policy::as_copy_t >
        class_bind& function_(std::string name, Function function_ptr, function_opts opts, Policy = {});

        template < detail::function_pointer_kind Function, function_policy_kind Policy = function_policy::as_copy_t >
        class_bind& function_(std::string name, Function function_ptr, string_ilist arguments, Policy = {});

        // member_

        template < detail::member_pointer_kind Member, member_policy_kind Policy = member_policy::as_copy_t >
            requires detail::class_bind_member_kind<Class, Member>
        class_bind& member_(std::string name, Member member_ptr, Policy = {});

        template < detail::member_pointer_kind Member, member_policy_kind Policy = member_policy::as_copy_t >
            requires detail::class_bind_member_kind<Class, Member>
        class_bind& member_(std::string name, Member member_ptr, member_opts opts, Policy = {});

        // method_

        template < detail::method_pointer_kind Method, method_policy_kind Policy = method_policy::as_copy_t >
            requires detail::class_bind_method_kind<Class, Method>
        class_bind& method_(std::string name, Method method_ptr, Policy = {});

        template < detail::method_pointer_kind Method, method_policy_kind Policy = method_policy::as_copy_t >
            requires detail::class_bind_method_kind<Class, Method>
        class_bind& method_(std::string name, Method method_ptr, method_opts opts, Policy = {});

        template < detail::method_pointer_kind Method, method_policy_kind Policy = method_policy::as_copy_t >
            requires detail::class_bind_method_kind<Class, Method>
        class_bind& method_(std::string name, Method method_ptr, string_ilist arguments, Policy = {});

        // typdef_

        template < typename Type >
        class_bind& typedef_(std::string name);

        // variable_

        template < detail::pointer_kind Pointer, variable_policy_kind Policy = variable_policy::as_copy_t >
        class_bind& variable_(std::string name, Pointer variable_ptr, Policy = {});

        template < detail::pointer_kind Pointer, variable_policy_kind Policy = variable_policy::as_copy_t >
        class_bind& variable_(std::string name, Pointer variable_ptr, variable_opts opts, Policy = {});
    };
}

namespace meta_hpp
{
    template < detail::enum_kind Enum >
    class enum_bind final : public type_bind_base<enum_type> {
    public:
        explicit enum_bind(metadata_map metadata);

        enum_bind& evalue_(std::string name, Enum value);
        enum_bind& evalue_(std::string name, Enum value, evalue_opts opts);
    };
}

namespace meta_hpp
{
    template < detail::function_pointer_kind Function >
    class function_bind final : public type_bind_base<function_type> {
    public:
        explicit function_bind(metadata_map metadata);
    };
}

namespace meta_hpp
{
    template < detail::member_pointer_kind Member >
    class member_bind final : public type_bind_base<member_type> {
    public:
        explicit member_bind(metadata_map metadata);
    };
}

namespace meta_hpp
{
    template < detail::method_pointer_kind Method >
    class method_bind final : public type_bind_base<method_type> {
    public:
        explicit method_bind(metadata_map metadata);
    };
}

namespace meta_hpp
{
    template < detail::nullptr_kind Nullptr >
    class nullptr_bind final : public type_bind_base<nullptr_type> {
    public:
        explicit nullptr_bind(metadata_map metadata);
    };
}

namespace meta_hpp
{
    template < detail::number_kind Number >
    class number_bind final : public type_bind_base<number_type> {
    public:
        explicit number_bind(metadata_map metadata);
    };
}

namespace meta_hpp
{
    template < detail::pointer_kind Pointer >
    class pointer_bind final : public type_bind_base<pointer_type> {
    public:
        explicit pointer_bind(metadata_map metadata);
    };
}

namespace meta_hpp
{
    template < detail::reference_kind Reference >
    class reference_bind final : public type_bind_base<reference_type> {
    public:
        explicit reference_bind(metadata_map metadata);
    };
}

namespace meta_hpp
{
    template < detail::void_kind Void >
    class void_bind final : public type_bind_base<void_type> {
    public:
        explicit void_bind(metadata_map metadata);
    };
}

namespace meta_hpp
{
    class scope_bind final : public state_bind_base<scope> {
    public:
        explicit scope_bind(const scope& scope, metadata_map metadata);

        // function_

        template < detail::function_pointer_kind Function, function_policy_kind Policy = function_policy::as_copy_t >
        scope_bind& function_(std::string name, Function function_ptr, Policy = {});

        template < detail::function_pointer_kind Function, function_policy_kind Policy = function_policy::as_copy_t >
        scope_bind& function_(std::string name, Function function_ptr, function_opts opts, Policy = {});

        template < detail::function_pointer_kind Function, function_policy_kind Policy = function_policy::as_copy_t >
        scope_bind& function_(std::string name, Function function_ptr, string_ilist arguments, Policy = {});

        // typedef_

        template < typename Type >
        scope_bind& typedef_(std::string name);

        // variable_

        template < detail::pointer_kind Pointer, variable_policy_kind Policy = variable_policy::as_copy_t >
        scope_bind& variable_(std::string name, Pointer variable_ptr, Policy = {});

        template < detail::pointer_kind Pointer, variable_policy_kind Policy = variable_policy::as_copy_t >
        scope_bind& variable_(std::string name, Pointer variable_ptr, variable_opts opts, Policy = {});
    };
}

namespace meta_hpp
{
    template < detail::array_kind Array >
    array_bind<Array> array_(metadata_map metadata = {}) {
        return array_bind<Array>{std::move(metadata)};
    }

    template < detail::class_kind Class >
    class_bind<Class> class_(metadata_map metadata = {}) {
        return class_bind<Class>{std::move(metadata)};
    }

    template < detail::enum_kind Enum >
    enum_bind<Enum> enum_(metadata_map metadata = {}) {
        return enum_bind<Enum>{std::move(metadata)};
    }

    template < detail::function_pointer_kind Function >
    function_bind<Function> function_(metadata_map metadata = {}) {
        return function_bind<Function>{std::move(metadata)};
    }

    template < detail::member_pointer_kind Member >
    member_bind<Member> member_(metadata_map metadata = {}) {
        return member_bind<Member>{std::move(metadata)};
    }

    template < detail::method_pointer_kind Method >
    method_bind<Method> method_(metadata_map metadata = {}) {
        return method_bind<Method>{std::move(metadata)};
    }

    template < detail::nullptr_kind Nullptr >
    nullptr_bind<Nullptr> nullptr_(metadata_map metadata = {}) {
        return nullptr_bind<Nullptr>{std::move(metadata)};
    }

    template < detail::number_kind Number >
    number_bind<Number> number_(metadata_map metadata = {}) {
        return number_bind<Number>{std::move(metadata)};
    }

    template < detail::pointer_kind Pointer >
    pointer_bind<Pointer> pointer_(metadata_map metadata = {}) {
        return pointer_bind<Pointer>{std::move(metadata)};
    }

    template < detail::reference_kind Reference >
    reference_bind<Reference> reference_(metadata_map metadata = {}) {
        return reference_bind<Reference>{std::move(metadata)};
    }

    template < detail::void_kind Void >
    void_bind<Void> void_(metadata_map metadata = {}) {
        return void_bind<Void>{std::move(metadata)};
    }
}

namespace meta_hpp
{
    inline scope_bind local_scope_(std::string name, metadata_map metadata = {}) {
        scope local_scope{detail::scope_state::make(std::move(name), std::move(metadata))};
        return scope_bind{local_scope, {}};
    }

    inline scope_bind static_scope_(std::string_view name, metadata_map metadata = {}) {
        scope static_scope{resolve_scope(name)};
        return scope_bind{static_scope, std::move(metadata)};
    }

    inline scope_bind extend_scope_(const scope& scope, metadata_map metadata = {}) {
        return scope_bind{scope, std::move(metadata)};
    }
}
