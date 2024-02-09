/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_states.hpp"

#include "../meta_detail/type_registry.hpp"
#include "../meta_types/enum_type.hpp"

namespace meta_hpp::detail
{
    inline evalue_state::evalue_state(evalue_index nindex, metadata_map nmetadata)
    : index{std::move(nindex)}
    , metadata{std::move(nmetadata)} {}

    template < enum_kind Enum >
    evalue_state::state_ptr evalue_state::make(std::string name, Enum evalue, metadata_map metadata) {
        type_registry& registry{type_registry::instance()};

        evalue_state state{
            evalue_index{registry.resolve_by_type<Enum>(), std::move(name)},
            std::move(metadata),
        };

        state.enum_value = uvalue{evalue};
        state.underlying_value = uvalue{to_underlying(evalue)};

        return std::make_shared<evalue_state>(std::move(state));
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
}
