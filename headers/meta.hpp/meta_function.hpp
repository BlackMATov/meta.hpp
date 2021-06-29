/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_fwd.hpp"

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
    private:
        function_info info_;
    };
}
