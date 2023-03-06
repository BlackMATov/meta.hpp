/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "base.hpp"

namespace meta_hpp::detail
{
    template < typename SortedContainerL, typename SortedContainerR, typename Compare >
    bool is_disjoint(const SortedContainerL& l, const SortedContainerR& r, Compare compare) {
        using std::begin;
        using std::end;

        for ( auto iter_l{begin(l)}, iter_r{begin(r)}; iter_l != end(l) && iter_r != end(r); ) {
            if ( compare(*iter_l, *iter_r) ) {
                ++iter_l;
            } else if ( compare(*iter_r, *iter_l) ) {
                ++iter_r;
            } else {
                return false;
            }
        }

        return true;
    }

    template < typename SortedContainerL, typename SortedContainerR >
    bool is_disjoint(const SortedContainerL& l, const SortedContainerR& r) {
        return is_disjoint(l, r, std::less<>{});
    }
}
