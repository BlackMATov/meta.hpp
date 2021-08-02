/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_fwd.hpp"
#include "../meta_utilities.hpp"

#include "../meta_infos/data_info.hpp"

namespace meta_hpp
{
    class data_ final {
    public:
        explicit data_(std::string name, class value value);

        data_info make_info() const;
    private:
        std::string name_;
        class value value_;
    };
}

namespace meta_hpp
{
    inline data_::data_(std::string name, class value value)
    : name_{std::move(name)}
    , value_{std::move(value)} {}

    inline data_info data_::make_info() const {
        return data_info{};
    }
}
