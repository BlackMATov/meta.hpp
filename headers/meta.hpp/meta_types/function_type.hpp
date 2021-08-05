/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "_types_fwd.hpp"

namespace meta_hpp
{
    class function_type final : public base_type {
    public:
        function_type() = default;

        function_type(function_type&&) = default;
        function_type& operator=(function_type&&) = default;

        function_type(const function_type&) = default;
        function_type& operator=(const function_type&) = default;

        template < typename T >
        explicit function_type(typename_arg_t<T>);

        any_type return_type() const noexcept;
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
    struct function_type::state final {
        const any_type return_type;
        const std::vector<any_type> argument_types;
        const std::size_t arity;
        const bool is_noexcept;
    };
}

namespace meta_hpp::detail
{
    template < typename T >
    struct function_pointer_traits;

    template < typename R, typename... Args >
    struct function_pointer_traits<R(*)(Args...)> {
        using return_type = R;
        using argument_types = std::tuple<Args...>;
        static constexpr std::size_t arity{sizeof...(Args)};
        static constexpr bool is_noexcept{false};
    };

    template < typename R, typename... Args >
    struct function_pointer_traits<R(*)(Args...) noexcept> : function_pointer_traits<R(*)(Args...)> {
        static constexpr bool is_noexcept{true};
    };
}

namespace meta_hpp
{
    template < typename T >
    inline function_type::function_type(typename_arg_t<T>)
    : base_type{typename_arg<T>}
    , state_{std::make_shared<state>(state{
        type_db::get<typename detail::function_pointer_traits<T>::return_type>(),
        type_db::multi_get<typename detail::function_pointer_traits<T>::argument_types>(),
        detail::function_pointer_traits<T>::arity,
        detail::function_pointer_traits<T>::is_noexcept,
    })} {
        static_assert(std::is_pointer_v<T>);
        static_assert(std::is_function_v<std::remove_pointer_t<T>>);
    }

    inline any_type function_type::return_type() const noexcept {
        return state_->return_type;
    }

    inline any_type function_type::argument_type(std::size_t i) const noexcept {
        return i < state_->argument_types.size()
            ? state_->argument_types[i]
            : any_type{};
    }

    inline const std::vector<any_type>& function_type::argument_types() const noexcept {
        return state_->argument_types;
    }

    inline std::size_t function_type::arity() const noexcept {
        return state_->arity;
    }

    inline bool function_type::is_noexcept() const noexcept {
        return state_->is_noexcept;
    }
}
