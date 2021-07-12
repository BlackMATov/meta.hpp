/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_fwd.hpp"

#include "meta_data.hpp"

#include "meta_method_info.hpp"

namespace meta_hpp
{
    template < typename MethodType >
    class method_ {
    public:
        static_assert(std::is_member_function_pointer_v<MethodType>);

        explicit method_(std::string id, MethodType method_ptr)
        : info_{std::move(id), method_ptr} {}

        const method_info& make_info() const {
            return info_;
        }

        template < typename... Internals >
        method_& operator()(Internals&&...internals) {
            (add_(std::forward<Internals>(internals)), ...);
            return *this;
        }
    private:
        void add_(data_ internal) {
            data_info info = std::move(internal).make_info();
            detail::merge_with(info_.datas_, info.id(), info, &data_info::merge);
        }
    private:
        method_info info_;
    };
}
