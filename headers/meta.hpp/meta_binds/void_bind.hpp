/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_binds.hpp"
#include "../meta_registry.hpp"

namespace meta_hpp
{
    template < detail::void_kind Void >
    void_bind<Void>::void_bind(metadata_map metadata)
    : data_{detail::type_access(resolve_type<void>())} {
        data_->metadata.swap(metadata);
        data_->metadata.merge(std::move(metadata));
    }

    template < detail::void_kind Void >
    void_bind<Void>::operator void_type() const noexcept {
        return void_type{data_};
    }
}
