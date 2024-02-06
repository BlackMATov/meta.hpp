/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_binds.hpp"
#include "../meta_registry.hpp"

namespace meta_hpp
{
    template < enum_kind Enum >
    enum_bind<Enum>::enum_bind(metadata_map metadata)
    : type_bind_base{resolve_type<Enum>(), std::move(metadata)} {}

    template < enum_kind Enum >
    template < typename... Opts >
    enum_bind<Enum>& enum_bind<Enum>::evalue_(std::string name, Enum value, Opts&&... opts) {
        metadata_bind::values_t metadata = metadata_bind::from_opts(META_HPP_FWD(opts)...);
        auto state = detail::evalue_state::make(std::move(name), std::move(value), std::move(metadata));
        detail::insert_or_assign(get_data().evalues, evalue{std::move(state)});
        return *this;
    }
}
