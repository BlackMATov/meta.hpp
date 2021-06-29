/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_fwd.hpp"

#include "meta_variable_info.hpp"

namespace meta_hpp
{
    template < auto Variable >
    class variable_ {
    public:
        explicit variable_(std::string id)
        : info_(std::move(id)) {
            info_.getter_ = &variable_detail::getter<Variable>;
            info_.setter_ = &variable_detail::setter<Variable>;
        }

        const variable_info& info() const noexcept {
            return info_;
        }
    private:
        variable_info info_;
    };
}
