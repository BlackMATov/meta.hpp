/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_registry.hpp"
#include "../meta_types.hpp"

#include "../meta_states/evalue.hpp"

#include "../meta_detail/type_sharing.hpp"
#include "../meta_detail/type_traits/enum_traits.hpp"
#include "../meta_detail/value_utilities/uarg.hpp"

namespace meta_hpp::detail
{
    template < enum_kind Enum >
    enum_type_data::enum_type_data(enum_traits<Enum>)
    : type_data_base{type_kind::enum_, shared_traits_hash<enum_traits<Enum>>{}(this)}
    , flags{enum_traits<Enum>::make_flags()}
    , underlying_type{resolve_type<typename enum_traits<Enum>::underlying_type>()} {}
}

namespace meta_hpp
{
    inline enum_bitflags enum_type::get_flags() const noexcept {
        return data_->flags;
    }

    inline number_type enum_type::get_underlying_type() const noexcept {
        return data_->underlying_type;
    }

    inline const evalue_list& enum_type::get_evalues() const noexcept {
        return data_->evalues;
    }

    template < enum_kind Enum >
    evalue enum_type::value_to_evalue(Enum value) const {
        if ( *this != resolve_type<Enum>() ) {
            return evalue{};
        }

        for ( const evalue& evalue : data_->evalues ) {
            if ( evalue.get_value().as<Enum>() == value ) {
                return evalue;
            }
        }

        return evalue{};
    }

    inline evalue enum_type::value_to_evalue(const uvalue& value) const {
        if ( *this != value.get_type() ) {
            return evalue{};
        }

        for ( const evalue& evalue : data_->evalues ) {
            if ( evalue.get_value().equals(value) ) {
                return evalue;
            }
        }

        return evalue{};
    }

    inline evalue enum_type::name_to_evalue(std::string_view name) const noexcept {
        for ( const evalue& evalue : data_->evalues ) {
            if ( evalue.get_name() == name ) {
                return evalue;
            }
        }
        return evalue{};
    }
}
