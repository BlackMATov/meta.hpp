/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_fwd.hpp"
#include "../meta_utilities.hpp"

#include "../meta_infos/method_info.hpp"

namespace meta_hpp
{
    template < typename Method >
    class method_ final {
    public:
        explicit method_(std::string name, Method instance);

        method_info make_info() const;
    private:
        std::string name_;
        Method instance_;
    };
}

namespace meta_hpp
{
    template < typename Method >
    inline method_<Method>::method_(std::string name, Method instance)
    : name_{std::move(name)}
    , instance_{std::move(instance)} {}

    template < typename Method >
    inline method_info method_<Method>::make_info() const {
        return method_info{name_, instance_};
    }
}
