/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "_types_fwd.hpp"

namespace meta_hpp
{
    class ctor_type final : public base_type {
    public:
        ctor_type() = default;

        ctor_type(ctor_type&&) = default;
        ctor_type& operator=(ctor_type&&) = default;

        ctor_type(const ctor_type&) = default;
        ctor_type& operator=(const ctor_type&) = default;

        template < typename Class, typename... Args >
        explicit ctor_type(typename_arg_t<Class>, typename_arg_t<Args...>);

        any_type class_type() const noexcept;
        any_type argument_type(std::size_t i) const noexcept;
        const std::vector<any_type>& argument_types() const noexcept;
        std::size_t arity() const noexcept;
        bool is_noexcept() const noexcept;
    private:
        struct state;
        std::shared_ptr<state> state_;
    };
}

namespace meta_hpp
{
    struct ctor_type::state final {
        const any_type class_type;
        const std::vector<any_type> argument_types;
        const std::size_t arity;
        const bool is_noexcept;
    };
}

namespace meta_hpp::detail
{
    template < typename C, typename... Args >
    struct ctor_traits {
        static_assert(std::is_constructible_v<C, Args...>);
        using class_type = C;
        using argument_types = std::tuple<Args...>;
        static constexpr std::size_t arity{sizeof...(Args)};
        static constexpr bool is_noexcept{std::is_nothrow_constructible_v<C, Args...>};
    };
}

namespace meta_hpp
{
    template < typename Class, typename... Args >
    inline ctor_type::ctor_type(typename_arg_t<Class>, typename_arg_t<Args...>)
    : base_type{typename_arg<Class, Args...>}
    , state_{std::make_shared<state>(state{
        type_db::get<typename detail::ctor_traits<Class, Args...>::class_type>(),
        type_db::multi_get<typename detail::ctor_traits<Class, Args...>::argument_types>(),
        detail::ctor_traits<Class, Args...>::arity,
        detail::ctor_traits<Class, Args...>::is_noexcept,
    })} {
        static_assert(std::is_class_v<Class>);
        static_assert(std::is_constructible_v<Class, Args...>);
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

    inline std::size_t ctor_type::arity() const noexcept {
        return state_->arity;
    }

    inline bool ctor_type::is_noexcept() const noexcept {
        return state_->is_noexcept;
    }
}
