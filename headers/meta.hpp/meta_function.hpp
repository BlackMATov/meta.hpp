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
        : info_{get_family_id<decltype(Function)>(), std::move(id)} {
            info_.invoke_ = &function_detail::invoke<Function>;
        }

        const function_info& info() const noexcept {
            return info_;
        }

        operator const function_info&() const noexcept {
            return info_;
        }

        template < typename... Internals >
        function_& operator()(Internals&&...internals) {
            (add_(std::forward<Internals>(internals)), ...);
            return *this;
        }
    private:
        void add_(const data_info& info) {
            detail::merge_with(info_.datas_, info.id(), info, &data_info::merge);
        }
    private:
        function_info info_;
    };
}
