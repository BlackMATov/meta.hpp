/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_fwd.hpp"
#include "../meta_utilities.hpp"

#include "../meta_infos/evalue_info.hpp"

namespace meta_hpp
{
    template < typename Enum >
    class evalue_ final {
    public:
        explicit evalue_(std::string name, Enum value);

        evalue_info make_info() const;
    private:
        std::string name_;
        Enum value_;
    };
}

namespace meta_hpp
{
    template < typename Enum >
    inline evalue_<Enum>::evalue_(std::string name, Enum value)
    : name_{std::move(name)}
    , value_{std::move(value)} {}

    template < typename Enum >
    inline evalue_info evalue_<Enum>::make_info() const {
        return evalue_info{};
    }
}
