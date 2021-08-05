/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "_types_fwd.hpp"

namespace meta_hpp
{
    class pointer_type final : public base_type {
    public:
        pointer_type() = default;

        pointer_type(pointer_type&&) = default;
        pointer_type& operator=(pointer_type&&) = default;

        pointer_type(const pointer_type&) = default;
        pointer_type& operator=(const pointer_type&) = default;

        template < typename T >
        explicit pointer_type(typename_arg_t<T>);

        any_type data_type() const noexcept;
        bool is_const() const noexcept;
    private:
        struct state;
        std::shared_ptr<state> state_;
    };
}

namespace meta_hpp
{
    struct pointer_type::state final {
        const any_type data_type;
        const bool is_const;
    };
}

namespace meta_hpp::detail
{
    template < typename T >
    struct pointer_traits;

    template < typename T >
    struct pointer_traits<T*> {
        using data_type = T;
        static constexpr bool is_const{false};
    };

    template < typename T >
    struct pointer_traits<T* const> {
        using data_type = T;
        static constexpr bool is_const{true};
    };
}

namespace meta_hpp
{
    template < typename T >
    inline pointer_type::pointer_type(typename_arg_t<T>)
    : base_type{typename_arg<T>}
    , state_{std::make_shared<state>(state{
        type_db::get<typename detail::pointer_traits<T>::data_type>(),
        detail::pointer_traits<T>::is_const,
    })} {
        static_assert(std::is_pointer_v<T>);
        static_assert(!std::is_function_v<std::remove_pointer_t<T>>);
    }

    inline any_type pointer_type::data_type() const noexcept {
        return state_->data_type;
    }

    inline bool pointer_type::is_const() const noexcept {
        return state_->is_const;
    }
}
