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
    template < typename FunctionType >
    class function_ {
    public:
        static_assert(std::is_function_v<std::remove_pointer_t<FunctionType>>);

        explicit function_(std::string id, FunctionType function_ptr)
        : info_{std::move(id), function_ptr} {}

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
