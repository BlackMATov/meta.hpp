/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_fwd.hpp"

#include "meta_field_info.hpp"

namespace meta_hpp
{
    template < auto Field >
    class field_ {
    public:
        static_assert(std::is_member_object_pointer_v<decltype(Field)>);

        explicit field_(std::string id)
        : info_(std::move(id)) {
            info_.getter_ = &field_detail::getter<Field>;
            info_.setter_ = &field_detail::setter<Field>;
        }

        const field_info& info() const noexcept {
            return info_;
        }
    private:
        field_info info_;
    };
}
