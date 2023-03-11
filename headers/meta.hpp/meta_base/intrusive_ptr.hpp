/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "base.hpp"
#include "hash_combiner.hpp"

namespace meta_hpp::detail
{
    template < typename Derived >
    class intrusive_ref_counter;

    template < typename Derived >
    void intrusive_ptr_add_ref(const intrusive_ref_counter<Derived>* ptr);

    template < typename Derived >
    void intrusive_ptr_release(const intrusive_ref_counter<Derived>* ptr);

    template < typename Derived >
    class intrusive_ref_counter {
    public:
        intrusive_ref_counter() = default;

        intrusive_ref_counter(intrusive_ref_counter&&) noexcept {}

        intrusive_ref_counter(const intrusive_ref_counter&) noexcept {}

        intrusive_ref_counter& operator=(intrusive_ref_counter&&) noexcept {
            return *this;
        }

        intrusive_ref_counter& operator=(const intrusive_ref_counter&) noexcept {
            return *this;
        }

        [[nodiscard]] std::size_t get_use_count() const noexcept {
            return counter_.load(std::memory_order_acquire);
        }

    protected:
        ~intrusive_ref_counter() = default;

    private:
        mutable std::atomic_size_t counter_{};
        friend void intrusive_ptr_add_ref<Derived>(const intrusive_ref_counter* ptr);
        friend void intrusive_ptr_release<Derived>(const intrusive_ref_counter* ptr);
    };

    template < typename Derived >
    void intrusive_ptr_add_ref(const intrusive_ref_counter<Derived>* ptr) {
        ptr->counter_.fetch_add(1, std::memory_order_acq_rel);
    }

    template < typename Derived >
    void intrusive_ptr_release(const intrusive_ref_counter<Derived>* ptr) {
        if ( ptr->counter_.fetch_sub(1, std::memory_order_acq_rel) == 1 ) {
            // NOLINTNEXTLINE(*-owning-memory)
            delete static_cast<const Derived*>(ptr);
        }
    }
}

namespace meta_hpp::detail
{
    template < typename T >
    class intrusive_ptr final {
    public:
        intrusive_ptr() = default;

        ~intrusive_ptr() {
            if ( ptr_ != nullptr ) {
                intrusive_ptr_release(ptr_);
            }
        }

        intrusive_ptr(T* ptr, bool add_ref = true)
        : ptr_{ptr} {
            if ( ptr_ != nullptr && add_ref ) {
                intrusive_ptr_add_ref(ptr_);
            }
        }

        intrusive_ptr(intrusive_ptr&& other) noexcept
        : ptr_{other.ptr_} {
            other.ptr_ = nullptr;
        }

        intrusive_ptr(const intrusive_ptr& other)
        : ptr_{other.ptr_} {
            if ( ptr_ != nullptr ) {
                intrusive_ptr_add_ref(ptr_);
            }
        }

        intrusive_ptr& operator=(T* ptr) noexcept {
            intrusive_ptr{ptr}.swap(*this);
            return *this;
        }

        intrusive_ptr& operator=(intrusive_ptr&& other) noexcept {
            intrusive_ptr{std::move(other)}.swap(*this);
            return *this;
        }

        intrusive_ptr& operator=(const intrusive_ptr& other) {
            intrusive_ptr{other}.swap(*this);
            return *this;
        }

        void reset() {
            intrusive_ptr{}.swap(*this);
        }

        void reset(T* ptr) {
            intrusive_ptr{ptr}.swap(*this);
        }

        void reset(T* ptr, bool add_ref) {
            intrusive_ptr{ptr, add_ref}.swap(*this);
        }

        T* release() noexcept {
            return std::exchange(ptr_, nullptr);
        }

        [[nodiscard]] T* get() const noexcept {
            return ptr_;
        }

        [[nodiscard]] T& operator*() const noexcept {
            return *ptr_;
        }

        [[nodiscard]] T* operator->() const noexcept {
            return ptr_;
        }

        [[nodiscard]] explicit operator bool() const noexcept {
            return ptr_ != nullptr;
        }

        void swap(intrusive_ptr& other) noexcept {
            ptr_ = std::exchange(other.ptr_, ptr_);
        }

        [[nodiscard]] std::size_t get_hash() const noexcept {
            return hash_combiner{}(ptr_);
        }

        [[nodiscard]] bool operator==(const intrusive_ptr& other) const noexcept {
            return ptr_ == other.ptr_;
        }

        [[nodiscard]] std::strong_ordering operator<=>(const intrusive_ptr& other) const noexcept {
            return ptr_ <=> other.ptr_;
        }

    private:
        T* ptr_{};
    };

    template < typename T, typename... Args >
    intrusive_ptr<T> make_intrusive(Args&&... args) {
        // NOLINTNEXTLINE(*-owning-memory)
        return new T(std::forward<Args>(args)...);
    }

    template < typename T >
    void swap(intrusive_ptr<T>& l, intrusive_ptr<T>& r) noexcept {
        return l.swap(r);
    }

    template < typename T >
    [[nodiscard]] bool operator==(const intrusive_ptr<T>& l, const T* r) noexcept {
        return l.get() == r;
    }

    template < typename T >
    [[nodiscard]] std::strong_ordering operator<=>(const intrusive_ptr<T>& l, const T* r) noexcept {
        return l.get() <=> r;
    }

    template < typename T >
    [[nodiscard]] bool operator==(const intrusive_ptr<T>& l, std::nullptr_t) noexcept {
        return l.get() == nullptr;
    }

    template < typename T >
    [[nodiscard]] std::strong_ordering operator<=>(const intrusive_ptr<T>& l, std::nullptr_t) noexcept {
        return l.get() <=> nullptr;
    }
}

namespace std
{
    template < typename T >
    struct hash<meta_hpp::detail::intrusive_ptr<T>> {
        size_t operator()(const meta_hpp::detail::intrusive_ptr<T>& ip) const noexcept {
            return ip.get_hash();
        }
    };
}
