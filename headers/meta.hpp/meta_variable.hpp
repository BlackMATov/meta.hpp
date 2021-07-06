/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_fwd.hpp"

#include "meta_data.hpp"

#include "meta_variable_info.hpp"

namespace meta_hpp
{
    template < auto Variable >
    class variable_ {
    public:
        explicit variable_(std::string id)
        : info_{detail::auto_arg<META_HPP_AUTO_T(Variable)>, std::move(id)} {}

        operator const variable_info&() const noexcept {
            return info_;
        }

        template < typename... Internals >
        variable_& operator()(Internals&&...internals) {
            (add_(std::forward<Internals>(internals)), ...);
            return *this;
        }
    private:
        void add_(const data_info& info) {
            detail::merge_with(info_.datas_, info.id(), info, &data_info::merge);
        }
    private:
        variable_info info_;
    };
}
