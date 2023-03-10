/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_types.hpp"

namespace meta_hpp::detail
{
    class type_registry final {
    public:
        class locker final {
        public:
            explicit locker()
            : lock_{instance().mutex_} {}

            ~locker() = default;

            locker(locker&&) = default;
            locker& operator=(locker&&) = default;

            locker(const locker&) = delete;
            locker& operator=(const locker&) = delete;

        private:
            std::unique_lock<std::recursive_mutex> lock_;
        };

        [[nodiscard]] static type_registry& instance() {
            static type_registry instance;
            return instance;
        }

    public:
        template < typename F >
        void for_each_type(F&& f) const {
            const locker lock;

            for ( const any_type& type : types_ ) {
                std::invoke(f, type);
            }
        }

        [[nodiscard]] any_type get_type_by_id(type_id id) const noexcept {
            const locker lock;

            if ( auto iter{types_.find(id)}; iter != types_.end() ) {
                return *iter;
            }

            return any_type{};
        }

    public:
        template < array_kind Array >
        [[nodiscard]] array_type resolve_type() {
            return resolve_array_type<Array>();
        }

        template < class_kind Class >
        [[nodiscard]] class_type resolve_type() {
            return resolve_class_type<Class>();
        }

        template < enum_kind Enum >
        [[nodiscard]] enum_type resolve_type() {
            return resolve_enum_type<Enum>();
        }

        template < function_pointer_kind Function >
        [[nodiscard]] function_type resolve_type() {
            return resolve_function_type<Function>();
        }

        template < member_pointer_kind Member >
        [[nodiscard]] member_type resolve_type() {
            return resolve_member_type<Member>();
        }

        template < method_pointer_kind Method >
        [[nodiscard]] method_type resolve_type() {
            return resolve_method_type<Method>();
        }

        template < nullptr_kind Nullptr >
        [[nodiscard]] nullptr_type resolve_type() {
            return resolve_nullptr_type<Nullptr>();
        }

        template < number_kind Number >
        [[nodiscard]] number_type resolve_type() {
            return resolve_number_type<Number>();
        }

        template < pointer_kind Pointer >
        [[nodiscard]] pointer_type resolve_type() {
            return resolve_pointer_type<Pointer>();
        }

        template < reference_kind Reference >
        [[nodiscard]] reference_type resolve_type() {
            return resolve_reference_type<Reference>();
        }

        template < void_kind Void >
        [[nodiscard]] void_type resolve_type() {
            return resolve_void_type<Void>();
        }

    public:
        template < array_kind Array >
        [[nodiscard]] array_type resolve_array_type() {
            return array_type{resolve_array_type_data<std::remove_cv_t<Array>>()};
        }

        template < class_kind Class >
        [[nodiscard]] class_type resolve_class_type() {
            return class_type{resolve_class_type_data<std::remove_cv_t<Class>>()};
        }

        template < class_kind Class, typename... Args >
        [[nodiscard]] constructor_type resolve_constructor_type() {
            return constructor_type{resolve_constructor_type_data<std::remove_cv_t<Class>, Args...>()};
        }

        template < class_kind Class >
        [[nodiscard]] destructor_type resolve_destructor_type() {
            return destructor_type{resolve_destructor_type_data<std::remove_cv_t<Class>>()};
        }

        template < enum_kind Enum >
        [[nodiscard]] enum_type resolve_enum_type() {
            return enum_type{resolve_enum_type_data<std::remove_cv_t<Enum>>()};
        }

        template < function_pointer_kind Function >
        [[nodiscard]] function_type resolve_function_type() {
            return function_type{resolve_function_type_data<std::remove_cv_t<Function>>()};
        }

        template < member_pointer_kind Member >
        [[nodiscard]] member_type resolve_member_type() {
            return member_type{resolve_member_type_data<std::remove_cv_t<Member>>()};
        }

