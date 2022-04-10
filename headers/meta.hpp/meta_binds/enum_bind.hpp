/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_binds.hpp"
#include "../meta_registry.hpp"

namespace meta_hpp
{
    template < detail::enum_kind Enum >
    enum_bind<Enum>::enum_bind(metadata_map metadata)
    : data_{detail::type_access(resolve_type<Enum>())} {
        data_->metadata.swap(metadata);
        data_->metadata.merge(metadata);
    }

    template < detail::enum_kind Enum >
    enum_bind<Enum>::operator enum_type() const noexcept {
        return enum_type{data_};
    }

    template < detail::enum_kind Enum >
    enum_bind<Enum>& enum_bind<Enum>::evalue_(std::string name, Enum value) {
        return evalue_(std::move(name), std::move(value), {});
    }

    template < detail::enum_kind Enum >
    enum_bind<Enum>& enum_bind<Enum>::evalue_(std::string name, Enum value, evalue_opts opts) {
        auto state = detail::evalue_state::make(
            std::move(name),
            std::move(value),
            std::move(opts.metadata));
        data_->evalues.insert_or_assign(state->index, std::move(state));
        return *this;
    }
}
