/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "_registry_fwd.hpp"

namespace meta_hpp
{
    class data_ final {
    public:
        template < typename T >
        explicit data_(std::string name, T&& value);

        data_info make_info() const;
    public:
        template < typename... Internals >
        data_& operator()(Internals&&...internals);
    private:
        void add_(const data_& internal);
        void add_(...) = delete;
    private:
        std::string name_;
        class value value_;
        data_info_map datas_;
    };
}

namespace meta_hpp
{
    template < typename T >
    data_::data_(std::string name, T&& value)
    : name_{std::move(name)}
    , value_{std::forward<T>(value)} {}

    inline data_info data_::make_info() const {
        data_info info{name_, value_};
        info.state_->datas.insert(datas_.begin(), datas_.end());
        return info;
    }

    template < typename... Internals >
    data_& data_::operator()(Internals&&...internals) {
        (add_(std::forward<Internals>(internals)), ...);
        return *this;
    }
}

namespace meta_hpp
{
    inline void data_::add_(const data_& internal) {
        auto info = internal.make_info();
        detail::merge_with(datas_, info.name(), info, &data_info::merge);
    }
}
