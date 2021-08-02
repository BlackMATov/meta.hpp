/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_fwd.hpp"
#include "../meta_utilities.hpp"

#include "../meta_infos/class_info.hpp"

namespace meta_hpp
{
    template < typename Class >
    class class_ final {
    public:
        explicit class_(std::string name);

        class_info make_info() const;
    private:
        std::string name_;
    };
}

namespace meta_hpp
{
    template < typename Class >
    inline class_<Class>::class_(std::string name)
    : name_{std::move(name)} {}

    template < typename Class >
    inline class_info class_<Class>::make_info() const {
        return class_info{};
    }
}
