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

        operator const data_info&() const noexcept {
            return info_;
        }

        template < typename... Internals >
        data_& operator()(Internals&&...internals) {
            (add_(std::forward<Internals>(internals)), ...);
            return *this;
        }
    private:
        void add_(const data_info& info) {
            detail::merge_with(info_.datas_, info.id(), info, &data_info::merge);
        }
    private:
        data_info info_;
    };
}
