/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "base.hpp"
#include "exceptions.hpp"

namespace meta_hpp::detail
{
    template < typename Key, typename Compare, typename Allocator >
        requires std::is_move_constructible_v<Key> && std::is_move_assignable_v<Key>
    typename std::set<Key, Compare, Allocator>::iterator insert_or_assign( //
        std::set<Key, Compare, Allocator>& set,
        typename std::set<Key, Compare, Allocator>::value_type&& value
    ) {
        auto&& [position, inserted] = set.insert(std::move(value));

        if ( inserted ) {
            return position;
        }

        auto node = set.extract(position++);

        META_HPP_TRY {
            node.value() = std::move(value);
        }
        META_HPP_CATCH(...) {
            set.insert(position, std::move(node));
            META_HPP_RETHROW();
        }

        return set.insert(position, std::move(node));
    }

    template < typename Key, typename Compare, typename Allocator >
        requires std::is_copy_constructible_v<Key> && std::is_copy_assignable_v<Key>
    typename std::set<Key, Compare, Allocator>::iterator insert_or_assign( //
        std::set<Key, Compare, Allocator>& set,
        const typename std::set<Key, Compare, Allocator>::value_type& value
    ) {
        auto&& [position, inserted] = set.insert(value);

        if ( inserted ) {
            return position;
        }

        auto node = set.extract(position++);

        META_HPP_TRY {
            node.value() = value;
        }
        META_HPP_CATCH(...) {
            set.insert(position, std::move(node));
            META_HPP_RETHROW();
        }

        return set.insert(position, std::move(node));
    }
}

namespace meta_hpp::detail
{
    template < typename Key, typename Compare, typename Allocator >
    void insert_or_assign( //
        std::set<Key, Compare, Allocator>& set,
        std::set<Key, Compare, Allocator>& value
    ) {
        set.swap(value);
        set.merge(value);
    }

    template < typename Key, typename Compare, typename Allocator >
    void insert_or_assign( //
        std::set<Key, Compare, Allocator>& set,
        std::set<Key, Compare, Allocator>&& value
    ) {
        set.swap(value);
        set.merge(std::move(value));
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
