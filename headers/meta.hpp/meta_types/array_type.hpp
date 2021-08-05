/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "_types_fwd.hpp"

namespace meta_hpp
{
    class array_type final : public base_type {
    public:
        array_type() = default;

        array_type(array_type&&) = default;
        array_type& operator=(array_type&&) = default;

        array_type(const array_type&) = default;
        array_type& operator=(const array_type&) = default;

        template < typename T >
        explicit array_type(typename_arg_t<T>);

        any_type data_type() const noexcept;
        std::size_t extent() const noexcept;
        bool is_bounded() const noexcept;
        bool is_unbounded() const noexcept;
    private:
        struct state;
        std::shared_ptr<state> state_;
    };
}

namespace meta_hpp
{
    struct array_type::state final {
        const any_type data_type;
        const std::size_t extent;
        const bool is_bounded;
        const bool is_unbounded;
    };
}

namespace meta_hpp::detail
{
    template < typename T >
    struct array_traits;

    template < typename T >
    struct array_traits<T[]> {
        using data_type = T;
        static constexpr std::size_t extent{0};
        static constexpr bool is_bounded{false};
        static constexpr bool is_unbounded{true};
    };

    template < typename T, std::size_t N >
    struct array_traits<T[N]> {
        using data_type = T;
        static constexpr std::size_t extent{N};
        static constexpr bool is_bounded{true};
        static constexpr bool is_unbounded{false};
    };
}

namespace meta_hpp
{
    template < typename T >
    inline array_type::array_type(typename_arg_t<T>)
    : base_type{typename_arg<T>}
    , state_{std::make_shared<state>(state{
        type_db::get<typename detail::array_traits<T>::data_type>(),
        detail::array_traits<T>::extent,
        detail::array_traits<T>::is_bounded,
        detail::array_traits<T>::is_unbounded,
    })} {
        static_assert(std::is_array_v<T>);
    }

    inline any_type array_type::data_type() const noexcept {
        return state_->data_type;
    }

    inline std::size_t array_type::extent() const noexcept {
        return state_->extent;
    }

    inline bool array_type::is_bounded() const noexcept {
        return state_->is_bounded;
    }

    inline bool array_type::is_unbounded() const noexcept {
        return state_->is_unbounded;
    }
}
