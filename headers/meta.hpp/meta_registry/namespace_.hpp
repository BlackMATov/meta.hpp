/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_fwd.hpp"
#include "../meta_utilities.hpp"

#include "../meta_infos/namespace_info.hpp"

namespace meta_hpp
{
    class namespace_ final {
    public:
        explicit namespace_(std::string name);

        namespace_info make_info() const;
    private:
        std::string name_;
    };
}

namespace meta_hpp
{
    inline namespace_::namespace_(std::string name)
    : name_{std::move(name)} {}

    inline namespace_info namespace_::make_info() const {
        return namespace_info{};
    }
}
