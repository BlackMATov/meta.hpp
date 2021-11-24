/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_registry.hpp"

namespace meta_hpp
{
    template < enum_kind Enum >
    enum_bind<Enum>::enum_bind()
    : data_{detail::get_type_data<Enum>()} {}

    template < enum_kind Enum >
    enum_bind<Enum>::operator enum_type() const noexcept {
        return enum_type{data_};
    }

    template < enum_kind Enum >
    enum_bind<Enum>& enum_bind<Enum>::evalue_(std::string name, Enum value) {
        auto evalue_state = detail::evalue_state::make(std::move(name), std::move(value));
        data_->evalues.emplace(evalue_state->index, std::move(evalue_state));
        return *this;
    }
}
