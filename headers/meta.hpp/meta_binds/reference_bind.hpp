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
    template < detail::reference_kind Reference >
    reference_bind<Reference>::reference_bind(metadata_map metadata)
    : data_{detail::type_access(resolve_type<Reference>())} {
        data_->metadata.swap(metadata);
        data_->metadata.merge(metadata);
    }

    template < detail::reference_kind Reference >
    reference_bind<Reference>::operator reference_type() const noexcept {
        return reference_type{data_};
    }
}
