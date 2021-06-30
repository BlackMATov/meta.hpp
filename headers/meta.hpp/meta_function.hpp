/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_fwd.hpp"

#include "meta_data.hpp"

#include "meta_function_info.hpp"

namespace meta_hpp
{
    template < auto Function >
    class function_ {
    public:
        static_assert(std::is_function_v<std::remove_pointer_t<decltype(Function)>>);

        explicit function_(std::string id)
        : info_(std::move(id)) {
            info_.invoke_ = &function_detail::invoke<Function>;
        }

        const function_info& info() const noexcept {
            return info_;
        }

        template < typename... Internals >
        function_& operator()(Internals&&...internals) {
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
        function_info info_;
    };
}
