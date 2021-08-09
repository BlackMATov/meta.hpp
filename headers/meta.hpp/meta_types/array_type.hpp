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
    struct array_traits {
        static_assert(std::is_array_v<T>);
        static constexpr std::size_t extent{std::extent_v<T>};

        using data_type = std::remove_extent_t<T>;

        static any_type make_data_type() {
            return type_db::get<data_type>();
        }

        static bitflags<array_flags> make_flags() noexcept {
            bitflags<array_flags> flags;
            if ( stdex::is_bounded_array_v<T> ) flags.set(array_flags::is_bounded);
            if ( stdex::is_unbounded_array_v<T> ) flags.set(array_flags::is_unbounded);
            return flags;
        }
    };
}

namespace meta_hpp
{
    template < typename T >
    array_type::array_type(typename_arg_t<T>)
    : type_base{typename_arg<struct array_type_tag, T>}
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
