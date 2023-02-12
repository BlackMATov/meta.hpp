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
        evalue_state state{evalue_index{resolve_type<Enum>(), std::move(name)}, std::move(metadata)};
        state.enum_value = uvalue{evalue};
        state.underlying_value = uvalue{to_underlying(evalue)};
        return make_intrusive<evalue_state>(std::move(state));
    }
}

namespace meta_hpp
{
    inline enum_type evalue::get_type() const noexcept {
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

    template < detail::enum_kind Enum >
    Enum evalue::get_value_as() const {
        return get_value().get_as<Enum>();
    }

    template < detail::enum_kind Enum >
    std::optional<Enum> evalue::safe_get_value_as() const noexcept {
        return get_value().safe_get_as<Enum>();
    }

    template < detail::number_kind Number >
    Number evalue::get_underlying_value_as() const {
        return get_underlying_value().get_as<Number>();
    }

    template < detail::number_kind Number >
    std::optional<Number> evalue::safe_get_underlying_value_as() const noexcept {
        return get_underlying_value().safe_get_as<Number>();
    }
}