        template < method_pointer_kind Method >
        [[nodiscard]] method_type resolve_method_type() {
            return method_type{resolve_method_type_data<std::remove_cv_t<Method>>()};
        }

        template < nullptr_kind Nullptr >
        [[nodiscard]] nullptr_type resolve_nullptr_type() {
            return nullptr_type{resolve_nullptr_type_data<std::remove_cv_t<Nullptr>>()};
        }

        template < number_kind Number >
        [[nodiscard]] number_type resolve_number_type() {
            return number_type{resolve_number_type_data<std::remove_cv_t<Number>>()};
        }

        template < pointer_kind Pointer >
        [[nodiscard]] pointer_type resolve_pointer_type() {
            return pointer_type{resolve_pointer_type_data<std::remove_cv_t<Pointer>>()};
        }

        template < reference_kind Reference >
        [[nodiscard]] reference_type resolve_reference_type() {
            return reference_type{resolve_reference_type_data<std::remove_cv_t<Reference>>()};
        }

        template < void_kind Void >
        [[nodiscard]] void_type resolve_void_type() {
            return void_type{resolve_void_type_data<std::remove_cv_t<Void>>()};
        }

    private:
        template < array_kind Array >
        [[nodiscard]] array_type_data* resolve_array_type_data() {
            static array_type_data data = (ensure_type<Array>(data), array_type_data{type_list<Array>{}});
            return &data;
        }

        template < class_kind Class >
        [[nodiscard]] class_type_data* resolve_class_type_data() {
            static class_type_data data = (ensure_type<Class>(data), class_type_data{type_list<Class>{}});
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
            static enum_type_data data = (ensure_type<Enum>(data), enum_type_data{type_list<Enum>{}});
            return &data;
        }

        template < function_pointer_kind Function >
        [[nodiscard]] function_type_data* resolve_function_type_data() {
            static function_type_data data = (ensure_type<Function>(data), function_type_data{type_list<Function>{}});
            return &data;
        }

        template < member_pointer_kind Member >
        [[nodiscard]] member_type_data* resolve_member_type_data() {
            static member_type_data data = (ensure_type<Member>(data), member_type_data{type_list<Member>{}});
            return &data;
        }

        template < method_pointer_kind Method >
        [[nodiscard]] method_type_data* resolve_method_type_data() {
            static method_type_data data = (ensure_type<Method>(data), method_type_data{type_list<Method>{}});
            return &data;
        }

        template < nullptr_kind Nullptr >
        [[nodiscard]] nullptr_type_data* resolve_nullptr_type_data() {
            static nullptr_type_data data = (ensure_type<Nullptr>(data), nullptr_type_data{type_list<Nullptr>{}});
            return &data;
        }

        template < number_kind Number >
        [[nodiscard]] number_type_data* resolve_number_type_data() {
            static number_type_data data = (ensure_type<Number>(data), number_type_data{type_list<Number>{}});
            return &data;
        }

        template < pointer_kind Pointer >
        [[nodiscard]] pointer_type_data* resolve_pointer_type_data() {
            static pointer_type_data data = (ensure_type<Pointer>(data), pointer_type_data{type_list<Pointer>{}});
            return &data;
        }

        template < reference_kind Reference >
        [[nodiscard]] reference_type_data* resolve_reference_type_data() {
            static reference_type_data data = (ensure_type<Reference>(data), reference_type_data{type_list<Reference>{}});
            return &data;
        }

        template < void_kind Void >
        [[nodiscard]] void_type_data* resolve_void_type_data() {
            static void_type_data data = (ensure_type<Void>(data), void_type_data{type_list<Void>{}});
            return &data;
        }

    private:
        type_registry() = default;

        template < typename Type, typename TypeData >
        void ensure_type(TypeData& type_data) {
            const locker lock;
            types_.emplace(any_type{&type_data});
        }

    private:
        std::recursive_mutex mutex_;
        std::set<any_type, std::less<>> types_;
    };
}
