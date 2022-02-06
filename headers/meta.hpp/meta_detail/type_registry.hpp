/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_types.hpp"

namespace meta_hpp::detail
{
    class type_registry final {
    public:
        [[nodiscard]] static type_registry& instance() {
            static type_registry instance;
            return instance;
        }

        [[nodiscard]] any_type get_type_by_id(type_id id) const noexcept {
            const std::lock_guard<std::mutex> lock{mutex_};
            if ( auto iter = type_by_id_.find(id); iter != type_by_id_.end() ) {
                return iter->second;
            }
            return any_type{};
        }

        [[nodiscard]] any_type get_type_by_rtti(const std::type_index& index) const noexcept {
            const std::lock_guard<std::mutex> lock{mutex_};
            if ( auto iter = type_by_rtti_.find(index); iter != type_by_rtti_.end() ) {
                return iter->second;
            }
            return any_type{};
        }

        //
        //
        //

        template < array_kind Array >
        [[nodiscard]] array_type resolve_type() { return resolve_array_type<Array>(); }

        template < class_kind Class >
        [[nodiscard]] class_type resolve_type() { return resolve_class_type<Class>(); }

        template < enum_kind Enum >
        [[nodiscard]] enum_type resolve_type() { return resolve_enum_type<Enum>(); }

        template < function_kind Function >
        [[nodiscard]] function_type resolve_type() { return resolve_function_type<Function>(); }

        template < member_kind Member >
        [[nodiscard]] member_type resolve_type() { return resolve_member_type<Member>(); }

        template < method_kind Method >
        [[nodiscard]] method_type resolve_type() { return resolve_method_type<Method>(); }

        template < nullptr_kind Nullptr >
        [[nodiscard]] nullptr_type resolve_type() { return resolve_nullptr_type<Nullptr>(); }

        template < number_kind Number >
        [[nodiscard]] number_type resolve_type() { return resolve_number_type<Number>(); }

        template < pointer_kind Pointer >
        [[nodiscard]] pointer_type resolve_type() { return resolve_pointer_type<Pointer>(); }

        template < reference_kind Reference >
        [[nodiscard]] reference_type resolve_type() { return resolve_reference_type<Reference>(); }

        template < void_kind Void >
        [[nodiscard]] void_type resolve_type() { return resolve_void_type<Void>(); }

        //
        //
        //

        template < array_kind Array >
        [[nodiscard]] array_type resolve_array_type() { return array_type{resolve_array_type_data<Array>()}; }

        template < class_kind Class >
        [[nodiscard]] class_type resolve_class_type() { return class_type{resolve_class_type_data<Class>()}; }

        template < class_kind Class, typename... Args >
        [[nodiscard]] ctor_type resolve_ctor_type() { return ctor_type{resolve_ctor_type_data<Class, Args...>()}; }

        template < class_kind Class >
        [[nodiscard]] dtor_type resolve_dtor_type() { return dtor_type{resolve_dtor_type_data<Class>()}; }

        template < enum_kind Enum >
        [[nodiscard]] enum_type resolve_enum_type() { return enum_type{resolve_enum_type_data<Enum>()}; }

        template < function_kind Function >
        [[nodiscard]] function_type resolve_function_type() { return function_type{resolve_function_type_data<Function>()}; }

        template < member_kind Member >
        [[nodiscard]] member_type resolve_member_type() { return member_type{resolve_member_type_data<Member>()}; }

        template < method_kind Method >
        [[nodiscard]] method_type resolve_method_type() { return method_type{resolve_method_type_data<Method>()}; }

        template < nullptr_kind Nullptr >
        [[nodiscard]] nullptr_type resolve_nullptr_type() { return nullptr_type{resolve_nullptr_type_data<Nullptr>()}; }

        template < number_kind Number >
        [[nodiscard]] number_type resolve_number_type() { return number_type{resolve_number_type_data<Number>()}; }

        template < pointer_kind Pointer >
        [[nodiscard]] pointer_type resolve_pointer_type() { return pointer_type{resolve_pointer_type_data<Pointer>()}; }

        template < reference_kind Reference >
        [[nodiscard]] reference_type resolve_reference_type() { return reference_type{resolve_reference_type_data<Reference>()}; }

        template < void_kind Void >
        [[nodiscard]] void_type resolve_void_type() { return void_type{resolve_void_type_data<Void>()}; }

        //
        //
        //

        template < array_kind Array >
        [[nodiscard]] array_type_data_ptr resolve_array_type_data() {
            static array_type_data_ptr data{std::make_shared<array_type_data>(type_list<Array>{})};
            return ensure_type<Array>(data);
        }

        template < class_kind Class >
        [[nodiscard]] class_type_data_ptr resolve_class_type_data() {
            static class_type_data_ptr data{std::make_shared<class_type_data>(type_list<Class>{})};
            return ensure_type<Class>(data);
        }

        template < class_kind Class, typename... Args >
        [[nodiscard]] ctor_type_data_ptr resolve_ctor_type_data() {
            static ctor_type_data_ptr data{std::make_shared<ctor_type_data>(type_list<Class>{}, type_list<Args...>{})};
            return data;
        }

