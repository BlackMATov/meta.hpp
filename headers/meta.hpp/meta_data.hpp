/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_fwd.hpp"

#include "meta_data_info.hpp"

namespace meta_hpp
{
    class data_ {
    public:
        explicit data_(std::string id, value value)
        : info_(std::move(id), std::move(value)) {}

        const data_info& info() const noexcept {
            return info_;
        }

        template < typename... Internals >
        data_& operator()(Internals&&...internals) {
            (add_(std::forward<Internals>(internals)), ...);
            return *this;
        }
    private:
        void add_(const data_& internal) {
            detail::merge_with(
                info_.datas_,
                internal.info().id(),
                internal.info(),
                &data_info::merge_with_);
        }
    private:
        data_info info_;
    };
}
