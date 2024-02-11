/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"

#include "type_family.hpp"

namespace meta_hpp::detail
{
    namespace impl
    {
        template < typename Type >
        struct shared_type;

        template < typename Traits >
        struct shared_traits;

        template < typename Type >
        concept shared_type_kind = requires {
            { shared_type<Type>{} };
        };

        template < typename Traits >
        concept shared_traits_kind = requires {
            { shared_traits<Traits>{} };
        };

        template < typename Type >
        struct is_shared_type : std::bool_constant<shared_type_kind<Type>> {};

        template < typename Traits >
        struct is_shared_traits : std::bool_constant<shared_traits_kind<Traits>> {};
    }

    template < typename Traits >
    struct shared_traits_hash {
        [[nodiscard]] std::size_t operator()(const void* type_data) const noexcept {
            return hash_composer{} << type_data;
        }
    };

    template < impl::shared_traits_kind Traits >
    struct shared_traits_hash<Traits> {
        [[nodiscard]] std::size_t operator()(const void*) const noexcept {
            return impl::shared_traits<Traits>{}();
        }
    };
}

namespace meta_hpp::detail::impl
{
    template < array_kind Array >
        requires shared_type_kind<typename array_traits<Array>::data_type>
    struct shared_type<Array> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            return shared_traits<type_to_traits_t<Array>>{}();
        }
    };

    template < function_kind Function >
        requires shared_type_kind<typename function_traits<Function>::return_type>
              && type_list_and_v<is_shared_type, typename function_traits<Function>::argument_types>
    struct shared_type<Function> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            return shared_traits<type_to_traits_t<Function>>{}();
        }
    };

    template < member_pointer_kind Member >
        requires shared_type_kind<typename member_traits<Member>::class_type>
              && shared_type_kind<typename member_traits<Member>::value_type>
    struct shared_type<Member> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            return shared_traits<type_to_traits_t<Member>>{}();
        }
    };

    template < method_pointer_kind Method >
        requires shared_type_kind<typename method_traits<Method>::class_type>
              && shared_type_kind<typename method_traits<Method>::return_type>
              && type_list_and_v<is_shared_type, typename method_traits<Method>::argument_types>
    struct shared_type<Method> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            return shared_traits<type_to_traits_t<Method>>{}();
        }
    };

    template < pointer_kind Pointer >
        requires shared_type_kind<typename pointer_traits<Pointer>::data_type>
    struct shared_type<Pointer> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            return shared_traits<type_to_traits_t<Pointer>>{}();
        }
    };

    template < reference_kind Reference >
        requires shared_type_kind<typename reference_traits<Reference>::data_type>
    struct shared_type<Reference> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            return shared_traits<type_to_traits_t<Reference>>{}();
        }
    };
}

namespace meta_hpp::detail::impl
{
    template < shared_type_kind Array >
    struct shared_traits<array_traits<Array>> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            hash_composer hash{};
            hash << type_kind::array_;

            using traits = array_traits<Array>;
            hash << traits::make_flags();

            hash << shared_type<typename traits::data_type>{}();

            hash << traits::extent;

