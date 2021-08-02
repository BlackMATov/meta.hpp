/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_fwd.hpp"
#include "meta_infos.hpp"

#include "meta_registry/class_.hpp"
#include "meta_registry/ctor_.hpp"
#include "meta_registry/data_.hpp"
#include "meta_registry/enum_.hpp"
#include "meta_registry/evalue_.hpp"
#include "meta_registry/function_.hpp"
#include "meta_registry/member_.hpp"
#include "meta_registry/method_.hpp"
#include "meta_registry/namespace_.hpp"

namespace meta_hpp
{
    class registry final {
    public:
        registry() = default;

        class_info get_class_by_name(std::string_view name) const noexcept;
        data_info get_data_by_name(std::string_view name) const noexcept;
        enum_info get_enum_by_name(std::string_view name) const noexcept;
        evalue_info get_evalue_by_name(std::string_view name) const noexcept;
        function_info get_function_by_name(std::string_view name) const noexcept;
        member_info get_member_by_name(std::string_view name) const noexcept;
        method_info get_method_by_name(std::string_view name) const noexcept;
        namespace_info get_namespace_by_name(std::string_view name) const noexcept;
    };
}
