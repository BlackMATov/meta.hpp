/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_fwd.hpp"
#include "../meta_utilities.hpp"

#include "../meta_infos/member_info.hpp"

namespace meta_hpp
{
    template < typename Member >
    class member_ final {
    public:
        explicit member_(std::string name, Member instance);

        member_info make_info() const;
    private:
        std::string name_;
        Member instance_;
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
        return member_info{};
    }
}
