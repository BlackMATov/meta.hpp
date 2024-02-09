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
#include "../meta_detail/type_traits/method_traits.hpp"

namespace meta_hpp::detail::method_type_data_impl
{
    template < method_pointer_kind Method >
    any_type_list make_argument_types() {
        using mt = method_traits<Method>;
        using mt_argument_types = typename mt::argument_types;

        return []<std::size_t... Is>(std::index_sequence<Is...>) {
            any_type_list argument_types;
            argument_types.reserve(type_list_arity_v<mt_argument_types>);

            [[maybe_unused]] const auto make_argument_type = []<std::size_t I>(index_constant<I>) {
                return resolve_type<type_list_at_t<I, mt_argument_types>>();
            };

            (argument_types.emplace_back(make_argument_type(index_constant<Is>{})), ...);
            return argument_types;
        }(std::make_index_sequence<type_list_arity_v<mt_argument_types>>());
    }
}

namespace meta_hpp::detail
{
    template < method_pointer_kind Method >
    method_type_data::method_type_data(method_traits<Method>)
    : type_data_base{type_kind::method_, shared_traits_hash<method_traits<Method>>{}(this)}
    , flags{method_traits<Method>::make_flags()}
    , owner_type{resolve_type<typename method_traits<Method>::class_type>()}
    , return_type{resolve_type<typename method_traits<Method>::return_type>()}
    , argument_types(method_type_data_impl::make_argument_types<Method>()) {}
}

namespace meta_hpp
{
    inline method_bitflags method_type::get_flags() const noexcept {
        return data_->flags;
    }

    inline std::size_t method_type::get_arity() const noexcept {
        return data_->argument_types.size();
    }

    inline class_type method_type::get_owner_type() const noexcept {
        return data_->owner_type;
    }

    inline any_type method_type::get_return_type() const noexcept {
        return data_->return_type;
    }

    inline any_type method_type::get_argument_type(std::size_t position) const noexcept {
        return position < data_->argument_types.size() ? data_->argument_types[position] : any_type{};
    }

    inline const any_type_list& method_type::get_argument_types() const noexcept {
        return data_->argument_types;
    }
}
