/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "_types_fwd.hpp"

namespace meta_hpp
{
    class method_type final : public base_type {
    public:
        method_type() = default;

        method_type(method_type&&) = default;
        method_type& operator=(method_type&&) = default;

        method_type(const method_type&) = default;
        method_type& operator=(const method_type&) = default;

        template < typename T >
        explicit method_type(typename_arg_t<T>);

        any_type class_type() const noexcept;
        any_type return_type() const noexcept;
        any_type argument_type(std::size_t i) const noexcept;
        const std::vector<any_type>& argument_types() const noexcept;
        std::size_t arity() const noexcept;
        bool is_const() const noexcept;
        bool is_noexcept() const noexcept;
    private:
        struct state;
        std::shared_ptr<state> state_;
    };
}

namespace meta_hpp
{
    struct method_type::state final {
        const any_type class_type;
        const any_type return_type;
        const std::vector<any_type> argument_types;
        const std::size_t arity;
        const bool is_const;
        const bool is_noexcept;
    };
}

namespace meta_hpp::detail
{
    template < typename T >
    struct method_pointer_traits;

    template < typename R, typename C, typename... Args >
    struct method_pointer_traits<R(C::*)(Args...)> {
        using class_type = C;
        using return_type = R;
        using argument_types = std::tuple<Args...>;
        static constexpr std::size_t arity{sizeof...(Args)};
        static constexpr bool is_const{false};
        static constexpr bool is_noexcept{false};
    };

    template < typename R, typename C, typename... Args >
    struct method_pointer_traits<R(C::*)(Args...) const> : method_pointer_traits<R(C::*)(Args...)> {
        static constexpr bool is_const{true};
    };

    template < typename R, typename C, typename... Args >
    struct method_pointer_traits<R(C::*)(Args...) noexcept> : method_pointer_traits<R(C::*)(Args...)> {
        static constexpr bool is_noexcept{true};
    };

    template < typename R, typename C, typename... Args >
    struct method_pointer_traits<R(C::*)(Args...) const noexcept> : method_pointer_traits<R(C::*)(Args...)> {
        static constexpr bool is_const{true};
        static constexpr bool is_noexcept{true};
    };
}

namespace meta_hpp
{
    template < typename T >
    inline method_type::method_type(typename_arg_t<T>)
    : base_type{typename_arg<T>}
    , state_{std::make_shared<state>(state{
        type_db::get<typename detail::method_pointer_traits<T>::class_type>(),
        type_db::get<typename detail::method_pointer_traits<T>::return_type>(),
        type_db::multi_get<typename detail::method_pointer_traits<T>::argument_types>(),
        detail::method_pointer_traits<T>::arity,
        detail::method_pointer_traits<T>::is_const,
        detail::method_pointer_traits<T>::is_noexcept,
    })} {
        static_assert(std::is_member_function_pointer_v<T>);
    }

    inline any_type method_type::class_type() const noexcept {
        return state_->class_type;
    }

    inline any_type method_type::return_type() const noexcept {
        return state_->return_type;
    }

    inline any_type method_type::argument_type(std::size_t i) const noexcept {
        return i < state_->argument_types.size()
            ? state_->argument_types[i]
            : any_type{};
    }

    inline const std::vector<any_type>& method_type::argument_types() const noexcept {
        return state_->argument_types;
    }

    inline std::size_t method_type::arity() const noexcept {
        return state_->arity;
    }

    inline bool method_type::is_const() const noexcept {
        return state_->is_const;
    }

    inline bool method_type::is_noexcept() const noexcept {
        return state_->is_noexcept;
    }
}
