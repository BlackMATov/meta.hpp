/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_fwd.hpp"

#include "meta_class.hpp"
#include "meta_function.hpp"
#include "meta_namespace.hpp"
#include "meta_variable.hpp"

namespace meta_hpp
{
    class registry {
    public:
        registry() = default;

        template < typename... Internals >
        registry& operator()(Internals&&...internals) {
            default_namespace_(std::forward<Internals>(internals)...);
            return *this;
        }

        std::optional<class_info> get_class(std::string_view id) const {
            return default_namespace_.info().get_class(id);
        }

        std::optional<function_info> get_function(std::string_view id) const {
            return default_namespace_.info().get_function(id);
        }

        std::optional<namespace_info> get_namespace(std::string_view id) const {
            return default_namespace_.info().get_namespace(id);
        }

        std::optional<variable_info> get_variable(std::string_view id) const {
            return default_namespace_.info().get_variable(id);
        }
    private:
        namespace_ default_namespace_{""};
    };
}
