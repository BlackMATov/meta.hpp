/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_binds.hpp"

#include "../meta_detail/type_registry.hpp"

namespace meta_hpp
{
    template < detail::function_kind Function >
    function_bind<Function>::function_bind(type_opts opts)
    : data_{detail::type_access(detail::resolve_type<Function>())} {
        data_->metadata.swap(opts.metadata);
        data_->metadata.merge(opts.metadata);
    }

    template < detail::function_kind Function >
    function_bind<Function>::operator function_type() const noexcept {
        return function_type{data_};
    }
}
