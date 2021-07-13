/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_fwd.hpp"

#include "meta_data.hpp"

#include "meta_ctor_info.hpp"

namespace meta_hpp
{
    template < typename... Args >
    class ctor_ {
    public:
        ctor_() = default;

        template < typename Class >
        ctor_info make_info() const {
            static_assert(std::is_constructible_v<Class, Args...>);
            ctor_info info{detail::typename_arg<Class>, detail::typename_args<Args...>};
            detail::merge_with(info.datas_, datas_, &data_info::merge);
            return info;
        }

        template < typename... Internals >
        ctor_& operator()(Internals&&...internals) {
            (add_(std::forward<Internals>(internals)), ...);
            return *this;
        }
    private:
        void add_(data_ internal) {
            data_info info = std::move(internal).make_info();
            detail::merge_with(datas_, info.id(), info, &data_info::merge);
        }
    private:
        std::map<std::string, data_info, std::less<>> datas_;
    };
}
