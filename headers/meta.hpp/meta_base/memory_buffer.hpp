/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "base.hpp"
#include "fnv1a_hash.hpp"

namespace meta_hpp::detail
{
    class memory_buffer final {
    public:
        memory_buffer() = default;

        memory_buffer(const memory_buffer&) = delete;
        memory_buffer& operator=(const memory_buffer&) = delete;

        memory_buffer(memory_buffer&& other) noexcept
        : data_{other.data_}
        , size_{other.size_}
        , align_{other.align_} {
            other.data_ = nullptr;
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
        : data_{::operator new(size, align)}
        , size_{size}
        , align_{align} {}

        explicit memory_buffer(const void* mem, std::size_t size, std::align_val_t align)
        : memory_buffer{size, align} {
            if ( mem != nullptr && size > 0 ) {
                std::memcpy(data_, mem, size);
            }
        }

        ~memory_buffer() noexcept {
            reset();
        }

        [[nodiscard]] bool is_valid() const noexcept {
            return data_ != nullptr;
        }

        [[nodiscard]] explicit operator bool() const noexcept {
            return is_valid();
        }

        void reset() noexcept {
            if ( data_ != nullptr ) {
                ::operator delete(data_, align_);
                data_ = nullptr;
                size_ = 0;
                align_ = std::align_val_t{};
            }
        }

        [[nodiscard]] void* get_data() noexcept {
            return data_;
        }

        [[nodiscard]] const void* get_data() const noexcept {
            return data_;
        }

        [[nodiscard]] std::size_t get_size() const noexcept {
            return size_;
        }

        [[nodiscard]] std::align_val_t get_align() const noexcept {
            return align_;
        }

        void swap(memory_buffer& other) noexcept {
            std::swap(data_, other.data_);
            std::swap(size_, other.size_);
            std::swap(align_, other.align_);
        }

        [[nodiscard]] std::size_t get_hash() const noexcept {
            return fnv1a_hash(data_, size_);
        }

        [[nodiscard]] bool operator==(const memory_buffer& other) const noexcept {
            return (size_ == other.size_) //
                && (size_ == 0 || std::memcmp(data_, other.data_, size_) == 0);
        }

        [[nodiscard]] std::strong_ordering operator<=>(const memory_buffer& other) const noexcept {
            if ( const std::strong_ordering cmp{size_ <=> other.size_}; cmp != std::strong_ordering::equal ) {
                return cmp;
            }
            return (size_ == 0 ? 0 : std::memcmp(data_, other.data_, size_)) <=> 0;
        }

    private:
        void* data_{};
        std::size_t size_{};
        std::align_val_t align_{};
    };

    inline void swap(memory_buffer& l, memory_buffer& r) noexcept {
        l.swap(r);
    }
}

namespace std
{
    template <>
    struct hash<meta_hpp::detail::memory_buffer> {
        size_t operator()(const meta_hpp::detail::memory_buffer& mb) const noexcept {
            return mb.get_hash();
        }
    };
}
