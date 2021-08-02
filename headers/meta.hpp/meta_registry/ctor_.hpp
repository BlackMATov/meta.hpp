/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_fwd.hpp"
#include "../meta_utilities.hpp"

#include "../meta_infos/ctor_info.hpp"

namespace meta_hpp
{
    template < typename... Args >
    class ctor_ final {
    public:
        explicit ctor_() = default;

        template < typename Class >
        ctor_info make_info() const;
    private:
    };
}

namespace meta_hpp
{
    template < typename... Args >
    template < typename Class >
    inline ctor_info ctor_<Args...>::make_info() const {
        return ctor_info{typename_arg<Class>, typename_arg<Args...>};
    }
}
