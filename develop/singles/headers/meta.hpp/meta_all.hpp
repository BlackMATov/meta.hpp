/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#include <algorithm>
#include <array>
#include <atomic>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <initializer_list>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <utility>
#include <variant>
#include <vector>

#if !defined(__cpp_exceptions)
#  define META_HPP_NO_EXCEPTIONS
#endif

#if !defined(__cpp_rtti)
#  define META_HPP_NO_RTTI
#endif

#if defined(META_HPP_NO_EXCEPTIONS)
#  define META_HPP_TRY if ( true )
#  define META_HPP_CATCH(e) if ( false )
#  define META_HPP_RETHROW() std::abort()
#else
#  define META_HPP_TRY try
#  define META_HPP_CATCH(e) catch(e)
#  define META_HPP_RETHROW() throw
#endif

namespace meta_hpp::detail
{
    template < typename Enum >
    class bitflags final {
        static_assert(std::is_enum_v<Enum>);
    public:
        using enum_type = Enum;
        using underlying_type = std::underlying_type_t<Enum>;

        bitflags() = default;
        bitflags(const bitflags&) = default;
        bitflags& operator=(const bitflags&) = default;
        bitflags(bitflags&&) noexcept = default;
        bitflags& operator=(bitflags&&) noexcept = default;
        ~bitflags() = default;

        constexpr bitflags(enum_type flags)
        : flags_(static_cast<underlying_type>(flags)) {}

        constexpr explicit bitflags(underlying_type flags)
        : flags_(flags) {}

        constexpr void swap(bitflags& other) noexcept {
            using std::swap;
            swap(flags_, other.flags_);
        }

        [[nodiscard]] constexpr explicit operator bool() const noexcept {
            return !!flags_;
        }

        [[nodiscard]] constexpr underlying_type as_raw() const noexcept {
            return flags_;
        }

        [[nodiscard]] constexpr enum_type as_enum() const noexcept {
            return static_cast<enum_type>(flags_);
        }

        [[nodiscard]] constexpr bool has(bitflags flags) const noexcept {
            return flags.flags_ == (flags_ & flags.flags_);
        }

        constexpr bitflags& set(bitflags flags) noexcept {
            flags_ |= flags.flags_;
            return *this;
        }

        constexpr bitflags& toggle(bitflags flags) noexcept {
            flags_ ^= flags.flags_;
            return *this;
        }

        constexpr bitflags& clear(bitflags flags) noexcept {
            flags_ &= ~flags.flags_;
            return *this;
        }
    private:
        underlying_type flags_{};
    };

    template < typename Enum >
    constexpr void swap(bitflags<Enum>& l, bitflags<Enum>& r) noexcept {
        l.swap(r);
    }
}

namespace std
{
    template < typename Enum >
    struct hash<meta_hpp::detail::bitflags<Enum>> {
        size_t operator()(meta_hpp::detail::bitflags<Enum> bf) const noexcept {
            return hash<Enum>{}(bf.as_enum());
        }
    };
}

namespace meta_hpp::detail
{
    #define META_HPP_DEFINE_BINARY_OPERATOR(op)\
        template < typename Enum >\
        constexpr bool operator op(Enum l, bitflags<Enum> r) noexcept {\
            return l op r.as_enum();\
        }\
        template < typename Enum >\
        constexpr bool operator op(bitflags<Enum> l, Enum r) noexcept {\
            return l.as_enum() op r;\
        }\
        template < typename Enum >\
        constexpr bool operator op(std::underlying_type_t<Enum> l, bitflags<Enum> r) noexcept {\
            return l op r.as_raw();\
        }\
        template < typename Enum >\
        constexpr bool operator op(bitflags<Enum> l, std::underlying_type_t<Enum> r) noexcept {\
            return l.as_raw() op r;\
        }\
        template < typename Enum >\
        constexpr bool operator op(bitflags<Enum> l, bitflags<Enum> r) noexcept {\
            return l.as_raw() op r.as_raw();\
        }
        META_HPP_DEFINE_BINARY_OPERATOR(<)
        META_HPP_DEFINE_BINARY_OPERATOR(>)
        META_HPP_DEFINE_BINARY_OPERATOR(<=)
        META_HPP_DEFINE_BINARY_OPERATOR(>=)
        META_HPP_DEFINE_BINARY_OPERATOR(==)
        META_HPP_DEFINE_BINARY_OPERATOR(!=)
    #undef META_HPP_DEFINE_BINARY_OPERATOR
}

namespace meta_hpp::detail
{
    template < typename Enum >
    constexpr bitflags<Enum> operator~(bitflags<Enum> l) noexcept {
        return static_cast<Enum>(~l.as_raw());
    }

    #define META_HPP_DEFINE_BINARY_OPERATOR(op)\
        template < typename Enum >\
        constexpr bitflags<Enum> operator op (Enum l, bitflags<Enum> r) noexcept {\
            return bitflags{l} op r;\
        }\
        template < typename Enum >\
        constexpr bitflags<Enum> operator op (bitflags<Enum> l, Enum r) noexcept {\
            return l op bitflags<Enum>{r};\
        }\
        template < typename Enum >\
        constexpr bitflags<Enum> operator op (bitflags<Enum> l, bitflags<Enum> r) noexcept {\
            return static_cast<Enum>(l.as_raw() op r.as_raw());\
        }\
        template < typename Enum >\
        constexpr bitflags<Enum>& operator op##= (bitflags<Enum>& l, Enum r) noexcept {\
            return l = l op bitflags<Enum>{r};\
        }\
        template < typename Enum >\
        constexpr bitflags<Enum>& operator op##= (bitflags<Enum>& l, bitflags<Enum> r) noexcept {\
            return l = l op r;\
        }
        META_HPP_DEFINE_BINARY_OPERATOR(|)
        META_HPP_DEFINE_BINARY_OPERATOR(&)
        META_HPP_DEFINE_BINARY_OPERATOR(^)
    #undef META_HPP_DEFINE_BINARY_OPERATOR
}

//
// META_HPP_BITFLAGS_OPERATORS_DECL
//

#define META_HPP_BITFLAGS_OPERATORS_DECL(Enum)\
    constexpr ::meta_hpp::detail::bitflags<Enum> operator~ [[maybe_unused]] (Enum l) noexcept {\
        return ~::meta_hpp::detail::bitflags<Enum>(l);\
    }\
    constexpr ::meta_hpp::detail::bitflags<Enum> operator| [[maybe_unused]] (Enum l, Enum r) noexcept {\
        return ::meta_hpp::detail::bitflags<Enum>(l) | ::meta_hpp::detail::bitflags<Enum>(r);\
    }\
    constexpr ::meta_hpp::detail::bitflags<Enum> operator& [[maybe_unused]] (Enum l, Enum r) noexcept {\
        return ::meta_hpp::detail::bitflags<Enum>(l) & ::meta_hpp::detail::bitflags<Enum>(r);\
    }\
    constexpr ::meta_hpp::detail::bitflags<Enum> operator^ [[maybe_unused]] (Enum l, Enum r) noexcept {\
        return ::meta_hpp::detail::bitflags<Enum>(l) ^ ::meta_hpp::detail::bitflags<Enum>(r);\
    }

namespace meta_hpp::detail
{
    template < typename From >
    struct cv_traits {
        static constexpr bool is_const = std::is_const_v<std::remove_reference_t<From>>;
        static constexpr bool is_volatile = std::is_volatile_v<std::remove_reference_t<From>>;

        template < bool yesno, template < typename > typename Q, typename V >
        using apply_t_if = std::conditional_t<yesno, Q<V>, V>;

        template < typename To >
        using add_to =
            apply_t_if<is_const, std::add_const_t,
            apply_t_if<is_volatile, std::add_volatile_t,
            To>>;

        template < typename To >
        using copy_to = add_to<std::remove_cv_t<To>>;
    };

    template < typename From, typename To >
    struct add_cv {
        using type = typename cv_traits<From>::template add_to<To>;
    };

    template < typename From, typename To >
    struct copy_cv {
        using type = typename cv_traits<From>::template copy_to<To>;
    };

    template < typename From, typename To >
    using add_cv_t = typename add_cv<From, To>::type;

    template < typename From, typename To >
    using copy_cv_t = typename copy_cv<From, To>::type;
}

namespace meta_hpp::detail
{
    template < typename From >
    struct cvref_traits {
        static constexpr bool is_lvalue = std::is_lvalue_reference_v<From>;
        static constexpr bool is_rvalue = std::is_rvalue_reference_v<From>;
        static constexpr bool is_const = std::is_const_v<std::remove_reference_t<From>>;
        static constexpr bool is_volatile = std::is_volatile_v<std::remove_reference_t<From>>;

        template < bool yesno, template < typename > typename Q, typename V >
        using apply_t_if = std::conditional_t<yesno, Q<V>, V>;

        template < typename To >
        using add_to =
            apply_t_if<is_lvalue, std::add_lvalue_reference_t,
            apply_t_if<is_rvalue, std::add_rvalue_reference_t,
            apply_t_if<is_const, std::add_const_t,
            apply_t_if<is_volatile, std::add_volatile_t,
            To>>>>;

        template < typename To >
        using copy_to = add_to<std::remove_cvref_t<To>>;
    };

    template < typename From, typename To >
    struct add_cvref {
        using type = typename cvref_traits<From>::template add_to<To>;
    };

    template < typename From, typename To >
    struct copy_cvref {
        using type = typename cvref_traits<From>::template copy_to<To>;
    };

    template < typename From, typename To >
    using add_cvref_t = typename add_cvref<From, To>::type;

    template < typename From, typename To >
    using copy_cvref_t = typename copy_cvref<From, To>::type;
}

namespace meta_hpp::detail
{
    template < typename Function >
    class fixed_function;

    template < typename R, typename... Args >
    class fixed_function<R(Args...)> final {
    public:
        using result_type = R;

        fixed_function() = default;
        ~fixed_function() { reset(); }

        fixed_function(const fixed_function& other) = delete;
        fixed_function& operator=(const fixed_function& other) = delete;

        fixed_function(fixed_function&& other) noexcept {
            if ( other.vtable_ ) {
                other.vtable_->move(other, *this);
            }
        }

        fixed_function& operator=(fixed_function&& other) noexcept {
            if ( this != &other ) {
                fixed_function{std::move(other)}.swap(*this);
            }
            return *this;
        }

        template < typename F >
            requires (!std::is_same_v<fixed_function, std::decay_t<F>>)
        // NOLINTNEXTLINE(*-forwarding-reference-overload)
        fixed_function(F&& fun) {
            vtable_t::construct(*this, std::forward<F>(fun));
        }

        template < typename F >
            requires (!std::is_same_v<fixed_function, std::decay_t<F>>)
        fixed_function& operator=(F&& fun) {
            fixed_function{std::forward<F>(fun)}.swap(*this);
            return *this;
        }

        [[nodiscard]] bool is_valid() const noexcept {
            return vtable_ != nullptr;
        }

        [[nodiscard]] explicit operator bool() const noexcept {
            return is_valid();
        }

        R operator()(Args... args) const {
            assert(vtable_ && "bad function call"); // NOLINT
            return vtable_->call(*this, std::forward<Args>(args)...);
        }

        void reset() noexcept {
            if ( vtable_ ) {
                vtable_->destroy(*this);
            }
        }

        void swap(fixed_function& other) noexcept {
            vtable_t::swap(*this, other);
        }
    private:
        struct vtable_t;
        vtable_t* vtable_{};
    private:
        struct buffer_t final {
            // NOLINTNEXTLINE(*-avoid-c-arrays)
            alignas(std::max_align_t) std::byte data[sizeof(void*) * 2];
        } buffer_{};
    };

    template < typename Function >
    void swap(fixed_function<Function>& l, fixed_function<Function>& r) noexcept {
        l.swap(r);
    }
}

namespace meta_hpp::detail
{
    template < typename R, typename... Args >
    struct fixed_function<R(Args...)>::vtable_t final {
        R (*const call)(const fixed_function& self, Args... args);
        void (*const move)(fixed_function& from, fixed_function& to) noexcept;
        void (*const destroy)(fixed_function& self);

        template < typename T >
        static T* buffer_cast(buffer_t& buffer) noexcept {
            // NOLINTNEXTLINE(*-reinterpret-cast)
            return std::launder(reinterpret_cast<T*>(buffer.data));
        }

        template < typename T >
        static const T* buffer_cast(const buffer_t& buffer) noexcept {
            // NOLINTNEXTLINE(*-reinterpret-cast)
            return std::launder(reinterpret_cast<const T*>(buffer.data));
        }

        template < typename Fp >
        static vtable_t* get() {
            static_assert(std::is_same_v<Fp, std::decay_t<Fp>>);

            static vtable_t table{
                .call = +[](const fixed_function& self, Args... args) -> R {
                    assert(self); // NOLINT

                    const Fp& src = *buffer_cast<Fp>(self.buffer_);
                    return std::invoke(src, std::forward<Args>(args)...);
                },

                .move = +[](fixed_function& from, fixed_function& to) noexcept {
                    assert(from && !to); // NOLINT

                    Fp& src = *buffer_cast<Fp>(from.buffer_);
                    std::construct_at(buffer_cast<Fp>(to.buffer_), std::move(src));
                    std::destroy_at(&src);

                    to.vtable_ = from.vtable_;
                    from.vtable_ = nullptr;
                },

                .destroy = +[](fixed_function& self){
                    assert(self); // NOLINT

                    Fp& src = *buffer_cast<Fp>(self.buffer_);
                    std::destroy_at(&src);

                    self.vtable_ = nullptr;
                },
            };
            return &table;
        }

        template < typename F, typename Fp = std::decay_t<F> >
        static void construct(fixed_function& dst, F&& fun) {
            assert(!dst); // NOLINT

            static_assert(sizeof(Fp) <= sizeof(buffer_t));
            static_assert(alignof(buffer_t) % alignof(Fp) == 0);
            static_assert(std::is_invocable_r_v<R, Fp, Args...>);
            static_assert(std::is_nothrow_move_constructible_v<Fp>);

            std::construct_at(
                buffer_cast<Fp>(dst.buffer_),
                std::forward<F>(fun));

            dst.vtable_ = vtable_t::get<Fp>();
        }

        static void swap(fixed_function& l, fixed_function& r) noexcept {
            if ( (&l == &r) || (!l && !r) ) {
                return;
            }

            if ( l && r ) {
                fixed_function temp;
                r.vtable_->move(r, temp);
                l.vtable_->move(l, r);
                temp.vtable_->move(temp, l);
            } else {
                if ( l ) {
                    l.vtable_->move(l, r);
                } else {
                    r.vtable_->move(r, l);
                }
            }
        }
    };
}

namespace meta_hpp::detail
{
    namespace impl
    {
        template < typename F >
        struct strip_signature_impl;

        template < typename R, typename C, typename... Args >
        struct strip_signature_impl<R(C::*)(Args...)> { using type = R(Args...); };
        template < typename R, typename C, typename... Args >
        struct strip_signature_impl<R(C::*)(Args...) const> { using type = R(Args...); };
        template < typename R, typename C, typename... Args >
        struct strip_signature_impl<R(C::*)(Args...) &> { using type = R(Args...); };
        template < typename R, typename C, typename... Args >
        struct strip_signature_impl<R(C::*)(Args...) const &> { using type = R(Args...); };

        template < typename R, typename C, typename... Args >
        struct strip_signature_impl<R(C::*)(Args...) noexcept> { using type = R(Args...); };
        template < typename R, typename C, typename... Args >
        struct strip_signature_impl<R(C::*)(Args...) const noexcept> { using type = R(Args...); };
        template < typename R, typename C, typename... Args >
        struct strip_signature_impl<R(C::*)(Args...) & noexcept> { using type = R(Args...); };
        template < typename R, typename C, typename... Args >
        struct strip_signature_impl<R(C::*)(Args...) const & noexcept> { using type = R(Args...); };

        template < typename F >
        using strip_signature_impl_t = typename strip_signature_impl<F>::type;
    }

    template < typename R, typename... Args >
    fixed_function(R(*)(Args...)) -> fixed_function<R(Args...)>;

    template < typename F, typename S = impl::strip_signature_impl_t<decltype(&F::operator())> >
    fixed_function(F) -> fixed_function<S>;
}

namespace meta_hpp::detail
{
    struct hash_combiner {
        template < typename T >
        [[nodiscard]] std::size_t operator()(const T& x) noexcept {
            return std::hash<T>{}(x);
        }

        template < typename T >
        [[nodiscard]] std::size_t operator()(std::size_t seed, const T& x) noexcept {
            // NOLINTNEXTLINE(*-magic-numbers)
            return (seed ^= std::hash<T>{}(x) + 0x9e3779b9 + (seed << 6) + (seed >> 2));
        }
    };
}

namespace meta_hpp::detail
{
    class hashed_string final {
    public:
        hashed_string() = default;
        ~hashed_string() = default;

        hashed_string(hashed_string&&) = default;
        hashed_string(const hashed_string&) = default;

        hashed_string& operator=(hashed_string&&) = default;
        hashed_string& operator=(const hashed_string&) = default;

        hashed_string(const char* str) noexcept
        : hash_{std::hash<std::string_view>{}(str)} {}

        hashed_string(std::string_view str) noexcept
        : hash_{std::hash<std::string_view>{}(str)} {}

        hashed_string(const std::string& str) noexcept
        : hash_{std::hash<std::string_view>{}(str)} {}

        void swap(hashed_string& other) noexcept {
            std::swap(hash_, other.hash_);
        }

        [[nodiscard]] std::size_t get_hash() const noexcept {
            return hash_;
        }
    private:
        std::size_t hash_{};
    };

    inline void swap(hashed_string& l, hashed_string& r) noexcept {
        l.swap(r);
    }

    [[nodiscard]] inline bool operator<(hashed_string l, hashed_string r) noexcept {
        return l.get_hash() < r.get_hash();
    }

    [[nodiscard]] inline bool operator==(hashed_string l, hashed_string r) noexcept {
        return l.get_hash() == r.get_hash();
    }

    [[nodiscard]] inline bool operator!=(hashed_string l, hashed_string r) noexcept {
        return l.get_hash() == r.get_hash();
    }
}

namespace std
{
    template <>
    struct hash<meta_hpp::detail::hashed_string> {
        size_t operator()(meta_hpp::detail::hashed_string hs) const noexcept {
            return hs.get_hash();
        }
    };
}

namespace meta_hpp::detail
{
    template < typename Key, typename Compare, typename Allocator >
    typename std::set<Key, Compare, Allocator>::iterator
    insert_or_assign(std::set<Key, Compare, Allocator>& set,
        typename std::set<Key, Compare, Allocator>::value_type&& value)
    {
        auto&& [position, inserted] = set.insert(std::move(value));

        if ( inserted ) {
            return position;
        }

        auto node = set.extract(position++);
        node.value() = std::move(value);
        return set.insert(position, std::move(node));
    }

    template < typename Key, typename Compare, typename Allocator >
    typename std::set<Key, Compare, Allocator>::iterator
    insert_or_assign(std::set<Key, Compare, Allocator>& set,
        const typename std::set<Key, Compare, Allocator>::value_type& value)
    {
        auto&& [position, inserted] = set.insert(value);

        if ( inserted ) {
            return position;
        }

        auto node = set.extract(position++);
        node.value() = value;
        return set.insert(position, std::move(node));
    }
}

namespace meta_hpp::detail
{
    template < typename Key, typename Compare, typename Allocator >
    void insert_or_assign(std::set<Key, Compare, Allocator>& set,
        std::set<Key, Compare, Allocator>& value)
    {
        set.swap(value);
        set.merge(value);
    }

    template < typename Key, typename Compare, typename Allocator >
    void insert_or_assign(std::set<Key, Compare, Allocator>& set,
        std::set<Key, Compare, Allocator>&& value)
    {
        set.swap(value);
        set.merge(std::move(value));
    }
}


namespace meta_hpp::detail
{
    template < typename Key, typename Value, typename Compare, typename Allocator >
    void insert_or_assign(std::map<Key, Value, Compare, Allocator>& map,
        std::map<Key, Value, Compare, Allocator>& value)
    {
        map.swap(value);
        map.merge(value);
    }

    template < typename Key, typename Value, typename Compare, typename Allocator >
    void insert_or_assign(std::map<Key, Value, Compare, Allocator>& map,
        std::map<Key, Value, Compare, Allocator>&& value)
    {
        map.swap(value);
        map.merge(std::move(value));
    }
}

namespace meta_hpp::detail
{
    template < typename T >
    struct is_in_place_type : std::false_type {};

    template < typename U >
    struct is_in_place_type<std::in_place_type_t<U>> : std::true_type {};

    template < typename T >
    inline constexpr bool is_in_place_type_v = is_in_place_type<T>::value;
}

namespace meta_hpp::detail
{
    class memory_buffer final {
    public:
        memory_buffer() = default;

        memory_buffer(const memory_buffer&) = delete;
        memory_buffer& operator=(const memory_buffer&) = delete;

        memory_buffer(memory_buffer&& other) noexcept
        : memory_{other.memory_}
        , size_{other.size_}
        , align_{other.align_} {
            other.memory_ = nullptr;
            other.size_ = 0;
            other.align_ = std::align_val_t{};
        }

        memory_buffer& operator=(memory_buffer&& other) noexcept {
            if ( this != &other ) {
                memory_buffer{std::move(other)}.swap(*this);
            }
            return *this;
        }

        explicit memory_buffer(std::size_t size, std::align_val_t align)
        : memory_{::operator new(size, align)}
        , size_{size}
        , align_{align} {}

        ~memory_buffer() noexcept {
            reset();
        }

        [[nodiscard]] bool is_valid() const noexcept {
            return memory_ != nullptr;
        }

        [[nodiscard]] explicit operator bool() const noexcept {
            return is_valid();
        }

        void reset() noexcept {
            if ( memory_ != nullptr ) {
                ::operator delete(memory_, align_);
                memory_ = nullptr;
                size_ = 0;
                align_ = std::align_val_t{};
            }
        }

        void swap(memory_buffer& other) noexcept {
            std::swap(memory_, other.memory_);
            std::swap(size_, other.size_);
            std::swap(align_, other.align_);
        }

        [[nodiscard]] void* get_memory() noexcept {
            return memory_;
        }

        [[nodiscard]] const void* get_memory() const noexcept {
            return memory_;
        }

        [[nodiscard]] std::size_t get_size() const noexcept {
            return size_;
        }

        [[nodiscard]] std::align_val_t get_align() const noexcept {
            return align_;
        }
    private:
        void* memory_{};
        std::size_t size_{};
        std::align_val_t align_{};
    };

    inline void swap(memory_buffer& l, memory_buffer& r) noexcept {
        l.swap(r);
    }
}

namespace meta_hpp::detail
{
    class noncopyable {
    protected:
        noncopyable() = default;
        ~noncopyable() = default;
    public:
        noncopyable(noncopyable&&) = delete;
        noncopyable(const noncopyable&) = delete;
        noncopyable& operator=(noncopyable&&) = delete;
        noncopyable& operator=(const noncopyable&) = delete;
    };
}

namespace meta_hpp::detail
{
    template < typename... Ts >
    struct overloaded : Ts... {
        using Ts::operator()...;
    };

    template < typename... Ts >
    overloaded(Ts...) -> overloaded<Ts...>;
}

namespace meta_hpp::detail
{
    template < typename C, typename R, typename... Args >
    constexpr auto select_const(R (C::*func)(Args...) const) -> decltype(func) {
        return func;
    }

    template < typename C, typename R, typename... Args >
    constexpr auto select_const(R (C::*func)(Args...) const noexcept) -> decltype(func) {
        return func;
    }

    template < typename C, typename R, typename... Args >
    constexpr auto select_non_const(R (C::*func)(Args...)) -> decltype(func) {
        return func;
    }

    template < typename C, typename R, typename... Args >
    constexpr auto select_non_const(R (C::*func)(Args...) noexcept) -> decltype(func) {
        return func;
    }

    template < typename Signature >
    constexpr auto select_overload(Signature* func) noexcept -> decltype(func) {
        return func;
    }

    template < typename Signature, typename C >
    constexpr auto select_overload(Signature C::*func) noexcept -> decltype(func) {
        return func;
    }
}

namespace meta_hpp::detail
{
    template < typename Enum >
    [[nodiscard]] constexpr std::underlying_type_t<Enum> to_underlying(Enum e) noexcept {
        return static_cast<std::underlying_type_t<Enum>>(e);
    }
}

namespace meta_hpp::detail
{
    template < typename... Types >
    struct type_list {};

    template < std::size_t Index, typename TypeList >
    struct type_list_at;

    template < std::size_t Index, typename... Types >
    struct type_list_at<Index, type_list<Types...>> {
        using type = std::tuple_element_t<Index, std::tuple<Types...>>;
    };

    template < std::size_t Index, typename TypeList >
    using type_list_at_t = typename type_list_at<Index, TypeList>::type;
}

namespace meta_hpp::detail
{
    class type_id final {
    public:
        template < typename T >
        explicit type_id(type_list<T>) noexcept
        : id_{type_to_id<T>()} {}

        type_id(type_id&&) = default;
        type_id(const type_id&) = default;

        type_id& operator=(type_id&&) = default;
        type_id& operator=(const type_id&) = default;

        ~type_id() = default;

        [[nodiscard]] std::size_t get_hash() const noexcept {
            return std::hash<underlying_type>{}(id_);
        }

        [[nodiscard]] friend bool operator<(type_id l, type_id r) noexcept {
            return l.id_ < r.id_;
        }

        [[nodiscard]] friend bool operator==(type_id l, type_id r) noexcept {
            return l.id_ == r.id_;
        }

        [[nodiscard]] friend bool operator!=(type_id l, type_id r) noexcept {
            return l.id_ != r.id_;
        }
    private:
        using underlying_type = std::uint32_t;
        underlying_type id_{};
    private:
        [[nodiscard]] static underlying_type next() noexcept {
            static std::atomic<underlying_type> id{};
            return ++id;
        }

        template < typename T >
        [[nodiscard]] static underlying_type type_to_id() noexcept {
            static const underlying_type id{next()};
            return id;
        }
    };
}

namespace std
{
    template <>
    struct hash<meta_hpp::detail::type_id> {
        size_t operator()(const meta_hpp::detail::type_id& id) const noexcept {
            return id.get_hash();
        }
    };
}

namespace meta_hpp::detail
{
    template < typename T >
    concept array_kind = std::is_array_v<T>;

    template < typename T >
    concept class_kind = std::is_class_v<T>;

    template < typename T >
    concept enum_kind = std::is_enum_v<T>;

    template < typename T >
    concept function_kind = std::is_pointer_v<T> && std::is_function_v<std::remove_pointer_t<T>>;

    template < typename T >
    concept member_kind = std::is_member_object_pointer_v<T>;

    template < typename T >
    concept method_kind = std::is_member_function_pointer_v<T>;

    template < typename T >
    concept nullptr_kind = std::is_null_pointer_v<T>;

    template < typename T >
    concept number_kind = std::is_arithmetic_v<T>;

    template < typename T >
    concept pointer_kind = std::is_pointer_v<T> && !std::is_function_v<std::remove_pointer_t<T>>;

    template < typename T >
    concept reference_kind = std::is_reference_v<T>;

    template < typename T >
    concept void_kind = std::is_void_v<T>;
}

namespace meta_hpp::detail
{
    enum class type_kind : std::uint32_t {
        array_,
        class_,
        constructor_,
        destructor_,
        enum_,
        function_,
        member_,
        method_,
        nullptr_,
        number_,
        pointer_,
        reference_,
        void_,
    };

    template < typename T >
    constexpr type_kind make_type_kind() noexcept {
        if constexpr ( array_kind<T> ) { return type_kind::array_; }
        if constexpr ( class_kind<T> ) { return type_kind::class_; }
        if constexpr ( enum_kind<T> ) { return type_kind::enum_; }
        if constexpr ( function_kind<T> ) { return type_kind::function_; }
        if constexpr ( member_kind<T> ) { return type_kind::member_; }
        if constexpr ( method_kind<T> ) { return type_kind::method_; }
        if constexpr ( nullptr_kind<T> ) { return type_kind::nullptr_; }
        if constexpr ( number_kind<T> ) { return type_kind::number_; }
        if constexpr ( pointer_kind<T> ) { return type_kind::pointer_; }
        if constexpr ( reference_kind<T> ) { return type_kind::reference_; }
        if constexpr ( void_kind<T> ) { return type_kind::void_; }
    }
}

namespace meta_hpp
{
    using detail::hashed_string;
    using detail::memory_buffer;

    using detail::select_const;
    using detail::select_non_const;
    using detail::select_overload;

    using detail::type_id;
    using detail::type_kind;
    using detail::type_list;
}

namespace meta_hpp
{
    class exception final : public std::runtime_error {
    public:
        explicit exception(const char* what)
        : std::runtime_error(what) {}
    };

    namespace detail
    {
        inline void throw_exception_with [[noreturn]] (const char* what) {
        #if !defined(META_HPP_NO_EXCEPTIONS)
            throw ::meta_hpp::exception(what);
        #else
            (void)what;
            std::abort();
        #endif
        }
    }
}

namespace meta_hpp
{
    class uvalue;

    namespace detail
    {
        class uarg_base;
        class uarg;

        class uinst_base;
        class uinst;
    }
}

namespace meta_hpp
{
    class argument;
    class constructor;
    class destructor;
    class evalue;
    class function;
    class member;
    class method;
    class scope;
    class variable;

    namespace detail
    {
        struct argument_state;
        struct constructor_state;
        struct destructor_state;
        struct evalue_state;
        struct function_state;
        struct member_state;
        struct method_state;
        struct scope_state;
        struct variable_state;

        using argument_state_ptr = std::shared_ptr<argument_state>;
        using constructor_state_ptr = std::shared_ptr<constructor_state>;
        using destructor_state_ptr = std::shared_ptr<destructor_state>;
        using evalue_state_ptr = std::shared_ptr<evalue_state>;
        using function_state_ptr = std::shared_ptr<function_state>;
        using member_state_ptr = std::shared_ptr<member_state>;
        using method_state_ptr = std::shared_ptr<method_state>;
        using scope_state_ptr = std::shared_ptr<scope_state>;
        using variable_state_ptr = std::shared_ptr<variable_state>;
    }
}

namespace meta_hpp
{
    class any_type;
    class array_type;
    class class_type;
    class constructor_type;
    class destructor_type;
    class enum_type;
    class function_type;
    class member_type;
    class method_type;
    class nullptr_type;
    class number_type;
    class pointer_type;
    class reference_type;
    class void_type;

    namespace detail
    {
        struct type_data_base;
        struct array_type_data;
        struct class_type_data;
        struct constructor_type_data;
        struct destructor_type_data;
        struct enum_type_data;
        struct function_type_data;
        struct member_type_data;
        struct method_type_data;
        struct nullptr_type_data;
        struct number_type_data;
        struct pointer_type_data;
        struct reference_type_data;
        struct void_type_data;
    }
}

namespace meta_hpp
{
    class argument_index;
    class constructor_index;
    class destructor_index;
    class evalue_index;
    class function_index;
    class member_index;
    class method_index;
    class scope_index;
    class variable_index;
}

namespace meta_hpp
{
    using any_type_list = std::vector<any_type>;
    using argument_list = std::vector<argument>;

    using metadata_map = std::map<std::string, uvalue, std::less<>>;
    using typedef_map = std::map<std::string, any_type, std::less<>>;

