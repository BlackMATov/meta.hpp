/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "_types_fwd.hpp"

namespace meta_hpp
{
    enum class ctor_flags : unsigned {
        is_noexcept = 1 << 0,
    };

    ENUM_HPP_OPERATORS_DECL(ctor_flags)

    class ctor_type final : public type_base {
    public:
        ctor_type() = default;

        ctor_type(ctor_type&&) = default;
        ctor_type& operator=(ctor_type&&) = default;

        ctor_type(const ctor_type&) = default;
        ctor_type& operator=(const ctor_type&) = default;

        template < typename Class, typename... Args >
        explicit ctor_type(typename_arg_t<Class>, typename_arg_t<Args...>);

        std::size_t arity() const noexcept;
        any_type class_type() const noexcept;
        any_type argument_type(std::size_t i) const noexcept;
        const std::vector<any_type>& argument_types() const noexcept;

        bitflags<ctor_flags> flags() const noexcept;
        bool is_noexcept() const noexcept;
    private:
        struct state;
        std::shared_ptr<state> state_;
    };
}

namespace meta_hpp::detail
{
    template < typename Class, typename... Args >
    struct ctor_traits {
        static_assert(std::is_constructible_v<Class, Args...>);
        static constexpr std::size_t arity{sizeof...(Args)};

        using class_type = Class;
        using argument_types = std::tuple<Args...>;

        static any_type make_class_type() {
            return type_db::get<class_type>();
        }

        static std::vector<any_type> make_argument_types() {
            return type_db::multi_get<argument_types>();
        }

        static bitflags<ctor_flags> make_flags() noexcept {
            bitflags<ctor_flags> flags;
            if ( std::is_nothrow_constructible_v<Class, Args...> ) flags.set(ctor_flags::is_noexcept);
            return flags;
        }
    };
}

namespace meta_hpp
{
    struct ctor_type::state final {
        const std::size_t arity;
        const any_type class_type;
        const std::vector<any_type> argument_types;
        const bitflags<ctor_flags> flags;
    };

    template < typename Class, typename... Args >
    ctor_type::ctor_type(typename_arg_t<Class>, typename_arg_t<Args...>)
    : type_base{typename_arg<struct ctor_type_tag, Class, Args...>}
    , state_{std::make_shared<state>(state{
        detail::ctor_traits<Class, Args...>::arity,
        detail::ctor_traits<Class, Args...>::make_class_type(),
        detail::ctor_traits<Class, Args...>::make_argument_types(),
        detail::ctor_traits<Class, Args...>::make_flags(),
    })} {
        static_assert(std::is_class_v<Class>);
        static_assert(std::is_constructible_v<Class, Args...>);
    }

    inline std::size_t ctor_type::arity() const noexcept {
        return state_->arity;
    }

    inline any_type ctor_type::class_type() const noexcept {
        return state_->class_type;
    }

    inline any_type ctor_type::argument_type(std::size_t i) const noexcept {
        return i < state_->argument_types.size()
            ? state_->argument_types[i]
            : any_type{};
    }

    inline const std::vector<any_type>& ctor_type::argument_types() const noexcept {
        return state_->argument_types;
    }

    inline bitflags<ctor_flags> ctor_type::flags() const noexcept {
        return state_->flags;
    }

    inline bool ctor_type::is_noexcept() const noexcept {
        return state_->flags.has(ctor_flags::is_noexcept);
    }
}
