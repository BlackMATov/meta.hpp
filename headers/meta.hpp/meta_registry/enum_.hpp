/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_fwd.hpp"
#include "../meta_utilities.hpp"

#include "../meta_infos/enum_info.hpp"

namespace meta_hpp
{
    template < typename Enum >
    class enum_ final {
    public:
        explicit enum_(std::string name);

        enum_info make_info() const;
    private:
        std::string name_;
    };
}

namespace meta_hpp
{
    template < typename Enum >
    inline enum_<Enum>::enum_(std::string name)
    : name_{std::move(name)} {}

    template < typename Enum >
    inline enum_info enum_<Enum>::make_info() const {
        return enum_info{};
    }
}