    using class_set = std::set<class_type, std::less<>>;
    using enum_set = std::set<enum_type, std::less<>>;

    using constructor_set = std::set<constructor, std::less<>>;
    using destructor_set = std::set<destructor, std::less<>>;
    using evalue_set = std::set<evalue, std::less<>>;
    using function_set = std::set<function, std::less<>>;
    using member_set = std::set<member, std::less<>>;
    using method_set = std::set<method, std::less<>>;
    using scope_set = std::set<scope, std::less<>>;
    using variable_set = std::set<variable, std::less<>>;
}

namespace meta_hpp::detail
{
    template < typename T >
    concept type_family =
        std::is_same_v<T, any_type> ||
        std::is_same_v<T, array_type> ||
        std::is_same_v<T, class_type> ||
        std::is_same_v<T, constructor_type> ||
        std::is_same_v<T, destructor_type> ||
        std::is_same_v<T, enum_type> ||
        std::is_same_v<T, function_type> ||
        std::is_same_v<T, member_type> ||
        std::is_same_v<T, method_type> ||
        std::is_same_v<T, nullptr_type> ||
        std::is_same_v<T, number_type> ||
        std::is_same_v<T, pointer_type> ||
        std::is_same_v<T, reference_type> ||
        std::is_same_v<T, void_type>;

    template < type_family T >
    [[nodiscard]] auto type_access(const T& type) {
        return type.data_;
    }
}

namespace meta_hpp::detail
{
    enum class array_flags : std::uint32_t {
        is_bounded = 1 << 0,
        is_unbounded = 1 << 1,
    };

    META_HPP_BITFLAGS_OPERATORS_DECL(array_flags)
    using array_bitflags = bitflags<array_flags>;
}

namespace meta_hpp::detail
{
    template < array_kind Array >
    struct array_traits {
        static constexpr std::size_t extent{std::extent_v<Array>};

        using data_type = std::remove_extent_t<Array>;

        [[nodiscard]] static constexpr array_bitflags make_flags() noexcept {
            array_bitflags flags{};

            if constexpr ( std::is_bounded_array_v<Array> ) {
                flags.set(array_flags::is_bounded);
            }

            if constexpr ( std::is_unbounded_array_v<Array> ) {
                flags.set(array_flags::is_unbounded);
            }

            return flags;
        }
    };
}

namespace meta_hpp::detail
{
    enum class class_flags : std::uint32_t {
        is_empty = 1 << 0,
        is_final = 1 << 1,
        is_abstract = 1 << 2,
        is_polymorphic = 1 << 3,
        is_template_instantiation = 1 << 4,
    };

    META_HPP_BITFLAGS_OPERATORS_DECL(class_flags)
    using class_bitflags = bitflags<class_flags>;
}

namespace meta_hpp::detail
{
    namespace impl
    {
        template < class_kind Class >
        struct class_traits_base {
            static constexpr std::size_t arity{0};

            using argument_types = type_list<>;

            [[nodiscard]] static constexpr class_bitflags make_flags() noexcept {
                return {};
            }
        };

        template < template < typename... > typename Class, typename... Args >
        struct class_traits_base<Class<Args...>> {
            static constexpr std::size_t arity{sizeof...(Args)};

            using argument_types = type_list<Args...>;

            [[nodiscard]] static constexpr class_bitflags make_flags() noexcept {
                return class_flags::is_template_instantiation;
            }
        };
    }

    template < class_kind Class >
    struct class_traits : impl::class_traits_base<Class> {
        static constexpr std::size_t size{sizeof(Class)};
        static constexpr std::size_t align{alignof(Class)};

        [[nodiscard]] static constexpr class_bitflags make_flags() noexcept {
            class_bitflags flags{};

            if constexpr ( std::is_empty_v<Class> ) {
                flags.set(class_flags::is_empty);
            }

            if constexpr ( std::is_final_v<Class> ) {
                flags.set(class_flags::is_final);
            }

            if constexpr ( std::is_abstract_v<Class> ) {
                flags.set(class_flags::is_abstract);
            }

            if constexpr ( std::is_polymorphic_v<Class> ) {
                flags.set(class_flags::is_polymorphic);
            }

            return flags | impl::class_traits_base<Class>::make_flags();
        }
    };
}

namespace meta_hpp::detail
{
    enum class constructor_flags : std::uint32_t {
        is_noexcept = 1 << 0,
    };

    META_HPP_BITFLAGS_OPERATORS_DECL(constructor_flags)
    using constructor_bitflags = bitflags<constructor_flags>;
}

namespace meta_hpp::detail
{
    template < class_kind Class, typename... Args >
    struct constructor_traits {
        static constexpr std::size_t arity{sizeof...(Args)};

        using class_type = Class;
        using argument_types = type_list<Args...>;

        [[nodiscard]] static constexpr constructor_bitflags make_flags() noexcept {
            constructor_bitflags flags{};

            if constexpr ( std::is_nothrow_constructible_v<Class, Args...> ) {
                flags.set(constructor_flags::is_noexcept);
            }

            return flags;
        }
    };
}

namespace meta_hpp::detail
{
    enum class destructor_flags : std::uint32_t {
        is_noexcept = 1 << 0,
        is_virtual = 1 << 1,
    };

    META_HPP_BITFLAGS_OPERATORS_DECL(destructor_flags)
    using destructor_bitflags = bitflags<destructor_flags>;
}

namespace meta_hpp::detail
{
    template < class_kind Class >
    struct destructor_traits {
        using class_type = Class;

        [[nodiscard]] static constexpr destructor_bitflags make_flags() noexcept {
            destructor_bitflags flags{};

            if constexpr ( std::is_nothrow_destructible_v<Class> ) {
                flags.set(destructor_flags::is_noexcept);
            }

            if constexpr ( std::has_virtual_destructor_v<Class> ) {
                flags.set(destructor_flags::is_virtual);
            }

            return flags;
        }
    };
}

namespace meta_hpp::detail
{
    enum class enum_flags : std::uint32_t {
        is_scoped = 1 << 0,
    };

    META_HPP_BITFLAGS_OPERATORS_DECL(enum_flags)
    using enum_bitflags = bitflags<enum_flags>;
}

namespace meta_hpp::detail
{
    template < enum_kind Enum >
    struct enum_traits {
        using underlying_type = std::underlying_type_t<Enum>;

        [[nodiscard]] static constexpr enum_bitflags make_flags() noexcept {
            enum_bitflags flags{};

            if constexpr ( !std::is_convertible_v<Enum, underlying_type> ) {
                flags.set(enum_flags::is_scoped);
            }

            return flags;
        }
    };
}

namespace meta_hpp::detail
{
    enum class function_flags : std::uint32_t {
        is_noexcept = 1 << 0,
    };

    META_HPP_BITFLAGS_OPERATORS_DECL(function_flags)
    using function_bitflags = bitflags<function_flags>;
}

namespace meta_hpp::detail
{
    template < function_kind Function >
    struct function_traits;

    template < typename R, typename... Args >
    struct function_traits<R(*)(Args...)> {
        static constexpr std::size_t arity{sizeof...(Args)};

        using return_type = R;
        using argument_types = type_list<Args...>;

        [[nodiscard]] static constexpr function_bitflags make_flags() noexcept {
            return {};
        }
    };

    template < typename R, typename... Args >
    struct function_traits<R(*)(Args...) noexcept> : function_traits<R(*)(Args...)> {
        [[nodiscard]] static constexpr function_bitflags make_flags() noexcept {
            return function_flags::is_noexcept;
        }
    };
}

namespace meta_hpp::detail
{
    enum class member_flags : std::uint32_t {
        is_readonly = 1 << 0,
    };

    META_HPP_BITFLAGS_OPERATORS_DECL(member_flags)
    using member_bitflags = bitflags<member_flags>;
}

namespace meta_hpp::detail
{
    template < member_kind Member >
    struct member_traits;

    template < typename V, typename C >
    struct member_traits<V C::*> {
        using class_type = C;
        using value_type = V;

        [[nodiscard]] static constexpr member_bitflags make_flags() noexcept {
            member_bitflags flags{};

            if constexpr ( std::is_const_v<value_type> ) {
                flags.set(member_flags::is_readonly);
            }

            return flags;
        }
    };
}

namespace meta_hpp::detail
{
    enum class method_flags : std::uint32_t {
        is_const = 1 << 0,
        is_noexcept = 1 << 1,
        is_lvalue_qualified = 1 << 2,
        is_rvalue_qualified = 1 << 3,
    };

    META_HPP_BITFLAGS_OPERATORS_DECL(method_flags)
    using method_bitflags = bitflags<method_flags>;
}

namespace meta_hpp::detail
{
    template < method_kind Method >
    struct method_traits;

    template < typename R, typename C, typename... Args >
    struct method_traits<R(C::*)(Args...)> {
        static constexpr std::size_t arity{sizeof...(Args)};

        using class_type = C;
        using return_type = R;
        using qualified_type = C;
        using argument_types = type_list<Args...>;

