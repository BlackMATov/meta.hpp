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
    template < typename >
    struct shared_type_name;

    template < type_kind, typename... >
    struct shared_type_hash;
}

namespace meta_hpp::detail
{
    template < typename Type >
    [[nodiscard]] constexpr std::size_t shared_hash() noexcept {
        return shared_type_hash<type_to_kind_v<Type>, Type>{}();
    }

    template < enum_kind Enum >
    [[nodiscard]] constexpr std::size_t shared_hash(Enum value) noexcept {
        return static_cast<std::size_t>(value);
    }

    template < enum_kind Enum >
    [[nodiscard]] constexpr std::size_t shared_hash(bitflags<Enum> value) noexcept {
        return static_cast<std::size_t>(value.as_raw());
    }

    template < typename Integral >
        requires(std::is_integral_v<Integral> && sizeof(Integral) <= sizeof(std::size_t))
    [[nodiscard]] constexpr std::size_t shared_hash(Integral value) noexcept {
        return static_cast<std::size_t>(value);
    }

    template < typename Value >
        requires(sizeof(std::size_t) == sizeof(std::uint32_t))
    [[nodiscard]] constexpr std::size_t shared_hash_append(std::size_t seed, Value value) noexcept {
        // NOLINTNEXTLINE(*-magic-numbers)
        return (seed ^= shared_hash(value) + 0x9e3779b9U + (seed << 6) + (seed >> 2));
    }

    template < typename Value >
        requires(sizeof(std::size_t) == sizeof(std::uint64_t))
    [[nodiscard]] constexpr std::size_t shared_hash_append(std::size_t seed, Value value) noexcept {
        // NOLINTNEXTLINE(*-magic-numbers)
        return (seed ^= shared_hash(value) + 0x9e3779b97f4a7c15LLU + (seed << 12) + (seed >> 4));
    }
}

namespace meta_hpp::detail
{
    template < typename T >
    struct is_shared_type : std::false_type {};

    template < typename T >
    concept shared_type_kind = is_shared_type<std::remove_cv_t<T>>::value;
}

namespace meta_hpp::detail
{
    template < type_kind, typename... >
    struct shared_type_data_hash {
        [[nodiscard]] std::size_t operator()(const void* type_data) const noexcept {
            return hash_combiner{}(type_data);
        }
    };

    template < type_kind Kind, shared_type_kind... Types >
    struct shared_type_data_hash<Kind, Types...> {
        [[nodiscard]] std::size_t operator()(const void*) const noexcept {
            return shared_type_hash<Kind, Types...>{}();
        }
    };
}

namespace meta_hpp::detail
{
    template < typename T >
        requires requires {
            { shared_type_name<T>{}() };
        }
    struct is_shared_type<T> : std::true_type {};

    template < array_kind Array >
        requires shared_type_kind<typename array_traits<Array>::data_type>
    struct is_shared_type<Array> : std::true_type {};

    template < function_kind Function >
        requires shared_type_kind<typename function_traits<Function>::return_type>
              && type_list_and_v<is_shared_type, typename function_traits<Function>::argument_types>
    struct is_shared_type<Function> : std::true_type {};

    template < member_pointer_kind Member >
        requires shared_type_kind<typename member_traits<Member>::class_type>
              && shared_type_kind<typename member_traits<Member>::value_type>
    struct is_shared_type<Member> : std::true_type {};

    template < method_pointer_kind Method >
        requires shared_type_kind<typename method_traits<Method>::class_type>
              && shared_type_kind<typename method_traits<Method>::return_type>
              && type_list_and_v<is_shared_type, typename method_traits<Method>::argument_types>
    struct is_shared_type<Method> : std::true_type {};

    template < pointer_kind Pointer >
        requires shared_type_kind<typename pointer_traits<Pointer>::data_type>
    struct is_shared_type<Pointer> : std::true_type {};

    template < reference_kind Reference >
        requires shared_type_kind<typename reference_traits<Reference>::data_type>
    struct is_shared_type<Reference> : std::true_type {};
}

namespace meta_hpp::detail
{
    template < shared_type_kind Array >
    struct shared_type_hash<type_kind::array_, Array> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            std::size_t hash = shared_hash(type_kind::array_);

            using traits = array_traits<Array>;
            hash = shared_hash_append(hash, traits::make_flags());

            hash = shared_hash_append(hash, shared_hash<typename traits::data_type>());

            hash = shared_hash_append(hash, traits::extent);

