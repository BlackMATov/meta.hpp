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
    template < typename Enum >
    class evalue_ final {
    public:
        explicit evalue_(std::string name, Enum value);

        evalue_info make_info() const;
    public:
        template < typename... Internals >
        evalue_& operator()(Internals&&...internals);
    private:
        void add_(const data_& internal);
        void add_(...) = delete;
    private:
        std::string name_;
        Enum value_;
        data_info_map datas_;
    };
}

namespace meta_hpp
{
    template < typename Enum >
    evalue_<Enum>::evalue_(std::string name, Enum value)
    : name_{std::move(name)}
    , value_{std::move(value)} {}

    template < typename Enum >
    evalue_info evalue_<Enum>::make_info() const {
        evalue_info info{name_, value_};
        info.state_->datas.insert(datas_.begin(), datas_.end());
        return info;
    }

    template < typename Enum >
    template < typename... Internals >
    evalue_<Enum>& evalue_<Enum>::operator()(Internals&&...internals) {
        (add_(std::forward<Internals>(internals)), ...);
        return *this;
    }
}

namespace meta_hpp
{
    template < typename Enum >
    void evalue_<Enum>::add_(const data_& internal) {
        auto info = internal.make_info();
        detail::merge_with(datas_, info.name(), info, &data_info::merge);
    }
}
