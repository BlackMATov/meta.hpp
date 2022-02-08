/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_states.hpp"

namespace meta_hpp::detail
{
    class state_registry final {
    public:
        [[nodiscard]] static state_registry& instance() {
            static state_registry instance;
            return instance;
        }

        [[nodiscard]] scope get_scope_by_name(std::string_view name) const noexcept {
            if ( auto iter = scopes_.find(name); iter != scopes_.end() ) {
                return iter->second;
            }
            return scope{};
        }

        [[nodiscard]] scope resolve_scope(std::string_view name) {
            return ensure_scope(name);
        }
    private:
        state_registry() = default;

        scope ensure_scope(std::string_view name) {
            if ( auto iter = scopes_.find(name); iter != scopes_.end() ) {
                return iter->second;
            }

            return scopes_.emplace(
                std::string{name},
                scope_state::make(std::string{name})).first->second;
        }
    private:
        std::map<std::string, scope, std::less<>> scopes_;
    };
}

namespace meta_hpp::detail
{
    [[nodiscard]] inline scope resolve_scope(std::string_view name) {
        state_registry& registry = state_registry::instance();
        return registry.resolve_scope(name);
    }
}
