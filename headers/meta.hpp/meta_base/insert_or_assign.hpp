/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "base.hpp"

namespace meta_hpp::detail
{
    template < typename Key, typename Compare, typename Allocator >
    typename std::set<Key, Compare, Allocator>::iterator
    insert_or_assign(std::set<Key, Compare, Allocator>& set,
        typename std::set<Key, Compare, Allocator>::value_type&& value)
    {
        auto&& [position, inserted] = set.insert(std::move(value));

        if ( inserted ) {
            return position;
        }

        auto node = set.extract(position++);
        node.value() = std::move(value);
        return set.insert(position, std::move(node));
    }

    template < typename Key, typename Compare, typename Allocator >
    typename std::set<Key, Compare, Allocator>::iterator
    insert_or_assign(std::set<Key, Compare, Allocator>& set,
        const typename std::set<Key, Compare, Allocator>::value_type& value)
    {
        auto&& [position, inserted] = set.insert(value);

        if ( inserted ) {
            return position;
        }

        auto node = set.extract(position++);
        node.value() = value;
        return set.insert(position, std::move(node));
    }
}
