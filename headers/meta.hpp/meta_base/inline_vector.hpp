/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "base.hpp"

namespace meta_hpp::detail
{
    template < typename T >
    class inline_vector final {
    public:
        inline_vector() = delete;

        inline_vector(inline_vector&&) = delete;
        inline_vector& operator=(inline_vector&&) = delete;

        inline_vector(const inline_vector&) = delete;
        inline_vector& operator=(const inline_vector&) = delete;

        inline_vector(T* mem, std::size_t capacity)
        : begin_{mem}
        , end_{mem}
        , capacity_{mem + capacity} {}

        ~inline_vector() {
            std::destroy(begin_, end_);
        }

        // clang-format off

        [[nodiscard]] T* data() noexcept { return begin_; }
        [[nodiscard]] const T* data() const noexcept { return begin_; }

        [[nodiscard]] T* begin() noexcept { return begin_; }
        [[nodiscard]] const T* begin() const noexcept { return begin_; }
        [[nodiscard]] const T* cbegin() const noexcept { return begin_; }

        [[nodiscard]] T* end() noexcept { return end_; }
        [[nodiscard]] const T* end() const noexcept { return end_; }
        [[nodiscard]] const T* cend() const noexcept { return end_; }

        // clang-format on

        template < typename... Args >
        T& emplace_back(Args&&... args) {
            META_HPP_ASSERT(end_ < capacity_ && "full vector");
            return *std::construct_at(end_++, std::forward<Args>(args)...);
        }

        [[nodiscard]] std::size_t get_size() const noexcept {
            return static_cast<std::size_t>(end_ - begin_);
        }

        [[nodiscard]] std::size_t get_capacity() const noexcept {
            return static_cast<std::size_t>(capacity_ - begin_);
        }

    private:
        T* begin_{};
        T* end_{};
        T* capacity_{};
    };
}
