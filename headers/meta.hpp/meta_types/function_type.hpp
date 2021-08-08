/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "_types_fwd.hpp"

namespace meta_hpp
{
    enum class function_flags : unsigned {
        is_noexcept = 1 << 0,
    };

    ENUM_HPP_OPERATORS_DECL(function_flags)

    class function_type final : public type_base {
    public:
        function_type() = default;

        function_type(function_type&&) = default;
        function_type& operator=(function_type&&) = default;

        function_type(const function_type&) = default;
        function_type& operator=(const function_type&) = default;

        template < typename T >
        explicit function_type(typename_arg_t<T>);

        std::size_t arity() const noexcept;
        any_type return_type() const noexcept;
        any_type argument_type(std::size_t i) const noexcept;
        const std::vector<any_type>& argument_types() const noexcept;

        bitflags<function_flags> flags() const noexcept;
        bool is_noexcept() const noexcept;
    private:
        struct state;
        std::shared_ptr<state> state_;
    };
}

namespace meta_hpp::detail
{
    template < typename T >
    struct function_pointer_traits;

    template < typename R, typename... Args >
    struct function_pointer_traits<R(*)(Args...)> {
        static constexpr std::size_t arity{sizeof...(Args)};

        static any_type make_return_type() {
            using return_type = R;
            return type_db::get<return_type>();
        }

        static std::vector<any_type> make_argument_types() {
            using argument_types = std::tuple<Args...>;
            return type_db::multi_get<argument_types>();
        }

        static bitflags<function_flags> make_flags() noexcept {
            return bitflags<function_flags>{};
        }
    };

    template < typename R, typename... Args >
    struct function_pointer_traits<R(*)(Args...) noexcept> : function_pointer_traits<R(*)(Args...)> {
        static bitflags<function_flags> make_flags() noexcept {
            return function_flags::is_noexcept;
        }
    };
}

namespace meta_hpp
{
    struct function_type::state final {
        const std::size_t arity;
        const any_type return_type;
        const std::vector<any_type> argument_types;
        const bitflags<function_flags> flags;
    };

    template < typename T >
    function_type::function_type(typename_arg_t<T>)
    : type_base{typename_arg<struct function_type_tag, T>}
    , state_{std::make_shared<state>(state{
        detail::function_pointer_traits<T>::arity,
        detail::function_pointer_traits<T>::make_return_type(),
        detail::function_pointer_traits<T>::make_argument_types(),
        detail::function_pointer_traits<T>::make_flags(),
    })} {
        static_assert(std::is_pointer_v<T>);
        static_assert(std::is_function_v<std::remove_pointer_t<T>>);
    }

    inline std::size_t function_type::arity() const noexcept {
        return state_->arity;
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

    inline bitflags<function_flags> function_type::flags() const noexcept {
        return state_->flags;
    }

    inline bool function_type::is_noexcept() const noexcept {
        return state_->flags.has(function_flags::is_noexcept);
    }
}
