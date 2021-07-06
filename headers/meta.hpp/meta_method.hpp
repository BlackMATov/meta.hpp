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
    template < auto Method >
    class method_ {
    public:
        static_assert(std::is_member_function_pointer_v<decltype(Method)>);

        explicit method_(std::string id)
        : info_{detail::auto_arg<META_HPP_AUTO_T(Method)>, std::move(id)} {}

        operator const method_info&() const {
            return info_;
        }

        template < typename... Internals >
        method_& operator()(Internals&&...internals) {
            (add_(std::forward<Internals>(internals)), ...);
            return *this;
        }
    private:
        void add_(const data_info& info) {
            detail::merge_with(info_.datas_, info.id(), info, &data_info::merge);
        }
    private:
        method_info info_;
    };
}
