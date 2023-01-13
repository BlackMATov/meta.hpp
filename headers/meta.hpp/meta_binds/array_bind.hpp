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
    template < detail::array_kind Array >
    array_bind<Array>::array_bind(metadata_map metadata)
    : data_{detail::type_access(resolve_type<Array>())} {
        detail::insert_or_assign(data_->metadata, std::move(metadata));
    }

    template < detail::array_kind Array >
    array_bind<Array>::operator array_type() const noexcept {
        return array_type{data_};
    }
}
