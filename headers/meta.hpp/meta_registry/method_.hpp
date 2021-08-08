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
    template < typename Method >
    class method_ final {
    public:
        explicit method_(std::string name, Method instance);

        method_info make_info() const;
    public:
        template < typename... Internals >
        method_& operator()(Internals&&...internals);
    private:
        void add_(const data_& internal);
        void add_(...) = delete;
    private:
        std::string name_;
        Method instance_;
        data_info_map datas_;
    };
}

namespace meta_hpp
{
    template < typename Method >
    method_<Method>::method_(std::string name, Method instance)
    : name_{std::move(name)}
    , instance_{std::move(instance)} {}

    template < typename Method >
    method_info method_<Method>::make_info() const {
        method_info info{name_, instance_};
        info.state_->datas.insert(datas_.begin(), datas_.end());
        return info;
    }

    template < typename Method >
    template < typename... Internals >
    method_<Method>& method_<Method>::operator()(Internals&&...internals) {
        (add_(std::forward<Internals>(internals)), ...);
        return *this;
    }
}

namespace meta_hpp
{
    template < typename Method >
    void method_<Method>::add_(const data_& internal) {
        auto info = internal.make_info();
        detail::merge_with(datas_, info.name(), info, &data_info::merge);
    }
}