        [[nodiscard]] static constexpr method_bitflags make_flags() noexcept {
            return {};
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R(C::*)(Args...) const> : method_traits<R(C::*)(Args...)> {
        using qualified_type = const C;

        [[nodiscard]] static constexpr method_bitflags make_flags() noexcept {
            return method_flags::is_const;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R(C::*)(Args...) noexcept> : method_traits<R(C::*)(Args...)> {
        using qualified_type = C;

        [[nodiscard]] static constexpr method_bitflags make_flags() noexcept {
            return method_flags::is_noexcept;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R(C::*)(Args...) const noexcept> : method_traits<R(C::*)(Args...)> {
        using qualified_type = const C;

        [[nodiscard]] static constexpr method_bitflags make_flags() noexcept {
            return method_flags::is_const | method_flags::is_noexcept;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R(C::*)(Args...) &> : method_traits<R(C::*)(Args...)> {
        using qualified_type = C&;

        [[nodiscard]] static constexpr method_bitflags make_flags() noexcept {
            return method_flags::is_lvalue_qualified;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R(C::*)(Args...) & noexcept> : method_traits<R(C::*)(Args...)> {
        using qualified_type = C&;

        [[nodiscard]] static constexpr method_bitflags make_flags() noexcept {
            return method_flags::is_noexcept | method_flags::is_lvalue_qualified;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R(C::*)(Args...) const &> : method_traits<R(C::*)(Args...)> {
        using qualified_type = const C&;

        [[nodiscard]] static constexpr method_bitflags make_flags() noexcept {
            return method_flags::is_const | method_flags::is_lvalue_qualified;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R(C::*)(Args...) const & noexcept> : method_traits<R(C::*)(Args...)> {
        using qualified_type = const C&;

        [[nodiscard]] static constexpr method_bitflags make_flags() noexcept {
            return method_flags::is_const | method_flags::is_noexcept | method_flags::is_lvalue_qualified;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R(C::*)(Args...) &&> : method_traits<R(C::*)(Args...)> {
        using qualified_type = C&&;

        [[nodiscard]] static constexpr method_bitflags make_flags() noexcept {
            return method_flags::is_rvalue_qualified;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R(C::*)(Args...) && noexcept> : method_traits<R(C::*)(Args...)> {
        using qualified_type = C&&;

        [[nodiscard]] static constexpr method_bitflags make_flags() noexcept {
            return method_flags::is_noexcept | method_flags::is_rvalue_qualified;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R(C::*)(Args...) const &&> : method_traits<R(C::*)(Args...)> {
        using qualified_type = const C&&;

        [[nodiscard]] static constexpr method_bitflags make_flags() noexcept {
            return method_flags::is_const | method_flags::is_rvalue_qualified;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R(C::*)(Args...) const && noexcept> : method_traits<R(C::*)(Args...)> {
        using qualified_type = const C&&;

        [[nodiscard]] static constexpr method_bitflags make_flags() noexcept {
            return method_flags::is_const | method_flags::is_noexcept | method_flags::is_rvalue_qualified;
        }
    };
}

namespace meta_hpp::detail
{
    enum class number_flags : std::uint32_t {
        is_signed = 1 << 0,
        is_unsigned = 1 << 1,
        is_integral = 1 << 2,
        is_floating_point = 1 << 3,
    };

    META_HPP_BITFLAGS_OPERATORS_DECL(number_flags)
    using number_bitflags = bitflags<number_flags>;
}

namespace meta_hpp::detail
{
    template < number_kind Number >
    struct number_traits {
        static constexpr std::size_t size{sizeof(Number)};
        static constexpr std::size_t align{alignof(Number)};

        [[nodiscard]] static constexpr number_bitflags make_flags() noexcept {
            number_bitflags flags{};

            if constexpr ( std::is_signed_v<Number> ) {
                flags.set(number_flags::is_signed);
            }

            if constexpr ( std::is_unsigned_v<Number> ) {
                flags.set(number_flags::is_unsigned);
            }

            if constexpr ( std::is_integral_v<Number> ) {
                flags.set(number_flags::is_integral);
            }

            if constexpr ( std::is_floating_point_v<Number> ) {
                flags.set(number_flags::is_floating_point);
            }

            return flags;
        }
    };
}

namespace meta_hpp::detail
{
    enum class pointer_flags : std::uint32_t {
        is_readonly = 1 << 0,
    };

    META_HPP_BITFLAGS_OPERATORS_DECL(pointer_flags)
    using pointer_bitflags = bitflags<pointer_flags>;
}

namespace meta_hpp::detail
{
    template < pointer_kind Pointer >
    struct pointer_traits {
        using data_type = std::remove_pointer_t<Pointer>;

        [[nodiscard]] static constexpr pointer_bitflags make_flags() noexcept {
            pointer_bitflags flags{};

            if constexpr ( std::is_const_v<data_type> ) {
                flags.set(pointer_flags::is_readonly);
            }

            return flags;
        }
    };
}

namespace meta_hpp::detail
{
    enum class reference_flags : std::uint32_t {
        is_readonly = 1 << 0,
        is_lvalue = 1 << 1,
        is_rvalue = 1 << 2,
    };

    META_HPP_BITFLAGS_OPERATORS_DECL(reference_flags)
    using reference_bitflags = bitflags<reference_flags>;
}

namespace meta_hpp::detail
{
    template < reference_kind Reference >
    struct reference_traits {
        using data_type = std::remove_reference_t<Reference>;

        [[nodiscard]] static constexpr reference_bitflags make_flags() noexcept {
            reference_bitflags flags{};

            if constexpr ( std::is_const_v<data_type> ) {
                flags.set(reference_flags::is_readonly);
            }

            if constexpr ( std::is_lvalue_reference_v<Reference> ) {
                flags.set(reference_flags::is_lvalue);
            }

            if constexpr ( std::is_rvalue_reference_v<Reference> ) {
                flags.set(reference_flags::is_rvalue);
            }

            return flags;
        }
    };
}

namespace meta_hpp
{
    using array_flags = detail::array_flags;
    using array_bitflags = detail::array_bitflags;

    using class_flags = detail::class_flags;
    using class_bitflags = detail::class_bitflags;

    using constructor_flags = detail::constructor_flags;
    using constructor_bitflags = detail::constructor_bitflags;

    using destructor_flags = detail::destructor_flags;
    using destructor_bitflags = detail::destructor_bitflags;

    using enum_flags = detail::enum_flags;
    using enum_bitflags = detail::enum_bitflags;

    using function_flags = detail::function_flags;
    using function_bitflags = detail::function_bitflags;

    using member_flags = detail::member_flags;
    using member_bitflags = detail::member_bitflags;

    using method_flags = detail::method_flags;
    using method_bitflags = detail::method_bitflags;

    using number_flags = detail::number_flags;
    using number_bitflags = detail::number_bitflags;

    using pointer_flags = detail::pointer_flags;
    using pointer_bitflags = detail::pointer_bitflags;

    using reference_flags = detail::reference_flags;
    using reference_bitflags = detail::reference_bitflags;
}

namespace meta_hpp
{
    class any_type final {
    public:
        explicit any_type() = default;

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] type_kind get_kind() const noexcept;

        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

        any_type(const array_type& other) noexcept;
        any_type(const class_type& other) noexcept;
        any_type(const constructor_type& other) noexcept;
        any_type(const destructor_type& other) noexcept;
        any_type(const enum_type& other) noexcept;
        any_type(const function_type& other) noexcept;
        any_type(const member_type& other) noexcept;
        any_type(const method_type& other) noexcept;
        any_type(const nullptr_type& other) noexcept;
        any_type(const number_type& other) noexcept;
        any_type(const pointer_type& other) noexcept;
        any_type(const reference_type& other) noexcept;
        any_type(const void_type& other) noexcept;

        [[nodiscard]] bool is_array() const noexcept;
        [[nodiscard]] bool is_class() const noexcept;
        [[nodiscard]] bool is_constructor() const noexcept;
        [[nodiscard]] bool is_destructor() const noexcept;
        [[nodiscard]] bool is_enum() const noexcept;
        [[nodiscard]] bool is_function() const noexcept;
        [[nodiscard]] bool is_member() const noexcept;
        [[nodiscard]] bool is_method() const noexcept;
        [[nodiscard]] bool is_nullptr() const noexcept;
        [[nodiscard]] bool is_number() const noexcept;
        [[nodiscard]] bool is_pointer() const noexcept;
        [[nodiscard]] bool is_reference() const noexcept;
        [[nodiscard]] bool is_void() const noexcept;

        [[nodiscard]] array_type as_array() const noexcept;
        [[nodiscard]] class_type as_class() const noexcept;
        [[nodiscard]] constructor_type as_constructor() const noexcept;
        [[nodiscard]] destructor_type as_destructor() const noexcept;
        [[nodiscard]] enum_type as_enum() const noexcept;
        [[nodiscard]] function_type as_function() const noexcept;
        [[nodiscard]] member_type as_member() const noexcept;
        [[nodiscard]] method_type as_method() const noexcept;
        [[nodiscard]] nullptr_type as_nullptr() const noexcept;
        [[nodiscard]] number_type as_number() const noexcept;
        [[nodiscard]] pointer_type as_pointer() const noexcept;
        [[nodiscard]] reference_type as_reference() const noexcept;
        [[nodiscard]] void_type as_void() const noexcept;
    private:
        detail::type_data_base* data_{};
        friend auto detail::type_access<any_type>(const any_type&);
    };

    class array_type final {
    public:
        array_type() = default;
        array_type(detail::array_type_data* data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] array_bitflags get_flags() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

        [[nodiscard]] std::size_t get_extent() const noexcept;
        [[nodiscard]] any_type get_data_type() const noexcept;
    private:
        detail::array_type_data* data_{};
        friend auto detail::type_access<array_type>(const array_type&);
    };

    class class_type final {
    public:
        class_type() = default;
        class_type(detail::class_type_data* data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] class_bitflags get_flags() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

        [[nodiscard]] std::size_t get_size() const noexcept;
        [[nodiscard]] std::size_t get_align() const noexcept;

        [[nodiscard]] std::size_t get_arity() const noexcept;
        [[nodiscard]] any_type get_argument_type(std::size_t position) const noexcept;
        [[nodiscard]] const any_type_list& get_argument_types() const noexcept;

        [[nodiscard]] const class_set& get_bases() const noexcept;
        [[nodiscard]] const constructor_set& get_constructors() const noexcept;
        [[nodiscard]] const destructor_set& get_destructors() const noexcept;
        [[nodiscard]] const function_set& get_functions() const noexcept;
        [[nodiscard]] const member_set& get_members() const noexcept;
        [[nodiscard]] const method_set& get_methods() const noexcept;
        [[nodiscard]] const typedef_map& get_typedefs() const noexcept;
        [[nodiscard]] const variable_set& get_variables() const noexcept;

        template < typename... Args >
        [[nodiscard]] uvalue create(Args&&... args) const;
        template < typename... Args >
        uvalue create_at(void* mem, Args&&... args) const;

        template < typename Arg >
        bool destroy(Arg&& arg) const;
        bool destroy_at(void* mem) const;

        template < detail::class_kind Derived >
        [[nodiscard]] bool is_base_of() const noexcept;
        [[nodiscard]] bool is_base_of(const class_type& derived) const noexcept;

        template < detail::class_kind Base >
        [[nodiscard]] bool is_derived_from() const noexcept;
        [[nodiscard]] bool is_derived_from(const class_type& base) const noexcept;

        [[nodiscard]] function get_function(std::string_view name) const noexcept;
        [[nodiscard]] member get_member(std::string_view name) const noexcept;
        [[nodiscard]] method get_method(std::string_view name) const noexcept;
        [[nodiscard]] any_type get_typedef(std::string_view name) const noexcept;
        [[nodiscard]] variable get_variable(std::string_view name) const noexcept;

        template < typename... Args >
        [[nodiscard]] constructor get_constructor_with() const noexcept;
        template < typename Iter >
        [[nodiscard]] constructor get_constructor_with(Iter first, Iter last) const noexcept;
        [[nodiscard]] constructor get_constructor_with(std::span<const any_type> args) const noexcept;
        [[nodiscard]] constructor get_constructor_with(std::initializer_list<any_type> args) const noexcept;

        [[nodiscard]] destructor get_destructor() const noexcept;

        template < typename... Args >
        [[nodiscard]] function get_function_with(std::string_view name) const noexcept;
        template < typename Iter >
        [[nodiscard]] function get_function_with(std::string_view name, Iter first, Iter last) const noexcept;
        [[nodiscard]] function get_function_with(std::string_view name, std::span<const any_type> args) const noexcept;
        [[nodiscard]] function get_function_with(std::string_view name, std::initializer_list<any_type> args) const noexcept;

        template < typename... Args >
        [[nodiscard]] method get_method_with(std::string_view name) const noexcept;
        template < typename Iter >
        [[nodiscard]] method get_method_with(std::string_view name, Iter first, Iter last) const noexcept;
        [[nodiscard]] method get_method_with(std::string_view name, std::span<const any_type> args) const noexcept;
        [[nodiscard]] method get_method_with(std::string_view name, std::initializer_list<any_type> args) const noexcept;
    private:
        detail::class_type_data* data_{};
        friend auto detail::type_access<class_type>(const class_type&);
    };

    class constructor_type final {
    public:
        constructor_type() = default;
        constructor_type(detail::constructor_type_data* data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] constructor_bitflags get_flags() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

        [[nodiscard]] std::size_t get_arity() const noexcept;
        [[nodiscard]] any_type get_class_type() const noexcept;
        [[nodiscard]] any_type get_argument_type(std::size_t position) const noexcept;
        [[nodiscard]] const any_type_list& get_argument_types() const noexcept;
    private:
        detail::constructor_type_data* data_{};
        friend auto detail::type_access<constructor_type>(const constructor_type&);
    };

    class destructor_type final {
    public:
        destructor_type() = default;
        destructor_type(detail::destructor_type_data* data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] destructor_bitflags get_flags() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

        [[nodiscard]] any_type get_class_type() const noexcept;
    private:
        detail::destructor_type_data* data_{};
        friend auto detail::type_access<destructor_type>(const destructor_type&);
    };

    class enum_type final {
    public:
        enum_type() = default;
        enum_type(detail::enum_type_data* data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] enum_bitflags get_flags() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

        [[nodiscard]] number_type get_underlying_type() const noexcept;

        [[nodiscard]] const evalue_set& get_evalues() const noexcept;

        [[nodiscard]] evalue get_evalue(std::string_view name) const noexcept;

        template < detail::enum_kind Enum >
        [[nodiscard]] std::string_view value_to_name(Enum value) const noexcept;
        [[nodiscard]] uvalue name_to_value(std::string_view name) const noexcept;

        template < typename T >
        [[nodiscard]] T name_to_value_as(std::string_view name) const;
    private:
        detail::enum_type_data* data_{};
        friend auto detail::type_access<enum_type>(const enum_type&);
    };

    class function_type final {
    public:
        function_type() = default;
        function_type(detail::function_type_data* data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] function_bitflags get_flags() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

        [[nodiscard]] std::size_t get_arity() const noexcept;
        [[nodiscard]] any_type get_return_type() const noexcept;
        [[nodiscard]] any_type get_argument_type(std::size_t position) const noexcept;
        [[nodiscard]] const any_type_list& get_argument_types() const noexcept;
    private:
        detail::function_type_data* data_{};
        friend auto detail::type_access<function_type>(const function_type&);
    };

    class member_type final {
    public:
        member_type() = default;
        member_type(detail::member_type_data* data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] member_bitflags get_flags() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

        [[nodiscard]] class_type get_owner_type() const noexcept;
        [[nodiscard]] any_type get_value_type() const noexcept;
    private:
        detail::member_type_data* data_{};
        friend auto detail::type_access<member_type>(const member_type&);
    };

    class method_type final {
    public:
        method_type() = default;
        method_type(detail::method_type_data* data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] method_bitflags get_flags() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

        [[nodiscard]] std::size_t get_arity() const noexcept;
        [[nodiscard]] class_type get_owner_type() const noexcept;
        [[nodiscard]] any_type get_return_type() const noexcept;
        [[nodiscard]] any_type get_argument_type(std::size_t position) const noexcept;
        [[nodiscard]] const any_type_list& get_argument_types() const noexcept;
    private:
        detail::method_type_data* data_{};
        friend auto detail::type_access<method_type>(const method_type&);
    };

    class nullptr_type final {
    public:
        nullptr_type() = default;
        nullptr_type(detail::nullptr_type_data* data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;
    private:
        detail::nullptr_type_data* data_{};
        friend auto detail::type_access<nullptr_type>(const nullptr_type&);
    };

    class number_type final {
    public:
        number_type() = default;
        number_type(detail::number_type_data* data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] number_bitflags get_flags() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

        [[nodiscard]] std::size_t get_size() const noexcept;
        [[nodiscard]] std::size_t get_align() const noexcept;
    private:
        detail::number_type_data* data_{};
        friend auto detail::type_access<number_type>(const number_type&);
    };

    class pointer_type final {
    public:
        pointer_type() = default;
        pointer_type(detail::pointer_type_data* data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] pointer_bitflags get_flags() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

        [[nodiscard]] any_type get_data_type() const noexcept;
    private:
        detail::pointer_type_data* data_{};
        friend auto detail::type_access<pointer_type>(const pointer_type&);
    };

    class reference_type final {
    public:
        reference_type() = default;
        reference_type(detail::reference_type_data* data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] reference_bitflags get_flags() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

        [[nodiscard]] any_type get_data_type() const noexcept;
    private:
        detail::reference_type_data* data_{};
        friend auto detail::type_access<reference_type>(const reference_type&);
    };

    class void_type final {
    public:
        void_type() = default;
        void_type(detail::void_type_data* data);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] type_id get_id() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;
    private:
        detail::void_type_data* data_{};
        friend auto detail::type_access<void_type>(const void_type&);
    };
}

namespace std
{
    template < meta_hpp::detail::type_family T >
    struct hash<T> {
        size_t operator()(const T& t) const noexcept {
            return meta_hpp::detail::hash_combiner{}(t.get_id());
        }
    };
}

namespace meta_hpp
{
    template < detail::type_family T, detail::type_family U >
    [[nodiscard]] bool operator<(const T& l, const U& r) noexcept {
        if ( !static_cast<bool>(r) ) {
            return false;
        }

        if ( !static_cast<bool>(l) ) {
            return true;
        }

        return l.get_id() < r.get_id();
    }

    template < detail::type_family T, detail::type_family U >
    [[nodiscard]] bool operator==(const T& l, const U& r) noexcept {
        if ( static_cast<bool>(l) != static_cast<bool>(r) ) {
            return false;
        }

        if ( !static_cast<bool>(l) ) {
            return true;
        }

        return l.get_id() == r.get_id();
    }

    template < detail::type_family T, detail::type_family U >
    [[nodiscard]] bool operator!=(const T& l, const U& r) noexcept {
        return !(l == r);
    }
}

namespace meta_hpp::detail
{
    struct type_data_base {
        const type_id id;
        const type_kind kind;

        metadata_map metadata;

        explicit type_data_base(type_id nid, type_kind nkind)
        : id{nid}
        , kind{nkind} {}
    };

    struct array_type_data final : type_data_base {
        const array_bitflags flags;
        const std::size_t extent;
        const any_type data_type;

        template < array_kind Array >
        explicit array_type_data(type_list<Array>);
    };

    struct class_type_data final : type_data_base {
        const class_bitflags flags;
        const std::size_t size;
        const std::size_t align;
        const any_type_list argument_types;

        class_set bases;
        constructor_set constructors;
        destructor_set destructors;
        function_set functions;
        member_set members;
        method_set methods;
        typedef_map typedefs;
        variable_set variables;

        struct base_info final {
            using upcast_fptr = void*(*)(void*);
            const upcast_fptr upcast;
        };

        using base_info_map = std::map<class_type, base_info, std::less<>>;
        base_info_map bases_info;

        template < class_kind Class >
        explicit class_type_data(type_list<Class>);
    };

    struct constructor_type_data final : type_data_base {
        const constructor_bitflags flags;
        const any_type class_type;
        const any_type_list argument_types;

        template < class_kind Class, typename... Args >
        explicit constructor_type_data(type_list<Class>, type_list<Args...>);
    };

    struct destructor_type_data final : type_data_base {
        const destructor_bitflags flags;
        const any_type class_type;

        template < class_kind Class >
        explicit destructor_type_data(type_list<Class>);
    };

    struct enum_type_data final : type_data_base {
        const enum_bitflags flags;
        const number_type underlying_type;

        evalue_set evalues;

        template < enum_kind Enum >
        explicit enum_type_data(type_list<Enum>);
    };

    struct function_type_data final : type_data_base {
        const function_bitflags flags;
        const any_type return_type;
        const any_type_list argument_types;

        template < function_kind Function >
        explicit function_type_data(type_list<Function>);
    };

    struct member_type_data final : type_data_base {
        const member_bitflags flags;
        const class_type owner_type;
        const any_type value_type;

        template < member_kind Member >
        explicit member_type_data(type_list<Member>);
    };

    struct method_type_data final : type_data_base {
        const method_bitflags flags;
        const class_type owner_type;
        const any_type return_type;
        const any_type_list argument_types;

        template < method_kind Method >
        explicit method_type_data(type_list<Method>);
    };

    struct nullptr_type_data final : type_data_base {
        template < nullptr_kind Nullptr >
        explicit nullptr_type_data(type_list<Nullptr>);
    };

    struct number_type_data final : type_data_base {
        const number_bitflags flags;
        const std::size_t size;
        const std::size_t align;

        template < number_kind Number >
        explicit number_type_data(type_list<Number>);
    };

    struct pointer_type_data final : type_data_base {
        const pointer_bitflags flags;
        const any_type data_type;

        template < pointer_kind Pointer >
        explicit pointer_type_data(type_list<Pointer>);
    };

    struct reference_type_data final : type_data_base {
        const reference_bitflags flags;
        const any_type data_type;

        template < reference_kind Reference >
        explicit reference_type_data(type_list<Reference>);
    };

    struct void_type_data final : type_data_base {
        template < void_kind Void >
        explicit void_type_data(type_list<Void>);
    };
}

namespace meta_hpp::detail
{
    template < typename T >
    concept index_family =
        std::is_same_v<T, argument_index> ||
        std::is_same_v<T, constructor_index> ||
        std::is_same_v<T, destructor_index> ||
        std::is_same_v<T, evalue_index> ||
        std::is_same_v<T, function_index> ||
        std::is_same_v<T, member_index> ||
        std::is_same_v<T, method_index> ||
        std::is_same_v<T, scope_index> ||
        std::is_same_v<T, variable_index>;
}

namespace meta_hpp
{
    class argument_index final {
    public:
        argument_index() = delete;
        [[nodiscard]] std::size_t get_hash() const noexcept;
        [[nodiscard]] const any_type& get_type() const noexcept;
        [[nodiscard]] std::size_t get_position() const noexcept;
    private:
        friend detail::argument_state;
        template < typename Argument >
        [[nodiscard]] static argument_index make(std::size_t position);
    private:
        explicit argument_index(any_type type, std::size_t position);
        friend bool operator<(const argument_index& l, const argument_index& r) noexcept;
        friend bool operator==(const argument_index& l, const argument_index& r) noexcept;
        friend bool operator!=(const argument_index& l, const argument_index& r) noexcept;
    private:
        any_type type_;
        std::size_t position_{};
    };

    class constructor_index final {
    public:
        constructor_index() = delete;
        [[nodiscard]] std::size_t get_hash() const noexcept;
        [[nodiscard]] const constructor_type& get_type() const noexcept;
    private:
        friend detail::constructor_state;
        template < detail::class_kind Class, typename... Args >
        [[nodiscard]] static constructor_index make();
    private:
        explicit constructor_index(constructor_type type);
        friend bool operator<(const constructor_index& l, const constructor_index& r) noexcept;
        friend bool operator==(const constructor_index& l, const constructor_index& r) noexcept;
        friend bool operator!=(const constructor_index& l, const constructor_index& r) noexcept;
    private:
        constructor_type type_;
    };

    class destructor_index final {
    public:
        destructor_index() = delete;
        [[nodiscard]] std::size_t get_hash() const noexcept;
        [[nodiscard]] const destructor_type& get_type() const noexcept;
    private:
        friend detail::destructor_state;
        template < detail::class_kind Class >
        [[nodiscard]] static destructor_index make();
    private:
        explicit destructor_index(destructor_type type);
        friend bool operator<(const destructor_index& l, const destructor_index& r) noexcept;
        friend bool operator==(const destructor_index& l, const destructor_index& r) noexcept;
        friend bool operator!=(const destructor_index& l, const destructor_index& r) noexcept;
    private:
        destructor_type type_;
    };

    class evalue_index final {
    public:
        evalue_index() = delete;
        [[nodiscard]] std::size_t get_hash() const noexcept;
        [[nodiscard]] const enum_type& get_type() const noexcept;
        [[nodiscard]] const std::string& get_name() const noexcept;
    private:
        friend detail::evalue_state;
        template < detail::enum_kind Enum >
        [[nodiscard]] static evalue_index make(std::string name);
    private:
        explicit evalue_index(enum_type type, std::string name);
        friend bool operator<(const evalue_index& l, const evalue_index& r) noexcept;
        friend bool operator==(const evalue_index& l, const evalue_index& r) noexcept;
        friend bool operator!=(const evalue_index& l, const evalue_index& r) noexcept;
    private:
        enum_type type_;
        std::string name_;
    };

    class function_index final {
    public:
        function_index() = delete;
        [[nodiscard]] std::size_t get_hash() const noexcept;
        [[nodiscard]] const function_type& get_type() const noexcept;
        [[nodiscard]] const std::string& get_name() const noexcept;
    private:
        friend detail::function_state;
        template < detail::function_kind Function >
        [[nodiscard]] static function_index make(std::string name);
    private:
        explicit function_index(function_type type, std::string name);
        friend bool operator<(const function_index& l, const function_index& r) noexcept;
        friend bool operator==(const function_index& l, const function_index& r) noexcept;
        friend bool operator!=(const function_index& l, const function_index& r) noexcept;
    private:
        function_type type_;
        std::string name_;
    };

    class member_index final {
    public:
        member_index() = delete;
        [[nodiscard]] std::size_t get_hash() const noexcept;
        [[nodiscard]] const member_type& get_type() const noexcept;
        [[nodiscard]] const std::string& get_name() const noexcept;
    private:
        friend detail::member_state;
        template < detail::member_kind Member >
        [[nodiscard]] static member_index make(std::string name);
    private:
        explicit member_index(member_type type, std::string name);
        friend bool operator<(const member_index& l, const member_index& r) noexcept;
        friend bool operator==(const member_index& l, const member_index& r) noexcept;
        friend bool operator!=(const member_index& l, const member_index& r) noexcept;
    private:
        member_type type_;
        std::string name_;
    };

    class method_index final {
    public:
        method_index() = delete;
        [[nodiscard]] std::size_t get_hash() const noexcept;
        [[nodiscard]] const method_type& get_type() const noexcept;
        [[nodiscard]] const std::string& get_name() const noexcept;
    private:
        friend detail::method_state;
        template < detail::method_kind Method >
        [[nodiscard]] static method_index make(std::string name);
    private:
        explicit method_index(method_type type, std::string name);
        friend bool operator<(const method_index& l, const method_index& r) noexcept;
        friend bool operator==(const method_index& l, const method_index& r) noexcept;
        friend bool operator!=(const method_index& l, const method_index& r) noexcept;
    private:
        method_type type_;
        std::string name_;
    };

    class scope_index final {
    public:
        scope_index() = delete;
        [[nodiscard]] std::size_t get_hash() const noexcept;
        [[nodiscard]] const std::string& get_name() const noexcept;
    private:
        friend detail::scope_state;
        [[nodiscard]] static scope_index make(std::string name);
    private:
        explicit scope_index(std::string name);
        friend bool operator<(const scope_index& l, const scope_index& r) noexcept;
        friend bool operator==(const scope_index& l, const scope_index& r) noexcept;
        friend bool operator!=(const scope_index& l, const scope_index& r) noexcept;
    private:
        std::string name_;
    };

    class variable_index final {
    public:
        variable_index() = delete;
        [[nodiscard]] std::size_t get_hash() const noexcept;
        [[nodiscard]] const pointer_type& get_type() const noexcept;
        [[nodiscard]] const std::string& get_name() const noexcept;
    private:
        friend detail::variable_state;
        template < detail::pointer_kind Pointer >
        [[nodiscard]] static variable_index make(std::string name);
    private:
        explicit variable_index(pointer_type type, std::string name);
        friend bool operator<(const variable_index& l, const variable_index& r) noexcept;
        friend bool operator==(const variable_index& l, const variable_index& r) noexcept;
        friend bool operator!=(const variable_index& l, const variable_index& r) noexcept;
    private:
        pointer_type type_;
        std::string name_;
    };
}

namespace std
{
    template < meta_hpp::detail::index_family T >
    struct hash<T> {
        size_t operator()(const T& t) const noexcept {
            return t.get_hash();
        }
    };
}

namespace meta_hpp::detail
{
    template < typename T >
    concept uvalue_kind
        = std::is_same_v<T, uvalue>;

    template < typename T >
    concept any_uvalue_kind =
        std::is_same_v<T, uarg_base> ||
        std::is_same_v<T, uarg> ||
        std::is_same_v<T, uinst_base> ||
        std::is_same_v<T, uinst> ||
        std::is_same_v<T, uvalue>;
}

namespace meta_hpp
{
    class uvalue final {
    public:
        uvalue() = default;
        ~uvalue();

        uvalue(uvalue&& other) noexcept;
        uvalue(const uvalue& other);

        uvalue& operator=(uvalue&& other) noexcept;
        uvalue& operator=(const uvalue& other);

        template < typename T, typename Tp = std::decay_t<T> >
            requires (!detail::any_uvalue_kind<Tp>)
                  && (!detail::is_in_place_type_v<Tp>)
                  && (std::is_copy_constructible_v<Tp>)
        // NOLINTNEXTLINE(*-forwarding-reference-overload)
        uvalue(T&& val);

        template < typename T, typename Tp = std::decay_t<T> >
            requires (!detail::any_uvalue_kind<Tp>)
                  && (!detail::is_in_place_type_v<Tp>)
                  && (std::is_copy_constructible_v<Tp>)
        uvalue& operator=(T&& val);

        template < typename T, typename... Args, typename Tp = std::decay_t<T> >
            requires std::is_copy_constructible_v<Tp>
                  && std::is_constructible_v<Tp, Args...>
        explicit uvalue(std::in_place_type_t<T>, Args&&... args);

        template < typename T, typename U, typename... Args, typename Tp = std::decay_t<T> >
            requires std::is_copy_constructible_v<Tp>
                  && std::is_constructible_v<Tp, std::initializer_list<U>&, Args...>
        explicit uvalue(std::in_place_type_t<T>, std::initializer_list<U> ilist, Args&&... args);

        template < typename T, typename... Args, typename Tp = std::decay_t<T> >
            requires std::is_copy_constructible_v<Tp>
                  && std::is_constructible_v<Tp, Args...>
        std::decay_t<T>& emplace(Args&&... args);

        template < typename T, typename U, typename... Args, typename Tp = std::decay_t<T> >
            requires std::is_copy_constructible_v<Tp>
                  && std::is_constructible_v<Tp, std::initializer_list<U>&, Args...>
        std::decay_t<T>& emplace(std::initializer_list<U> ilist, Args&&... args);

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        void reset();
        void swap(uvalue& other) noexcept;

        [[nodiscard]] const any_type& get_type() const noexcept;

        [[nodiscard]] void* data() noexcept;
        [[nodiscard]] const void* data() const noexcept;
        [[nodiscard]] const void* cdata() const noexcept;

        [[nodiscard]] uvalue operator*() const;
        [[nodiscard]] uvalue operator[](std::size_t index) const;

        template < typename T >
        [[nodiscard]] T get_as() &&;

        template < typename T >
        [[nodiscard]] auto get_as() &
            -> std::conditional_t<detail::pointer_kind<T>, T, T&>;

        template < typename T >
        [[nodiscard]] auto get_as() const &
            -> std::conditional_t<detail::pointer_kind<T>, T, const T&>;

        template < typename T >
        [[nodiscard]] auto try_get_as() noexcept
            -> std::conditional_t<detail::pointer_kind<T>, T, T*>;

        template < typename T >
        [[nodiscard]] auto try_get_as() const noexcept
            -> std::conditional_t<detail::pointer_kind<T>, T, const T*>;
    private:
        struct vtable_t;
        vtable_t* vtable_{};
    private:
        struct buffer_t final {
            // NOLINTNEXTLINE(*-avoid-c-arrays)
            alignas(std::max_align_t) std::byte data[sizeof(void*) * 2];
        };
        using storage_u = std::variant<std::monostate, void*, buffer_t>;
        storage_u storage_{};
    };

    inline void swap(uvalue& l, uvalue& r) noexcept {
        l.swap(r);
    }

    template < typename T, typename... Args >
    uvalue make_uvalue(Args&&... args) {
        return uvalue(std::in_place_type<T>, std::forward<Args>(args)...);
    }

    template < typename T, typename U, typename... Args >
    uvalue make_uvalue(std::initializer_list<U> ilist, Args&&... args) {
        return uvalue(std::in_place_type<T>, ilist, std::forward<Args>(args)...);
    }
}

namespace meta_hpp::detail
{
    template < typename T >
    concept state_family =
        std::is_same_v<T, argument> ||
        std::is_same_v<T, constructor> ||
        std::is_same_v<T, destructor> ||
        std::is_same_v<T, evalue> ||
        std::is_same_v<T, function> ||
        std::is_same_v<T, member> ||
        std::is_same_v<T, method> ||
        std::is_same_v<T, scope> ||
        std::is_same_v<T, variable>;

    template < state_family T >
    [[nodiscard]] auto state_access(const T& state) {
        return state.state_;
    }
}

namespace meta_hpp
{
    namespace constructor_policy
    {
        struct as_object_t final {};
        struct as_raw_pointer_t final {};
        struct as_shared_pointer_t final {};

        inline constexpr as_object_t as_object{};
        inline constexpr as_raw_pointer_t as_raw_pointer{};
        inline constexpr as_shared_pointer_t as_shared_pointer{};
    }

    namespace function_policy
    {
        struct as_copy_t final {};
        struct discard_return_t final {};
        struct return_reference_as_pointer_t final {};

        inline constexpr as_copy_t as_copy{};
        inline constexpr discard_return_t discard_return{};
        inline constexpr return_reference_as_pointer_t return_reference_as_pointer{};
    }

    namespace member_policy
    {
        struct as_copy_t final {};
        struct as_pointer_t final {};
        struct as_reference_wrapper_t final {};

        inline constexpr as_copy_t as_copy{};
        inline constexpr as_pointer_t as_pointer{};
        inline constexpr as_reference_wrapper_t as_reference_wrapper{};
    }

    namespace method_policy
    {
        struct as_copy_t final {};
        struct discard_return_t final {};
        struct return_reference_as_pointer_t final {};

        inline constexpr as_copy_t as_copy{};
        inline constexpr discard_return_t discard_return{};
        inline constexpr return_reference_as_pointer_t return_reference_as_pointer{};
    }

    namespace variable_policy
    {
        struct as_copy_t final {};
        struct as_pointer_t final {};
        struct as_reference_wrapper_t final {};

        inline constexpr as_copy_t as_copy{};
        inline constexpr as_pointer_t as_pointer{};
        inline constexpr as_reference_wrapper_t as_reference_wrapper{};
    }

    template < typename Policy >
    concept constructor_policy_kind =
        std::is_same_v<Policy, constructor_policy::as_object_t> ||
        std::is_same_v<Policy, constructor_policy::as_raw_pointer_t> ||
        std::is_same_v<Policy, constructor_policy::as_shared_pointer_t>;

    template < typename Policy >
    concept function_policy_kind =
        std::is_same_v<Policy, function_policy::as_copy_t> ||
        std::is_same_v<Policy, function_policy::discard_return_t> ||
        std::is_same_v<Policy, function_policy::return_reference_as_pointer_t>;

    template < typename Policy >
    concept member_policy_kind =
        std::is_same_v<Policy, member_policy::as_copy_t> ||
        std::is_same_v<Policy, member_policy::as_pointer_t> ||
        std::is_same_v<Policy, member_policy::as_reference_wrapper_t>;

    template < typename Policy >
    concept method_policy_kind =
        std::is_same_v<Policy, method_policy::as_copy_t> ||
        std::is_same_v<Policy, method_policy::discard_return_t> ||
        std::is_same_v<Policy, method_policy::return_reference_as_pointer_t>;

    template < typename Policy >
    concept variable_policy_kind =
        std::is_same_v<Policy, variable_policy::as_copy_t> ||
        std::is_same_v<Policy, variable_policy::as_pointer_t> ||
        std::is_same_v<Policy, variable_policy::as_reference_wrapper_t>;
}

namespace meta_hpp
{
    class argument final {
    public:
        using index_type = argument_index;

        explicit argument() = default;
        explicit argument(detail::argument_state_ptr state) noexcept;
        argument& operator=(detail::argument_state_ptr state) noexcept;

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] const argument_index& get_index() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

        [[nodiscard]] const any_type& get_type() const noexcept;
        [[nodiscard]] std::size_t get_position() const noexcept;

        [[nodiscard]] const std::string& get_name() const noexcept;
    private:
        detail::argument_state_ptr state_;
        friend auto detail::state_access<argument>(const argument&);
    };

    class constructor final {
    public:
        using index_type = constructor_index;

        explicit constructor() = default;
        constructor(detail::constructor_state_ptr state) noexcept;
        constructor& operator=(detail::constructor_state_ptr state) noexcept;

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] const constructor_index& get_index() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

        [[nodiscard]] const constructor_type& get_type() const noexcept;

        template < typename... Args >
        [[nodiscard]] uvalue create(Args&&... args) const;

        template < typename... Args >
        uvalue create_at(void* mem, Args&&... args) const;

        template < typename... Args >
        [[nodiscard]] bool is_invocable_with() const noexcept;

        template < typename... Args >
        [[nodiscard]] bool is_invocable_with(Args&&... args) const noexcept;

        [[nodiscard]] argument get_argument(std::size_t position) const noexcept;
        [[nodiscard]] const argument_list& get_arguments() const noexcept;
    private:
        detail::constructor_state_ptr state_;
        friend auto detail::state_access<constructor>(const constructor&);
    };

    class destructor final {
    public:
        using index_type = destructor_index;

        explicit destructor() = default;
        destructor(detail::destructor_state_ptr state) noexcept;
        destructor& operator=(detail::destructor_state_ptr state) noexcept;

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] const destructor_index& get_index() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

        [[nodiscard]] const destructor_type& get_type() const noexcept;

        template < typename Arg >
        bool destroy(Arg&& arg) const;

        void destroy_at(void* mem) const;
    private:
        detail::destructor_state_ptr state_;
        friend auto detail::state_access<destructor>(const destructor&);
    };

    class evalue final {
    public:
        using index_type = evalue_index;

        explicit evalue() = default;
        evalue(detail::evalue_state_ptr state) noexcept;
        evalue& operator=(detail::evalue_state_ptr state) noexcept;

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] const evalue_index& get_index() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

        [[nodiscard]] const enum_type& get_type() const noexcept;
        [[nodiscard]] const std::string& get_name() const noexcept;

        [[nodiscard]] const uvalue& get_value() const noexcept;
        [[nodiscard]] const uvalue& get_underlying_value() const noexcept;

        template < typename T >
        [[nodiscard]] T get_value_as() const;

        template < typename T >
        [[nodiscard]] T get_underlying_value_as() const;
    private:
        detail::evalue_state_ptr state_;
        friend auto detail::state_access<evalue>(const evalue&);
    };

    class function final {
    public:
        using index_type = function_index;

        explicit function() = default;
        function(detail::function_state_ptr state) noexcept;
        function& operator=(detail::function_state_ptr state) noexcept;

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] const function_index& get_index() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

        [[nodiscard]] const function_type& get_type() const noexcept;
        [[nodiscard]] const std::string& get_name() const noexcept;

        template < typename... Args >
        uvalue invoke(Args&&... args) const;

        template < typename... Args >
        uvalue operator()(Args&&... args) const;

        template < typename... Args >
        [[nodiscard]] bool is_invocable_with() const noexcept;

        template < typename... Args >
        [[nodiscard]] bool is_invocable_with(Args&&... args) const noexcept;

        [[nodiscard]] argument get_argument(std::size_t position) const noexcept;
        [[nodiscard]] const argument_list& get_arguments() const noexcept;
    private:
        detail::function_state_ptr state_;
        friend auto detail::state_access<function>(const function&);
    };

    class member final {
    public:
        using index_type = member_index;

        explicit member() = default;
        member(detail::member_state_ptr state) noexcept;
        member& operator=(detail::member_state_ptr state) noexcept;

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] const member_index& get_index() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

        [[nodiscard]] const member_type& get_type() const noexcept;
        [[nodiscard]] const std::string& get_name() const noexcept;

        template < typename Instance >
        [[nodiscard]] uvalue get(Instance&& instance) const;

        template < typename T, typename Instance >
        [[nodiscard]] T get_as(Instance&& instance) const;

        template < typename Instance, typename Value >
        void set(Instance&& instance, Value&& value) const;

        template < typename Instance >
        [[nodiscard]] uvalue operator()(Instance&& instance) const;

        template < typename Instance, typename Value >
        void operator()(Instance&& instance, Value&& value) const;

        template < typename Instance >
        [[nodiscard]] bool is_gettable_with() const noexcept;

        template < typename Instance >
        [[nodiscard]] bool is_gettable_with(Instance&& instance) const noexcept;

        template < typename Instance, typename Value >
        [[nodiscard]] bool is_settable_with() const noexcept;

        template < typename Instance, typename Value >
        [[nodiscard]] bool is_settable_with(Instance&& instance, Value&& value) const noexcept;
    private:
        detail::member_state_ptr state_;
        friend auto detail::state_access<member>(const member&);
    };

    class method final {
    public:
        using index_type = method_index;

        explicit method() = default;
        method(detail::method_state_ptr state) noexcept;
        method& operator=(detail::method_state_ptr state) noexcept;

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] const method_index& get_index() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

        [[nodiscard]] const method_type& get_type() const noexcept;
        [[nodiscard]] const std::string& get_name() const noexcept;

        template < typename Instance, typename... Args >
        uvalue invoke(Instance&& instance, Args&&... args) const;

        template < typename Instance, typename... Args >
        uvalue operator()(Instance&& instance, Args&&... args) const;

        template < typename Instance, typename... Args >
        [[nodiscard]] bool is_invocable_with() const noexcept;

        template < typename Instance, typename... Args >
        [[nodiscard]] bool is_invocable_with(Instance&& instance, Args&&... args) const noexcept;

        [[nodiscard]] argument get_argument(std::size_t position) const noexcept;
        [[nodiscard]] const argument_list& get_arguments() const noexcept;
    private:
        detail::method_state_ptr state_;
        friend auto detail::state_access<method>(const method&);
    };

    class scope final {
    public:
        using index_type = scope_index;

        explicit scope() = default;
        explicit scope(detail::scope_state_ptr state) noexcept;
        scope& operator=(detail::scope_state_ptr state) noexcept;

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] const scope_index& get_index() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

        [[nodiscard]] const std::string& get_name() const noexcept;

        [[nodiscard]] const function_set& get_functions() const noexcept;
        [[nodiscard]] const typedef_map& get_typedefs() const noexcept;
        [[nodiscard]] const variable_set& get_variables() const noexcept;

        [[nodiscard]] function get_function(std::string_view name) const noexcept;
        [[nodiscard]] any_type get_typedef(std::string_view name) const noexcept;
        [[nodiscard]] variable get_variable(std::string_view name) const noexcept;

        template < typename... Args >
        [[nodiscard]] function get_function_with(std::string_view name) const noexcept;
        template < typename Iter >
        [[nodiscard]] function get_function_with(std::string_view name, Iter first, Iter last) const noexcept;
        [[nodiscard]] function get_function_with(std::string_view name, std::span<const any_type> args) const noexcept;
        [[nodiscard]] function get_function_with(std::string_view name, std::initializer_list<any_type> args) const noexcept;
    private:
        detail::scope_state_ptr state_;
        friend auto detail::state_access<scope>(const scope&);
    };

    class variable final {
    public:
        using index_type = variable_index;

        explicit variable() = default;
        variable(detail::variable_state_ptr state) noexcept;
        variable& operator=(detail::variable_state_ptr state) noexcept;

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] const variable_index& get_index() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

        [[nodiscard]] const pointer_type& get_type() const noexcept;
        [[nodiscard]] const std::string& get_name() const noexcept;

        [[nodiscard]] uvalue get() const;

        template < typename T >
        [[nodiscard]] T get_as() const;

        template < typename Value >
        void set(Value&& value) const;

        [[nodiscard]] uvalue operator()() const;

        template < typename Value >
        void operator()(Value&& value) const;

        template < typename Value >
        [[nodiscard]] bool is_settable_with() const noexcept;

        template < typename Value >
        [[nodiscard]] bool is_settable_with(Value&& value) const noexcept;
    private:
        detail::variable_state_ptr state_;
        friend auto detail::state_access<variable>(const variable&);
    };
}

namespace meta_hpp
{
    template < detail::state_family T, detail::state_family U >
    [[nodiscard]] bool operator<(const T& l, const U& r) noexcept {
        if ( !static_cast<bool>(r) ) {
            return false;
        }

        if ( !static_cast<bool>(l) ) {
            return true;
        }

        return l.get_index() < r.get_index();
    }

    template < detail::state_family T, detail::state_family U >
    [[nodiscard]] bool operator==(const T& l, const U& r) noexcept {
        if ( static_cast<bool>(l) != static_cast<bool>(r) ) {
            return false;
        }

        if ( !static_cast<bool>(l) ) {
            return true;
        }

        return l.get_index() == r.get_index();
    }

    template < detail::state_family T, detail::state_family U >
    [[nodiscard]] bool operator!=(const T& l, const U& r) noexcept {
        return !(l == r);
    }
}

namespace meta_hpp
{
    template < detail::state_family T, typename U >
        requires std::is_same_v<U, typename T::index_type>
    [[nodiscard]] bool operator<(const T& l, const U& r) noexcept {
        return !static_cast<bool>(l) || l.get_index() < r;
    }

    template < typename T, detail::state_family U >
        requires std::is_same_v<T, typename U::index_type>
    [[nodiscard]] bool operator<(const T& l, const U& r) noexcept {
        return static_cast<bool>(r) && l < r.get_index();
    }

    template < detail::state_family T, typename U >
        requires std::is_same_v<U, typename T::index_type>
    [[nodiscard]] bool operator==(const T& l, const U& r) noexcept {
        return static_cast<bool>(l) || l.get_index() == r;
    }

    template < typename T, detail::state_family U >
        requires std::is_same_v<T, typename U::index_type>
    [[nodiscard]] bool operator==(const T& l, const U& r) noexcept {
        return static_cast<bool>(r) && l == r.get_index();
    }

    template < detail::state_family T, typename U >
        requires std::is_same_v<U, typename T::index_type>
    [[nodiscard]] bool operator!=(const T& l, const U& r) noexcept {
        return !(l == r);
    }

    template < typename T, detail::state_family U >
        requires std::is_same_v<T, typename U::index_type>
    [[nodiscard]] bool operator!=(const T& l, const U& r) noexcept {
        return !(l == r);
    }
}

namespace meta_hpp::detail
{
    struct argument_state final {
        argument_index index;
        metadata_map metadata;

        std::string name{};

        template < typename Argument >
        [[nodiscard]] static argument_state_ptr make(std::size_t position, metadata_map metadata);
    };

    struct constructor_state final {
        using create_impl = fixed_function<uvalue(std::span<const uarg>)>;
        using create_at_impl = fixed_function<uvalue(void*, std::span<const uarg>)>;
        using is_invocable_with_impl = fixed_function<bool(std::span<const uarg_base>)>;

        constructor_index index;
        metadata_map metadata;

        create_impl create;
        create_at_impl create_at;
        is_invocable_with_impl is_invocable_with;

        argument_list arguments;

        template < constructor_policy_kind Policy, class_kind Class, typename... Args >
        [[nodiscard]] static constructor_state_ptr make(metadata_map metadata);
    };

    struct destructor_state final {
        using destroy_impl = fixed_function<bool(const uarg&)>;
        using destroy_at_impl = fixed_function<void(void*)>;

        destructor_index index;
        metadata_map metadata;

        destroy_impl destroy;
        destroy_at_impl destroy_at;

        template < class_kind Class >
        [[nodiscard]] static destructor_state_ptr make(metadata_map metadata);
    };

    struct evalue_state final {
        evalue_index index;
        metadata_map metadata;

        uvalue enum_value;
        uvalue underlying_value;

        template < enum_kind Enum >
        [[nodiscard]] static evalue_state_ptr make(std::string name, Enum evalue, metadata_map metadata);
    };

    struct function_state final {
        using invoke_impl = fixed_function<uvalue(std::span<const uarg>)>;
        using is_invocable_with_impl = fixed_function<bool(std::span<const uarg_base>)>;

        function_index index;
        metadata_map metadata;

        invoke_impl invoke;
        is_invocable_with_impl is_invocable_with;

        argument_list arguments;

        template < function_policy_kind Policy, function_kind Function >
        [[nodiscard]] static function_state_ptr make(std::string name, Function function, metadata_map metadata);
    };

    struct member_state final {
        using getter_impl = fixed_function<uvalue(const uinst&)>;
        using setter_impl = fixed_function<void(const uinst&, const uarg&)>;

        using is_gettable_with_impl = fixed_function<bool(const uinst_base&)>;
        using is_settable_with_impl = fixed_function<bool(const uinst_base&, const uarg_base&)>;

        member_index index;
        metadata_map metadata;

        getter_impl getter;
        setter_impl setter;
        is_gettable_with_impl is_gettable_with;
        is_settable_with_impl is_settable_with;


        template < member_policy_kind Policy, member_kind Member >
        [[nodiscard]] static member_state_ptr make(std::string name, Member member, metadata_map metadata);
    };

    struct method_state final {
        using invoke_impl = fixed_function<uvalue(const uinst&, std::span<const uarg>)>;
        using is_invocable_with_impl = fixed_function<bool(const uinst_base&, std::span<const uarg_base>)>;

        method_index index;
        metadata_map metadata;

        invoke_impl invoke;
        is_invocable_with_impl is_invocable_with;

        argument_list arguments;

        template < method_policy_kind Policy, method_kind Method >
        [[nodiscard]] static method_state_ptr make(std::string name, Method method, metadata_map metadata);
    };

    struct scope_state final {
        scope_index index;
        metadata_map metadata;

        function_set functions{};
        typedef_map typedefs{};
        variable_set variables{};

        [[nodiscard]] static scope_state_ptr make(std::string name, metadata_map metadata);
    };

    struct variable_state final {
        using getter_impl = fixed_function<uvalue()>;
        using setter_impl = fixed_function<void(const uarg&)>;
        using is_settable_with_impl = fixed_function<bool(const uarg_base&)>;

        variable_index index;
        metadata_map metadata;

        getter_impl getter;
        setter_impl setter;
        is_settable_with_impl is_settable_with;

        template < variable_policy_kind Policy, pointer_kind Pointer >
        [[nodiscard]] static variable_state_ptr make(std::string name, Pointer pointer, metadata_map metadata);
    };
}

namespace meta_hpp::detail
{
    class state_registry final {
    public:
        class locker final {
        public:
            explicit locker() : lock_{instance().mutex_} {}
            ~locker() = default;

            locker(locker&&) = default;
            locker& operator=(locker&&) = default;

            locker(const locker&) = delete;
            locker& operator=(const locker&) = delete;
        private:
            std::unique_lock<std::recursive_mutex> lock_;
        };

        [[nodiscard]] static state_registry& instance() {
            static state_registry instance;
            return instance;
        }
    public:
        [[nodiscard]] scope get_scope_by_name(std::string_view name) const noexcept {
            const locker lock;

            if ( auto iter = scopes_.find(name); iter != scopes_.end() ) {
                return iter->second;
            }

            return scope{};
        }

        [[nodiscard]] scope resolve_scope(std::string_view name) {
            const locker lock;

            if ( auto iter = scopes_.find(name); iter != scopes_.end() ) {
                return iter->second;
            }

            auto state = scope_state::make(std::string{name}, metadata_map{});
            auto&& [iter, _] = scopes_.insert_or_assign(std::string{name}, std::move(state));
            return iter->second;
        }
    private:
        state_registry() = default;
    private:
        std::recursive_mutex mutex_;
        std::map<std::string, scope, std::less<>> scopes_;
    };
}

namespace meta_hpp::detail
{
    class type_registry final {
    public:
        class locker final {
        public:
            explicit locker() : lock_{instance().mutex_} {}
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
            #if !defined(META_HPP_NO_RTTI)
                META_HPP_TRY {
                    type_by_rtti_.emplace(typeid(Type), any_type{&type_data});
                } META_HPP_CATCH(...) {
                    type_by_id_.erase(type_data.id);
                    META_HPP_RETHROW();
                }
            #endif
            });
        }
    private:
        std::recursive_mutex mutex_;
        std::map<type_id, any_type, std::less<>> type_by_id_;
        std::map<std::type_index, any_type, std::less<>> type_by_rtti_;
    };
}

namespace meta_hpp::detail
{
    template < typename Class, typename... Args >
    concept class_bind_constructor_kind =
        class_kind<Class> &&
        std::is_constructible_v<Class, Args...>;

    template < typename Class >
    concept class_bind_destructor_kind =
        class_kind<Class> &&
        std::is_destructible_v<Class>;

    template < typename Class, typename Base >
    concept class_bind_base_kind =
        class_kind<Class> && class_kind<Base> &&
        std::derived_from<Class, Base>;

    template < typename Class, typename Member >
    concept class_bind_member_kind =
        class_kind<Class> && member_kind<Member> &&
        std::is_same_v<Class, typename member_traits<Member>::class_type>;

    template < typename Class, typename Method >
    concept class_bind_method_kind =
        class_kind<Class> && method_kind<Method> &&
        std::is_same_v<Class, typename method_traits<Method>::class_type>;
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
        argument_opts_list arguments{};
        metadata_map metadata{};
    };

    struct variable_opts final {
        metadata_map metadata{};
    };
}

namespace meta_hpp
{
    template < detail::array_kind Array >
    class array_bind final {
    public:
        explicit array_bind(metadata_map metadata);
        operator array_type() const noexcept;
    private:
        detail::array_type_data* data_{};
        detail::type_registry::locker locker_{};
    };
}

namespace meta_hpp
{
    template < detail::class_kind Class >
    class class_bind final {
    public:
        explicit class_bind(metadata_map metadata);
        operator class_type() const noexcept;

        // base_

        template < detail::class_kind... Bases >
        class_bind& base_()
            requires (... && detail::class_bind_base_kind<Class, Bases>);

        // constructor_

        template < typename... Args
                 , constructor_policy_kind Policy = constructor_policy::as_object_t >
        class_bind& constructor_(Policy = Policy{})
            requires detail::class_bind_constructor_kind<Class, Args...>;

        template < typename... Args
                 , constructor_policy_kind Policy = constructor_policy::as_object_t >
        class_bind& constructor_(constructor_opts opts, Policy = Policy{})
            requires detail::class_bind_constructor_kind<Class, Args...>;

        // destructor_

        class_bind& destructor_()
            requires detail::class_bind_destructor_kind<Class>;

        class_bind& destructor_(destructor_opts opts)
            requires detail::class_bind_destructor_kind<Class>;

        // function_

        template < detail::function_kind Function
                 , function_policy_kind Policy = function_policy::as_copy_t >
        class_bind& function_(
            std::string name,
            Function function,
            Policy = Policy{});

        template < detail::function_kind Function
                 , function_policy_kind Policy = function_policy::as_copy_t >
        class_bind& function_(
            std::string name,
            Function function,
            function_opts opts,
            Policy = Policy{});

        template < detail::function_kind Function
                 , function_policy_kind Policy = function_policy::as_copy_t >
        class_bind& function_(
            std::string name,
            Function function,
            std::initializer_list<std::string_view> arguments,
            Policy = Policy{});

        // member_

        template < detail::member_kind Member
                 , member_policy_kind Policy = member_policy::as_copy_t >
        class_bind& member_(
            std::string name,
            Member member,
            Policy = Policy{})
            requires detail::class_bind_member_kind<Class, Member>;

        template < detail::member_kind Member
                 , member_policy_kind Policy = member_policy::as_copy_t >
        class_bind& member_(
            std::string name,
            Member member,
            member_opts opts,
            Policy = Policy{})
            requires detail::class_bind_member_kind<Class, Member>;

        // method_

        template < detail::method_kind Method
                 , method_policy_kind Policy = method_policy::as_copy_t >
        class_bind& method_(
            std::string name,
            Method method,
            Policy = Policy{})
            requires detail::class_bind_method_kind<Class, Method>;

        template < detail::method_kind Method
                 , method_policy_kind Policy = method_policy::as_copy_t >
        class_bind& method_(
            std::string name,
            Method method,
            method_opts opts,
            Policy = Policy{})
            requires detail::class_bind_method_kind<Class, Method>;

        template < detail::method_kind Method
                 , method_policy_kind Policy = method_policy::as_copy_t >
        class_bind& method_(
            std::string name,
            Method method,
            std::initializer_list<std::string_view> arguments,
            Policy = Policy{})
            requires detail::class_bind_method_kind<Class, Method>;

        // typdef_

        template < typename Type >
        class_bind& typedef_(std::string name);

        // variable_

        template < detail::pointer_kind Pointer
                 , variable_policy_kind Policy = variable_policy::as_copy_t >
        class_bind& variable_(
            std::string name,
            Pointer pointer,
            Policy = Policy{});

        template < detail::pointer_kind Pointer
                 , variable_policy_kind Policy = variable_policy::as_copy_t >
        class_bind& variable_(
            std::string name,
            Pointer pointer,
            variable_opts opts,
            Policy = Policy{});
    private:
        detail::class_type_data* data_{};
        detail::type_registry::locker locker_{};
    };
}

namespace meta_hpp
{
    template < detail::enum_kind Enum >
    class enum_bind final {
    public:
        explicit enum_bind(metadata_map metadata);
        operator enum_type() const noexcept;

        enum_bind& evalue_(std::string name, Enum value);
        enum_bind& evalue_(std::string name, Enum value, evalue_opts opts);
    private:
        detail::enum_type_data* data_{};
        detail::type_registry::locker locker_{};
    };
}

namespace meta_hpp
{
    template < detail::function_kind Function >
    class function_bind final {
    public:
        explicit function_bind(metadata_map metadata);
        operator function_type() const noexcept;
    private:
        detail::function_type_data* data_{};
        detail::type_registry::locker locker_{};
    };
}

namespace meta_hpp
{
    template < detail::member_kind Member >
    class member_bind final {
    public:
        explicit member_bind(metadata_map metadata);
        operator member_type() const noexcept;
    private:
        detail::member_type_data* data_{};
        detail::type_registry::locker locker_{};
    };
}

namespace meta_hpp
{
    template < detail::method_kind Method >
    class method_bind final {
    public:
        explicit method_bind(metadata_map metadata);
        operator method_type() const noexcept;
    private:
        detail::method_type_data* data_{};
        detail::type_registry::locker locker_{};
    };
}

namespace meta_hpp
{
    template < detail::nullptr_kind Nullptr >
    class nullptr_bind final {
    public:
        explicit nullptr_bind(metadata_map metadata);
        operator nullptr_type() const noexcept;
    private:
        detail::nullptr_type_data* data_{};
        detail::type_registry::locker locker_{};
    };
}

namespace meta_hpp
{
    template < detail::number_kind Number >
    class number_bind final {
    public:
        explicit number_bind(metadata_map metadata);
        operator number_type() const noexcept;
    private:
        detail::number_type_data* data_{};
        detail::type_registry::locker locker_{};
    };
}

namespace meta_hpp
{
    template < detail::pointer_kind Pointer >
    class pointer_bind final {
    public:
        explicit pointer_bind(metadata_map metadata);
        operator pointer_type() const noexcept;
    private:
        detail::pointer_type_data* data_{};
        detail::type_registry::locker locker_{};
    };
}

namespace meta_hpp
{
    template < detail::reference_kind Reference >
    class reference_bind final {
    public:
        explicit reference_bind(metadata_map metadata);
        operator reference_type() const noexcept;
    private:
        detail::reference_type_data* data_{};
        detail::type_registry::locker locker_{};
    };
}

namespace meta_hpp
{
    template < detail::void_kind Void >
    class void_bind final {
    public:
        explicit void_bind(metadata_map metadata);
        operator void_type() const noexcept;
    private:
        detail::void_type_data* data_{};
        detail::type_registry::locker locker_{};
    };
}

namespace meta_hpp
{
    class scope_bind final {
    public:
        struct local_tag {};
        struct static_tag {};

        explicit scope_bind(std::string name, metadata_map metadata, local_tag);
        explicit scope_bind(std::string_view name, metadata_map metadata, static_tag);
        operator scope() const noexcept;

        // function_

        template < detail::function_kind Function
                 , function_policy_kind Policy = function_policy::as_copy_t >
        scope_bind& function_(
            std::string name,
            Function function,
            Policy = Policy{});

        template < detail::function_kind Function
                 , function_policy_kind Policy = function_policy::as_copy_t >
        scope_bind& function_(
            std::string name,
            Function function,
            function_opts opts,
            Policy = Policy{});

        template < detail::function_kind Function
                 , function_policy_kind Policy = function_policy::as_copy_t >
        scope_bind& function_(
            std::string name,
            Function function,
            std::initializer_list<std::string_view> arguments,
            Policy = Policy{});

        // typedef_

        template < typename Type >
        scope_bind& typedef_(std::string name);

        // variable_

        template < detail::pointer_kind Pointer
                 , variable_policy_kind Policy = variable_policy::as_copy_t >
        scope_bind& variable_(
            std::string name,
            Pointer pointer,
            Policy = Policy{});

        template < detail::pointer_kind Pointer
                 , variable_policy_kind Policy = variable_policy::as_copy_t >
        scope_bind& variable_(
            std::string name,
            Pointer pointer,
            variable_opts opts,
            Policy = Policy{});
    private:
        detail::scope_state_ptr state_;
        detail::state_registry::locker locker_{};
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

    template < detail::function_kind Function >
    function_bind<Function> function_(metadata_map metadata = {}) {
        return function_bind<Function>{std::move(metadata)};
    }

    template < detail::member_kind Member >
    member_bind<Member> member_(metadata_map metadata = {}) {
        return member_bind<Member>{std::move(metadata)};
    }

    template < detail::method_kind Method >
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
        return scope_bind{std::move(name), std::move(metadata), scope_bind::local_tag()};
    }

    inline scope_bind static_scope_(std::string_view name, metadata_map metadata = {}) {
        return scope_bind{name, std::move(metadata), scope_bind::static_tag()};
    }
}

namespace meta_hpp
{
    template < typename T >
    [[nodiscard]] auto resolve_type() {
        using namespace detail;
        type_registry& registry = type_registry::instance();
        return registry.resolve_type<std::remove_cv_t<T>>();
    }

    template < typename... Ts >
    [[nodiscard]] any_type_list resolve_types() {
        return { resolve_type<Ts>()... };
    }
}

namespace meta_hpp
{
    template < typename T >
    [[nodiscard]] auto resolve_type(T&&) {
        return resolve_type<std::remove_reference_t<T>>();
    }

    template < typename... Ts >
    [[nodiscard]] any_type_list resolve_types(type_list<Ts...>) {
        return { resolve_type<Ts>()... };
    }
}

namespace meta_hpp
{
    template < detail::class_kind Class, typename... Args >
    constructor_type resolve_constructor_type() {
        using namespace detail;
        type_registry& registry = type_registry::instance();
        return registry.resolve_constructor_type<Class, Args...>();
    }

    template < detail::class_kind Class >
    destructor_type resolve_destructor_type() {
        using namespace detail;
        type_registry& registry = type_registry::instance();
        return registry.resolve_destructor_type<Class>();
    }
}

namespace meta_hpp
{
    template < typename T >
    [[nodiscard]] any_type resolve_polymorphic_type(T&& v) noexcept {
    #if !defined(META_HPP_NO_RTTI)
        using namespace detail;
        type_registry& registry = type_registry::instance();
        return registry.get_type_by_rtti(typeid(v));
    #else
        (void)v;
        return any_type{};
    #endif
    }
}

namespace meta_hpp
{
    [[nodiscard]] inline scope resolve_scope(std::string_view name) {
        using namespace detail;
        state_registry& registry = state_registry::instance();
        return registry.resolve_scope(name);
    }
}

namespace meta_hpp
{
    template < detail::array_kind Array >
    array_bind<Array>::array_bind(metadata_map metadata)
    : data_{detail::type_access(resolve_type<Array>())} {
        detail::insert_or_assign(data_->metadata, std::move(metadata));
    }

    template < detail::array_kind Array >
    array_bind<Array>::operator array_type() const noexcept {
        return array_type{data_};
    }
}

namespace meta_hpp
{
    template < detail::class_kind Class >
    class_bind<Class>::class_bind(metadata_map metadata)
    : data_{detail::type_access(resolve_type<Class>())} {
        detail::insert_or_assign(data_->metadata, std::move(metadata));

        if constexpr ( std::is_destructible_v<Class> ) {
            destructor_();
        }
    }

    template < detail::class_kind Class >
    class_bind<Class>::operator class_type() const noexcept {
        return class_type{data_};
    }

    //
    // base_
    //

    template < detail::class_kind Class >
    template < detail::class_kind... Bases >
    class_bind<Class>& class_bind<Class>::base_()
        requires (... && detail::class_bind_base_kind<Class, Bases>)
    {
        ([this]<detail::class_kind Base>(std::in_place_type_t<Base>) {
            const class_type& base_type = resolve_type<Base>();

            if ( auto&& [_, emplaced] = data_->bases.emplace(base_type); !emplaced ) {
                return;
            }

            META_HPP_TRY {
                data_->bases_info.emplace(base_type, detail::class_type_data::base_info{
                    .upcast = +[](void* derived) -> void* {
                        return static_cast<Base*>(static_cast<Class*>(derived));
                    }
                });
            } META_HPP_CATCH(...) {
                data_->bases.erase(base_type);
                META_HPP_RETHROW();
            }
        }(std::in_place_type<Bases>), ...);

        return *this;
    }

    //
    // constructor_
    //

    template < detail::class_kind Class >
    template < typename... Args, constructor_policy_kind Policy >
    class_bind<Class>& class_bind<Class>::constructor_(Policy policy)
        requires detail::class_bind_constructor_kind<Class, Args...>
    {
        return constructor_<Args...>({}, policy);
    }

    template < detail::class_kind Class >
    template < typename... Args, constructor_policy_kind Policy >
    class_bind<Class>& class_bind<Class>::constructor_(
        constructor_opts opts,
        Policy)
        requires detail::class_bind_constructor_kind<Class, Args...>
    {
        auto state = detail::constructor_state::make<Policy, Class, Args...>(std::move(opts.metadata));

        if ( opts.arguments.size() > state->arguments.size() ) {
            detail::throw_exception_with("provided argument names don't match constructor argument count");
        }

        for ( std::size_t i = 0; i < opts.arguments.size(); ++i ) {
            argument& arg = state->arguments[i];
            detail::state_access(arg)->name = std::move(opts.arguments[i].name);
            detail::state_access(arg)->metadata = std::move(opts.arguments[i].metadata);
        }

        detail::insert_or_assign(data_->constructors, std::move(state));
        return *this;
    }

    //
    // destructor_
    //

    template < detail::class_kind Class >
    class_bind<Class>& class_bind<Class>::destructor_()
        requires detail::class_bind_destructor_kind<Class>
    {
        return destructor_({});
    }

    template < detail::class_kind Class >
    class_bind<Class>& class_bind<Class>::destructor_(destructor_opts opts)
        requires detail::class_bind_destructor_kind<Class>
    {
        auto state = detail::destructor_state::make<Class>(std::move(opts.metadata));
        detail::insert_or_assign(data_->destructors, std::move(state));
        return *this;
    }

    //
    // function_
    //

    template < detail::class_kind Class >
    template < detail::function_kind Function, function_policy_kind Policy >
    class_bind<Class>& class_bind<Class>::function_(
        std::string name,
        Function function,
        Policy policy)
    {
        return function_(std::move(name), std::move(function), {}, policy);
    }

    template < detail::class_kind Class >
    template < detail::function_kind Function, function_policy_kind Policy >
    class_bind<Class>& class_bind<Class>::function_(
        std::string name,
        Function function,
        function_opts opts,
        Policy)
    {
        auto state = detail::function_state::make<Policy>(
            std::move(name),
            std::move(function),
            std::move(opts.metadata));

        if ( opts.arguments.size() > state->arguments.size() ) {
            detail::throw_exception_with("provided arguments don't match function argument count");
        }

        for ( std::size_t i = 0; i < opts.arguments.size(); ++i ) {
            argument& arg = state->arguments[i];
            detail::state_access(arg)->name = std::move(opts.arguments[i].name);
            detail::state_access(arg)->metadata = std::move(opts.arguments[i].metadata);
        }

        detail::insert_or_assign(data_->functions, std::move(state));
        return *this;
    }

    template < detail::class_kind Class >
    template < detail::function_kind Function, function_policy_kind Policy >
    class_bind<Class>& class_bind<Class>::function_(
        std::string name,
        Function function,
        std::initializer_list<std::string_view> arguments,
        Policy)
    {
        auto state = detail::function_state::make<Policy>(
            std::move(name),
            std::move(function),
            {});

        if ( arguments.size() > state->arguments.size() ) {
            detail::throw_exception_with("provided argument names don't match function argument count");
        }

        for ( std::size_t i = 0; i < arguments.size(); ++i ) {
            argument& arg = state->arguments[i];
            // NOLINTNEXTLINE(*-pointer-arithmetic)
            detail::state_access(arg)->name = std::data(arguments)[i];
        }

        detail::insert_or_assign(data_->functions, std::move(state));
        return *this;
    }

    //
    // member_
    //

    template < detail::class_kind Class >
    template < detail::member_kind Member, member_policy_kind Policy >
    class_bind<Class>& class_bind<Class>::member_(
        std::string name,
        Member member,
        Policy policy)
        requires detail::class_bind_member_kind<Class, Member>
    {
        return member_(std::move(name), std::move(member), {}, policy);
    }

    template < detail::class_kind Class >
    template < detail::member_kind Member, member_policy_kind Policy >
    class_bind<Class>& class_bind<Class>::member_(
        std::string name,
        Member member,
        member_opts opts,
        Policy)
        requires detail::class_bind_member_kind<Class, Member>
    {
        auto state = detail::member_state::make<Policy>(
            std::move(name),
            std::move(member),
            std::move(opts.metadata));
        detail::insert_or_assign(data_->members, std::move(state));
        return *this;
    }

    //
    // method_
    //

    template < detail::class_kind Class >
    template < detail::method_kind Method, method_policy_kind Policy >
    class_bind<Class>& class_bind<Class>::method_(
        std::string name,
        Method method,
        Policy policy)
        requires detail::class_bind_method_kind<Class, Method>
    {
        return method_(std::move(name), std::move(method), {}, policy);
    }

    template < detail::class_kind Class >
    template < detail::method_kind Method, method_policy_kind Policy >
    class_bind<Class>& class_bind<Class>::method_(
        std::string name,
        Method method,
        method_opts opts,
        Policy)
        requires detail::class_bind_method_kind<Class, Method>
    {
        auto state = detail::method_state::make<Policy>(
            std::move(name),
            std::move(method),
            std::move(opts.metadata));

        if ( opts.arguments.size() > state->arguments.size() ) {
            detail::throw_exception_with("provided arguments don't match method argument count");
        }

        for ( std::size_t i = 0; i < opts.arguments.size(); ++i ) {
            argument& arg = state->arguments[i];
            detail::state_access(arg)->name = std::move(opts.arguments[i].name);
            detail::state_access(arg)->metadata = std::move(opts.arguments[i].metadata);
        }

        detail::insert_or_assign(data_->methods, std::move(state));
        return *this;
    }

    template < detail::class_kind Class >
    template < detail::method_kind Method, method_policy_kind Policy >
    class_bind<Class>& class_bind<Class>::method_(
        std::string name,
        Method method,
        std::initializer_list<std::string_view> arguments,
        Policy)
        requires detail::class_bind_method_kind<Class, Method>
    {
        auto state = detail::method_state::make<Policy>(
            std::move(name),
            std::move(method),
            {});

        if ( arguments.size() > state->arguments.size() ) {
            detail::throw_exception_with("provided argument names don't match method argument count");
        }

        for ( std::size_t i = 0; i < arguments.size(); ++i ) {
            argument& arg = state->arguments[i];
            // NOLINTNEXTLINE(*-pointer-arithmetic)
            detail::state_access(arg)->name = std::data(arguments)[i];
        }

        detail::insert_or_assign(data_->methods, std::move(state));
        return *this;
    }

    //
    // typedef_
    //

    template < detail::class_kind Class >
    template < typename Type >
    class_bind<Class>& class_bind<Class>::typedef_(std::string name) {
        data_->typedefs.insert_or_assign(std::move(name), resolve_type<Type>());
        return *this;
    }

    //
    // variable_
    //

    template < detail::class_kind Class >
    template < detail::pointer_kind Pointer, variable_policy_kind Policy >
    class_bind<Class>& class_bind<Class>::variable_(
        std::string name,
        Pointer pointer,
        Policy policy)
    {
        return variable_(std::move(name), std::move(pointer), {}, policy);
    }

    template < detail::class_kind Class >
    template < detail::pointer_kind Pointer, variable_policy_kind Policy >
    class_bind<Class>& class_bind<Class>::variable_(
        std::string name,
        Pointer pointer,
        variable_opts opts,
        Policy)
    {
        auto state = detail::variable_state::make<Policy>(
            std::move(name),
            std::move(pointer),
            std::move(opts.metadata));
        detail::insert_or_assign(data_->variables, std::move(state));
        return *this;
    }
}

namespace meta_hpp
{
    template < detail::enum_kind Enum >
    enum_bind<Enum>::enum_bind(metadata_map metadata)
    : data_{detail::type_access(resolve_type<Enum>())} {
        detail::insert_or_assign(data_->metadata, std::move(metadata));
    }

    template < detail::enum_kind Enum >
    enum_bind<Enum>::operator enum_type() const noexcept {
        return enum_type{data_};
    }

    template < detail::enum_kind Enum >
    enum_bind<Enum>& enum_bind<Enum>::evalue_(std::string name, Enum value) {
        return evalue_(std::move(name), std::move(value), {});
    }

    template < detail::enum_kind Enum >
    enum_bind<Enum>& enum_bind<Enum>::evalue_(std::string name, Enum value, evalue_opts opts) {
        auto state = detail::evalue_state::make(
            std::move(name),
            std::move(value),
            std::move(opts.metadata));
        detail::insert_or_assign(data_->evalues, std::move(state));
        return *this;
    }
}

namespace meta_hpp
{
    template < detail::function_kind Function >
    function_bind<Function>::function_bind(metadata_map metadata)
    : data_{detail::type_access(resolve_type<Function>())} {
        detail::insert_or_assign(data_->metadata, std::move(metadata));
    }

    template < detail::function_kind Function >
    function_bind<Function>::operator function_type() const noexcept {
        return function_type{data_};
    }
}

namespace meta_hpp
{
    template < detail::member_kind Member >
    member_bind<Member>::member_bind(metadata_map metadata)
    : data_{detail::type_access(resolve_type<Member>())} {
        detail::insert_or_assign(data_->metadata, std::move(metadata));
    }

    template < detail::member_kind Member >
    member_bind<Member>::operator member_type() const noexcept {
        return member_type{data_};
    }
}

namespace meta_hpp
{
    template < detail::method_kind Method >
    method_bind<Method>::method_bind(metadata_map metadata)
    : data_{detail::type_access(resolve_type<Method>())} {
        detail::insert_or_assign(data_->metadata, std::move(metadata));
    }

    template < detail::method_kind Method >
    method_bind<Method>::operator method_type() const noexcept {
        return method_type{data_};
    }
}

namespace meta_hpp
{
    template < detail::nullptr_kind Nullptr >
    nullptr_bind<Nullptr>::nullptr_bind(metadata_map metadata)
    : data_{detail::type_access(resolve_type<Nullptr>())} {
        detail::insert_or_assign(data_->metadata, std::move(metadata));
    }

    template < detail::nullptr_kind Nullptr >
    nullptr_bind<Nullptr>::operator nullptr_type() const noexcept {
        return nullptr_type{data_};
    }
}

namespace meta_hpp
{
    template < detail::number_kind Number >
    number_bind<Number>::number_bind(metadata_map metadata)
    : data_{detail::type_access(resolve_type<Number>())} {
        detail::insert_or_assign(data_->metadata, std::move(metadata));
    }

    template < detail::number_kind Number >
    number_bind<Number>::operator number_type() const noexcept {
        return number_type{data_};
    }
}

namespace meta_hpp
{
    template < detail::pointer_kind Pointer >
    pointer_bind<Pointer>::pointer_bind(metadata_map metadata)
    : data_{detail::type_access(resolve_type<Pointer>())} {
        detail::insert_or_assign(data_->metadata, std::move(metadata));
    }

    template < detail::pointer_kind Pointer >
    pointer_bind<Pointer>::operator pointer_type() const noexcept {
        return pointer_type{data_};
    }
}

namespace meta_hpp
{
    template < detail::reference_kind Reference >
    reference_bind<Reference>::reference_bind(metadata_map metadata)
    : data_{detail::type_access(resolve_type<Reference>())} {
        detail::insert_or_assign(data_->metadata, std::move(metadata));
    }

    template < detail::reference_kind Reference >
    reference_bind<Reference>::operator reference_type() const noexcept {
        return reference_type{data_};
    }
}

namespace meta_hpp
{
    inline scope_bind::scope_bind(std::string name, metadata_map metadata, local_tag)
    : state_{detail::scope_state::make(std::move(name), std::move(metadata))} {}

    inline scope_bind::scope_bind(std::string_view name, metadata_map metadata, static_tag)
    : state_{detail::state_access(resolve_scope(name))} {
        detail::insert_or_assign(state_->metadata, std::move(metadata));
    }

    inline scope_bind::operator scope() const noexcept {
        return scope{state_};
    }

    //
    // function_
    //

    template < detail::function_kind Function, function_policy_kind Policy >
    scope_bind& scope_bind::function_(
        std::string name,
        Function function,
        Policy policy)
    {
        return function_(std::move(name), std::move(function), {}, policy);
    }

    template < detail::function_kind Function, function_policy_kind Policy >
    scope_bind& scope_bind::function_(
        std::string name,
        Function function,
        function_opts opts,
        Policy)
    {
        auto state = detail::function_state::make<Policy>(
            std::move(name),
            std::move(function),
            std::move(opts.metadata));

        if ( opts.arguments.size() > state->arguments.size() ) {
            detail::throw_exception_with("provided arguments don't match function argument count");
        }

        for ( std::size_t i = 0; i < opts.arguments.size(); ++i ) {
            argument& arg = state->arguments[i];
            detail::state_access(arg)->name = std::move(opts.arguments[i].name);
            detail::state_access(arg)->metadata = std::move(opts.arguments[i].metadata);
        }

        detail::insert_or_assign(state_->functions, std::move(state));
        return *this;
    }

    template < detail::function_kind Function, function_policy_kind Policy >
    scope_bind& scope_bind::function_(
        std::string name,
        Function function,
        std::initializer_list<std::string_view> arguments,
        Policy)
    {
        auto state = detail::function_state::make<Policy>(
            std::move(name),
            std::move(function),
            {});

        if ( arguments.size() > state->arguments.size() ) {
            detail::throw_exception_with("provided argument names don't match function argument count");
        }

        for ( std::size_t i = 0; i < arguments.size(); ++i ) {
            argument& arg = state->arguments[i];
            // NOLINTNEXTLINE(*-pointer-arithmetic)
            detail::state_access(arg)->name = std::data(arguments)[i];
        }

        detail::insert_or_assign(state_->functions, std::move(state));
        return *this;
    }

    //
    // typedef_
    //

    template < typename Type >
    scope_bind& scope_bind::typedef_(std::string name) {
        state_->typedefs.insert_or_assign(std::move(name), resolve_type<Type>());
        return *this;
    }

    //
    // variable_
    //

    template < detail::pointer_kind Pointer, variable_policy_kind Policy >
    scope_bind& scope_bind::variable_(
        std::string name,
        Pointer pointer,
        Policy policy)
    {
        return variable_(std::move(name), std::move(pointer), {}, policy);
    }

    template < detail::pointer_kind Pointer, variable_policy_kind Policy >
    scope_bind& scope_bind::variable_(
        std::string name,
        Pointer pointer,
        variable_opts opts,
        Policy)
    {
        auto state = detail::variable_state::make<Policy>(
            std::move(name),
            std::move(pointer),
            std::move(opts.metadata));
        detail::insert_or_assign(state_->variables, std::move(state));
        return *this;
    }
}

namespace meta_hpp
{
    template < detail::void_kind Void >
    void_bind<Void>::void_bind(metadata_map metadata)
    : data_{detail::type_access(resolve_type<void>())} {
        detail::insert_or_assign(data_->metadata, std::move(metadata));
    }

    template < detail::void_kind Void >
    void_bind<Void>::operator void_type() const noexcept {
        return void_type{data_};
    }
}

namespace meta_hpp
{
    inline argument_index::argument_index(any_type type, std::size_t position)
    : type_{type}
    , position_{position} {}

    template < typename Argument >
    inline argument_index argument_index::make(std::size_t position) {
        return argument_index{resolve_type<Argument>(), position};
    }

    inline std::size_t argument_index::get_hash() const noexcept {
        return detail::hash_combiner{}(detail::hash_combiner{}(type_), position_);
    }

    inline const any_type& argument_index::get_type() const noexcept {
        return type_;
    }

    inline std::size_t argument_index::get_position() const noexcept {
        return position_;
    }

    inline bool operator<(const argument_index& l, const argument_index& r) noexcept {
        return l.type_ < r.type_ || (l.type_ == r.type_ && l.position_ < r.position_);
    }

    inline bool operator==(const argument_index& l, const argument_index& r) noexcept {
        return l.type_ == r.type_ && l.position_ == r.position_;
    }

    inline bool operator!=(const argument_index& l, const argument_index& r) noexcept {
        return !(l == r);
    }
}

namespace meta_hpp
{
    inline constructor_index::constructor_index(constructor_type type)
    : type_{type} {}

    template < detail::class_kind Class, typename... Args >
    constructor_index constructor_index::make() {
        return constructor_index{resolve_constructor_type<Class, Args...>()};
    }

    inline std::size_t constructor_index::get_hash() const noexcept {
        return detail::hash_combiner{}(type_);
    }

    inline const constructor_type& constructor_index::get_type() const noexcept {
        return type_;
    }

    inline bool operator<(const constructor_index& l, const constructor_index& r) noexcept {
        return l.type_ < r.type_;
    }

    inline bool operator==(const constructor_index& l, const constructor_index& r) noexcept {
        return l.type_ == r.type_;
    }

    inline bool operator!=(const constructor_index& l, const constructor_index& r) noexcept {
        return !(l == r);
    }
}

namespace meta_hpp
{
    inline destructor_index::destructor_index(destructor_type type)
    : type_{type} {}

    template < detail::class_kind Class >
    destructor_index destructor_index::make() {
        return destructor_index{resolve_destructor_type<Class>()};
    }

    inline std::size_t destructor_index::get_hash() const noexcept {
        return detail::hash_combiner{}(type_);
    }

    inline const destructor_type& destructor_index::get_type() const noexcept {
        return type_;
    }

    inline bool operator<(const destructor_index& l, const destructor_index& r) noexcept {
        return l.type_ < r.type_;
    }

    inline bool operator==(const destructor_index& l, const destructor_index& r) noexcept {
        return l.type_ == r.type_;
    }

    inline bool operator!=(const destructor_index& l, const destructor_index& r) noexcept {
        return !(l == r);
    }
}

namespace meta_hpp
{
    inline evalue_index::evalue_index(enum_type type, std::string name)
    : type_{type}
    , name_{std::move(name)} {}

    template < detail::enum_kind Enum >
    evalue_index evalue_index::make(std::string name) {
        return evalue_index{resolve_type<Enum>(), std::move(name)};
    }

    inline std::size_t evalue_index::get_hash() const noexcept {
        return detail::hash_combiner{}(detail::hash_combiner{}(type_), name_);
    }

    inline const enum_type& evalue_index::get_type() const noexcept {
        return type_;
    }

    inline const std::string& evalue_index::get_name() const noexcept {
        return name_;
    }

    inline bool operator<(const evalue_index& l, const evalue_index& r) noexcept {
        return l.type_ < r.type_ || (l.type_ == r.type_ && std::less<>{}(l.name_, r.name_));
    }

    inline bool operator==(const evalue_index& l, const evalue_index& r) noexcept {
        return l.type_ == r.type_ && std::equal_to<>{}(l.name_, r.name_);
    }

    inline bool operator!=(const evalue_index& l, const evalue_index& r) noexcept {
        return !(l == r);
    }
}

namespace meta_hpp
{
    inline function_index::function_index(function_type type, std::string name)
    : type_{type}
    , name_{std::move(name)} {}

    template < detail::function_kind Function >
    function_index function_index::make(std::string name) {
        return function_index{resolve_type<Function>(), std::move(name)};
    }

    inline std::size_t function_index::get_hash() const noexcept {
        return detail::hash_combiner{}(detail::hash_combiner{}(type_), name_);
    }

    inline const function_type& function_index::get_type() const noexcept {
        return type_;
    }

    inline const std::string& function_index::get_name() const noexcept {
        return name_;
    }

    inline bool operator<(const function_index& l, const function_index& r) noexcept {
        return l.type_ < r.type_ || (l.type_ == r.type_ && std::less<>{}(l.name_, r.name_));
    }

    inline bool operator==(const function_index& l, const function_index& r) noexcept {
        return l.type_ == r.type_ && std::equal_to<>{}(l.name_, r.name_);
    }

    inline bool operator!=(const function_index& l, const function_index& r) noexcept {
        return !(l == r);
    }
}

namespace meta_hpp
{
    inline member_index::member_index(member_type type, std::string name)
    : type_{type}
    , name_{std::move(name)} {}

    template < detail::member_kind Member >
    member_index member_index::make(std::string name) {
        return member_index{resolve_type<Member>(), std::move(name)};
    }

    inline std::size_t member_index::get_hash() const noexcept {
        return detail::hash_combiner{}(detail::hash_combiner{}(type_), name_);
    }

    inline const member_type& member_index::get_type() const noexcept {
        return type_;
    }

    inline const std::string& member_index::get_name() const noexcept {
        return name_;
    }

    inline bool operator<(const member_index& l, const member_index& r) noexcept {
        return l.type_ < r.type_ || (l.type_ == r.type_ && std::less<>{}(l.name_, r.name_));
    }

    inline bool operator==(const member_index& l, const member_index& r) noexcept {
        return l.type_ == r.type_ && std::equal_to<>{}(l.name_, r.name_);
    }

    inline bool operator!=(const member_index& l, const member_index& r) noexcept {
        return !(l == r);
    }
}

namespace meta_hpp
{
    inline method_index::method_index(method_type type, std::string name)
    : type_{type}
    , name_{std::move(name)} {}

    template < detail::method_kind Method >
    method_index method_index::make(std::string name) {
        return method_index{resolve_type<Method>(), std::move(name)};
    }

    inline std::size_t method_index::get_hash() const noexcept {
        return detail::hash_combiner{}(detail::hash_combiner{}(type_), name_);
    }

    inline const method_type& method_index::get_type() const noexcept {
        return type_;
    }

    inline const std::string& method_index::get_name() const noexcept {
        return name_;
    }

    inline bool operator<(const method_index& l, const method_index& r) noexcept {
        return l.type_ < r.type_ || (l.type_ == r.type_ && std::less<>{}(l.name_, r.name_));
    }

    inline bool operator==(const method_index& l, const method_index& r) noexcept {
        return l.type_ == r.type_ && std::equal_to<>{}(l.name_, r.name_);
    }

    inline bool operator!=(const method_index& l, const method_index& r) noexcept {
        return !(l == r);
    }
}

namespace meta_hpp
{
    inline scope_index::scope_index(std::string name)
    : name_{std::move(name)} {}

    inline scope_index scope_index::make(std::string name) {
        return scope_index{std::move(name)};
    }

    inline std::size_t scope_index::get_hash() const noexcept {
        return detail::hash_combiner{}(name_);
    }

    inline const std::string& scope_index::get_name() const noexcept {
        return name_;
    }

    inline bool operator<(const scope_index& l, const scope_index& r) noexcept {
        return std::less<>{}(l.name_, r.name_);
    }

    inline bool operator==(const scope_index& l, const scope_index& r) noexcept {
        return std::equal_to<>{}(l.name_, r.name_);
    }

    inline bool operator!=(const scope_index& l, const scope_index& r) noexcept {
        return !(l == r);
    }
}

namespace meta_hpp
{
    inline variable_index::variable_index(pointer_type type, std::string name)
    : type_{type}
    , name_{std::move(name)} {}

    template < detail::pointer_kind Pointer >
    variable_index variable_index::make(std::string name) {
        return variable_index{resolve_type<Pointer>(), std::move(name)};
    }

    inline std::size_t variable_index::get_hash() const noexcept {
        return detail::hash_combiner{}(detail::hash_combiner{}(type_), name_);
    }

    inline const pointer_type& variable_index::get_type() const noexcept {
        return type_;
    }

    inline const std::string& variable_index::get_name() const noexcept {
        return name_;
    }

    inline bool operator<(const variable_index& l, const variable_index& r) noexcept {
        return l.type_ < r.type_ || (l.type_ == r.type_ && std::less<>{}(l.name_, r.name_));
    }

    inline bool operator==(const variable_index& l, const variable_index& r) noexcept {
        return l.type_ == r.type_ && std::equal_to<>{}(l.name_, r.name_);
    }

    inline bool operator!=(const variable_index& l, const variable_index& r) noexcept {
        return !(l == r);
    }
}

namespace meta_hpp::detail
{
    template < typename Argument >
    inline argument_state_ptr argument_state::make(std::size_t position, metadata_map metadata) {
        return std::make_shared<argument_state>(argument_state{
            .index{argument_index::make<Argument>(position)},
            .metadata{std::move(metadata)},
        });
    }
}

namespace meta_hpp
{

    inline argument::argument(detail::argument_state_ptr state) noexcept
    : state_{std::move(state)} {}

    inline argument& argument::operator=(detail::argument_state_ptr state) noexcept {
        state_ = std::move(state);
        return *this;
    }

    inline bool argument::is_valid() const noexcept {
        return !!state_;
    }

    inline argument::operator bool() const noexcept {
        return is_valid();
    }

    inline const argument_index& argument::get_index() const noexcept {
        return state_->index;
    }

    inline const metadata_map& argument::get_metadata() const noexcept {
        return state_->metadata;
    }

    inline const any_type& argument::get_type() const noexcept {
        return state_->index.get_type();
    }

    inline std::size_t argument::get_position() const noexcept {
        return state_->index.get_position();
    }

    inline const std::string& argument::get_name() const noexcept {
        return state_->name;
    }
}

namespace meta_hpp::detail
{
    template < class_kind Class, typename... Args >
    struct constructor_tag {};

    template < class_kind Class, typename... Args >
    constructor_type_data::constructor_type_data(type_list<Class>, type_list<Args...>)
    : type_data_base{type_id{type_list<constructor_tag<Class, Args...>>{}}, type_kind::constructor_}
    , flags{constructor_traits<Class, Args...>::make_flags()}
    , class_type{resolve_type<typename constructor_traits<Class, Args...>::class_type>()}
    , argument_types{resolve_types(typename constructor_traits<Class, Args...>::argument_types{})} {}
}

namespace meta_hpp
{
    inline constructor_type::constructor_type(detail::constructor_type_data* data)
    : data_{data} {}

    inline bool constructor_type::is_valid() const noexcept {
        return data_ != nullptr;
    }

    inline constructor_type::operator bool() const noexcept {
        return is_valid();
    }

    inline type_id constructor_type::get_id() const noexcept {
        return data_->id;
    }

    inline constructor_bitflags constructor_type::get_flags() const noexcept {
        return data_->flags;
    }

    inline const metadata_map& constructor_type::get_metadata() const noexcept {
        return data_->metadata;
    }

    inline std::size_t constructor_type::get_arity() const noexcept {
        return data_->argument_types.size();
    }

    inline any_type constructor_type::get_class_type() const noexcept {
        return data_->class_type;
    }

    inline any_type constructor_type::get_argument_type(std::size_t position) const noexcept {
        return position < data_->argument_types.size() ? data_->argument_types[position] : any_type{};
    }

    inline const any_type_list& constructor_type::get_argument_types() const noexcept {
        return data_->argument_types;
    }
}

namespace meta_hpp::detail
{
    template < typename T, typename Tp = std::decay_t<T> >
    concept arg_lvalue_ref_kind =
        (!any_uvalue_kind<Tp>) &&
        (std::is_lvalue_reference_v<T>);

    template < typename T, typename Tp = std::decay_t<T> >
    concept arg_rvalue_ref_kind =
        (!any_uvalue_kind<Tp>) &&
        (!std::is_reference_v<T> || std::is_rvalue_reference_v<T>);
}

namespace meta_hpp::detail
{
    template < typename T >
    concept inst_class_ref_kind =
        (std::is_class_v<T>) ||
        (std::is_reference_v<T> && std::is_class_v<std::remove_reference_t<T>>);

    template < typename T, typename Tp = std::decay_t<T> >
    concept inst_class_lvalue_ref_kind =
        (!any_uvalue_kind<Tp>) &&
        (std::is_lvalue_reference_v<T>) &&
        (std::is_class_v<std::remove_pointer_t<std::remove_reference_t<T>>>);

    template < typename T, typename Tp = std::decay_t<T> >
    concept inst_class_rvalue_ref_kind =
        (!any_uvalue_kind<Tp>) &&
        (!std::is_reference_v<T> || std::is_rvalue_reference_v<T>) &&
        (std::is_class_v<std::remove_pointer_t<std::remove_reference_t<T>>>);
}

namespace meta_hpp::detail
{
    namespace impl
    {
        template < inst_class_ref_kind Q, bool is_const, bool is_lvalue, bool is_rvalue >
        struct inst_traits_impl;

        template < inst_class_ref_kind Q >
        struct inst_traits_impl<Q, false, false, false> {
            using class_type = std::remove_cvref_t<Q>;
            using method_type = void(class_type::*)();
        };

        template < inst_class_ref_kind Q >
        struct inst_traits_impl<Q, false, true, false> {
            using class_type = std::remove_cvref_t<Q>;
            using method_type = void(class_type::*)() &;
        };

        template < inst_class_ref_kind Q >
        struct inst_traits_impl<Q, false, false, true> {
            using class_type = std::remove_cvref_t<Q>;
            using method_type = void(class_type::*)() &&;
        };

        template < inst_class_ref_kind Q >
        struct inst_traits_impl<Q, true, false, false> {
            using class_type = std::remove_cvref_t<Q>;
            using method_type = void(class_type::*)() const;
        };

        template < inst_class_ref_kind Q >
        struct inst_traits_impl<Q, true, true, false> {
            using class_type = std::remove_cvref_t<Q>;
            using method_type = void(class_type::*)() const &;
        };

        template < inst_class_ref_kind Q >
        struct inst_traits_impl<Q, true, false, true> {
            using class_type = std::remove_cvref_t<Q>;
            using method_type = void(class_type::*)() const &&;
        };
    }

    template < inst_class_ref_kind Q >
    struct inst_traits final : impl::inst_traits_impl<Q,
        cvref_traits<Q>::is_const,
        cvref_traits<Q>::is_lvalue,
        cvref_traits<Q>::is_rvalue> {};
}

namespace meta_hpp::detail
{
    [[nodiscard]] inline void* pointer_upcast(void* ptr, const class_type& from, const class_type& to) {
        if ( nullptr == ptr || !from || !to ) {
            return nullptr;
        }

        if ( from == to ) {
            return ptr;
        }

        for ( auto&& [base_type, base_info] : type_access(from)->bases_info ) {
            if ( base_type == to ) {
                return base_info.upcast(ptr);
            }

            if ( base_type.is_derived_from(to) ) {
                return pointer_upcast(base_info.upcast(ptr), base_type, to);
            }
        }

        return nullptr;
    }

    [[nodiscard]] inline const void* pointer_upcast(const void* ptr, const class_type& from, const class_type& to) {
        // NOLINTNEXTLINE(*-const-cast)
        return pointer_upcast(const_cast<void*>(ptr), from, to);
    }

    template < class_kind To, class_kind From >
    [[nodiscard]] To* pointer_upcast(From* ptr) {
        return static_cast<To*>(pointer_upcast(ptr, resolve_type<From>(), resolve_type<To>()));
    }

    template < class_kind To, class_kind From >
    [[nodiscard]] const To* pointer_upcast(const From* ptr) {
        return static_cast<const To*>(pointer_upcast(ptr, resolve_type<From>(), resolve_type<To>()));
    }
}

namespace meta_hpp::detail
{
    class uarg_base {
    public:
        enum class ref_types {
            lvalue,
            const_lvalue,
            rvalue,
            const_rvalue,
        };
    public:
        uarg_base() = delete;
        ~uarg_base() = default;

        uarg_base(uarg_base&&) = default;
        uarg_base(const uarg_base&) = default;

        uarg_base& operator=(uarg_base&&) = delete;
        uarg_base& operator=(const uarg_base&) = delete;

        template < typename T >
        uarg_base(type_list<T>) = delete;

        template < typename T, typename Tp = std::decay_t<T> >
            requires (!any_uvalue_kind<Tp>)
        // NOLINTNEXTLINE(*-forwarding-reference-overload)
        explicit uarg_base(T&&)
        : uarg_base{type_list<T&&>{}} {}

        template < arg_lvalue_ref_kind T >
        explicit uarg_base(type_list<T>)
        : ref_type_{std::is_const_v<std::remove_reference_t<T>>
            ? ref_types::const_lvalue
            : ref_types::lvalue}
        , raw_type_{resolve_type<std::remove_cvref_t<T>>()} {}

        template < arg_rvalue_ref_kind T >
        explicit uarg_base(type_list<T>)
        : ref_type_{std::is_const_v<std::remove_reference_t<T>>
            ? ref_types::const_rvalue
            : ref_types::rvalue}
        , raw_type_{resolve_type<std::remove_cvref_t<T>>()} {}

        explicit uarg_base(uvalue& v)
        : ref_type_{ref_types::lvalue}
        , raw_type_{v.get_type()} {}

        explicit uarg_base(const uvalue& v)
        : ref_type_{ref_types::const_lvalue}
        , raw_type_{v.get_type()} {}

        explicit uarg_base(uvalue&& v)
        : ref_type_{ref_types::rvalue}
        , raw_type_{v.get_type()} {}

        explicit uarg_base(const uvalue&& v)
        : ref_type_{ref_types::const_rvalue}
        , raw_type_{v.get_type()} {}

        [[nodiscard]] bool is_ref_const() const noexcept {
            return ref_type_ == ref_types::const_lvalue
                || ref_type_ == ref_types::const_rvalue;
        }

        [[nodiscard]] ref_types get_ref_type() const noexcept {
            return ref_type_;
        }

        [[nodiscard]] const any_type& get_raw_type() const noexcept {
            return raw_type_;
        }

        template < typename To >
        [[nodiscard]] bool can_cast_to() const noexcept;
    private:
        ref_types ref_type_{};
        any_type raw_type_{};
    };
}

namespace meta_hpp::detail
{
    class uarg final : public uarg_base {
    public:
        uarg() = delete;
        ~uarg() = default;

        uarg(uarg&&) = default;
        uarg(const uarg&) = default;

        uarg& operator=(uarg&&) = delete;
        uarg& operator=(const uarg&) = delete;

        template < typename T, uvalue_kind Tp = std::decay_t<T> >
        // NOLINTNEXTLINE(*-forwarding-reference-overload)
        explicit uarg(T&& v)
        : uarg_base{std::forward<T>(v)}
        // NOLINTNEXTLINE(*-const-cast)
        , data_{const_cast<void*>(v.data())} {}

        template < typename T, typename Tp = std::decay_t<T> >
            requires (!any_uvalue_kind<Tp>)
        // NOLINTNEXTLINE(*-forwarding-reference-overload)
        explicit uarg(T&& v)
        : uarg_base{std::forward<T>(v)}
        // NOLINTNEXTLINE(*-const-cast)
        , data_{const_cast<std::remove_cvref_t<T>*>(std::addressof(v))} {}

        template < typename To >
        [[nodiscard]] To cast() const;
    private:
        void* data_{};
    };
}

namespace meta_hpp::detail
{
    template < typename To >
    // NOLINTNEXTLINE(*-cognitive-complexity)
    bool uarg_base::can_cast_to() const noexcept {
        using to_raw_type_cv = std::remove_reference_t<To>;
        using to_raw_type = std::remove_cv_t<to_raw_type_cv>;

        static_assert(
            !(std::is_reference_v<To> && std::is_pointer_v<to_raw_type>),
            "references to pointers are not supported yet");

        const any_type& from_type = get_raw_type();
        const any_type& to_type = resolve_type<to_raw_type>();

        const auto is_a = [](const any_type& base, const any_type& derived){
            return (base == derived)
                || (base.is_class() && derived.is_class() && base.as_class().is_base_of(derived.as_class()));
        };

        if constexpr ( std::is_pointer_v<To> ) {
            if ( to_type.is_pointer() && from_type.is_nullptr() ) {
                return true;
            }

            if ( to_type.is_pointer() && from_type.is_array() ) {
                const pointer_type& to_type_ptr = to_type.as_pointer();
                const bool to_type_ptr_readonly = to_type_ptr.get_flags().has(pointer_flags::is_readonly);

                const array_type& from_type_array = from_type.as_array();
                const bool from_type_array_readonly = is_ref_const();

                const any_type& to_data_type = to_type_ptr.get_data_type();
                const any_type& from_data_type = from_type_array.get_data_type();

                if ( to_type_ptr_readonly >= from_type_array_readonly ) {
                    if ( to_data_type.is_void() || is_a(to_data_type, from_data_type) ) {
                        return true;
                    }
                }
            }

            if ( to_type.is_pointer() && from_type.is_pointer() ) {
                const pointer_type& to_type_ptr = to_type.as_pointer();
                const bool to_type_ptr_readonly = to_type_ptr.get_flags().has(pointer_flags::is_readonly);

                const pointer_type& from_type_ptr = from_type.as_pointer();
                const bool from_type_ptr_readonly = from_type_ptr.get_flags().has(pointer_flags::is_readonly);

                const any_type& to_data_type = to_type_ptr.get_data_type();
                const any_type& from_data_type = from_type_ptr.get_data_type();

                if ( to_type_ptr_readonly >= from_type_ptr_readonly ) {
                    if ( to_data_type.is_void() || is_a(to_data_type, from_data_type) ) {
                        return true;
                    }
                }
            }
        }

        if constexpr ( std::is_reference_v<To> ) {
            const auto is_convertible = [this](){
                switch ( get_ref_type() ) {
                case ref_types::lvalue:
                    return std::is_convertible_v<noncopyable&, copy_cvref_t<To, noncopyable>>;
                case ref_types::const_lvalue:
                    return std::is_convertible_v<const noncopyable&, copy_cvref_t<To, noncopyable>>;
                case ref_types::rvalue:
                    return std::is_convertible_v<noncopyable&&, copy_cvref_t<To, noncopyable>>;
                case ref_types::const_rvalue:
                    return std::is_convertible_v<const noncopyable&&, copy_cvref_t<To, noncopyable>>;
                }
                return false;
            };

            if ( is_a(to_type, from_type) && is_convertible() ) {
                return true;
            }
        }

        if constexpr ( !std::is_pointer_v<To> && !std::is_reference_v<To> ) {
            const auto is_constructible = [this](){
                switch ( get_ref_type() ) {
                case ref_types::lvalue:
                    return std::is_constructible_v<To, to_raw_type&> && can_cast_to<to_raw_type&>();
                case ref_types::const_lvalue:
                    return std::is_constructible_v<To, const to_raw_type&> && can_cast_to<const to_raw_type&>();
                case ref_types::rvalue:
                    return std::is_constructible_v<To, to_raw_type&&> && can_cast_to<to_raw_type&&>();
                case ref_types::const_rvalue:
                    return std::is_constructible_v<To, const to_raw_type&&> && can_cast_to<const to_raw_type&&>();
                }
                return false;
            };

            if ( is_a(to_type, from_type) && is_constructible() ) {
                return true;
            }
        }

        return false;
    }
}

namespace meta_hpp::detail
{
    template < typename To >
    // NOLINTNEXTLINE(*-cognitive-complexity)
    To uarg::cast() const {
        if ( !can_cast_to<To>() ) {
            throw_exception_with("bad argument cast");
        }

        using to_raw_type_cv = std::remove_reference_t<To>;
        using to_raw_type = std::remove_cv_t<to_raw_type_cv>;

        const any_type& from_type = get_raw_type();
        const any_type& to_type = resolve_type<to_raw_type>();

        if constexpr ( std::is_pointer_v<To> ) {
            if ( to_type.is_pointer() && from_type.is_nullptr() ) {
                return static_cast<to_raw_type_cv>(nullptr);
            }

            if ( to_type.is_pointer() && from_type.is_array() ) {
                const pointer_type& to_type_ptr = to_type.as_pointer();
                const array_type& from_type_array = from_type.as_array();

                const any_type& to_data_type = to_type_ptr.get_data_type();
                const any_type& from_data_type = from_type_array.get_data_type();

                if ( to_data_type.is_void() || to_data_type == from_data_type ) {
                    return static_cast<to_raw_type_cv>(data_);
                }

                if ( to_data_type.is_class() && from_data_type.is_class() ) {
                    const class_type& to_data_class = to_data_type.as_class();
                    const class_type& from_data_class = from_data_type.as_class();

                    void* to_ptr = pointer_upcast(data_, from_data_class, to_data_class);
                    return static_cast<to_raw_type_cv>(to_ptr);
                }
            }

            if ( to_type.is_pointer() && from_type.is_pointer() ) {
                const pointer_type& to_type_ptr = to_type.as_pointer();
                const pointer_type& from_type_ptr = from_type.as_pointer();

                const any_type& to_data_type = to_type_ptr.get_data_type();
                const any_type& from_data_type = from_type_ptr.get_data_type();

                void** from_data_ptr = static_cast<void**>(data_);

                if ( to_data_type.is_void() || to_data_type == from_data_type ) {
                    return static_cast<to_raw_type_cv>(*from_data_ptr);
                }

                if ( to_data_type.is_class() && from_data_type.is_class() ) {
                    const class_type& to_data_class = to_data_type.as_class();
                    const class_type& from_data_class = from_data_type.as_class();

                    void* to_ptr = pointer_upcast(*from_data_ptr, from_data_class, to_data_class);
                    return static_cast<to_raw_type_cv>(to_ptr);
                }
            }
        }

        if constexpr ( std::is_reference_v<To> ) {
            if ( to_type == from_type ) {
                if constexpr ( std::is_lvalue_reference_v<To> ) {
                    return *static_cast<to_raw_type_cv*>(data_);
                }

                if constexpr ( std::is_rvalue_reference_v<To> ) {
                    return std::move(*static_cast<to_raw_type_cv*>(data_));
                }
            }

            if ( to_type.is_class() && from_type.is_class() ) {
                const class_type& to_class = to_type.as_class();
                const class_type& from_class = from_type.as_class();

                void* to_ptr = pointer_upcast(data_, from_class, to_class);

                if constexpr ( std::is_lvalue_reference_v<To> ) {
                    return *static_cast<to_raw_type_cv*>(to_ptr);
                }

                if constexpr ( std::is_rvalue_reference_v<To> ) {
                    return std::move(*static_cast<to_raw_type_cv*>(to_ptr));
                }
            }
        }

        if constexpr ( !std::is_pointer_v<To> && !std::is_reference_v<To> ) {
            if constexpr ( std::is_constructible_v<To, to_raw_type&> ) {
                if ( get_ref_type() == ref_types::lvalue ) {
                    return To{cast<to_raw_type&>()};
                }
            }

            if constexpr ( std::is_constructible_v<To, const to_raw_type&> ) {
                if ( get_ref_type() == ref_types::const_lvalue ) {
                    return To{cast<const to_raw_type&>()};
                }
            }

            if constexpr ( std::is_constructible_v<To, to_raw_type&&> ) {
                if ( get_ref_type() == ref_types::rvalue ) {
                    return To{cast<to_raw_type&&>()};
                }
            }

            if constexpr ( std::is_constructible_v<To, const to_raw_type&&> ) {
                if ( get_ref_type() == ref_types::const_rvalue ) {
                    return To{cast<const to_raw_type&&>()};
                }
            }
        }

        throw_exception_with("bad argument cast");
    }
}

namespace meta_hpp::detail
{
    template < constructor_policy_kind Policy, class_kind Class, typename... Args >
    uvalue raw_constructor_create(std::span<const uarg> args) {
        using ct = constructor_traits<Class, Args...>;
        using class_type = typename ct::class_type;
        using argument_types = typename ct::argument_types;

        constexpr bool as_object =
            std::is_copy_constructible_v<class_type> &&
            std::is_same_v<Policy, constructor_policy::as_object_t>;

        constexpr bool as_raw_ptr =
            std::is_same_v<Policy, constructor_policy::as_raw_pointer_t>;

        constexpr bool as_shared_ptr =
            std::is_same_v<Policy, constructor_policy::as_shared_pointer_t>;

        static_assert(as_object || as_raw_ptr || as_shared_ptr);

        if ( args.size() != ct::arity ) {
            throw_exception_with("an attempt to call a constructor with an incorrect arity");
        }

        return [args]<std::size_t... Is>(std::index_sequence<Is...>) -> uvalue {
            if ( !(... && args[Is].can_cast_to<type_list_at_t<Is, argument_types>>()) ) {
                throw_exception_with("an attempt to call a constructor with incorrect argument types");
            }

            if constexpr ( as_object ) {
                return make_uvalue<class_type>(
                    args[Is].cast<type_list_at_t<Is, argument_types>>()...);
            }

            if constexpr ( as_raw_ptr ) {
                return uvalue{std::make_unique<class_type>(
                    args[Is].cast<type_list_at_t<Is, argument_types>>()...).release()};
            }

            if constexpr ( as_shared_ptr ) {
                return uvalue{std::make_shared<class_type>(
                    args[Is].cast<type_list_at_t<Is, argument_types>>()...)};
            }
        }(std::make_index_sequence<ct::arity>());
    }

    template < class_kind Class, typename... Args >
    uvalue raw_constructor_create_at(void* mem, std::span<const uarg> args) {
        using ct = constructor_traits<Class, Args...>;
        using class_type = typename ct::class_type;
        using argument_types = typename ct::argument_types;

        if ( args.size() != ct::arity ) {
            throw_exception_with("an attempt to call a constructor with an incorrect arity");
        }

        return [mem, args]<std::size_t... Is>(std::index_sequence<Is...>) -> uvalue {
            if ( !(... && args[Is].can_cast_to<type_list_at_t<Is, argument_types>>()) ) {
                throw_exception_with("an attempt to call a constructor with incorrect argument types");
            }
            return uvalue{std::construct_at(
                static_cast<class_type*>(mem),
                args[Is].cast<type_list_at_t<Is, argument_types>>()...)};
        }(std::make_index_sequence<ct::arity>());
    }

    template < class_kind Class, typename... Args >
    bool raw_constructor_is_invocable_with(std::span<const uarg_base> args) {
        using ct = constructor_traits<Class, Args...>;
        using argument_types = typename ct::argument_types;

        if ( args.size() != ct::arity ) {
            return false;
        }

        return [args]<std::size_t... Is>(std::index_sequence<Is...>){
            return (... && args[Is].can_cast_to<type_list_at_t<Is, argument_types>>());
        }(std::make_index_sequence<ct::arity>());
    }
}

namespace meta_hpp::detail
{
    template < constructor_policy_kind Policy, class_kind Class, typename... Args >
    constructor_state::create_impl make_constructor_create() {
        return &raw_constructor_create<Policy, Class, Args...>;
    }

    template < class_kind Class, typename... Args >
    constructor_state::create_at_impl make_constructor_create_at() {
        return &raw_constructor_create_at<Class, Args...>;
    }

    template < class_kind Class, typename... Args >
    constructor_state::is_invocable_with_impl make_constructor_is_invocable_with() {
        return &raw_constructor_is_invocable_with<Class, Args...>;
    }

    template < class_kind Class, typename... Args >
    argument_list make_constructor_arguments() {
        using ct = constructor_traits<Class, Args...>;
        using ct_argument_types = typename ct::argument_types;

        return []<std::size_t... Is>(std::index_sequence<Is...>){
            [[maybe_unused]] const auto make_argument = []<std::size_t I>(std::index_sequence<I>){
                using P = type_list_at_t<I, ct_argument_types>;
                return argument{argument_state::make<P>(I, metadata_map{})};
            };
            return argument_list{make_argument(std::index_sequence<Is>{})...};
        }(std::make_index_sequence<ct::arity>());
    }
}

namespace meta_hpp::detail
{
    template < constructor_policy_kind Policy, class_kind Class, typename... Args >
    constructor_state_ptr constructor_state::make(metadata_map metadata) {
        return std::make_shared<constructor_state>(constructor_state{
            .index{constructor_index::make<Class, Args...>()},
            .metadata{std::move(metadata)},
            .create{make_constructor_create<Policy, Class, Args...>()},
            .create_at{make_constructor_create_at<Class, Args...>()},
            .is_invocable_with{make_constructor_is_invocable_with<Class, Args...>()},
            .arguments{make_constructor_arguments<Class, Args...>()},
        });
    }
}

namespace meta_hpp
{
    inline constructor::constructor(detail::constructor_state_ptr state) noexcept
    : state_{std::move(state)} {}

    inline constructor& constructor::operator=(detail::constructor_state_ptr state) noexcept {
        state_ = std::move(state);
        return *this;
    }

    inline bool constructor::is_valid() const noexcept {
        return !!state_;
    }

    inline constructor::operator bool() const noexcept {
        return is_valid();
    }

    inline const constructor_index& constructor::get_index() const noexcept {
        return state_->index;
    }

    inline const metadata_map& constructor::get_metadata() const noexcept {
        return state_->metadata;
    }

    inline const constructor_type& constructor::get_type() const noexcept {
        return state_->index.get_type();
    }

    template < typename... Args >
    uvalue constructor::create(Args&&... args) const {
        if constexpr ( sizeof...(Args) > 0 ) {
            using namespace detail;
            const std::array<uarg, sizeof...(Args)> vargs{uarg{std::forward<Args>(args)}...};
            return state_->create(vargs);
        } else {
            return state_->create({});
        }
    }

    template < typename... Args >
    uvalue constructor::create_at(void* mem, Args&&... args) const {
        if constexpr ( sizeof...(Args) > 0 ) {
            using namespace detail;
            const std::array<uarg, sizeof...(Args)> vargs{uarg{std::forward<Args>(args)}...};
            return state_->create_at(mem, vargs);
        } else {
            return state_->create_at(mem, {});
        }
    }

    template < typename... Args >
    bool constructor::is_invocable_with() const noexcept {
        if constexpr ( sizeof...(Args) > 0 ) {
            using namespace detail;
            const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{type_list<Args>{}}...};
            return state_->is_invocable_with(vargs);
        } else {
            return state_->is_invocable_with({});
        }
    }

    template < typename... Args >
    bool constructor::is_invocable_with(Args&&... args) const noexcept {
        if constexpr ( sizeof...(Args) > 0 ) {
            using namespace detail;
            const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{std::forward<Args>(args)}...};
            return state_->is_invocable_with(vargs);
        } else {
            return state_->is_invocable_with({});
        }
    }

    inline argument constructor::get_argument(std::size_t position) const noexcept {
        return position < state_->arguments.size() ? state_->arguments[position] : argument{};
    }

    inline const argument_list& constructor::get_arguments() const noexcept {
        return state_->arguments;
    }
}

namespace meta_hpp::detail
{
    template < class_kind Class >
    struct destructor_tag {};

    template < class_kind Class >
    destructor_type_data::destructor_type_data(type_list<Class>)
    : type_data_base{type_id{type_list<destructor_tag<Class>>{}}, type_kind::destructor_}
    , flags{destructor_traits<Class>::make_flags()}
    , class_type{resolve_type<typename destructor_traits<Class>::class_type>()} {}
}

namespace meta_hpp
{
    inline destructor_type::destructor_type(detail::destructor_type_data* data)
    : data_{data} {}

    inline bool destructor_type::is_valid() const noexcept {
        return data_ != nullptr;
    }

    inline destructor_type::operator bool() const noexcept {
        return is_valid();
    }

    inline type_id destructor_type::get_id() const noexcept {
        return data_->id;
    }

    inline destructor_bitflags destructor_type::get_flags() const noexcept {
        return data_->flags;
    }

    inline const metadata_map& destructor_type::get_metadata() const noexcept {
        return data_->metadata;
    }

    inline any_type destructor_type::get_class_type() const noexcept {
        return data_->class_type;
    }
}

namespace meta_hpp::detail
{
    template < class_kind Class >
    bool raw_destructor_destroy(const uarg& arg) {
        using dt = destructor_traits<Class>;
        using class_type = typename dt::class_type;

        if ( !arg.can_cast_to<class_type*>() ) {
            return false;
        }

        std::unique_ptr<class_type>{arg.cast<class_type*>()}.reset();
        return true;
    }

    template < class_kind Class >
    void raw_destructor_destroy_at(void* mem) {
        using dt = destructor_traits<Class>;
        using class_type = typename dt::class_type;

        std::destroy_at(static_cast<class_type*>(mem));
    }
}

namespace meta_hpp::detail
{
    template < class_kind Class >
    destructor_state::destroy_impl make_destructor_destroy() {
        return &raw_destructor_destroy<Class>;
    }

    template < class_kind Class >
    destructor_state::destroy_at_impl make_destructor_destroy_at() {
        return &raw_destructor_destroy_at<Class>;
    }
}

namespace meta_hpp::detail
{
    template < class_kind Class >
    destructor_state_ptr destructor_state::make(metadata_map metadata) {
        return std::make_shared<destructor_state>(destructor_state{
            .index{destructor_index::make<Class>()},
            .metadata{std::move(metadata)},
            .destroy{make_destructor_destroy<Class>()},
            .destroy_at{make_destructor_destroy_at<Class>()},
        });
    }
}

namespace meta_hpp
{
    inline destructor::destructor(detail::destructor_state_ptr state) noexcept
    : state_{std::move(state)} {}

    inline destructor& destructor::operator=(detail::destructor_state_ptr state) noexcept {
        state_ = std::move(state);
        return *this;
    }

    inline bool destructor::is_valid() const noexcept {
        return !!state_;
    }

    inline destructor::operator bool() const noexcept {
        return is_valid();
    }

    inline const destructor_index& destructor::get_index() const noexcept {
        return state_->index;
    }

    inline const metadata_map& destructor::get_metadata() const noexcept {
        return state_->metadata;
    }

    inline const destructor_type& destructor::get_type() const noexcept {
        return state_->index.get_type();
    }

    template < typename Arg >
    bool destructor::destroy(Arg&& arg) const {
        using namespace detail;
        const uarg varg{std::forward<Arg>(arg)};
        return state_->destroy(varg);
    }

    inline void destructor::destroy_at(void* mem) const {
        state_->destroy_at(mem);
    }
}

namespace meta_hpp::detail
{
    template < enum_kind Enum >
    struct enum_tag {};

    template < enum_kind Enum >
    enum_type_data::enum_type_data(type_list<Enum>)
    : type_data_base{type_id{type_list<enum_tag<Enum>>{}}, type_kind::enum_}
    , flags{enum_traits<Enum>::make_flags()}
    , underlying_type{resolve_type<typename enum_traits<Enum>::underlying_type>()} {}
}

namespace meta_hpp
{
    inline enum_type::enum_type(detail::enum_type_data* data)
    : data_{data} {}

    inline bool enum_type::is_valid() const noexcept {
        return data_ != nullptr;
    }

    inline enum_type::operator bool() const noexcept {
        return is_valid();
    }

    inline type_id enum_type::get_id() const noexcept {
        return data_->id;
    }

    inline enum_bitflags enum_type::get_flags() const noexcept {
        return data_->flags;
    }

    inline const metadata_map& enum_type::get_metadata() const noexcept {
        return data_->metadata;
    }

    inline number_type enum_type::get_underlying_type() const noexcept {
        return data_->underlying_type;
    }

    inline const evalue_set& enum_type::get_evalues() const noexcept {
        return data_->evalues;
    }

    inline evalue enum_type::get_evalue(std::string_view name) const noexcept {
        for ( const evalue& evalue : data_->evalues ) {
            if ( evalue.get_name() == name ) {
                return evalue;
            }
        }
        return evalue{};
    }

    template < detail::enum_kind Enum >
    std::string_view enum_type::value_to_name(Enum value) const noexcept {
        if ( resolve_type<Enum>() != *this ) {
            return std::string_view{};
        }

        for ( const evalue& evalue : data_->evalues ) {
            if ( evalue.get_value_as<Enum>() == value ) {
                return evalue.get_name();
            }
        }

        return std::string_view{};
    }

    inline uvalue enum_type::name_to_value(std::string_view name) const noexcept {
        if ( const evalue& value = get_evalue(name); value ) {
            return value.get_value();
        }

        return uvalue{};
    }

    template < typename T >
    T enum_type::name_to_value_as(std::string_view name) const {
        return name_to_value(name).get_as<T>();
    }
}

namespace meta_hpp::detail
{
    template < enum_kind Enum >
    evalue_state_ptr evalue_state::make(std::string name, Enum evalue, metadata_map metadata) {
        return std::make_shared<evalue_state>(evalue_state{
            .index{evalue_index::make<Enum>(std::move(name))},
            .metadata{std::move(metadata)},
            .enum_value{uvalue{evalue}},
            .underlying_value{uvalue{to_underlying(evalue)}},
        });
    }
}

namespace meta_hpp
{
    inline evalue::evalue(detail::evalue_state_ptr state) noexcept
    : state_{std::move(state)} {}

    inline evalue& evalue::operator=(detail::evalue_state_ptr state) noexcept {
        state_ = std::move(state);
        return *this;
    }

    inline bool evalue::is_valid() const noexcept {
        return !!state_;
    }

    inline evalue::operator bool() const noexcept {
        return is_valid();
    }

    inline const evalue_index& evalue::get_index() const noexcept {
        return state_->index;
    }

    inline const metadata_map& evalue::get_metadata() const noexcept {
        return state_->metadata;
    }

    inline const enum_type& evalue::get_type() const noexcept {
        return state_->index.get_type();
    }

    inline const std::string& evalue::get_name() const noexcept {
        return state_->index.get_name();
    }

    inline const uvalue& evalue::get_value() const noexcept {
        return state_->enum_value;
    }

    inline const uvalue& evalue::get_underlying_value() const noexcept {
        return state_->underlying_value;
    }

    template < typename T >
    T evalue::get_value_as() const {
        return get_value().get_as<T>();
    }

    template < typename T >
    T evalue::get_underlying_value_as() const {
        return get_underlying_value().get_as<T>();
    }
}

namespace meta_hpp::detail
{
    template < function_kind Function >
    struct function_tag {};

    template < function_kind Function >
    function_type_data::function_type_data(type_list<Function>)
    : type_data_base{type_id{type_list<function_tag<Function>>{}}, type_kind::function_}
    , flags{function_traits<Function>::make_flags()}
    , return_type{resolve_type<typename function_traits<Function>::return_type>()}
    , argument_types{resolve_types(typename function_traits<Function>::argument_types{})} {}
}

namespace meta_hpp
{
    inline function_type::function_type(detail::function_type_data* data)
    : data_{data} {}

    inline bool function_type::is_valid() const noexcept {
        return data_ != nullptr;
    }

    inline function_type::operator bool() const noexcept {
        return is_valid();
    }

    inline type_id function_type::get_id() const noexcept {
        return data_->id;
    }

    inline function_bitflags function_type::get_flags() const noexcept {
        return data_->flags;
    }

    inline const metadata_map& function_type::get_metadata() const noexcept {
        return data_->metadata;
    }

    inline std::size_t function_type::get_arity() const noexcept {
        return data_->argument_types.size();
    }

    inline any_type function_type::get_return_type() const noexcept {
        return data_->return_type;
    }

    inline any_type function_type::get_argument_type(std::size_t position) const noexcept {
        return position < data_->argument_types.size() ? data_->argument_types[position] : any_type{};
    }

    inline const any_type_list& function_type::get_argument_types() const noexcept {
        return data_->argument_types;
    }
}

namespace meta_hpp::detail
{
    template < function_policy_kind Policy, function_kind Function >
    uvalue raw_function_invoke(const Function& function, std::span<const uarg> args) {
        using ft = function_traits<Function>;
        using return_type = typename ft::return_type;
        using argument_types = typename ft::argument_types;

        constexpr bool as_copy =
            std::is_copy_constructible_v<return_type> &&
            std::is_same_v<Policy, function_policy::as_copy_t>;

        constexpr bool as_void =
            std::is_void_v<return_type> ||
            std::is_same_v<Policy, function_policy::discard_return_t>;

        constexpr bool ref_as_ptr =
            std::is_reference_v<return_type> &&
            std::is_same_v<Policy, function_policy::return_reference_as_pointer_t>;

        static_assert(as_copy || as_void || ref_as_ptr);

        if ( args.size() != ft::arity ) {
            throw_exception_with("an attempt to call a function with an incorrect arity");
        }

        return [&function, args]<std::size_t... Is>(std::index_sequence<Is...>) -> uvalue {
            if ( !(... && args[Is].can_cast_to<type_list_at_t<Is, argument_types>>()) ) {
                throw_exception_with("an attempt to call a function with incorrect argument types");
            }

            if constexpr ( std::is_void_v<return_type> ) {
                function(
                    args[Is].cast<type_list_at_t<Is, argument_types>>()...);
                return uvalue{};
            } else if constexpr ( std::is_same_v<Policy, function_policy::discard_return_t> ) {
                std::ignore = function(
                    args[Is].cast<type_list_at_t<Is, argument_types>>()...);
                return uvalue{};
            } else {
                return_type&& return_value = function(
                    args[Is].cast<type_list_at_t<Is, argument_types>>()...);

                if constexpr ( ref_as_ptr ) {
                    return uvalue{std::addressof(return_value)};
                } else {
                    return uvalue{std::forward<decltype(return_value)>(return_value)};
                }
            }
        }(std::make_index_sequence<ft::arity>());
    }

    template < function_kind Function >
    bool raw_function_is_invocable_with(std::span<const uarg_base> args) {
        using ft = function_traits<Function>;
        using argument_types = typename ft::argument_types;

        if ( args.size() != ft::arity ) {
            return false;
        }

        return [args]<std::size_t... Is>(std::index_sequence<Is...>){
            return (... && args[Is].can_cast_to<type_list_at_t<Is, argument_types>>());
        }(std::make_index_sequence<ft::arity>());
    }
}

namespace meta_hpp::detail
{
    template < function_policy_kind Policy, function_kind Function >
    function_state::invoke_impl make_function_invoke(Function function) {
        return [function = std::move(function)](std::span<const uarg> args){
            return raw_function_invoke<Policy>(function, args);
        };
    }

    template < function_kind Function >
    function_state::is_invocable_with_impl make_function_is_invocable_with() {
        return &raw_function_is_invocable_with<Function>;
    }

    template < function_kind Function >
    argument_list make_function_arguments() {
        using ft = function_traits<Function>;
        using ft_argument_types = typename ft::argument_types;

        return []<std::size_t... Is>(std::index_sequence<Is...>){
            [[maybe_unused]] const auto make_argument = []<std::size_t I>(std::index_sequence<I>){
                using P = type_list_at_t<I, ft_argument_types>;
                return argument{argument_state::make<P>(I, metadata_map{})};
            };
            return argument_list{make_argument(std::index_sequence<Is>{})...};
        }(std::make_index_sequence<ft::arity>());
    }
}

namespace meta_hpp::detail
{
    template < function_policy_kind Policy, function_kind Function >
    function_state_ptr function_state::make(std::string name, Function function, metadata_map metadata) {
        return std::make_shared<function_state>(function_state{
            .index{function_index::make<Function>(std::move(name))},
            .metadata{std::move(metadata)},
            .invoke{make_function_invoke<Policy>(std::move(function))},
            .is_invocable_with{make_function_is_invocable_with<Function>()},
            .arguments{make_function_arguments<Function>()},
        });
    }
}

namespace meta_hpp
{
    inline function::function(detail::function_state_ptr state) noexcept
    : state_{std::move(state)} {}

    inline function& function::operator=(detail::function_state_ptr state) noexcept {
        state_ = std::move(state);
        return *this;
    }

    inline bool function::is_valid() const noexcept {
        return !!state_;
    }

    inline function::operator bool() const noexcept {
        return is_valid();
    }

    inline const function_index& function::get_index() const noexcept {
        return state_->index;
    }

    inline const metadata_map& function::get_metadata() const noexcept {
        return state_->metadata;
    }

    inline const function_type& function::get_type() const noexcept {
        return state_->index.get_type();
    }

    inline const std::string& function::get_name() const noexcept {
        return state_->index.get_name();
    }

    template < typename... Args >
    uvalue function::invoke(Args&&... args) const {
        if constexpr ( sizeof...(Args) > 0 ) {
            using namespace detail;
            const std::array<uarg, sizeof...(Args)> vargs{uarg{std::forward<Args>(args)}...};
            return state_->invoke(vargs);
        } else {
            return state_->invoke({});
        }
    }

    template < typename... Args >
    uvalue function::operator()(Args&&... args) const {
        return invoke(std::forward<Args>(args)...);
    }

    template < typename... Args >
    bool function::is_invocable_with() const noexcept {
        if constexpr ( sizeof...(Args) > 0 ) {
            using namespace detail;
            const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{type_list<Args>{}}...};
            return state_->is_invocable_with(vargs);
        } else {
            return state_->is_invocable_with({});
        }
    }

    template < typename... Args >
    bool function::is_invocable_with(Args&&... args) const noexcept {
        if constexpr ( sizeof...(Args) > 0 ) {
            using namespace detail;
            const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{std::forward<Args>(args)}...};
            return state_->is_invocable_with(vargs);
        } else {
            return state_->is_invocable_with({});
        }
    }

    inline argument function::get_argument(std::size_t position) const noexcept {
        return position < state_->arguments.size() ? state_->arguments[position] : argument{};
    }

    inline const argument_list& function::get_arguments() const noexcept {
        return state_->arguments;
    }
}

namespace meta_hpp::detail
{
    template < member_kind Member >
    struct member_tag {};

    template < member_kind Member >
    member_type_data::member_type_data(type_list<Member>)
    : type_data_base{type_id{type_list<member_tag<Member>>{}}, type_kind::member_}
    , flags{member_traits<Member>::make_flags()}
    , owner_type{resolve_type<typename member_traits<Member>::class_type>()}
    , value_type{resolve_type<typename member_traits<Member>::value_type>()} {}
}

namespace meta_hpp
{
    inline member_type::member_type(detail::member_type_data* data)
    : data_{data} {}

    inline bool member_type::is_valid() const noexcept {
        return data_ != nullptr;
    }

    inline member_type::operator bool() const noexcept {
        return is_valid();
    }

    inline type_id member_type::get_id() const noexcept {
        return data_->id;
    }

    inline member_bitflags member_type::get_flags() const noexcept {
        return data_->flags;
    }

    inline const metadata_map& member_type::get_metadata() const noexcept {
        return data_->metadata;
    }

    inline class_type member_type::get_owner_type() const noexcept {
        return data_->owner_type;
    }

    inline any_type member_type::get_value_type() const noexcept {
        return data_->value_type;
    }
}

namespace meta_hpp::detail
{
    class uinst_base {
    public:
        enum class ref_types {
            lvalue,
            const_lvalue,
            rvalue,
            const_rvalue,
        };
    public:
        uinst_base() = delete;
        ~uinst_base() = default;

        uinst_base(uinst_base&&) = default;
        uinst_base(const uinst_base&) = default;

        uinst_base& operator=(uinst_base&&) = delete;
        uinst_base& operator=(const uinst_base&) = delete;

        template < typename T >
        uinst_base(type_list<T>) = delete;

        template < typename T, typename Tp = std::decay_t<T> >
            requires (!any_uvalue_kind<Tp>)
        // NOLINTNEXTLINE(*-forwarding-reference-overload)
        explicit uinst_base(T&&)
        : uinst_base{type_list<T&&>{}} {}

        template < inst_class_lvalue_ref_kind T >
        explicit uinst_base(type_list<T>)
        : ref_type_{std::is_const_v<std::remove_reference_t<T>>
            ? ref_types::const_lvalue
            : ref_types::lvalue}
        , raw_type_{resolve_type<std::remove_cvref_t<T>>()} {}

        template < inst_class_rvalue_ref_kind T >
        explicit uinst_base(type_list<T>)
        : ref_type_{std::is_const_v<std::remove_reference_t<T>>
            ? ref_types::const_rvalue
            : ref_types::rvalue}
        , raw_type_{resolve_type<std::remove_cvref_t<T>>()} {}

        explicit uinst_base(uvalue& v)
        : ref_type_{ref_types::lvalue}
        , raw_type_{v.get_type()} {}

        explicit uinst_base(const uvalue& v)
        : ref_type_{ref_types::const_lvalue}
        , raw_type_{v.get_type()} {}

        explicit uinst_base(uvalue&& v)
        : ref_type_{ref_types::rvalue}
        , raw_type_{v.get_type()} {}

        explicit uinst_base(const uvalue&& v)
        : ref_type_{ref_types::const_rvalue}
        , raw_type_{v.get_type()} {}

        [[nodiscard]] bool is_inst_const() const noexcept {
            if ( raw_type_.is_pointer() ) {
                const pointer_type& from_type_ptr = raw_type_.as_pointer();
                const bool from_type_ptr_readonly = from_type_ptr.get_flags().has(pointer_flags::is_readonly);
                return from_type_ptr_readonly;
            }
            return ref_type_ == ref_types::const_lvalue
                || ref_type_ == ref_types::const_rvalue;
        }

        [[nodiscard]] ref_types get_ref_type() const noexcept {
            return ref_type_;
        }

        [[nodiscard]] const any_type& get_raw_type() const noexcept {
            return raw_type_;
        }

        template < inst_class_ref_kind Q >
        [[nodiscard]] bool can_cast_to() const noexcept;
    private:
        ref_types ref_type_{};
        any_type raw_type_{};
    };
}

namespace meta_hpp::detail
{
    class uinst final : public uinst_base {
    public:
        uinst() = delete;
        ~uinst() = default;

        uinst(uinst&&) = default;
        uinst(const uinst&) = default;

        uinst& operator=(uinst&&) = delete;
        uinst& operator=(const uinst&) = delete;

        template < typename T, uvalue_kind Tp = std::decay_t<T> >
        // NOLINTNEXTLINE(*-forwarding-reference-overload)
        explicit uinst(T&& v)
        : uinst_base{std::forward<T>(v)}
        // NOLINTNEXTLINE(*-const-cast)
        , data_{const_cast<void*>(v.data())} {}

        template < typename T, typename Tp = std::decay_t<T> >
            requires (!any_uvalue_kind<Tp>)
        // NOLINTNEXTLINE(*-forwarding-reference-overload)
        explicit uinst(T&& v)
        : uinst_base{std::forward<T>(v)}
        // NOLINTNEXTLINE(*-const-cast)
        , data_{const_cast<std::remove_cvref_t<T>*>(std::addressof(v))} {}

        template < inst_class_ref_kind Q >
        [[nodiscard]] decltype(auto) cast() const;
    private:
        void* data_{};
    };
}

namespace meta_hpp::detail
{
    template < inst_class_ref_kind Q >
    bool uinst_base::can_cast_to() const noexcept {
        using inst_class = typename inst_traits<Q>::class_type;
        using inst_method = typename inst_traits<Q>::method_type;

        const any_type& from_type = get_raw_type();
        const any_type& to_type = resolve_type<inst_class>();

        const auto is_a = [](const any_type& base, const any_type& derived){
            return (base == derived)
                || (base.is_class() && derived.is_class() && base.as_class().is_base_of(derived.as_class()));
        };

        if ( from_type.is_class() ) {
            const auto is_invocable = [this](){
                switch ( get_ref_type() ) {
                case ref_types::lvalue:
                    return std::is_invocable_v<inst_method, inst_class&>;
                case ref_types::const_lvalue:
                    return std::is_invocable_v<inst_method, const inst_class&>;
                case ref_types::rvalue:
                    return std::is_invocable_v<inst_method, inst_class&&>;
                case ref_types::const_rvalue:
                    return std::is_invocable_v<inst_method, const inst_class&&>;
                }
                return false;
            };

            return is_invocable() && is_a(to_type, from_type);
        }

        if ( from_type.is_pointer() ) {
            const pointer_type& from_type_ptr = from_type.as_pointer();
            const bool from_type_ptr_readonly = from_type_ptr.get_flags().has(pointer_flags::is_readonly);
            const any_type& from_data_type = from_type_ptr.get_data_type();

            const auto is_invocable = [from_type_ptr_readonly](){
                return from_type_ptr_readonly
                    ? std::is_invocable_v<inst_method, const inst_class&>
                    : std::is_invocable_v<inst_method, inst_class&>;
            };

            return is_invocable() && is_a(to_type, from_data_type);
        }

        return false;
    }
}

namespace meta_hpp::detail
{
    template < inst_class_ref_kind Q >
    decltype(auto) uinst::cast() const {
        if ( !can_cast_to<Q>() ) {
            throw_exception_with("bad instance cast");
        }

        using inst_class_cv = std::remove_reference_t<Q>;
        using inst_class = std::remove_cv_t<inst_class_cv>;

        const any_type& from_type = get_raw_type();
        const any_type& to_type = resolve_type<inst_class>();

        if ( from_type.is_class() && to_type.is_class() ) {
            const class_type& from_class = from_type.as_class();
            const class_type& to_class = to_type.as_class();

            void* to_ptr = pointer_upcast(data_, from_class, to_class);

            if constexpr ( !std::is_reference_v<Q> ) {
                return *static_cast<inst_class_cv*>(to_ptr);
            }

            if constexpr ( std::is_lvalue_reference_v<Q> ) {
                return *static_cast<inst_class_cv*>(to_ptr);
            }

            if constexpr ( std::is_rvalue_reference_v<Q> ) {
                return std::move(*static_cast<inst_class_cv*>(to_ptr));
            }
        }

        if ( from_type.is_pointer() ) {
            const pointer_type& from_type_ptr = from_type.as_pointer();
            const any_type& from_data_type = from_type_ptr.get_data_type();

            if ( from_data_type.is_class() && to_type.is_class() ) {
                const class_type& from_data_class = from_data_type.as_class();
                const class_type& to_class = to_type.as_class();

                void** from_data_ptr = static_cast<void**>(data_);
                void* to_ptr = pointer_upcast(*from_data_ptr, from_data_class, to_class);

                if constexpr ( !std::is_reference_v<Q> ) {
                    return *static_cast<inst_class_cv*>(to_ptr);
                }

                if constexpr ( std::is_lvalue_reference_v<Q> ) {
                    return *static_cast<inst_class_cv*>(to_ptr);
                }
            }
        }

        throw_exception_with("bad instance cast");
    }
}

namespace meta_hpp::detail
{
    template < member_policy_kind Policy, member_kind Member >
    uvalue raw_member_getter(const Member& member, const uinst& inst) {
        using mt = member_traits<Member>;
        using class_type = typename mt::class_type;
        using value_type = typename mt::value_type;

        constexpr bool as_copy =
            std::is_copy_constructible_v<value_type> &&
            std::is_same_v<Policy, member_policy::as_copy_t>;

        constexpr bool as_ptr =
            std::is_same_v<Policy, member_policy::as_pointer_t>;

        constexpr bool as_ref_wrap =
            std::is_same_v<Policy, member_policy::as_reference_wrapper_t>;

        static_assert(as_copy || as_ptr || as_ref_wrap);

        if ( !inst.can_cast_to<const class_type>() ) {
            throw_exception_with("an attempt to get a member with an incorrect instance type");
        }

        if ( inst.is_inst_const() ) {
            auto&& return_value = inst.cast<const class_type>().*member;

            if constexpr ( as_copy ) {
                return uvalue{std::forward<decltype(return_value)>(return_value)};
            }

            if constexpr ( as_ptr ) {
                return uvalue{std::addressof(return_value)};
            }

            if constexpr ( as_ref_wrap ) {
                return uvalue{std::ref(return_value)};
            }
        } else {
            auto&& return_value = inst.cast<class_type>().*member;

            if constexpr ( as_copy ) {
                return uvalue{std::forward<decltype(return_value)>(return_value)};
            }

            if constexpr ( as_ptr ) {
                return uvalue{std::addressof(return_value)};
            }

            if constexpr ( as_ref_wrap ) {
                return uvalue{std::ref(return_value)};
            }
        }
    }

    template < member_kind Member >
    bool raw_member_is_gettable_with(const uinst_base& inst) {
        using mt = member_traits<Member>;
        using class_type = typename mt::class_type;

        return inst.can_cast_to<const class_type>();
    }
}

namespace meta_hpp::detail
{
    template < member_kind Member >
    void raw_member_setter([[maybe_unused]] const Member& member, const uinst& inst, const uarg& arg) {
        using mt = member_traits<Member>;
        using class_type = typename mt::class_type;
        using value_type = typename mt::value_type;

        if constexpr ( std::is_const_v<value_type> ) {
            throw_exception_with("an attempt to set a constant member");
        } else {
            if ( inst.is_inst_const() ) {
                throw_exception_with("an attempt to set a member with an const instance type");
            }

            if ( !inst.can_cast_to<class_type>() ) {
                throw_exception_with("an attempt to set a member with an incorrect instance type");
            }

            if ( !arg.can_cast_to<value_type>() ) {
                throw_exception_with("an attempt to set a member with an incorrect argument type");
            }

            inst.cast<class_type>().*member = arg.cast<value_type>();
        }
    }

    template < member_kind Member >
    bool raw_member_is_settable_with(const uinst_base& inst, const uarg_base& arg) {
        using mt = member_traits<Member>;
        using class_type = typename mt::class_type;
        using value_type = typename mt::value_type;

        return !std::is_const_v<value_type>
            && !inst.is_inst_const()
            && inst.can_cast_to<class_type>()
            && arg.can_cast_to<value_type>();
    }
}

namespace meta_hpp::detail
{
    template < member_policy_kind Policy, member_kind Member >
    member_state::getter_impl make_member_getter(Member member) {
        return [member = std::move(member)](const uinst& inst){
            return raw_member_getter<Policy>(member, inst);
        };
    }

    template < member_kind Member >
    member_state::is_gettable_with_impl make_member_is_gettable_with() {
        return &raw_member_is_gettable_with<Member>;
    }

    template < member_kind Member >
    member_state::setter_impl make_member_setter(Member member) {
        return [member = std::move(member)](const uinst& inst, const uarg& arg){
            return raw_member_setter(member, inst, arg);
        };
    }

    template < member_kind Member >
    member_state::is_settable_with_impl make_member_is_settable_with() {
        return &raw_member_is_settable_with<Member>;
    }
}

namespace meta_hpp::detail
{
    template < member_policy_kind Policy, member_kind Member >
    member_state_ptr member_state::make(std::string name, Member member, metadata_map metadata) {
        return std::make_shared<member_state>(member_state{
            .index{member_index::make<Member>(std::move(name))},
            .metadata{std::move(metadata)},
            .getter{make_member_getter<Policy>(member)},
            .setter{make_member_setter(member)},
            .is_gettable_with{make_member_is_gettable_with<Member>()},
            .is_settable_with{make_member_is_settable_with<Member>()},
        });
    }
}

namespace meta_hpp
{
    inline member::member(detail::member_state_ptr state) noexcept
    : state_{std::move(state)} {}

    inline member& member::operator=(detail::member_state_ptr state) noexcept {
        state_ = std::move(state);
        return *this;
    }

    inline bool member::is_valid() const noexcept {
        return !!state_;
    }

    inline member::operator bool() const noexcept {
        return is_valid();
    }

    inline const member_index& member::get_index() const noexcept {
        return state_->index;
    }

    inline const metadata_map& member::get_metadata() const noexcept {
        return state_->metadata;
    }

    inline const member_type& member::get_type() const noexcept {
        return state_->index.get_type();
    }

    inline const std::string& member::get_name() const noexcept {
        return state_->index.get_name();
    }

    template < typename Instance >
    uvalue member::get(Instance&& instance) const {
        using namespace detail;
        const uinst vinst{std::forward<Instance>(instance)};
        return state_->getter(vinst);
    }

    template < typename T, typename Instance >
    T member::get_as(Instance&& instance) const {
        return get(std::forward<Instance>(instance)).template get_as<T>();
    }

    template < typename Instance, typename Value >
    void member::set(Instance&& instance, Value&& value) const {
        using namespace detail;
        const uinst vinst{std::forward<Instance>(instance)};
        const uarg vvalue{std::forward<Value>(value)};
        state_->setter(vinst, vvalue);
    }

    template < typename Instance >
    uvalue member::operator()(Instance&& instance) const {
        return get(std::forward<Instance>(instance));
    }

    template < typename Instance, typename Value >
    void member::operator()(Instance&& instance, Value&& value) const {
        set(std::forward<Instance>(instance), std::forward<Value>(value));
    }

    template < typename Instance >
    [[nodiscard]] bool member::is_gettable_with() const noexcept {
        using namespace detail;
        const uinst_base vinst{type_list<Instance>{}};
        return state_->is_gettable_with(vinst);
    }

    template < typename Instance >
    [[nodiscard]] bool member::is_gettable_with(Instance&& instance) const noexcept {
        using namespace detail;
        const uinst_base vinst{std::forward<Instance>(instance)};
        return state_->is_gettable_with(vinst);
    }

    template < typename Instance, typename Value >
    [[nodiscard]] bool member::is_settable_with() const noexcept {
        using namespace detail;
        const uinst_base vinst{type_list<Instance>{}};
        const uarg_base vvalue{type_list<Value>{}};
        return state_->is_settable_with(vinst, vvalue);
    }

    template < typename Instance, typename Value >
    [[nodiscard]] bool member::is_settable_with(Instance&& instance, Value&& value) const noexcept {
        using namespace detail;
        const uinst_base vinst{std::forward<Instance>(instance)};
        const uarg_base vvalue{std::forward<Value>(value)};
        return state_->is_settable_with(vinst, vvalue);
    }
}

namespace meta_hpp::detail
{
    template < method_kind Method >
    struct method_tag {};

    template < method_kind Method >
    method_type_data::method_type_data(type_list<Method>)
    : type_data_base{type_id{type_list<method_tag<Method>>{}}, type_kind::method_}
    , flags{method_traits<Method>::make_flags()}
    , owner_type{resolve_type<typename method_traits<Method>::class_type>()}
    , return_type{resolve_type<typename method_traits<Method>::return_type>()}
    , argument_types{resolve_types(typename method_traits<Method>::argument_types{})} {}
}

namespace meta_hpp
{
    inline method_type::method_type(detail::method_type_data* data)
    : data_{data} {}

    inline bool method_type::is_valid() const noexcept {
        return data_ != nullptr;
    }

    inline method_type::operator bool() const noexcept {
        return is_valid();
    }

    inline type_id method_type::get_id() const noexcept {
        return data_->id;
    }

    inline method_bitflags method_type::get_flags() const noexcept {
        return data_->flags;
    }

    inline const metadata_map& method_type::get_metadata() const noexcept {
        return data_->metadata;
    }

    inline std::size_t method_type::get_arity() const noexcept {
        return data_->argument_types.size();
    }

    inline class_type method_type::get_owner_type() const noexcept {
        return data_->owner_type;
    }

    inline any_type method_type::get_return_type() const noexcept {
        return data_->return_type;
    }

    inline any_type method_type::get_argument_type(std::size_t position) const noexcept {
        return position < data_->argument_types.size() ? data_->argument_types[position] : any_type{};
    }

    inline const any_type_list& method_type::get_argument_types() const noexcept {
        return data_->argument_types;
    }
}

namespace meta_hpp::detail
{
    template < method_policy_kind Policy, method_kind Method >
    uvalue raw_method_invoke(const Method& method, const uinst& inst, std::span<const uarg> args) {
        using mt = method_traits<Method>;
        using return_type = typename mt::return_type;
        using qualified_type = typename mt::qualified_type;
        using argument_types = typename mt::argument_types;

        constexpr bool as_copy =
            std::is_copy_constructible_v<return_type> &&
            std::is_same_v<Policy, method_policy::as_copy_t>;

        constexpr bool as_void =
            std::is_void_v<return_type> ||
            std::is_same_v<Policy, method_policy::discard_return_t>;

        constexpr bool ref_as_ptr =
            std::is_reference_v<return_type> &&
            std::is_same_v<Policy, method_policy::return_reference_as_pointer_t>;

        static_assert(as_copy || as_void || ref_as_ptr);

        if ( args.size() != mt::arity ) {
            throw_exception_with("an attempt to call a method with an incorrect arity");
        }

        if ( !inst.can_cast_to<qualified_type>() ) {
            throw_exception_with("an attempt to call a method with an incorrect instance type");
        }

        return [&method, &inst, args]<std::size_t... Is>(std::index_sequence<Is...>) -> uvalue {
            if ( !(... && args[Is].can_cast_to<type_list_at_t<Is, argument_types>>()) ) {
                throw_exception_with("an attempt to call a method with incorrect argument types");
            }

            if constexpr ( std::is_void_v<return_type> ) {
                (inst.cast<qualified_type>().*method)(
                    args[Is].cast<type_list_at_t<Is, argument_types>>()...);
                return uvalue{};
            } else if constexpr ( std::is_same_v<Policy, method_policy::discard_return_t> ) {
                std::ignore = (inst.cast<qualified_type>().*method)(
                    args[Is].cast<type_list_at_t<Is, argument_types>>()...);
                return uvalue{};
            } else {
                return_type&& return_value = (inst.cast<qualified_type>().*method)(
                    args[Is].cast<type_list_at_t<Is, argument_types>>()...);

                if constexpr ( ref_as_ptr ) {
                    return uvalue{std::addressof(return_value)};
                } else {
                    return uvalue{std::forward<decltype(return_value)>(return_value)};
                }
            }
        }(std::make_index_sequence<mt::arity>());
    }

    template < method_kind Method >
    bool raw_method_is_invocable_with(const uinst_base& inst, std::span<const uarg_base> args) {
        using mt = method_traits<Method>;
        using qualified_type = typename mt::qualified_type;
        using argument_types = typename mt::argument_types;

        if ( args.size() != mt::arity ) {
            return false;
        }

        if ( !inst.can_cast_to<qualified_type>() ) {
            return false;
        }

        return [args]<std::size_t... Is>(std::index_sequence<Is...>){
            return (... && args[Is].can_cast_to<type_list_at_t<Is, argument_types>>());
        }(std::make_index_sequence<mt::arity>());
    }
}

namespace meta_hpp::detail
{
    template < method_policy_kind Policy, method_kind Method >
    method_state::invoke_impl make_method_invoke(Method method) {
        return [method = std::move(method)](const uinst& inst, std::span<const uarg> args){
            return raw_method_invoke<Policy>(method, inst, args);
        };
    }

    template < method_kind Method >
    method_state::is_invocable_with_impl make_method_is_invocable_with() {
        return &raw_method_is_invocable_with<Method>;
    }

    template < method_kind Method >
    argument_list make_method_arguments() {
        using mt = method_traits<Method>;
        using mt_argument_types = typename mt::argument_types;

        return []<std::size_t... Is>(std::index_sequence<Is...>){
            [[maybe_unused]] const auto make_argument = []<std::size_t I>(std::index_sequence<I>){
                using P = type_list_at_t<I, mt_argument_types>;
                return argument{argument_state::make<P>(I, metadata_map{})};
            };
            return argument_list{make_argument(std::index_sequence<Is>{})...};
        }(std::make_index_sequence<mt::arity>());
    }
}

namespace meta_hpp::detail
{
    template < method_policy_kind Policy, method_kind Method >
    method_state_ptr method_state::make(std::string name, Method method, metadata_map metadata) {
        return std::make_shared<method_state>(method_state{
            .index{method_index::make<Method>(std::move(name))},
            .metadata{std::move(metadata)},
            .invoke{make_method_invoke<Policy>(std::move(method))},
            .is_invocable_with{make_method_is_invocable_with<Method>()},
            .arguments{make_method_arguments<Method>()},
        });
    }
}

namespace meta_hpp
{
    inline method::method(detail::method_state_ptr state) noexcept
    : state_{std::move(state)} {}

    inline method& method::operator=(detail::method_state_ptr state) noexcept {
        state_ = std::move(state);
        return *this;
    }

    inline bool method::is_valid() const noexcept {
        return !!state_;
    }

    inline method::operator bool() const noexcept {
        return is_valid();
    }

    inline const method_index& method::get_index() const noexcept {
        return state_->index;
    }

    inline const metadata_map& method::get_metadata() const noexcept {
        return state_->metadata;
    }

    inline const method_type& method::get_type() const noexcept {
        return state_->index.get_type();
    }

    inline const std::string& method::get_name() const noexcept {
        return state_->index.get_name();
    }

    template < typename Instance, typename... Args >
    uvalue method::invoke(Instance&& instance, Args&&... args) const {
        using namespace detail;
        const uinst vinst{std::forward<Instance>(instance)};
        if constexpr ( sizeof...(Args) > 0 ) {
            const std::array<uarg, sizeof...(Args)> vargs{uarg{std::forward<Args>(args)}...};
            return state_->invoke(vinst, vargs);
        } else {
            return state_->invoke(vinst, {});
        }
    }

    template < typename Instance, typename... Args >
    uvalue method::operator()(Instance&& instance, Args&&... args) const {
        return invoke(std::forward<Instance>(instance), std::forward<Args>(args)...);
    }

    template < typename Instance, typename... Args >
    bool method::is_invocable_with() const noexcept {
        using namespace detail;
        const uinst_base vinst{type_list<Instance>{}};
        if constexpr ( sizeof...(Args) > 0 ) {
            const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{type_list<Args>{}}...};
            return state_->is_invocable_with(vinst, vargs);
        } else {
            return state_->is_invocable_with(vinst, {});
        }
    }

    template < typename Instance, typename... Args >
    bool method::is_invocable_with(Instance&& instance, Args&&... args) const noexcept {
        using namespace detail;
        const uinst_base vinst{std::forward<Instance>(instance)};
        if constexpr ( sizeof...(Args) > 0 ) {
            const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{std::forward<Args>(args)}...};
            return state_->is_invocable_with(vinst, vargs);
        } else {
            return state_->is_invocable_with(vinst, {});
        }
    }

    inline argument method::get_argument(std::size_t position) const noexcept {
        return position < state_->arguments.size() ? state_->arguments[position] : argument{};
    }

    inline const argument_list& method::get_arguments() const noexcept {
        return state_->arguments;
    }
}

namespace meta_hpp::detail
{
    template < pointer_kind Pointer >
    struct pointer_tag {};

    template < pointer_kind Pointer >
    pointer_type_data::pointer_type_data(type_list<Pointer>)
    : type_data_base{type_id{type_list<pointer_tag<Pointer>>{}}, type_kind::pointer_}
    , flags{pointer_traits<Pointer>::make_flags()}
    , data_type{resolve_type<typename pointer_traits<Pointer>::data_type>()} {}
}

namespace meta_hpp
{
    inline pointer_type::pointer_type(detail::pointer_type_data* data)
    : data_{data} {}

    inline bool pointer_type::is_valid() const noexcept {
        return data_ != nullptr;
    }

    inline pointer_type::operator bool() const noexcept {
        return is_valid();
    }

    inline type_id pointer_type::get_id() const noexcept {
        return data_->id;
    }

    inline pointer_bitflags pointer_type::get_flags() const noexcept {
        return data_->flags;
    }

    inline const metadata_map& pointer_type::get_metadata() const noexcept {
        return data_->metadata;
    }

    inline any_type pointer_type::get_data_type() const noexcept {
        return data_->data_type;
    }
}

namespace meta_hpp::detail
{
    template < variable_policy_kind Policy, pointer_kind Pointer >
    uvalue raw_variable_getter(const Pointer& pointer) {
        using pt = pointer_traits<Pointer>;
        using data_type = typename pt::data_type;

        constexpr bool as_copy =
            std::is_copy_constructible_v<data_type> &&
            std::is_same_v<Policy, variable_policy::as_copy_t>;

        constexpr bool as_ptr =
            std::is_same_v<Policy, variable_policy::as_pointer_t>;

        constexpr bool as_ref_wrap =
            std::is_same_v<Policy, variable_policy::as_reference_wrapper_t>;

        static_assert(as_copy || as_ptr || as_ref_wrap);

        auto&& return_value = *pointer;

        if constexpr ( as_copy ) {
            return uvalue{std::forward<decltype(return_value)>(return_value)};
        }

        if constexpr ( as_ptr ) {
            return uvalue{std::addressof(return_value)};
        }

        if constexpr ( as_ref_wrap) {
            return uvalue{std::ref(return_value)};
        }
    }

    template < pointer_kind Pointer >
    void raw_variable_setter([[maybe_unused]] const Pointer& pointer, const uarg& arg) {
        using pt = pointer_traits<Pointer>;
        using data_type = typename pt::data_type;

        if constexpr ( std::is_const_v<data_type> ) {
            throw_exception_with("an attempt to set a constant variable");
        } else {
            if ( !arg.can_cast_to<data_type>() ) {
                throw_exception_with("an attempt to set a variable with an incorrect argument type");
            }
            *pointer = arg.cast<data_type>();
        }
    }

    template < pointer_kind Pointer >
    bool raw_variable_is_settable_with(const uarg_base& arg) {
        using pt = pointer_traits<Pointer>;
        using data_type = typename pt::data_type;

        return !std::is_const_v<data_type>
            && arg.can_cast_to<data_type>();
    }
}

namespace meta_hpp::detail
{
    template < variable_policy_kind Policy, pointer_kind Pointer >
    variable_state::getter_impl make_variable_getter(Pointer pointer) {
        return [pointer = std::move(pointer)](){
            return raw_variable_getter<Policy>(pointer);
        };
    }

    template < pointer_kind Pointer >
    variable_state::setter_impl make_variable_setter(Pointer pointer) {
        return [pointer = std::move(pointer)](const uarg& arg){
            return raw_variable_setter(pointer, arg);
        };
    }

    template < pointer_kind Pointer >
    variable_state::is_settable_with_impl make_variable_is_settable_with() {
        return &raw_variable_is_settable_with<Pointer>;
    }
}

namespace meta_hpp::detail
{
    template < variable_policy_kind Policy, pointer_kind Pointer >
    variable_state_ptr variable_state::make(std::string name, Pointer pointer, metadata_map metadata) {
        return std::make_shared<variable_state>(variable_state{
            .index{variable_index::make<Pointer>(std::move(name))},
            .metadata{std::move(metadata)},
            .getter{make_variable_getter<Policy>(pointer)},
            .setter{make_variable_setter(pointer)},
            .is_settable_with{make_variable_is_settable_with<Pointer>()},
        });
    }
}

namespace meta_hpp
{
    inline variable::variable(detail::variable_state_ptr state) noexcept
    : state_{std::move(state)} {}

    inline variable& variable::operator=(detail::variable_state_ptr state) noexcept {
        state_ = std::move(state);
        return *this;
    }

    inline bool variable::is_valid() const noexcept {
        return !!state_;
    }

    inline variable::operator bool() const noexcept {
        return is_valid();
    }

    inline const variable_index& variable::get_index() const noexcept {
        return state_->index;
    }

    inline const metadata_map& variable::get_metadata() const noexcept {
        return state_->metadata;
    }

    inline const pointer_type& variable::get_type() const noexcept {
        return state_->index.get_type();
    }

    inline const std::string& variable::get_name() const noexcept {
        return state_->index.get_name();
    }

    inline uvalue variable::get() const {
        return state_->getter();
    }

    template < typename T >
    T variable::get_as() const {
        return get().get_as<T>();
    }

    template < typename Value >
    void variable::set(Value&& value) const {
        using namespace detail;
        const uarg vvalue{std::forward<Value>(value)};
        state_->setter(vvalue);
    }

    inline uvalue variable::operator()() const {
        return get();
    }

    template < typename Value >
    void variable::operator()(Value&& value) const {
        set(std::forward<Value>(value));
    }

    template < typename Value >
    bool variable::is_settable_with() const noexcept {
        using namespace detail;
        const uarg_base vvalue{type_list<Value>{}};
        return state_->is_settable_with(vvalue);
    }

    template < typename Value >
    bool variable::is_settable_with(Value&& value) const noexcept {
        using namespace detail;
        const uarg vvalue{std::forward<Value>(value)};
        return state_->is_settable_with(vvalue);
    }
}

namespace meta_hpp::detail
{
    template < class_kind Class >
    struct class_tag {};

    template < class_kind Class >
    class_type_data::class_type_data(type_list<Class>)
    : type_data_base{type_id{type_list<class_tag<Class>>{}}, type_kind::class_}
    , flags{class_traits<Class>::make_flags()}
    , size{class_traits<Class>::size}
    , align{class_traits<Class>::align}
    , argument_types{resolve_types(typename class_traits<Class>::argument_types{})} {}
}

namespace meta_hpp
{
    inline class_type::class_type(detail::class_type_data* data)
    : data_{data} {}

    inline bool class_type::is_valid() const noexcept {
        return data_ != nullptr;
    }

    inline class_type::operator bool() const noexcept {
        return is_valid();
    }

    inline type_id class_type::get_id() const noexcept {
        return data_->id;
    }

    inline class_bitflags class_type::get_flags() const noexcept {
        return data_->flags;
    }

    inline const metadata_map& class_type::get_metadata() const noexcept {
        return data_->metadata;
    }

    inline std::size_t class_type::get_size() const noexcept {
        return data_->size;
    }

    inline std::size_t class_type::get_align() const noexcept {
        return data_->align;
    }

    inline std::size_t class_type::get_arity() const noexcept {
        return data_->argument_types.size();
    }

    inline any_type class_type::get_argument_type(std::size_t position) const noexcept {
        return position < data_->argument_types.size() ? data_->argument_types[position] : any_type{};
    }

    inline const any_type_list& class_type::get_argument_types() const noexcept {
        return data_->argument_types;
    }

    inline const class_set& class_type::get_bases() const noexcept {
        return data_->bases;
    }

    inline const constructor_set& class_type::get_constructors() const noexcept {
        return data_->constructors;
    }

    inline const destructor_set& class_type::get_destructors() const noexcept {
        return data_->destructors;
    }

    inline const function_set& class_type::get_functions() const noexcept {
        return data_->functions;
    }

    inline const member_set& class_type::get_members() const noexcept {
        return data_->members;
    }

    inline const method_set& class_type::get_methods() const noexcept {
        return data_->methods;
    }

    inline const typedef_map& class_type::get_typedefs() const noexcept {
        return data_->typedefs;
    }

    inline const variable_set& class_type::get_variables() const noexcept {
        return data_->variables;
    }

    template < typename... Args >
    uvalue class_type::create(Args&&... args) const {
        for ( const constructor& ctor : data_->constructors ) {
            if ( ctor.is_invocable_with(std::forward<Args>(args)...) ) {
                return ctor.create(std::forward<Args>(args)...);
            }
        }
        return uvalue{};
    }

    template < typename... Args >
    uvalue class_type::create_at(void* mem, Args&&... args) const {
        for ( const constructor& ctor : data_->constructors ) {
            if ( ctor.is_invocable_with(std::forward<Args>(args)...) ) {
                return ctor.create_at(mem, std::forward<Args>(args)...);
            }
        }
        return uvalue{};
    }

    template < typename Arg >
    bool class_type::destroy(Arg&& arg) const {
        if ( const destructor& dtor = get_destructor() ) {
            return dtor.destroy(std::forward<Arg>(arg));
        }
        return false;
    }

    inline bool class_type::destroy_at(void* mem) const {
        if ( const destructor& dtor = get_destructor() ) {
            dtor.destroy_at(mem);
            return true;
        }
        return false;
    }

    template < detail::class_kind Derived >
    bool class_type::is_base_of() const noexcept {
        return is_base_of(resolve_type<Derived>());
    }

    inline bool class_type::is_base_of(const class_type& derived) const noexcept {
        if ( !is_valid() || !derived.is_valid() ) {
            return false;
        }

        if ( derived.data_->bases.contains(*this) ) {
            return true;
        }

        // NOLINTNEXTLINE(*-use-anyofallof)
        for ( const class_type& derived_base : derived.data_->bases ) {
            if ( is_base_of(derived_base) ) {
                return true;
            }
        }

        return false;
    }

    template < detail::class_kind Base >
    bool class_type::is_derived_from() const noexcept {
        return is_derived_from(resolve_type<Base>());
    }

    inline bool class_type::is_derived_from(const class_type& base) const noexcept {
        if ( !is_valid() || !base.is_valid() ) {
            return false;
        }

        if ( data_->bases.contains(base) ) {
            return true;
        }

        // NOLINTNEXTLINE(*-use-anyofallof)
        for ( const class_type& self_base : data_->bases ) {
            if ( self_base.is_derived_from(base) ) {
                return true;
            }
        }

        return false;
    }

    inline function class_type::get_function(std::string_view name) const noexcept {
        for ( const function& function : data_->functions ) {
            if ( function.get_name() == name ) {
                return function;
            }
        }

        for ( const class_type& base : data_->bases ) {
            if ( const function& function = base.get_function(name); function ) {
                return function;
            }
        }

        return function{};
    }

    inline member class_type::get_member(std::string_view name) const noexcept {
        for ( const member& member : data_->members ) {
            if ( member.get_name() == name ) {
                return member;
            }
        }

        for ( const class_type& base : data_->bases ) {
            if ( const member& member = base.get_member(name); member ) {
                return member;
            }
        }

        return member{};
    }

    inline method class_type::get_method(std::string_view name) const noexcept {
        for ( const method& method : data_->methods ) {
            if ( method.get_name() == name ) {
                return method;
            }
        }

        for ( const class_type& base : data_->bases ) {
            if ( const method& method = base.get_method(name); method ) {
                return method;
            }
        }

        return method{};
    }

    inline any_type class_type::get_typedef(std::string_view name) const noexcept {
        if ( auto iter{data_->typedefs.find(name)}; iter != data_->typedefs.end() ) {
            return iter->second;
        }

        for ( const class_type& base : data_->bases ) {
            if ( const any_type& type = base.get_typedef(name); type ) {
                return type;
            }
        }

        return any_type{};
    }

    inline variable class_type::get_variable(std::string_view name) const noexcept {
        for ( const variable& variable : data_->variables ) {
            if ( variable.get_name() == name ) {
                return variable;
            }
        }

        for ( const class_type& base : data_->bases ) {
            if ( const variable& variable = base.get_variable(name); variable ) {
                return variable;
            }
        }

        return variable{};
    }

    //
    // get_constructor_with
    //

    template < typename... Args >
    constructor class_type::get_constructor_with() const noexcept {
        return get_constructor_with({resolve_type<Args>()...});
    }

    template < typename Iter >
    constructor class_type::get_constructor_with(Iter first, Iter last) const noexcept {
        for ( const constructor& ctor : data_->constructors ) {
            const any_type_list& args = ctor.get_type().get_argument_types();
            if ( std::equal(first, last, args.begin(), args.end()) ) {
                return ctor;
            }
        }
        return constructor{};
    }

    inline constructor class_type::get_constructor_with(std::span<const any_type> args) const noexcept {
        return get_constructor_with(args.begin(), args.end());
    }

    inline constructor class_type::get_constructor_with(std::initializer_list<any_type> args) const noexcept {
        return get_constructor_with(args.begin(), args.end());
    }

    //
    // get_destructor
    //

    inline destructor class_type::get_destructor() const noexcept {
        if ( data_->destructors.empty() ) {
            return destructor{};
        }
        return *data_->destructors.begin();
    }

    //
    // get_function_with
    //

    template < typename... Args >
    function class_type::get_function_with(std::string_view name) const noexcept {
        return get_function_with(name, {resolve_type<Args>()...});
    }

    template < typename Iter >
    function class_type::get_function_with(std::string_view name, Iter first, Iter last) const noexcept {
        for ( const function& function : data_->functions ) {
            if ( function.get_name() != name ) {
                continue;
            }

            const any_type_list& args = function.get_type().get_argument_types();
            if ( std::equal(first, last, args.begin(), args.end()) ) {
                return function;
            }
        }

        for ( const class_type& base : data_->bases ) {
            if ( const function& function = base.get_function_with(name, first, last); function ) {
                return function;
            }
        }

        return function{};
    }

    inline function class_type::get_function_with(std::string_view name, std::span<const any_type> args) const noexcept {
        return get_function_with(name, args.begin(), args.end());
    }

    inline function class_type::get_function_with(std::string_view name, std::initializer_list<any_type> args) const noexcept {
        return get_function_with(name, args.begin(), args.end());
    }

    //
    // get_method_with
    //

    template < typename... Args >
    method class_type::get_method_with(std::string_view name) const noexcept {
        return get_method_with(name, {resolve_type<Args>()...});
    }

    template < typename Iter >
    method class_type::get_method_with(std::string_view name, Iter first, Iter last) const noexcept {
        for ( const method& method : data_->methods ) {
            if ( method.get_name() != name ) {
                continue;
            }

            const any_type_list& args = method.get_type().get_argument_types();
            if ( std::equal(first, last, args.begin(), args.end()) ) {
                return method;
            }
        }

        for ( const class_type& base : data_->bases ) {
            if ( const method& method = base.get_method_with(name, first, last); method ) {
                return method;
            }
        }

        return method{};
    }

    inline method class_type::get_method_with(std::string_view name, std::span<const any_type> args) const noexcept {
        return get_method_with(name, args.begin(), args.end());
    }

    inline method class_type::get_method_with(std::string_view name, std::initializer_list<any_type> args) const noexcept {
        return get_method_with(name, args.begin(), args.end());
    }
}

namespace meta_hpp::detail
{
    inline scope_state_ptr scope_state::make(std::string name, metadata_map metadata) {
        return std::make_shared<scope_state>(scope_state{
            .index{scope_index::make(std::move(name))},
            .metadata{std::move(metadata)},
        });
    }
}

namespace meta_hpp
{
    inline scope::scope(detail::scope_state_ptr state) noexcept
    : state_{std::move(state)} {}

    inline scope& scope::operator=(detail::scope_state_ptr state) noexcept {
        state_ = std::move(state);
        return *this;
    }

    inline bool scope::is_valid() const noexcept {
        return !!state_;
    }

    inline scope::operator bool() const noexcept {
        return is_valid();
    }

    inline const scope_index& scope::get_index() const noexcept {
        return state_->index;
    }

    inline const metadata_map& scope::get_metadata() const noexcept {
        return state_->metadata;
    }

    inline const std::string& scope::get_name() const noexcept {
        return state_->index.get_name();
    }

    inline const function_set& scope::get_functions() const noexcept {
        return state_->functions;
    }

    inline const typedef_map& scope::get_typedefs() const noexcept {
        return state_->typedefs;
    }

    inline const variable_set& scope::get_variables() const noexcept {
        return state_->variables;
    }

    inline function scope::get_function(std::string_view name) const noexcept {
        for ( const function& function : state_->functions ) {
            if ( function.get_name() == name ) {
                return function;
            }
        }
        return function{};
    }

    inline any_type scope::get_typedef(std::string_view name) const noexcept {
        if ( auto iter{state_->typedefs.find(name)}; iter != state_->typedefs.end() ) {
            return iter->second;
        }
        return any_type{};
    }

    inline variable scope::get_variable(std::string_view name) const noexcept {
        for ( const variable& variable : state_->variables ) {
            if ( variable.get_name() == name ) {
                return variable;
            }
        }
        return variable{};
    }

    template < typename... Args >
    function scope::get_function_with(std::string_view name) const noexcept {
        return get_function_with(name, {resolve_type<Args>()...});
    }

    template < typename Iter >
    function scope::get_function_with(std::string_view name, Iter first, Iter last) const noexcept {
        for ( const function& function : state_->functions ) {
            if ( function.get_name() != name ) {
                continue;
            }

            const any_type_list& args = function.get_type().get_argument_types();
            if ( std::equal(first, last, args.begin(), args.end()) ) {
                return function;
            }
        }
        return function{};
    }

    inline function scope::get_function_with(std::string_view name, std::span<const any_type> args) const noexcept {
        return get_function_with(name, args.begin(), args.end());
    }

    inline function scope::get_function_with(std::string_view name, std::initializer_list<any_type> args) const noexcept {
        return get_function_with(name, args.begin(), args.end());
    }
}

namespace meta_hpp
{
    inline bool any_type::is_valid() const noexcept {
        return data_ != nullptr;
    }

    inline any_type::operator bool() const noexcept {
        return is_valid();
    }

    inline type_id any_type::get_id() const noexcept {
        return data_->id;
    }

    inline type_kind any_type::get_kind() const noexcept {
        return data_->kind;
    }

    inline const metadata_map& any_type::get_metadata() const noexcept {
        return data_->metadata;
    }

    inline any_type::any_type(const array_type& other) noexcept
    : data_{detail::type_access(other)} {}

    inline any_type::any_type(const class_type& other) noexcept
    : data_{detail::type_access(other)} {}

    inline any_type::any_type(const constructor_type& other) noexcept
    : data_{detail::type_access(other)} {}

    inline any_type::any_type(const destructor_type& other) noexcept
    : data_{detail::type_access(other)} {}

    inline any_type::any_type(const enum_type& other) noexcept
    : data_{detail::type_access(other)} {}

    inline any_type::any_type(const function_type& other) noexcept
    : data_{detail::type_access(other)} {}

    inline any_type::any_type(const member_type& other) noexcept
    : data_{detail::type_access(other)} {}

    inline any_type::any_type(const method_type& other) noexcept
    : data_{detail::type_access(other)} {}

    inline any_type::any_type(const nullptr_type& other) noexcept
    : data_{detail::type_access(other)} {}

    inline any_type::any_type(const number_type& other) noexcept
    : data_{detail::type_access(other)} {}

    inline any_type::any_type(const pointer_type& other) noexcept
    : data_{detail::type_access(other)} {}

    inline any_type::any_type(const reference_type& other) noexcept
    : data_{detail::type_access(other)} {}

    inline any_type::any_type(const void_type& other) noexcept
    : data_{detail::type_access(other)} {}

    inline bool any_type::is_array() const noexcept {
        return is_valid() && data_->kind == type_kind::array_;
    }

    inline bool any_type::is_class() const noexcept {
        return is_valid() && data_->kind == type_kind::class_;
    }

    inline bool any_type::is_constructor() const noexcept {
        return is_valid() && data_->kind == type_kind::constructor_;
    }

    inline bool any_type::is_destructor() const noexcept {
        return is_valid() && data_->kind == type_kind::destructor_;
    }

    inline bool any_type::is_enum() const noexcept {
        return is_valid() && data_->kind == type_kind::enum_;
    }

    inline bool any_type::is_function() const noexcept {
        return is_valid() && data_->kind == type_kind::function_;
    }

    inline bool any_type::is_member() const noexcept {
        return is_valid() && data_->kind == type_kind::member_;
    }

    inline bool any_type::is_method() const noexcept {
        return is_valid() && data_->kind == type_kind::method_;
    }

    inline bool any_type::is_nullptr() const noexcept {
        return is_valid() && data_->kind == type_kind::nullptr_;
    }

    inline bool any_type::is_number() const noexcept {
        return is_valid() && data_->kind == type_kind::number_;
    }

    inline bool any_type::is_pointer() const noexcept {
        return is_valid() && data_->kind == type_kind::pointer_;
    }

    inline bool any_type::is_reference() const noexcept {
        return is_valid() && data_->kind == type_kind::reference_;
    }

    inline bool any_type::is_void() const noexcept {
        return is_valid() && data_->kind == type_kind::void_;
    }

    inline array_type any_type::as_array() const noexcept {
        // NOLINTNEXTLINE(*-static-cast-downcast)
        return is_array() ? array_type{static_cast<detail::array_type_data*>(data_)} : array_type{};
    }

    inline class_type any_type::as_class() const noexcept {
        // NOLINTNEXTLINE(*-static-cast-downcast)
        return is_class() ? class_type{static_cast<detail::class_type_data*>(data_)} : class_type{};
    }

    inline constructor_type any_type::as_constructor() const noexcept {
        // NOLINTNEXTLINE(*-static-cast-downcast)
        return is_constructor() ? constructor_type{static_cast<detail::constructor_type_data*>(data_)} : constructor_type{};
    }

    inline destructor_type any_type::as_destructor() const noexcept {
        // NOLINTNEXTLINE(*-static-cast-downcast)
        return is_destructor() ? destructor_type{static_cast<detail::destructor_type_data*>(data_)} : destructor_type{};
    }

    inline enum_type any_type::as_enum() const noexcept {
        // NOLINTNEXTLINE(*-static-cast-downcast)
        return is_enum() ? enum_type{static_cast<detail::enum_type_data*>(data_)} : enum_type{};
    }

    inline function_type any_type::as_function() const noexcept {
        // NOLINTNEXTLINE(*-static-cast-downcast)
        return is_function() ? function_type{static_cast<detail::function_type_data*>(data_)} : function_type{};
    }

    inline member_type any_type::as_member() const noexcept {
        // NOLINTNEXTLINE(*-static-cast-downcast)
        return is_member() ? member_type{static_cast<detail::member_type_data*>(data_)} : member_type{};
    }

    inline method_type any_type::as_method() const noexcept {
        // NOLINTNEXTLINE(*-static-cast-downcast)
        return is_method() ? method_type{static_cast<detail::method_type_data*>(data_)} : method_type{};
    }

    inline nullptr_type any_type::as_nullptr() const noexcept {
        // NOLINTNEXTLINE(*-static-cast-downcast)
        return is_nullptr() ? nullptr_type{static_cast<detail::nullptr_type_data*>(data_)} : nullptr_type{};
    }

    inline number_type any_type::as_number() const noexcept {
        // NOLINTNEXTLINE(*-static-cast-downcast)
        return is_number() ? number_type{static_cast<detail::number_type_data*>(data_)} : number_type{};
    }

    inline pointer_type any_type::as_pointer() const noexcept {
        // NOLINTNEXTLINE(*-static-cast-downcast)
        return is_pointer() ? pointer_type{static_cast<detail::pointer_type_data*>(data_)} : pointer_type{};
    }

    inline reference_type any_type::as_reference() const noexcept {
        // NOLINTNEXTLINE(*-static-cast-downcast)
        return is_reference() ? reference_type{static_cast<detail::reference_type_data*>(data_)} : reference_type{};
    }

    inline void_type any_type::as_void() const noexcept {
        // NOLINTNEXTLINE(*-static-cast-downcast)
        return is_void() ? void_type{static_cast<detail::void_type_data*>(data_)} : void_type{};
    }
}

namespace meta_hpp::detail
{
    template < array_kind Array >
    struct array_tag {};

    template < array_kind Array >
    array_type_data::array_type_data(type_list<Array>)
    : type_data_base{type_id{type_list<array_tag<Array>>{}}, type_kind::array_}
    , flags{array_traits<Array>::make_flags()}
    , extent{array_traits<Array>::extent}
    , data_type{resolve_type<typename array_traits<Array>::data_type>()} {}
}

namespace meta_hpp
{
    inline array_type::array_type(detail::array_type_data* data)
    : data_{data} {}

    inline bool array_type::is_valid() const noexcept {
        return data_ != nullptr;
    }

    inline array_type::operator bool() const noexcept {
        return is_valid();
    }

    inline type_id array_type::get_id() const noexcept {
        return data_->id;
    }

    inline array_bitflags array_type::get_flags() const noexcept {
        return data_->flags;
    }

    inline const metadata_map& array_type::get_metadata() const noexcept {
        return data_->metadata;
    }

    inline std::size_t array_type::get_extent() const noexcept {
        return data_->extent;
    }

    inline any_type array_type::get_data_type() const noexcept {
        return data_->data_type;
    }
}

namespace meta_hpp::detail
{
    template < nullptr_kind Nullptr >
    struct nullptr_tag {};

    template < nullptr_kind Nullptr >
    nullptr_type_data::nullptr_type_data(type_list<Nullptr>)
    : type_data_base{type_id{type_list<nullptr_tag<Nullptr>>{}}, type_kind::nullptr_} {}
}

namespace meta_hpp
{
    inline nullptr_type::nullptr_type(detail::nullptr_type_data* data)
    : data_{data} {}

    inline bool nullptr_type::is_valid() const noexcept {
        return data_ != nullptr;
    }

    inline nullptr_type::operator bool() const noexcept {
        return is_valid();
    }

    inline type_id nullptr_type::get_id() const noexcept {
        return data_->id;
    }

    inline const metadata_map& nullptr_type::get_metadata() const noexcept {
        return data_->metadata;
    }
}

namespace meta_hpp::detail
{
    template < number_kind Number >
    struct number_tag {};

    template < number_kind Number >
    number_type_data::number_type_data(type_list<Number>)
    : type_data_base{type_id{type_list<number_tag<Number>>{}}, type_kind::number_}
    , flags{number_traits<Number>::make_flags()}
    , size{number_traits<Number>::size}
    , align{number_traits<Number>::align} {}
}

namespace meta_hpp
{
    inline number_type::number_type(detail::number_type_data* data)
    : data_{data} {}

    inline bool number_type::is_valid() const noexcept {
        return data_ != nullptr;
    }

    inline number_type::operator bool() const noexcept {
        return is_valid();
    }

    inline type_id number_type::get_id() const noexcept {
        return data_->id;
    }

    inline number_bitflags number_type::get_flags() const noexcept {
        return data_->flags;
    }

    inline const metadata_map& number_type::get_metadata() const noexcept {
        return data_->metadata;
    }

    inline std::size_t number_type::get_size() const noexcept {
        return data_->size;
    }

    inline std::size_t number_type::get_align() const noexcept {
        return data_->align;
    }
}

namespace meta_hpp::detail
{
    template < reference_kind Reference >
    struct reference_tag {};

    template < reference_kind Reference >
    reference_type_data::reference_type_data(type_list<Reference>)
    : type_data_base{type_id{type_list<reference_tag<Reference>>{}}, type_kind::reference_}
    , flags{reference_traits<Reference>::make_flags()}
    , data_type{resolve_type<typename reference_traits<Reference>::data_type>()} {}
}

namespace meta_hpp
{
    inline reference_type::reference_type(detail::reference_type_data* data)
    : data_{data} {}

    inline bool reference_type::is_valid() const noexcept {
        return data_ != nullptr;
    }

    inline reference_type::operator bool() const noexcept {
        return is_valid();
    }

    inline type_id reference_type::get_id() const noexcept {
        return data_->id;
    }

    inline reference_bitflags reference_type::get_flags() const noexcept {
        return data_->flags;
    }

    inline const metadata_map& reference_type::get_metadata() const noexcept {
        return data_->metadata;
    }

    inline any_type reference_type::get_data_type() const noexcept {
        return data_->data_type;
    }
}

namespace meta_hpp::detail
{
    template < void_kind Void >
    struct void_tag {};

    template < void_kind Void >
    void_type_data::void_type_data(type_list<Void>)
    : type_data_base{type_id{type_list<void_tag<Void>>{}}, type_kind::void_} {}
}

namespace meta_hpp
{
    inline void_type::void_type(detail::void_type_data* data)
    : data_{data} {}

    inline bool void_type::is_valid() const noexcept {
        return data_ != nullptr;
    }

    inline void_type::operator bool() const noexcept {
        return is_valid();
    }

    inline type_id void_type::get_id() const noexcept {
        return data_->id;
    }

    inline const metadata_map& void_type::get_metadata() const noexcept {
        return data_->metadata;
    }
}

namespace meta_hpp
{
    template < typename... Args >
    uvalue invoke(const function& function, Args&&... args) {
        return function.invoke(std::forward<Args>(args)...);
    }

    template < detail::function_kind Function, typename... Args >
    uvalue invoke(Function&& function, Args&&... args) {
        using namespace detail;
        if constexpr ( sizeof...(Args) > 0 ) {
            const std::array<uarg, sizeof...(Args)> vargs{uarg{std::forward<Args>(args)}...};
            return raw_function_invoke<function_policy::as_copy_t>(std::forward<Function>(function), vargs);
        } else {
            return raw_function_invoke<function_policy::as_copy_t>(std::forward<Function>(function), {});
        }
    }
}

namespace meta_hpp
{
    template < typename Instance >
    uvalue invoke(const member& member, Instance&& instance) {
        return member.get(std::forward<Instance>(instance));
    }

    template < detail::member_kind Member, typename Instance >
    uvalue invoke(Member&& member, Instance&& instance) {
        using namespace detail;
        const uinst vinst{std::forward<Instance>(instance)};
        return raw_member_getter<member_policy::as_copy_t>(std::forward<Member>(member), vinst);
    }
}

namespace meta_hpp
{
    template < typename Instance, typename... Args >
    uvalue invoke(const method& method, Instance&& instance, Args&&... args) {
        return method.invoke(std::forward<Instance>(instance), std::forward<Args>(args)...);
    }

    template < detail::method_kind Method, typename Instance, typename... Args >
    uvalue invoke(Method&& method, Instance&& instance, Args&&... args) {
        using namespace detail;
        const uinst vinst{std::forward<Instance>(instance)};
        if constexpr ( sizeof...(Args) > 0 ) {
            const std::array<uarg, sizeof...(Args)> vargs{uarg{std::forward<Args>(args)}...};
            return raw_method_invoke<method_policy::as_copy_t>(std::forward<Method>(method), vinst, vargs);
        } else {
            return raw_method_invoke<method_policy::as_copy_t>(std::forward<Method>(method), vinst, {});
        }
    }
}

namespace meta_hpp
{
    template < typename... Args >
    bool is_invocable_with(const function& function) {
        return function.is_invocable_with<Args...>();
    }

    template < typename... Args >
    bool is_invocable_with(const function& function, Args&&... args) {
        return function.is_invocable_with(std::forward<Args>(args)...);
    }

    template < detail::function_kind Function, typename... Args >
    bool is_invocable_with() {
        if constexpr ( sizeof...(Args) > 0 ) {
            using namespace detail;
            const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{type_list<Args>{}}...};
            return raw_function_is_invocable_with<Function>(vargs);
        } else {
            return raw_function_is_invocable_with<Function>({});
        }
    }

    template < detail::function_kind Function, typename... Args >
    bool is_invocable_with(Args&&... args) {
        if constexpr ( sizeof...(Args) > 0 ) {
            using namespace detail;
            const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{std::forward<Args>(args)}...};
            return raw_function_is_invocable_with<Function>(vargs);
        } else {
            return raw_function_is_invocable_with<Function>({});
        }
    }
}

namespace meta_hpp
{
    template < typename Instance >
    bool is_invocable_with(const member& member) {
        return member.is_gettable_with<Instance>();
    }

