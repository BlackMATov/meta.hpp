/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "_registry_fwd.hpp"

#include "data_.hpp"
#include "evalue_.hpp"

namespace meta_hpp
{
    template < typename Enum >
    class enum_ final {
    public:
        explicit enum_(std::string name);

        enum_info make_info() const;
    public:
        template < typename... Internals >
        enum_& operator()(Internals&&...internals);
    private:
        void add_(const data_& internal);
        void add_(const evalue_<Enum>& internal);
        void add_(...) = delete;
    private:
        std::string name_;
        data_info_map datas_;
        evalue_info_map evalues_;
    };
}

namespace meta_hpp
{
    template < typename Enum >
    inline enum_<Enum>::enum_(std::string name)
    : name_{std::move(name)} {}

    template < typename Enum >
    inline enum_info enum_<Enum>::make_info() const {
        enum_info info{typename_arg<Enum>, name_};
        info.state_->datas.insert(datas_.begin(), datas_.end());
        info.state_->evalues.insert(evalues_.begin(), evalues_.end());
        return info;
    }

    template < typename Enum >
    template < typename... Internals >
    inline enum_<Enum>& enum_<Enum>::operator()(Internals&&...internals) {
        (add_(std::forward<Internals>(internals)), ...);
        return *this;
    }
}

namespace meta_hpp
{
    template < typename Enum >
    inline void enum_<Enum>::add_(const data_& internal) {
        auto info = internal.make_info();
        detail::merge_with(datas_, info.name(), info, &data_info::merge);
    }

    template < typename Enum >
    inline void enum_<Enum>::add_(const evalue_<Enum>& internal) {
        auto info = internal.make_info();
        detail::merge_with(evalues_, info.name(), info, &evalue_info::merge);
    }
}