        template < class_kind Class >
        [[nodiscard]] dtor_type_data_ptr resolve_dtor_type_data() {
            static dtor_type_data_ptr data{std::make_shared<dtor_type_data>(type_list<Class>{})};
            return data;
        }

        template < enum_kind Enum >
        [[nodiscard]] enum_type_data_ptr resolve_enum_type_data() {
            static enum_type_data_ptr data{std::make_shared<enum_type_data>(type_list<Enum>{})};
            return ensure_type<Enum>(data);
        }

        template < function_kind Function >
        [[nodiscard]] function_type_data_ptr resolve_function_type_data() {
            static function_type_data_ptr data{std::make_shared<function_type_data>(type_list<Function>{})};
            return ensure_type<Function>(data);
        }

        template < member_kind Member >
        [[nodiscard]] member_type_data_ptr resolve_member_type_data() {
            static member_type_data_ptr data{std::make_shared<member_type_data>(type_list<Member>{})};
            return ensure_type<Member>(data);
        }

        template < method_kind Method >
        [[nodiscard]] method_type_data_ptr resolve_method_type_data() {
            static method_type_data_ptr data{std::make_shared<method_type_data>(type_list<Method>{})};
            return ensure_type<Method>(data);
        }

        template < nullptr_kind Nullptr >
        [[nodiscard]] nullptr_type_data_ptr resolve_nullptr_type_data() {
            static nullptr_type_data_ptr data{std::make_shared<nullptr_type_data>(type_list<Nullptr>{})};
            return ensure_type<Nullptr>(data);
        }

        template < number_kind Number >
        [[nodiscard]] number_type_data_ptr resolve_number_type_data() {
            static number_type_data_ptr data{std::make_shared<number_type_data>(type_list<Number>{})};
            return ensure_type<Number>(data);
        }

        template < pointer_kind Pointer >
        [[nodiscard]] pointer_type_data_ptr resolve_pointer_type_data() {
            static pointer_type_data_ptr data{std::make_shared<pointer_type_data>(type_list<Pointer>{})};
            return ensure_type<Pointer>(data);
        }

        template < reference_kind Reference >
        [[nodiscard]] reference_type_data_ptr resolve_reference_type_data() {
            static reference_type_data_ptr data{std::make_shared<reference_type_data>(type_list<Reference>{})};
            return ensure_type<Reference>(data);
        }

        template < void_kind Void >
        [[nodiscard]] void_type_data_ptr resolve_void_type_data() {
            static void_type_data_ptr data{std::make_shared<void_type_data>(type_list<Void>{})};
            return ensure_type<Void>(data);
        }
    private:
        type_registry() = default;

        template < typename Type, typename TypeData >
        TypeData ensure_type(const TypeData& type_data) {
            static std::once_flag init_flag{};
            std::call_once(init_flag, [this, &type_data](){
                const std::lock_guard<std::mutex> lock{mutex_};
                type_by_id_[type_data->id] = any_type{type_data};
            #ifndef META_HPP_NO_RTTI
                type_by_rtti_[typeid(Type)] = any_type{type_data};
            #endif
            });
            return type_data;
        }
    private:
        mutable std::mutex mutex_;
        std::map<type_id, any_type, std::less<>> type_by_id_;
        std::map<std::type_index, any_type, std::less<>> type_by_rtti_;
    };
}

namespace meta_hpp::detail
{
    template < typename T >
    [[nodiscard]] auto resolve_type() {
        type_registry& registry = type_registry::instance();
        return registry.resolve_type<std::remove_cv_t<T>>();
    }

    template < typename... Ts >
    [[nodiscard]] std::vector<any_type> resolve_types() {
        return { resolve_type<Ts>()... };
    }
}

namespace meta_hpp::detail
{
    template < class_kind Class, typename... Args >
    ctor_type resolve_ctor_type() {
        type_registry& registry = type_registry::instance();
        return registry.resolve_ctor_type<Class, Args...>();
    }

    template < class_kind Class >
    dtor_type resolve_dtor_type() {
        type_registry& registry = type_registry::instance();
        return registry.resolve_dtor_type<Class>();
    }
}

namespace meta_hpp::detail
{
    template < typename T >
    // NOLINTNEXTLINE(readability-named-parameter)
    [[nodiscard]] auto resolve_type(T&&) {
        return resolve_type<std::remove_reference_t<T>>();
    }

    template < typename... Ts >
    // NOLINTNEXTLINE(readability-named-parameter)
    [[nodiscard]] std::vector<any_type> resolve_types(type_list<Ts...>) {
        return { resolve_type<Ts>()... };
    }
}

namespace meta_hpp::detail
{
    template < typename T >
    [[nodiscard]] any_type resolve_polymorphic_type(T&& v) noexcept {
    #ifndef META_HPP_NO_RTTI
        type_registry& registry = type_registry::instance();
        return registry.get_type_by_rtti(typeid(v));
    #else
        (void)v;
        return any_type{};
    #endif
    }
}
