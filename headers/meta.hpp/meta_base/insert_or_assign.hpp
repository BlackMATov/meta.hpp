/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "base.hpp"
#include "exceptions.hpp"

namespace meta_hpp::detail
{
    template < typename Value, typename Allocator >
        requires std::is_move_constructible_v<Value> && std::is_move_assignable_v<Value>
    typename std::vector<Value, Allocator>::iterator insert_or_assign( //
        std::vector<Value, Allocator>& vector,
        typename std::vector<Value, Allocator>::value_type&& value
    ) {
        if ( auto&& position{std::find(vector.begin(), vector.end(), value)}; position != vector.end() ) {
            *position = std::move(value);
            return position;
        }
        return vector.insert(vector.end(), std::move(value));
    }

    template < typename Value, typename Allocator >
        requires std::is_copy_constructible_v<Value> && std::is_copy_assignable_v<Value>
    typename std::vector<Value, Allocator>::iterator insert_or_assign( //
        std::vector<Value, Allocator>& vector,
        const typename std::vector<Value, Allocator>::value_type& value
    ) {
        if ( auto&& position{std::find(vector.begin(), vector.end(), value)}; position != vector.end() ) {
            *position = value;
            return position;
        }
        return vector.insert(vector.end(), value);
    }
}

namespace meta_hpp::detail
{
    template < typename Key, typename Value, typename Compare, typename Allocator >
    void insert_or_assign( //
        std::map<Key, Value, Compare, Allocator>& map,
        std::map<Key, Value, Compare, Allocator>& value
    ) {
        map.swap(value);
        map.merge(value);
    }

    template < typename Key, typename Value, typename Compare, typename Allocator >
    void insert_or_assign( //
        std::map<Key, Value, Compare, Allocator>& map,
        std::map<Key, Value, Compare, Allocator>&& value
    ) {
        map.swap(value);
        map.merge(std::move(value));
    }
}
