/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_registry.hpp"
#include "../meta_types.hpp"

#include "../meta_detail/type_sharing.hpp"
#include "../meta_detail/type_traits/constructor_traits.hpp"

namespace meta_hpp::detail::constructor_type_data_impl
{
    template < class_kind Class, typename... Args >
    any_type_list make_argument_types() {
        using ct = constructor_traits<Class, Args...>;
        using ct_argument_types = typename ct::argument_types;

        return []<std::size_t... Is>(std::index_sequence<Is...>) {
            any_type_list argument_types;
            argument_types.reserve(type_list_arity_v<ct_argument_types>);

            [[maybe_unused]] const auto make_argument_type = []<std::size_t I>(index_constant<I>) {
                return resolve_type<type_list_at_t<I, ct_argument_types>>();
            };

            (argument_types.emplace_back(make_argument_type(index_constant<Is>{})), ...);
            return argument_types;
        }(std::make_index_sequence<type_list_arity_v<ct_argument_types>>());
    }
}

namespace meta_hpp::detail
{
    template < class_kind Class, typename... Args >
    constructor_type_data::constructor_type_data(constructor_traits<Class, Args...>)
    : type_data_base{type_kind::constructor_, shared_traits_hash<constructor_traits<Class, Args...>>{}(this)}
    , flags{constructor_traits<Class, Args...>::make_flags()}
    , owner_type{resolve_type<typename constructor_traits<Class, Args...>::class_type>()}
    , argument_types(constructor_type_data_impl::make_argument_types<Class, Args...>()) {}
}

namespace meta_hpp
{
    inline constructor_bitflags constructor_type::get_flags() const noexcept {
        return data_->flags;
    }

    inline std::size_t constructor_type::get_arity() const noexcept {
        return data_->argument_types.size();
    }

    inline class_type constructor_type::get_owner_type() const noexcept {
        return data_->owner_type;
    }

    inline any_type constructor_type::get_argument_type(std::size_t position) const noexcept {
        return position < data_->argument_types.size() ? data_->argument_types[position] : any_type{};
    }

    inline const any_type_list& constructor_type::get_argument_types() const noexcept {
        return data_->argument_types;
    }
}
