/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_base.hpp"
#include "meta_types.hpp"

namespace meta_hpp
{
    class argument_index final {
    public:
        argument_index() = delete;
        explicit argument_index(any_type type, std::size_t position);

        [[nodiscard]] any_type get_type() const noexcept;
        [[nodiscard]] std::size_t get_position() const noexcept;

        void swap(argument_index& other) noexcept;
        [[nodiscard]] std::size_t get_hash() const noexcept;
        [[nodiscard]] std::strong_ordering operator<=>(const argument_index&) const = default;

    private:
        any_type type_;
        std::size_t position_{};
    };

    class constructor_index final {
    public:
        constructor_index() = delete;
        explicit constructor_index(constructor_type type);

        [[nodiscard]] constructor_type get_type() const noexcept;

        void swap(constructor_index& other) noexcept;
        [[nodiscard]] std::size_t get_hash() const noexcept;
        [[nodiscard]] std::strong_ordering operator<=>(const constructor_index&) const = default;

    private:
        constructor_type type_;
    };

    class destructor_index final {
    public:
        destructor_index() = delete;
        explicit destructor_index(destructor_type type);

        [[nodiscard]] destructor_type get_type() const noexcept;

        void swap(destructor_index& other) noexcept;
        [[nodiscard]] std::size_t get_hash() const noexcept;
        [[nodiscard]] std::strong_ordering operator<=>(const destructor_index&) const = default;

    private:
        destructor_type type_;
    };

    class evalue_index final {
    public:
        evalue_index() = delete;
        explicit evalue_index(enum_type type, std::string name);

        [[nodiscard]] enum_type get_type() const noexcept;

        [[nodiscard]] std::string get_name() && noexcept;
        [[nodiscard]] const std::string& get_name() const& noexcept;

        void swap(evalue_index& other) noexcept;
        [[nodiscard]] std::size_t get_hash() const noexcept;
        [[nodiscard]] std::strong_ordering operator<=>(const evalue_index&) const = default;

    private:
        enum_type type_;
        std::string name_;
    };

    class function_index final {
    public:
        function_index() = delete;
        explicit function_index(function_type type, std::string name);

        [[nodiscard]] function_type get_type() const noexcept;

        [[nodiscard]] std::string get_name() && noexcept;
        [[nodiscard]] const std::string& get_name() const& noexcept;

        void swap(function_index& other) noexcept;
        [[nodiscard]] std::size_t get_hash() const noexcept;
        [[nodiscard]] std::strong_ordering operator<=>(const function_index&) const = default;

    private:
        function_type type_;
        std::string name_;
    };

    class member_index final {
    public:
        member_index() = delete;
        explicit member_index(member_type type, std::string name);

        [[nodiscard]] member_type get_type() const noexcept;

        [[nodiscard]] std::string get_name() && noexcept;
        [[nodiscard]] const std::string& get_name() const& noexcept;

        void swap(member_index& other) noexcept;
        [[nodiscard]] std::size_t get_hash() const noexcept;
        [[nodiscard]] std::strong_ordering operator<=>(const member_index&) const = default;

    private:
        member_type type_;
        std::string name_;
    };

    class method_index final {
    public:
        method_index() = delete;
        explicit method_index(method_type type, std::string name);

        [[nodiscard]] method_type get_type() const noexcept;

        [[nodiscard]] std::string get_name() && noexcept;
        [[nodiscard]] const std::string& get_name() const& noexcept;

        void swap(method_index& other) noexcept;
        [[nodiscard]] std::size_t get_hash() const noexcept;
        [[nodiscard]] std::strong_ordering operator<=>(const method_index&) const = default;

    private:
        method_type type_;
        std::string name_;
    };

    class scope_index final {
    public:
        scope_index() = delete;
        explicit scope_index(std::string name);

        [[nodiscard]] std::string get_name() && noexcept;
        [[nodiscard]] const std::string& get_name() const& noexcept;

        void swap(scope_index& other) noexcept;
        [[nodiscard]] std::size_t get_hash() const noexcept;
        [[nodiscard]] std::strong_ordering operator<=>(const scope_index&) const = default;

    private:
        std::string name_;
    };

    class variable_index final {
    public:
        variable_index() = delete;
        explicit variable_index(pointer_type type, std::string name);

        [[nodiscard]] pointer_type get_type() const noexcept;

        [[nodiscard]] std::string get_name() && noexcept;
        [[nodiscard]] const std::string& get_name() const& noexcept;

        void swap(variable_index& other) noexcept;
        [[nodiscard]] std::size_t get_hash() const noexcept;
        [[nodiscard]] std::strong_ordering operator<=>(const variable_index&) const = default;

    private:
        pointer_type type_;
        std::string name_;
    };

    template < index_family Index >
    void swap(Index& l, Index& r) noexcept {
        l.swap(r);
    }
}

namespace std
{
    template < meta_hpp::index_family Index >
    struct hash<Index> {
        size_t operator()(const Index& index) const noexcept {
            return index.get_hash();
        }
    };
}
