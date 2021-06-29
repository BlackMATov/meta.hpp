/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_fwd.hpp"

#include "meta_method_info.hpp"

namespace meta_hpp
{
    template < auto Method >
    class method_ {
    public:
        static_assert(std::is_member_function_pointer_v<decltype(Method)>);

        explicit method_(std::string id)
        : info_(std::move(id)) {
            info_.invoke_ = &method_detail::invoke<Method>;
            info_.cinvoke_ = &method_detail::cinvoke<Method>;
        }

        const method_info& info() const noexcept {
            return info_;
        }
    private:
        method_info info_;
    };
}