            return hash;
        }
    };

    template < shared_type_kind Class >
    struct shared_type_hash<type_kind::class_, Class> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            std::size_t hash = shared_hash(type_kind::class_);

            using traits = class_traits<Class>;
            hash = shared_hash_append(hash, traits::make_flags());

            hash = shared_hash_append(hash, shared_type_name<Class>{}());

            return hash;
        }
    };

    template < shared_type_kind Class, shared_type_kind... Args >
    struct shared_type_hash<type_kind::constructor_, Class, Args...> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            std::size_t hash = shared_hash(type_kind::constructor_);

            using traits = constructor_traits<Class, Args...>;
            hash = shared_hash_append(hash, traits::make_flags());

            hash = shared_hash_append(hash, shared_hash<typename traits::class_type>());

            traits::argument_types::for_each([&hash]<typename Arg>() { //
                hash = shared_hash_append(hash, shared_hash<Arg>());
            });

            return hash;
        }
    };

    template < shared_type_kind Class >
    struct shared_type_hash<type_kind::destructor_, Class> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            std::size_t hash = shared_hash(type_kind::destructor_);

            using traits = destructor_traits<Class>;
            hash = shared_hash_append(hash, traits::make_flags());

            hash = shared_hash_append(hash, shared_hash<typename traits::class_type>());

            return hash;
        }
    };

    template < shared_type_kind Enum >
    struct shared_type_hash<type_kind::enum_, Enum> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            std::size_t hash = shared_hash(type_kind::enum_);

            using traits = enum_traits<Enum>;
            hash = shared_hash_append(hash, traits::make_flags());

            hash = shared_hash_append(hash, shared_type_name<Enum>{}());

            return hash;
        }
    };

    template < shared_type_kind Function >
    struct shared_type_hash<type_kind::function_, Function> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            std::size_t hash = shared_hash(type_kind::function_);

            using traits = function_traits<Function>;
            hash = shared_hash_append(hash, traits::make_flags());

            hash = shared_hash_append(hash, shared_hash<typename traits::return_type>());

            traits::argument_types::for_each([&hash]<typename Arg>() { //
                hash = shared_hash_append(hash, shared_hash<Arg>());
            });

            return hash;
        }
    };

    template < shared_type_kind Member >
    struct shared_type_hash<type_kind::member_, Member> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            std::size_t hash = shared_hash(type_kind::member_);

            using traits = member_traits<Member>;
            hash = shared_hash_append(hash, traits::make_flags());

            hash = shared_hash_append(hash, shared_hash<typename traits::class_type>());
            hash = shared_hash_append(hash, shared_hash<typename traits::value_type>());

            return hash;
        }
    };

    template < shared_type_kind Method >
    struct shared_type_hash<type_kind::method_, Method> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            std::size_t hash = shared_hash(type_kind::method_);

            using traits = method_traits<Method>;
            hash = shared_hash_append(hash, traits::make_flags());

            hash = shared_hash_append(hash, shared_hash<typename traits::class_type>());
            hash = shared_hash_append(hash, shared_hash<typename traits::return_type>());

            traits::argument_types::for_each([&hash]<typename Arg>() { //
                hash = shared_hash_append(hash, shared_hash<Arg>());
            });

            return hash;
        }
    };

    template < shared_type_kind Nullptr >
    struct shared_type_hash<type_kind::nullptr_, Nullptr> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            std::size_t hash = shared_hash(type_kind::nullptr_);

            hash = shared_hash_append(hash, shared_type_name<Nullptr>{}());

            return hash;
        }
    };

    template < shared_type_kind Number >
    struct shared_type_hash<type_kind::number_, Number> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            std::size_t hash = shared_hash(type_kind::number_);

            using traits = number_traits<Number>;
            hash = shared_hash_append(hash, traits::make_flags());

            hash = shared_hash_append(hash, shared_type_name<Number>{}());

            return hash;
        }
    };

    template < shared_type_kind Pointer >
    struct shared_type_hash<type_kind::pointer_, Pointer> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            std::size_t hash = shared_hash(type_kind::pointer_);

            using traits = pointer_traits<Pointer>;
            hash = shared_hash_append(hash, traits::make_flags());

            hash = shared_hash_append(hash, shared_hash<typename traits::data_type>());

            return hash;
        }
    };

    template < shared_type_kind Reference >
    struct shared_type_hash<type_kind::reference_, Reference> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            std::size_t hash = shared_hash(type_kind::reference_);

            using traits = reference_traits<Reference>;
            hash = shared_hash_append(hash, traits::make_flags());

            hash = shared_hash_append(hash, shared_hash<typename traits::data_type>());

            return hash;
        }
    };

    template < shared_type_kind Void >
    struct shared_type_hash<type_kind::void_, Void> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            std::size_t hash = shared_hash(type_kind::void_);

            hash = shared_hash_append(hash, shared_type_name<Void>{}());

            return hash;
        }
    };
}

#define META_HPP_DEFINE_SHARED_TYPE(Type, Name) \
    namespace meta_hpp::detail \
    { \
        template <> \
        struct shared_type_name<Type> { \
            [[nodiscard]] constexpr std::size_t operator()() const noexcept { \
                return hashed_string{Name}.get_hash(); \
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
