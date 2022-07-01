/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_types.hpp"

namespace meta_hpp::detail
{
    class type_registry final {
    public:
        class locker final : noncopyable {
        public:
            explicit locker()
            : lock_{instance().mutex_} {}
        private:
            std::lock_guard<std::recursive_mutex> lock_;
        };

        [[nodiscard]] static type_registry& instance() {
            static type_registry instance;
            return instance;
        }
    public:
        [[nodiscard]] any_type get_type_by_id(type_id id) const noexcept {
            const locker lock;

            if ( auto iter = type_by_id_.find(id); iter != type_by_id_.end() ) {
                return iter->second;
            }

            return any_type{};
        }

        [[nodiscard]] any_type get_type_by_rtti(const std::type_index& index) const noexcept {
            const locker lock;

            if ( auto iter = type_by_rtti_.find(index); iter != type_by_rtti_.end() ) {
                return iter->second;
            }

            return any_type{};
        }
    public:
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
    public:
        template < array_kind Array >
        [[nodiscard]] array_type resolve_array_type() { return array_type{resolve_array_type_data<Array>()}; }

        template < class_kind Class >
        [[nodiscard]] class_type resolve_class_type() { return class_type{resolve_class_type_data<Class>()}; }

        template < class_kind Class, typename... Args >
        [[nodiscard]] constructor_type resolve_constructor_type() { return constructor_type{resolve_constructor_type_data<Class, Args...>()}; }

        template < class_kind Class >
        [[nodiscard]] destructor_type resolve_destructor_type() { return destructor_type{resolve_destructor_type_data<Class>()}; }

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
    private:
        template < array_kind Array >
        [[nodiscard]] array_type_data* resolve_array_type_data() {
            static array_type_data data{type_list<Array>{}};
            ensure_type<Array>(data);
            return &data;
        }

        template < class_kind Class >
        [[nodiscard]] class_type_data* resolve_class_type_data() {
            static class_type_data data{type_list<Class>{}};
            ensure_type<Class>(data);
            return &data;
        }

        template < class_kind Class, typename... Args >
        [[nodiscard]] constructor_type_data* resolve_constructor_type_data() {
            static constructor_type_data data{type_list<Class>{}, type_list<Args...>{}};
            return &data;
        }

        template < class_kind Class >
        [[nodiscard]] destructor_type_data* resolve_destructor_type_data() {
            static destructor_type_data data{type_list<Class>{}};
            return &data;
        }

        template < enum_kind Enum >
        [[nodiscard]] enum_type_data* resolve_enum_type_data() {
            static enum_type_data data{type_list<Enum>{}};
            ensure_type<Enum>(data);
            return &data;
        }

        template < function_kind Function >
        [[nodiscard]] function_type_data* resolve_function_type_data() {
            static function_type_data data{type_list<Function>{}};
            ensure_type<Function>(data);
            return &data;
        }

        template < member_kind Member >
        [[nodiscard]] member_type_data* resolve_member_type_data() {
            static member_type_data data{type_list<Member>{}};
            ensure_type<Member>(data);
            return &data;
        }

        template < method_kind Method >
        [[nodiscard]] method_type_data* resolve_method_type_data() {
            static method_type_data data{type_list<Method>{}};
            ensure_type<Method>(data);
            return &data;
        }

        template < nullptr_kind Nullptr >
        [[nodiscard]] nullptr_type_data* resolve_nullptr_type_data() {
            static nullptr_type_data data{type_list<Nullptr>{}};
            ensure_type<Nullptr>(data);
            return &data;
        }

        template < number_kind Number >
        [[nodiscard]] number_type_data* resolve_number_type_data() {
            static number_type_data data{type_list<Number>{}};
            ensure_type<Number>(data);
            return &data;
        }

        template < pointer_kind Pointer >
        [[nodiscard]] pointer_type_data* resolve_pointer_type_data() {
            static pointer_type_data data{type_list<Pointer>{}};
            ensure_type<Pointer>(data);
            return &data;
        }

        template < reference_kind Reference >
        [[nodiscard]] reference_type_data* resolve_reference_type_data() {
            static reference_type_data data{type_list<Reference>{}};
            ensure_type<Reference>(data);
            return &data;
        }

        template < void_kind Void >
        [[nodiscard]] void_type_data* resolve_void_type_data() {
            static void_type_data data{type_list<Void>{}};
            ensure_type<Void>(data);
            return &data;
        }
    private:
        type_registry() = default;

        template < typename Type, typename TypeData >
        void ensure_type(TypeData& type_data) {
            static std::once_flag init_flag{};
            std::call_once(init_flag, [this, &type_data](){
                const locker lock;
                type_by_id_.emplace(type_data.id, any_type{&type_data});
            #ifndef META_HPP_NO_RTTI
                type_by_rtti_.emplace(typeid(Type), any_type{&type_data});
            #endif
            });
        }
    private:
        std::recursive_mutex mutex_;
        std::map<type_id, any_type, std::less<>> type_by_id_;
        std::map<std::type_index, any_type, std::less<>> type_by_rtti_;
    };
}
