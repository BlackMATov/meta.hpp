/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "base.hpp"

namespace meta_hpp::detail
{
    class memory_buffer final {
    public:
        memory_buffer() = default;

        memory_buffer(const memory_buffer&) = delete;
        memory_buffer& operator=(const memory_buffer&) = delete;

        memory_buffer(memory_buffer&& other) noexcept
        : memory_{other.memory_}
        , size_{other.size_}
        , align_{other.align_} {
            other.memory_ = nullptr;
            other.size_ = 0;
            other.align_ = std::align_val_t{};
        }

        memory_buffer& operator=(memory_buffer&& other) noexcept {
            if ( this != &other ) {
                memory_buffer{std::move(other)}.swap(*this);
            }
            return *this;
        }

        explicit memory_buffer(std::size_t size, std::align_val_t align)
        : memory_{::operator new(size, align)}
        , size_{size}
        , align_{align} {}

        ~memory_buffer() noexcept {
            reset();
        }

        [[nodiscard]] bool is_valid() const noexcept {
            return memory_ != nullptr;
        }

        [[nodiscard]] explicit operator bool() const noexcept {
            return is_valid();
        }

        void reset() noexcept {
            if ( memory_ != nullptr ) {
                ::operator delete(memory_, align_);
                memory_ = nullptr;
                size_ = 0;
                align_ = std::align_val_t{};
            }
        }

        void swap(memory_buffer& other) noexcept {
            std::swap(memory_, other.memory_);
            std::swap(size_, other.size_);
            std::swap(align_, other.align_);
        }

        [[nodiscard]] void* get_memory() noexcept {
            return memory_;
        }

        [[nodiscard]] const void* get_memory() const noexcept {
            return memory_;
        }

        [[nodiscard]] std::size_t get_size() const noexcept {
            return size_;
        }

        [[nodiscard]] std::align_val_t get_align() const noexcept {
            return align_;
        }
    private:
        void* memory_{};
        std::size_t size_{};
        std::align_val_t align_{};
    };

    inline void swap(memory_buffer& l, memory_buffer& r) noexcept {
        l.swap(r);
    }
}
