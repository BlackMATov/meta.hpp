/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_states.hpp"

#include "../meta_detail/type_registry.hpp"

namespace meta_hpp::detail
{
    inline argument_state::argument_state(argument_index nindex, metadata_map nmetadata)
    : index{nindex}
    , metadata{std::move(nmetadata)} {}

    template < typename Argument >
    inline argument_state::state_ptr argument_state::make(std::size_t position, metadata_map metadata) {
        type_registry& registry{type_registry::instance()};

        argument_state state{
            argument_index{registry.resolve_by_type<Argument>(), position},
            std::move(metadata),
        };

        return std::make_shared<argument_state>(std::move(state));
    }
}

namespace meta_hpp
{
    inline any_type argument::get_type() const noexcept {
        return state_->index.get_type();
    }

    inline std::size_t argument::get_position() const noexcept {
        return state_->index.get_position();
    }

    inline const std::string& argument::get_name() const noexcept {
        return state_->name;
    }
}
