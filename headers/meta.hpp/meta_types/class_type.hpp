/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "_types_fwd.hpp"

namespace meta_hpp
{
    enum class class_flags : unsigned {
        is_const = 1 << 0,
        is_empty = 1 << 1,
        is_final = 1 << 2,
        is_abstract = 1 << 3,
        is_polymorphic = 1 << 4,
    };

    ENUM_HPP_OPERATORS_DECL(class_flags)

    class class_type final : public base_type {
    public:
        class_type() = default;

        class_type(class_type&&) = default;
        class_type& operator=(class_type&&) = default;

        class_type(const class_type&) = default;
        class_type& operator=(const class_type&) = default;

        template < typename T >
        explicit class_type(typename_arg_t<T>);

        std::size_t size() const noexcept;
        any_type raw_type() const noexcept;

        bitflags<class_flags> flags() const noexcept;
        bool is_const() const noexcept;
        bool is_empty() const noexcept;
        bool is_final() const noexcept;
        bool is_abstract() const noexcept;
        bool is_polymorphic() const noexcept;
    private:
        struct state;
        std::shared_ptr<state> state_;
    };
}

namespace meta_hpp::detail
{
    template < typename T, typename = void >
    struct class_traits;

    template < typename T >
    struct class_traits<T, std::enable_if_t<std::is_class_v<T>>> {
        static constexpr std::size_t size{sizeof(T)};

        static any_type make_raw_type(const class_type& self) {
            using raw_type = std::remove_const_t<T>;
            return std::is_same_v<T, raw_type>
                ? any_type{self}
                : type_db::get<raw_type>();
        }

        static bitflags<class_flags> make_flags() noexcept {
            bitflags<class_flags> flags;
            if ( std::is_const_v<T> ) flags.set(class_flags::is_const);
            if ( std::is_empty_v<T> ) flags.set(class_flags::is_empty);
            if ( std::is_final_v<T> ) flags.set(class_flags::is_final);
            if ( std::is_abstract_v<T> ) flags.set(class_flags::is_abstract);
            if ( std::is_polymorphic_v<T> ) flags.set(class_flags::is_polymorphic);
            return flags;
        }
    };
}

namespace meta_hpp
{
    struct class_type::state final {
        const std::size_t size;
        const any_type raw_type;
        const bitflags<class_flags> flags;
    };

    template < typename T >
    inline class_type::class_type(typename_arg_t<T>)
    : base_type{typename_arg<T>}
    , state_{std::make_shared<state>(state{
        detail::class_traits<T>::size,
        detail::class_traits<T>::make_raw_type(*this),
        detail::class_traits<T>::make_flags(),
    })} {
        static_assert(std::is_class_v<T>);
    }

    inline std::size_t class_type::size() const noexcept {
        return state_->size;
    }

    inline any_type class_type::raw_type() const noexcept {
        return state_->raw_type;
    }

    inline bitflags<class_flags> class_type::flags() const noexcept {
        return state_->flags;
    }

    inline bool class_type::is_const() const noexcept {
        return state_->flags.has(class_flags::is_const);
    }

    inline bool class_type::is_empty() const noexcept {
        return state_->flags.has(class_flags::is_empty);
    }

    inline bool class_type::is_final() const noexcept {
        return state_->flags.has(class_flags::is_final);
    }

    inline bool class_type::is_abstract() const noexcept {
        return state_->flags.has(class_flags::is_abstract);
    }

    inline bool class_type::is_polymorphic() const noexcept {
        return state_->flags.has(class_flags::is_polymorphic);
    }
}
