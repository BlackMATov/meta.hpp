/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
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
        // NOLINTNEXTLINE(*-missing-std-forward)
        void for_each_type(F&& f) const {
            const locker lock;

            // we use an index based for loop to avoid the iterator invalidation issues
            // that can happen when adding a new type inside the loop
            for ( std::size_t i{}; i < types_.size(); ++i ) {
                std::invoke(f, types_[i]);
            }
        }

    public:
        template < typename T >
        [[nodiscard]] auto resolve_type() {
            // clang-format off
            if constexpr ( array_kind<T> ) { return resolve_array_type<T>(); }
            if constexpr ( class_kind<T> ) { return resolve_class_type<T>(); }
            if constexpr ( enum_kind<T> ) { return resolve_enum_type<T>(); }
            if constexpr ( function_kind<T> ) { return resolve_function_type<T>(); }
            if constexpr ( member_pointer_kind<T> ) { return resolve_member_type<T>(); }
            if constexpr ( method_pointer_kind<T> ) { return resolve_method_type<T>(); }
            if constexpr ( nullptr_kind<T> ) { return resolve_nullptr_type<T>(); }
            if constexpr ( number_kind<T> ) { return resolve_number_type<T>(); }
            if constexpr ( pointer_kind<T> ) { return resolve_pointer_type<T>(); }
            if constexpr ( reference_kind<T> ) { return resolve_reference_type<T>(); }
            if constexpr ( void_kind<T> ) { return resolve_void_type<T>(); }
            // clang-format on
        }

    public:
        template < array_kind Array >
        [[nodiscard]] array_type resolve_array_type() {
            using array_t = std::remove_cv_t<Array>;
            static array_type type{ensure_type<array_type_data>(type_list<array_t>{})};
            return type;
        }

        template < class_kind Class >
        [[nodiscard]] class_type resolve_class_type() {
            using class_t = std::remove_cv_t<Class>;
            static class_type type{ensure_type<class_type_data>(type_list<class_t>{})};
            return type;
        }

        template < class_kind Class, typename... Args >
        [[nodiscard]] constructor_type resolve_constructor_type() {
            using class_t = std::remove_cv_t<Class>;
            static constructor_type type{ensure_type<constructor_type_data>(type_list<class_t>{}, type_list<Args...>{})};
            return type;
        }

        template < class_kind Class >
        [[nodiscard]] destructor_type resolve_destructor_type() {
            using class_t = std::remove_cv_t<Class>;
            static destructor_type type{ensure_type<destructor_type_data>(type_list<class_t>{})};
            return type;
        }

        template < enum_kind Enum >
        [[nodiscard]] enum_type resolve_enum_type() {
            using enum_t = std::remove_cv_t<Enum>;
            static enum_type type{ensure_type<enum_type_data>(type_list<enum_t>{})};
            return type;
        }

        template < function_kind Function >
        [[nodiscard]] function_type resolve_function_type() {
            using function_t = std::remove_cv_t<Function>;
            static function_type type{ensure_type<function_type_data>(type_list<function_t>{})};
            return type;
        }

        template < member_pointer_kind Member >
        [[nodiscard]] member_type resolve_member_type() {
            using member_t = std::remove_cv_t<Member>;
            static member_type type{ensure_type<member_type_data>(type_list<member_t>{})};
            return type;
        }

        template < method_pointer_kind Method >
        [[nodiscard]] method_type resolve_method_type() {
            using method_t = std::remove_cv_t<Method>;
            static method_type type{ensure_type<method_type_data>(type_list<method_t>{})};
            return type;
        }

        template < nullptr_kind Nullptr >
        [[nodiscard]] nullptr_type resolve_nullptr_type() {
            using nullptr_t = std::remove_cv_t<Nullptr>;
            static nullptr_type type{ensure_type<nullptr_type_data>(type_list<nullptr_t>{})};
            return type;
        }

        template < number_kind Number >
        [[nodiscard]] number_type resolve_number_type() {
            using number_t = std::remove_cv_t<Number>;
            static number_type type{ensure_type<number_type_data>(type_list<number_t>{})};
            return type;
        }

        template < pointer_kind Pointer >
        [[nodiscard]] pointer_type resolve_pointer_type() {
            using pointer_t = std::remove_cv_t<Pointer>;
            static pointer_type type{ensure_type<pointer_type_data>(type_list<pointer_t>{})};
            return type;
        }

        template < reference_kind Reference >
        [[nodiscard]] reference_type resolve_reference_type() {
            using reference_t = std::remove_cv_t<Reference>;
            static reference_type type{ensure_type<reference_type_data>(type_list<reference_t>{})};
            return type;
        }

        template < void_kind Void >
        [[nodiscard]] void_type resolve_void_type() {
            using void_t = std::remove_cv_t<Void>;
            static void_type type{ensure_type<void_type_data>(type_list<void_t>{})};
            return type;
        }

    private:
        type_registry() = default;

        template < typename TypeData, typename... Args >
        TypeData* ensure_type(Args&&... args) {
            static auto data{std::make_unique<TypeData>(std::forward<Args>(args)...)};

            const locker lock;
            types_.emplace_back(data.get());

            return data.get();
        }

    private:
        std::recursive_mutex mutex_;
        std::vector<any_type> types_;
    };
}
