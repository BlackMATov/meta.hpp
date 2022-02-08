/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_states.hpp"

#include "../meta_types/enum_type.hpp"

namespace meta_hpp::detail
{
    template < enum_kind Enum >
    evalue_state_ptr evalue_state::make(std::string name, Enum evalue) {
        return std::make_shared<evalue_state>(evalue_state{
            .index{evalue_index::make<Enum>(std::move(name))},
            .enum_value{uvalue{evalue}},
            .underlying_value{uvalue{stdex::to_underlying(evalue)}},
        });
    }
}

namespace meta_hpp
{
    inline evalue::evalue(detail::evalue_state_ptr state)
    : state_{std::move(state)} {}

    inline bool evalue::is_valid() const noexcept {
        return !!state_;
    }

    inline evalue::operator bool() const noexcept {
        return is_valid();
    }

    inline const evalue_index& evalue::get_index() const noexcept {
        return state_->index;
    }

    inline const enum_type& evalue::get_type() const noexcept {
        return state_->index.get_type();
    }

    inline const std::string& evalue::get_name() const noexcept {
        return state_->index.get_name();
    }

    inline const uvalue& evalue::get_value() const noexcept {
        return state_->enum_value;
    }

    inline const uvalue& evalue::get_underlying_value() const noexcept {
        return state_->underlying_value;
    }
}