    template < typename Instance >
    bool is_invocable_with(const member& member, Instance&& instance) {
        return member.is_gettable_with(std::forward<Instance>(instance));
    }
}

namespace meta_hpp
{
    template < typename Instance, typename... Args >
    bool is_invocable_with(const method& method) {
        return method.is_invocable_with<Instance, Args...>();
    }

    template < typename Instance, typename... Args >
    bool is_invocable_with(const method& method, Instance&& instance, Args&&... args) {
        return method.is_invocable_with(std::forward<Instance>(instance), std::forward<Args>(args)...);
    }
}

namespace meta_hpp
{
    template < detail::member_kind Member, typename Instance >
    bool is_invocable_with() {
        using namespace detail;
        const uinst_base vinst{type_list<Instance>{}};
        return raw_member_is_gettable_with<Member>(vinst);
    }

    template < detail::member_kind Member, typename Instance >
    bool is_invocable_with(Instance&& instance) {
        using namespace detail;
        const uinst_base vinst{std::forward<Instance>(instance)};
        return raw_member_is_gettable_with<Member>(vinst);
    }
}

namespace meta_hpp
{
    template < detail::method_kind Method, typename Instance, typename... Args >
    bool is_invocable_with() {
        using namespace detail;
        const uinst_base vinst{type_list<Instance>{}};
        if constexpr ( sizeof...(Args) > 0 ) {
            const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{type_list<Args>{}}...};
            return raw_method_is_invocable_with<Method>(vinst, vargs);
        } else {
            return raw_method_is_invocable_with<Method>(vinst, {});
        }
    }

