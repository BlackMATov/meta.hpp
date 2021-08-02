/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_fwd.hpp"
#include "../meta_utilities.hpp"

#include "../meta_infos/function_info.hpp"

namespace meta_hpp
{
    template < typename Function >
    class function_ final {
    public:
        explicit function_(std::string name, Function instance);

        function_info make_info() const;
    private:
        std::string name_;
        Function instance_;
    };
}

namespace meta_hpp
{
    template < typename Function >
    inline function_<Function>::function_(std::string name, Function instance)
    : name_{std::move(name)}
    , instance_{std::move(instance)} {}

    template < typename Function >
    inline function_info function_<Function>::make_info() const {
        return function_info{};
    }
}