            return hash.hash;
        }
    };

    template < shared_type_kind Class >
    struct shared_traits<class_traits<Class>> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            hash_composer hash{};
            hash << type_kind::class_;

            using traits = class_traits<Class>;
            hash << traits::make_flags();

            hash << shared_type<Class>{}();

            return hash.hash;
        }
    };

    template < shared_type_kind Class, shared_type_kind... Args >
    struct shared_traits<constructor_traits<Class, Args...>> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            hash_composer hash{};
            hash << type_kind::constructor_;

            using traits = constructor_traits<Class, Args...>;
            hash << traits::make_flags();

            hash << shared_type<typename traits::class_type>{}();

            [&hash]<typename... ArgTypes>(type_list<ArgTypes...>) {
                ((hash << shared_type<ArgTypes>{}()), ...);
            }(typename traits::argument_types{});

            return hash.hash;
        }
    };

    template < shared_type_kind Class >
    struct shared_traits<destructor_traits<Class>> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            hash_composer hash{};
            hash << type_kind::destructor_;

            using traits = destructor_traits<Class>;
            hash << traits::make_flags();

            hash << shared_type<typename traits::class_type>{}();

            return hash.hash;
        }
    };

    template < shared_type_kind Enum >
    struct shared_traits<enum_traits<Enum>> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            hash_composer hash{};
            hash << type_kind::enum_;

            using traits = enum_traits<Enum>;
            hash << traits::make_flags();

            hash << shared_type<Enum>{}();

            return hash.hash;
        }
    };

    template < shared_type_kind Function >
    struct shared_traits<function_traits<Function>> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            hash_composer hash{};
            hash << type_kind::function_;

            using traits = function_traits<Function>;
            hash << traits::make_flags();

            hash << shared_type<typename traits::return_type>{}();

            [&hash]<typename... ArgTypes>(type_list<ArgTypes...>) {
                ((hash << shared_type<ArgTypes>{}()), ...);
            }(typename traits::argument_types{});

            return hash.hash;
        }
    };

    template < shared_type_kind Member >
    struct shared_traits<member_traits<Member>> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            hash_composer hash{};
            hash << type_kind::member_;

            using traits = member_traits<Member>;
            hash << traits::make_flags();

            hash << shared_type<typename traits::class_type>{}();
            hash << shared_type<typename traits::value_type>{}();

            return hash.hash;
        }
    };

    template < shared_type_kind Method >
    struct shared_traits<method_traits<Method>> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            hash_composer hash{};
            hash << type_kind::method_;

            using traits = method_traits<Method>;
            hash << traits::make_flags();

            hash << shared_type<typename traits::class_type>{}();
            hash << shared_type<typename traits::return_type>{}();

            [&hash]<typename... ArgTypes>(type_list<ArgTypes...>) {
                ((hash << shared_type<ArgTypes>{}()), ...);
            }(typename traits::argument_types{});

            return hash.hash;
        }
    };

    template < shared_type_kind Nullptr >
    struct shared_traits<nullptr_traits<Nullptr>> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            hash_composer hash{};
            hash << type_kind::nullptr_;

            hash << shared_type<Nullptr>{}();

            return hash.hash;
        }
    };

    template < shared_type_kind Number >
    struct shared_traits<number_traits<Number>> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            hash_composer hash{};
            hash << type_kind::number_;

            using traits = number_traits<Number>;
            hash << traits::make_flags();

            hash << shared_type<Number>{}();

            return hash.hash;
        }
    };

    template < shared_type_kind Pointer >
    struct shared_traits<pointer_traits<Pointer>> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            hash_composer hash{};
            hash << type_kind::pointer_;

            using traits = pointer_traits<Pointer>;
            hash << traits::make_flags();

            hash << shared_type<typename traits::data_type>{}();

            return hash.hash;
        }
    };

    template < shared_type_kind Reference >
    struct shared_traits<reference_traits<Reference>> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            hash_composer hash{};
            hash << type_kind::reference_;

            using traits = reference_traits<Reference>;
            hash << traits::make_flags();

            hash << shared_type<typename traits::data_type>{}();

            return hash.hash;
        }
    };

    template < shared_type_kind Void >
    struct shared_traits<void_traits<Void>> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            hash_composer hash{};
            hash << type_kind::void_;

            hash << shared_type<Void>{}();

            return hash.hash;
        }
    };
}

#define META_HPP_DEFINE_SHARED_TYPE(Type, Name) \
    namespace meta_hpp::detail::impl \
    { \
        template <> \
        struct shared_type<Type> { \
            [[nodiscard]] constexpr std::size_t operator()() const noexcept { \
                return hash_composer{} << std::string_view{Name}; \
            } \
        }; \
    }

META_HPP_DEFINE_SHARED_TYPE(void, "void")
META_HPP_DEFINE_SHARED_TYPE(bool, "bool")
META_HPP_DEFINE_SHARED_TYPE(wchar_t, "wchar_t")
META_HPP_DEFINE_SHARED_TYPE(decltype(nullptr), "nullptr_t")

META_HPP_DEFINE_SHARED_TYPE(char8_t, "char8_t")
META_HPP_DEFINE_SHARED_TYPE(char16_t, "char16_t")
META_HPP_DEFINE_SHARED_TYPE(char32_t, "char32_t")

META_HPP_DEFINE_SHARED_TYPE(float, "float")
META_HPP_DEFINE_SHARED_TYPE(double, "double")
META_HPP_DEFINE_SHARED_TYPE(long double, "long double")

META_HPP_DEFINE_SHARED_TYPE(signed char, "schar")
META_HPP_DEFINE_SHARED_TYPE(unsigned char, "uchar")
META_HPP_DEFINE_SHARED_TYPE(signed short, "sshort")
META_HPP_DEFINE_SHARED_TYPE(unsigned short, "ushort")
META_HPP_DEFINE_SHARED_TYPE(signed int, "sint")
META_HPP_DEFINE_SHARED_TYPE(unsigned int, "uint")
META_HPP_DEFINE_SHARED_TYPE(signed long, "slong")
META_HPP_DEFINE_SHARED_TYPE(unsigned long, "ulong")
META_HPP_DEFINE_SHARED_TYPE(signed long long, "sllong")
META_HPP_DEFINE_SHARED_TYPE(unsigned long long, "ullong")