    template < detail::method_kind Method, typename Instance, typename... Args >
    bool is_invocable_with(Instance&& instance, Args&&... args) {
        using namespace detail;
        const uinst_base vinst{std::forward<Instance>(instance)};
        if constexpr ( sizeof...(Args) > 0 ) {
            const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{std::forward<Args>(args)}...};
            return raw_method_is_invocable_with<Method>(vinst, vargs);
        } else {
            return raw_method_is_invocable_with<Method>(vinst, {});
        }
    }
}

namespace meta_hpp::detail
{
    template < typename T >
    struct deref_traits;

    template < typename T >
    concept has_deref_traits = requires(const T& v) {
        { deref_traits<T>{}(v) } -> std::convertible_to<uvalue>;
    };
}

namespace meta_hpp::detail
{
    template < typename T >
        requires std::is_copy_constructible_v<T>
    struct deref_traits<T*> {
        uvalue operator()(T* v) const {
            return uvalue{*v};
        }
    };

    template < typename T >
        requires std::is_copy_constructible_v<T>
    struct deref_traits<const T*> {
        uvalue operator()(const T* v) const {
            return uvalue{*v};
        }
    };

    template < typename T >
        requires std::is_copy_constructible_v<T>
    struct deref_traits<std::shared_ptr<T>> {
        uvalue operator()(const std::shared_ptr<T>& v) const {
            return uvalue{*v};
        }
    };

