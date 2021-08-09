/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "_registry_fwd.hpp"

#include "data_.hpp"

namespace meta_hpp
{
    template < typename Base >
    class base_ final {
    public:
        explicit base_() = default;

        template < typename Derived >
        base_info make_info() const;
    public:
        template < typename... Internals >
        base_& operator()(Internals&&...internals);
    private:
        void add_(const data_& internal);
        void add_(...) = delete;
    private:
        data_info_map datas_;
    };
}

namespace meta_hpp
{
    template < typename Base >
    template < typename Derived >
    base_info base_<Base>::make_info() const {
        base_info info{typename_arg<Base>, typename_arg<Derived>};
        info.state_->datas.insert(datas_.begin(), datas_.end());
        return info;
    }

    template < typename Base >
    template < typename... Internals >
    base_<Base>& base_<Base>::operator()(Internals&&...internals) {
        (add_(std::forward<Internals>(internals)), ...);
        return *this;
    }
}

namespace meta_hpp
{
    template < typename Base >
    void base_<Base>::add_(const data_& internal) {
        auto info = internal.make_info();
        detail::merge_with(datas_, info.name(), info, &data_info::merge);
    }
}
