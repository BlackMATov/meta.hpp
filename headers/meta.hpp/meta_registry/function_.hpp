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
    template < typename Function >
    class function_ final {
    public:
        explicit function_(std::string name, Function instance);

        function_info make_info() const;
    public:
        template < typename... Internals >
        function_& operator()(Internals&&...internals);
    private:
        void add_(const data_& internal);
        void add_(...) = delete;
    private:
        std::string name_;
        Function instance_;
        data_info_map datas_;
    };
}

namespace meta_hpp
{
    template < typename Function >
    function_<Function>::function_(std::string name, Function instance)
    : name_{std::move(name)}
    , instance_{std::move(instance)} {}

    template < typename Function >
    function_info function_<Function>::make_info() const {
        function_info info{name_, instance_};
        info.state_->datas.insert(datas_.begin(), datas_.end());
        return info;
    }

    template < typename Function >
    template < typename... Internals >
    function_<Function>& function_<Function>::operator()(Internals&&...internals) {
        (add_(std::forward<Internals>(internals)), ...);
        return *this;
    }
}

namespace meta_hpp
{
    template < typename Function >
    void function_<Function>::add_(const data_& internal) {
        auto info = internal.make_info();
        detail::merge_with(datas_, info.name(), info, &data_info::merge);
    }
}