    template < typename T >
        requires std::is_copy_constructible_v<T>
    struct deref_traits<std::unique_ptr<T>> {
        uvalue operator()(const std::unique_ptr<T>& v) const {
            return uvalue{*v};
        }
    };
}

namespace meta_hpp::detail
{
    template < typename T >
    struct index_traits;

    template < typename T >
    concept has_index_traits = requires(const T& v, std::size_t i) {
        { index_traits<T>{}(v, i) } -> std::convertible_to<uvalue>;
    };
}

namespace meta_hpp::detail
{
    template < typename T >
        requires std::is_copy_constructible_v<T>
    struct index_traits<T*> {
        uvalue operator()(T* v, std::size_t i) const {
            // NOLINTNEXTLINE(*-pointer-arithmetic)
            return uvalue{v[i]};
        }
    };

    template < typename T >
        requires std::is_copy_constructible_v<T>
    struct index_traits<const T*> {
        uvalue operator()(const T* v, std::size_t i) const {
            // NOLINTNEXTLINE(*-pointer-arithmetic)
            return uvalue{v[i]};
        }
    };

    template < typename T, std::size_t Size >
        requires std::is_copy_constructible_v<T>
    struct index_traits<std::array<T, Size>> {
        uvalue operator()(const std::array<T, Size>& v, std::size_t i) const {
            return uvalue{v[i]};
        }
    };

