/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_states.hpp"

#include "../meta_types/enum_type.hpp"

namespace meta_hpp::detail
{
    inline evalue_state::evalue_state(evalue_index nindex, metadata_map nmetadata)
    : index{std::move(nindex)}
    , metadata{std::move(nmetadata)} {}

    template < enum_kind Enum >
    evalue_state_ptr evalue_state::make(std::string name, Enum evalue, metadata_map metadata) {
        evalue_state state{evalue_index::make<Enum>(std::move(name)), std::move(metadata)};
        state.enum_value = uvalue{evalue};
        state.underlying_value = uvalue{to_underlying(evalue)};
        return make_intrusive<evalue_state>(std::move(state));
    }
}

namespace meta_hpp
{
    inline evalue::evalue(state_ptr state) noexcept
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

    inline const metadata_map& evalue::get_metadata() const noexcept {
        return state_->metadata;
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

    template < typename T >
    T evalue::get_value_as() const {
        return get_value().get_as<T>();
    }

    template < typename T >
    T evalue::get_underlying_value_as() const {
        return get_underlying_value().get_as<T>();
    }
}
