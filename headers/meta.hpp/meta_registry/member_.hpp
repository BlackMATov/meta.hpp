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
    template < typename Member >
    class member_ final {
    public:
        explicit member_(std::string name, Member instance);

        member_info make_info() const;
    public:
        template < typename... Internals >
        member_& operator()(Internals&&...internals);
    private:
        void add_(const data_& internal);
        void add_(...) = delete;
    private:
        std::string name_;
        Member instance_;
        data_info_map datas_;
    };
}

namespace meta_hpp
{
    template < typename Member >
    inline member_<Member>::member_(std::string name, Member instance)
    : name_{std::move(name)}
    , instance_{std::move(instance)} {}

    template < typename Member >
    inline member_info member_<Member>::make_info() const {
        member_info info{name_, instance_};
        info.state_->datas.insert(datas_.begin(), datas_.end());
        return info;
    }

    template < typename Member >
    template < typename... Internals >
    inline member_<Member>& member_<Member>::operator()(Internals&&...internals) {
        (add_(std::forward<Internals>(internals)), ...);
        return *this;
    }
}

namespace meta_hpp
{
    template < typename Member >
    inline void member_<Member>::add_(const data_& internal) {
        auto info = internal.make_info();
        detail::merge_with(datas_, info.name(), info, &data_info::merge);
    }
}
