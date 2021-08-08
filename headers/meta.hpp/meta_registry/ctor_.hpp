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
    template < typename... Args >
    class ctor_ final {
    public:
        explicit ctor_() = default;

        template < typename Class >
        ctor_info make_info() const;
    public:
        template < typename... Internals >
        ctor_& operator()(Internals&&...internals);
    private:
        void add_(const data_& internal);
        void add_(...) = delete;
    private:
        data_info_map datas_;
    };
}

namespace meta_hpp
{
    template < typename... Args >
    template < typename Class >
    ctor_info ctor_<Args...>::make_info() const {
        ctor_info info{typename_arg<Class>, typename_arg<Args...>};
        info.state_->datas.insert(datas_.begin(), datas_.end());
        return info;
    }

    template < typename... Args >
    template < typename... Internals >
    ctor_<Args...>& ctor_<Args...>::operator()(Internals&&...internals) {
        (add_(std::forward<Internals>(internals)), ...);
        return *this;
    }
}

namespace meta_hpp
{
    template < typename... Args >
    void ctor_<Args...>::add_(const data_& internal) {
        auto info = internal.make_info();
        detail::merge_with(datas_, info.name(), info, &data_info::merge);
    }
}
