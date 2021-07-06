/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_fwd.hpp"

#include "meta_data.hpp"

#include "meta_field_info.hpp"

namespace meta_hpp
{
    template < typename FieldType >
    class field_ {
    public:
        static_assert(std::is_member_object_pointer_v<FieldType>);

        explicit field_(std::string id, FieldType field_ptr)
        : info_{std::move(id), field_ptr} {}

        operator const field_info&() const noexcept {
            return info_;
        }

        template < typename... Internals >
        field_& operator()(Internals&&...internals) {
            (add_(std::forward<Internals>(internals)), ...);
            return *this;
        }
    private:
        void add_(const data_info& info) {
            detail::merge_with(info_.datas_, info.id(), info, &data_info::merge);
        }
    private:
        field_info info_;
    };
}
