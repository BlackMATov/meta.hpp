/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "_types_fwd.hpp"

namespace meta_hpp
{
    class class_type final : public base_type {
    public:
        class_type() = default;

        class_type(class_type&&) = default;
        class_type& operator=(class_type&&) = default;

        class_type(const class_type&) = default;
        class_type& operator=(const class_type&) = default;

        template < typename T >
        explicit class_type(typename_arg_t<T>);

        any_type raw_type() const noexcept;
        std::size_t size() const noexcept;
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

namespace meta_hpp
{
    struct class_type::state final {
        const any_type raw_type;
        const std::size_t size;
        const bool is_const;
        const bool is_empty;
        const bool is_final;
        const bool is_abstract;
        const bool is_polymorphic;
    };
}

namespace meta_hpp::detail
{
    template < typename T, typename = void >
    struct class_traits;

    template < typename T >
    struct class_traits<T, std::enable_if_t<std::is_class_v<T>>> {
        using raw_type = std::remove_const_t<T>;
        static constexpr std::size_t size{sizeof(T)};
        static constexpr bool is_const{std::is_const_v<T>};
        static constexpr bool is_empty{std::is_empty_v<T>};
        static constexpr bool is_final{std::is_final_v<T>};
        static constexpr bool is_abstract{std::is_abstract_v<T>};
        static constexpr bool is_polymorphic{std::is_polymorphic_v<T>};
    };
}

namespace meta_hpp
{
    template < typename T >
    inline class_type::class_type(typename_arg_t<T>)
    : base_type{typename_arg<T>}
    , state_{std::make_shared<state>(state{
        std::is_same_v<T, typename detail::class_traits<T>::raw_type>
            ? any_type{*this}
            : type_db::get<typename detail::class_traits<T>::raw_type>(),
        detail::class_traits<T>::size,
        detail::class_traits<T>::is_const,
        detail::class_traits<T>::is_empty,
        detail::class_traits<T>::is_final,
        detail::class_traits<T>::is_abstract,
        detail::class_traits<T>::is_polymorphic,
    })} {
        static_assert(std::is_class_v<T>);
    }

    inline any_type class_type::raw_type() const noexcept {
        return state_->raw_type;
    }

    inline std::size_t class_type::size() const noexcept {
        return state_->size;
    }

    inline bool class_type::is_const() const noexcept {
        return state_->is_const;
    }

    inline bool class_type::is_empty() const noexcept {
        return state_->is_empty;
    }

    inline bool class_type::is_final() const noexcept {
        return state_->is_final;
    }

    inline bool class_type::is_abstract() const noexcept {
        return state_->is_abstract;
    }

    inline bool class_type::is_polymorphic() const noexcept {
        return state_->is_polymorphic;
    }
}
