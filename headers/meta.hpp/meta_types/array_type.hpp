/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "_types_fwd.hpp"

namespace meta_hpp
{
    enum class array_flags : unsigned {
        is_bounded = 1 << 0,
        is_unbounded = 1 << 1,
    };

    ENUM_HPP_OPERATORS_DECL(array_flags)

    class array_type final : public type_base {
    public:
        array_type() = default;

        array_type(array_type&&) = default;
        array_type& operator=(array_type&&) = default;

        array_type(const array_type&) = default;
        array_type& operator=(const array_type&) = default;

        template < typename T >
        explicit array_type(typename_arg_t<T>);

        std::size_t extent() const noexcept;
        any_type data_type() const noexcept;

        bitflags<array_flags> flags() const noexcept;
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
        const std::size_t extent;
        const any_type data_type;
        const bitflags<array_flags> flags;
    };
}

namespace meta_hpp::detail
{
    template < typename T >
    struct array_traits;

    template < typename T >
    struct array_traits<T[]> {
        static constexpr std::size_t extent{0};

        static any_type make_data_type() {
            using data_type = T;
            return type_db::get<data_type>();
        }

        static bitflags<array_flags> make_flags() noexcept {
            return array_flags::is_unbounded;
        }
    };

    template < typename T, std::size_t N >
    struct array_traits<T[N]> {
        static constexpr std::size_t extent{N};

        static any_type make_data_type() {
            using data_type = T;
            return type_db::get<data_type>();
        }

        static bitflags<array_flags> make_flags() noexcept {
            return array_flags::is_bounded;
        }
    };
}

namespace meta_hpp
{
    template < typename T >
    inline array_type::array_type(typename_arg_t<T>)
    : type_base{typename_arg<T>}
    , state_{std::make_shared<state>(state{
        detail::array_traits<T>::extent,
        detail::array_traits<T>::make_data_type(),
        detail::array_traits<T>::make_flags(),
    })} {
        static_assert(std::is_array_v<T>);
    }

    inline std::size_t array_type::extent() const noexcept {
        return state_->extent;
    }

    inline any_type array_type::data_type() const noexcept {
        return state_->data_type;
    }

    inline bitflags<array_flags> array_type::flags() const noexcept {
        return state_->flags;
    }

    inline bool array_type::is_bounded() const noexcept {
        return state_->flags.has(array_flags::is_bounded);
    }

    inline bool array_type::is_unbounded() const noexcept {
        return state_->flags.has(array_flags::is_unbounded);
    }
}