    template < typename T, std::size_t Extent >
        requires std::is_copy_constructible_v<T>
    struct index_traits<std::span<T, Extent>> {
        uvalue operator()(const std::span<T, Extent>& v, std::size_t i) const {
            return uvalue{v[i]};
        }
    };

    template < typename T, typename Traits, typename Allocator >
        requires std::is_copy_constructible_v<T>
    struct index_traits<std::basic_string<T, Traits, Allocator>> {
        uvalue operator()(const std::basic_string<T, Traits, Allocator>& v, std::size_t i) const {
            return uvalue{v[i]};
        }
    };

    template < typename T, typename Allocator >
        requires std::is_copy_constructible_v<T>
    struct index_traits<std::vector<T, Allocator>> {
        uvalue operator()(const std::vector<T, Allocator>& v, std::size_t i) {
            return uvalue{v[i]};
        }
    };
}

namespace meta_hpp
{
    struct uvalue::vtable_t final {
        const any_type type;

        void* (*const data)(storage_u& from) noexcept;
        const void* (*const cdata)(const storage_u& from) noexcept;

        void (*const move)(uvalue& from, uvalue& to) noexcept;
        void (*const copy)(const uvalue& from, uvalue& to);
        void (*const destroy)(uvalue& self) noexcept;

        uvalue (*const deref)(const storage_u& from);
        uvalue (*const index)(const storage_u& from, std::size_t);

