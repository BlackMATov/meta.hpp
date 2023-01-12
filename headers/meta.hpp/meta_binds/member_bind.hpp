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
    template < detail::member_kind Member >
    member_bind<Member>::member_bind(metadata_map metadata)
    : data_{detail::type_access(resolve_type<Member>())} {
        data_->metadata.swap(metadata);
        data_->metadata.merge(std::move(metadata));
    }

    template < detail::member_kind Member >
    member_bind<Member>::operator member_type() const noexcept {
        return member_type{data_};
    }
}
