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
    template < detail::pointer_kind Pointer >
    pointer_bind<Pointer>::pointer_bind(metadata_map metadata)
    : data_{detail::type_access(resolve_type<Pointer>())} {
        detail::insert_or_assign(data_->metadata, std::move(metadata));
    }

    template < detail::pointer_kind Pointer >
    pointer_bind<Pointer>::operator pointer_type() const noexcept {
        return pointer_type{data_};
    }
}