        template < typename T >
        static T* buffer_cast(buffer_t& buffer) noexcept {
            // NOLINTNEXTLINE(*-reinterpret-cast)
            return std::launder(reinterpret_cast<T*>(buffer.data));
        }

        template < typename T >
        static const T* buffer_cast(const buffer_t& buffer) noexcept {
            // NOLINTNEXTLINE(*-reinterpret-cast)
            return std::launder(reinterpret_cast<const T*>(buffer.data));
        }

        template < typename T >
        static T* storage_cast(storage_u& storage) noexcept {
            return std::visit(detail::overloaded {
                [](void* ptr) { return static_cast<T*>(ptr); },
                [](buffer_t& buffer) { return buffer_cast<T>(buffer); },
                [](...) -> T* { return nullptr; },
            }, storage);
        }

        template < typename T >
        static const T* storage_cast(const storage_u& storage) noexcept {
            return std::visit(detail::overloaded {
                [](const void* ptr) { return static_cast<const T*>(ptr); },
                [](const buffer_t& buffer) { return buffer_cast<T>(buffer); },
                [](...) -> const T* { return nullptr; },
            }, storage);
        }

        template < typename T, typename... Args, typename Tp = std::decay_t<T> >
        static Tp& construct(uvalue& dst, Args&&... args) {
            assert(!dst); // NOLINT

            constexpr bool in_buffer =
                (sizeof(Tp) <= sizeof(buffer_t)) &&
                (alignof(buffer_t) % alignof(Tp) == 0) &&
                std::is_nothrow_move_constructible_v<Tp>;

            if constexpr ( in_buffer ) {
                std::construct_at(
                    buffer_cast<Tp>(dst.storage_.emplace<buffer_t>()),
                    std::forward<Args>(args)...);
            } else {
                dst.storage_.emplace<void*>(
                    std::make_unique<Tp>(std::forward<Args>(args)...).release());
            }

            dst.vtable_ = vtable_t::get<Tp>();
            return *storage_cast<Tp>(dst.storage_);
        }

        static void swap(uvalue& l, uvalue& r) noexcept {
            if ( (&l == &r) || (!l && !r) ) {
                return;
            }

            if ( l && r ) {
                if ( std::holds_alternative<buffer_t>(l.storage_) ) {
                    uvalue temp;
                    r.vtable_->move(r, temp);
                    l.vtable_->move(l, r);
                    temp.vtable_->move(temp, l);
                } else {
                    uvalue temp;
                    l.vtable_->move(l, temp);
                    r.vtable_->move(r, l);
                    temp.vtable_->move(temp, r);
                }
            } else {
                if ( l ) {
                    l.vtable_->move(l, r);
                } else {
                    r.vtable_->move(r, l);
                }
            }
        }

        template < typename Tp >
        // NOLINTNEXTLINE(*-cognitive-complexity)
        static vtable_t* get() {
            static_assert(std::is_same_v<Tp, std::decay_t<Tp>>);

            static vtable_t table{
                .type = resolve_type<Tp>(),

                .data = [](storage_u& from) noexcept -> void* {
                    return storage_cast<Tp>(from);
                },

                .cdata = [](const storage_u& from) noexcept -> const void* {
                    return storage_cast<Tp>(from);
                },

                .move = [](uvalue& from, uvalue& to) noexcept {
                    assert(from && !to); // NOLINT

                    std::visit(detail::overloaded {
                        [&to](void* ptr) {
                            Tp* src = static_cast<Tp*>(ptr);
                            to.storage_.emplace<void*>(src);
                        },
                        [&to](buffer_t& buffer) {
                            Tp& src = *buffer_cast<Tp>(buffer);
                            std::construct_at(buffer_cast<Tp>(to.storage_.emplace<buffer_t>()), std::move(src));
                            std::destroy_at(&src);
                        },
                        [](...){}
                    }, from.storage_);

                    to.vtable_ = from.vtable_;
                    from.vtable_ = nullptr;
                },

                .copy = [](const uvalue& from, uvalue& to){
                    assert(from && !to); // NOLINT

                    std::visit(detail::overloaded {
                        [&to](void* ptr) {
                            const Tp& src = *static_cast<const Tp*>(ptr);
                            to.storage_.emplace<void*>(std::make_unique<Tp>(src).release());
                        },
                        [&to](const buffer_t& buffer) {
                            const Tp& src = *buffer_cast<Tp>(buffer);
                            std::construct_at(buffer_cast<Tp>(to.storage_.emplace<buffer_t>()), src);
                        },
                        [](...){}
                    }, from.storage_);

                    to.vtable_ = from.vtable_;
                },

                .destroy = [](uvalue& self) noexcept {
                    assert(self); // NOLINT

                    std::visit(detail::overloaded {
                        [](void* ptr) {
                            Tp* src = static_cast<Tp*>(ptr);
                            std::unique_ptr<Tp>{src}.reset();
                        },
                        [](buffer_t& buffer) {
                            Tp& src = *buffer_cast<Tp>(buffer);
                            std::destroy_at(&src);
                        },
                        [](...){}
                    }, self.storage_);

                    self.vtable_ = nullptr;
                },

                .deref = +[]([[maybe_unused]] const storage_u& from) -> uvalue {
                    if constexpr ( detail::has_deref_traits<Tp> ) {
                        return detail::deref_traits<Tp>{}(*storage_cast<Tp>(from));
                    } else {
                        detail::throw_exception_with("value type doesn't have value deref traits");
                    }
                },

                .index = +[]([[maybe_unused]] const storage_u& from, [[maybe_unused]] std::size_t i) -> uvalue {
                    if constexpr ( detail::has_index_traits<Tp> ) {
                        return detail::index_traits<Tp>{}(*storage_cast<Tp>(from), i);
                    } else {
                        detail::throw_exception_with("value type doesn't have value index traits");
                    }
                },
            };

            return &table;
        }
    };
}

namespace meta_hpp
{
    inline uvalue::~uvalue() {
        reset();
    }

    inline uvalue::uvalue(uvalue&& other) noexcept {
        if ( other.vtable_ != nullptr ) {
            other.vtable_->move(other, *this);
        }
    }

    inline uvalue::uvalue(const uvalue& other) {
        if ( other.vtable_ != nullptr ) {
            other.vtable_->copy(other, *this);
        }
    }

    inline uvalue& uvalue::operator=(uvalue&& other) noexcept {
        if ( this != &other ) {
            uvalue{std::move(other)}.swap(*this);
        }
        return *this;
    }

    inline uvalue& uvalue::operator=(const uvalue& other) {
        if ( this != &other ) {
            uvalue{other}.swap(*this);
        }
        return *this;
    }

    template < typename T, typename Tp >
        requires (!detail::any_uvalue_kind<Tp>)
              && (!detail::is_in_place_type_v<Tp>)
              && (std::is_copy_constructible_v<Tp>)
    // NOLINTNEXTLINE(*-forwarding-reference-overload)
    uvalue::uvalue(T&& val) {
        vtable_t::construct<T>(*this, std::forward<T>(val));
    }

    template < typename T, typename Tp >
        requires (!detail::any_uvalue_kind<Tp>)
              && (!detail::is_in_place_type_v<Tp>)
              && (std::is_copy_constructible_v<Tp>)
    uvalue& uvalue::operator=(T&& val) {
        uvalue{std::forward<T>(val)}.swap(*this);
        return *this;
    }

    template < typename T, typename... Args, typename Tp >
        requires std::is_copy_constructible_v<Tp>
              && std::is_constructible_v<Tp, Args...>
    uvalue::uvalue(std::in_place_type_t<T>, Args&&... args) {
        vtable_t::construct<T>(*this, std::forward<Args>(args)...);
    }

    template < typename T, typename U, typename... Args, typename Tp >
        requires std::is_copy_constructible_v<Tp>
              && std::is_constructible_v<Tp, std::initializer_list<U>&, Args...>
    uvalue::uvalue(std::in_place_type_t<T>, std::initializer_list<U> ilist, Args&&... args) {
        vtable_t::construct<T>(*this, ilist, std::forward<Args>(args)...);
    }

    template < typename T, typename... Args, typename Tp >
        requires std::is_copy_constructible_v<Tp>
              && std::is_constructible_v<Tp, Args...>
    std::decay_t<T>& uvalue::emplace(Args&&... args) {
        reset();
        return vtable_t::construct<T>(*this, std::forward<Args>(args)...);
    }

    template < typename T, typename U, typename... Args, typename Tp >
        requires std::is_copy_constructible_v<Tp>
              && std::is_constructible_v<Tp, std::initializer_list<U>&, Args...>
    std::decay_t<T>& uvalue::emplace(std::initializer_list<U> ilist, Args&&... args) {
        reset();
        return vtable_t::construct<T>(*this, ilist, std::forward<Args>(args)...);
    }

    inline bool uvalue::is_valid() const noexcept {
        return vtable_ != nullptr;
    }

    inline uvalue::operator bool() const noexcept {
        return is_valid();
    }

    inline void uvalue::reset() {
        if ( vtable_ != nullptr ) {
            vtable_->destroy(*this);
        }
    }

    inline void uvalue::swap(uvalue& other) noexcept {
        vtable_t::swap(*this, other);
    }

    inline const any_type& uvalue::get_type() const noexcept {
        static any_type void_type = resolve_type<void>();
        return vtable_ != nullptr ? vtable_->type : void_type;
    }

    inline void* uvalue::data() noexcept {
        return vtable_ != nullptr ? vtable_->data(storage_) : nullptr;
    }

    inline const void* uvalue::data() const noexcept {
        return vtable_ != nullptr ? vtable_->cdata(storage_) : nullptr;
    }

    inline const void* uvalue::cdata() const noexcept {
        return vtable_ != nullptr ? vtable_->cdata(storage_) : nullptr;
    }

    inline uvalue uvalue::operator*() const {
        return vtable_ != nullptr ? vtable_->deref(storage_) : uvalue{};
    }

    inline uvalue uvalue::operator[](std::size_t index) const {
        return vtable_ != nullptr ? vtable_->index(storage_, index) : uvalue{};
    }

    template < typename T >
    [[nodiscard]] T uvalue::get_as() && {
        static_assert(std::is_same_v<T, std::decay_t<T>>);

        if constexpr ( detail::pointer_kind<T> ) {
            if ( T ptr = try_get_as<T>(); ptr || get_type().is_nullptr() ) {
                return ptr;
            }
        } else {
            if ( T* ptr = try_get_as<T>() ) {
                return std::move(*ptr);
            }
        }

        detail::throw_exception_with("bad value cast");
    }

    template < typename T >
    auto uvalue::get_as() & -> std::conditional_t<detail::pointer_kind<T>, T, T&> {
        static_assert(std::is_same_v<T, std::decay_t<T>>);

        if constexpr ( detail::pointer_kind<T> ) {
            if ( T ptr = try_get_as<T>(); ptr || get_type().is_nullptr() ) {
                return ptr;
            }
        } else {
            if ( T* ptr = try_get_as<T>() ) {
                return *ptr;
            }
        }

        detail::throw_exception_with("bad value cast");
    }

    template < typename T >
    auto uvalue::get_as() const & -> std::conditional_t<detail::pointer_kind<T>, T, const T&> {
        static_assert(std::is_same_v<T, std::decay_t<T>>);

        if constexpr ( detail::pointer_kind<T> ) {
            if ( T ptr = try_get_as<T>(); ptr || get_type().is_nullptr() ) {
                return ptr;
            }
        } else {
            if ( const T* ptr = try_get_as<T>() ) {
                return *ptr;
            }
        }

        detail::throw_exception_with("bad value cast");
    }

    template < typename T >
    // NOLINTNEXTLINE(*-cognitive-complexity)
    auto uvalue::try_get_as() noexcept -> std::conditional_t<detail::pointer_kind<T>, T, T*> {
        static_assert(std::is_same_v<T, std::decay_t<T>>);

        const any_type& from_type = get_type();
        const any_type& to_type = resolve_type<T>();

        const auto is_a = [](const any_type& base, const any_type& derived){
            return (base == derived)
                || (base.is_class() && derived.is_class() && base.as_class().is_base_of(derived.as_class()));
        };

        if constexpr ( detail::pointer_kind<T> ) {
            if ( to_type.is_pointer() && from_type.is_nullptr() ) {
                return static_cast<T>(nullptr);
            }

            if ( to_type.is_pointer() && from_type.is_pointer() ) {
                const pointer_type& to_type_ptr = to_type.as_pointer();
                const bool to_type_ptr_readonly = to_type_ptr.get_flags().has(pointer_flags::is_readonly);

                const pointer_type& from_type_ptr = from_type.as_pointer();
                const bool from_type_ptr_readonly = from_type_ptr.get_flags().has(pointer_flags::is_readonly);

                const any_type& to_data_type = to_type_ptr.get_data_type();
                const any_type& from_data_type = from_type_ptr.get_data_type();

                if ( to_type_ptr_readonly >= from_type_ptr_readonly ) {
                    void** from_data_ptr = static_cast<void**>(data());

                    if ( to_data_type.is_void() || to_data_type == from_data_type ) {
                        void* to_ptr = *from_data_ptr;
                        return static_cast<T>(to_ptr);
                    }

                    if ( is_a(to_data_type, from_data_type) ) {
                        const class_type& to_data_class = to_data_type.as_class();
                        const class_type& from_data_class = from_data_type.as_class();

                        void* to_ptr = detail::pointer_upcast(*from_data_ptr, from_data_class, to_data_class);
                        return static_cast<T>(to_ptr);
                    }
                }
            }
        }

        if constexpr ( !detail::pointer_kind<T> ) {
            if ( from_type == to_type ) {
                T* to_ptr = static_cast<T*>(data());
                return to_ptr;
            }

            if ( is_a(to_type, from_type) ) {
                const class_type& to_class = to_type.as_class();
                const class_type& from_class = from_type.as_class();

                T* to_ptr = static_cast<T*>(detail::pointer_upcast(data(), from_class, to_class));
                return to_ptr;
            }
        }

        return nullptr;
    }

    template < typename T >
    // NOLINTNEXTLINE(*-cognitive-complexity)
    auto uvalue::try_get_as() const noexcept -> std::conditional_t<detail::pointer_kind<T>, T, const T*> {
        static_assert(std::is_same_v<T, std::decay_t<T>>);

        const any_type& from_type = get_type();
        const any_type& to_type = resolve_type<T>();

        const auto is_a = [](const any_type& base, const any_type& derived){
            return (base == derived)
                || (base.is_class() && derived.is_class() && base.as_class().is_base_of(derived.as_class()));
        };

        if constexpr ( detail::pointer_kind<T> ) {
            if ( to_type.is_pointer() && from_type.is_nullptr() ) {
                return static_cast<T>(nullptr);
            }

            if ( to_type.is_pointer() && from_type.is_pointer() ) {
                const pointer_type& to_type_ptr = to_type.as_pointer();
                const bool to_type_ptr_readonly = to_type_ptr.get_flags().has(pointer_flags::is_readonly);

                const pointer_type& from_type_ptr = from_type.as_pointer();
                const bool from_type_ptr_readonly = from_type_ptr.get_flags().has(pointer_flags::is_readonly);

                const any_type& to_data_type = to_type_ptr.get_data_type();
                const any_type& from_data_type = from_type_ptr.get_data_type();

                if ( to_type_ptr_readonly >= from_type_ptr_readonly ) {
                    void* const* from_data_ptr = static_cast<void* const*>(data());

                    if ( to_data_type.is_void() || to_data_type == from_data_type ) {
                        void* to_ptr = *from_data_ptr;
                        return static_cast<T>(to_ptr);
                    }

                    if ( is_a(to_data_type, from_data_type) ) {
                        const class_type& to_data_class = to_data_type.as_class();
                        const class_type& from_data_class = from_data_type.as_class();

                        void* to_ptr = detail::pointer_upcast(*from_data_ptr, from_data_class, to_data_class);
                        return static_cast<T>(to_ptr);
                    }
                }
            }
        }

        if constexpr ( !detail::pointer_kind<T> ) {
            if ( from_type == to_type ) {
                const T* to_ptr = static_cast<const T*>(data());
                return to_ptr;
            }

            if ( is_a(to_type, from_type) ) {
                const class_type& to_class = to_type.as_class();
                const class_type& from_class = from_type.as_class();

                const T* to_ptr = static_cast<const T*>(detail::pointer_upcast(data(), from_class, to_class));
                return to_ptr;
            }
        }

        return nullptr;
    }
}
