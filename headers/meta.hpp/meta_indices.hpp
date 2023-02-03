/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_base.hpp"
#include "meta_types.hpp"

#include "meta_detail/index_family.hpp"

namespace meta_hpp
{
    class argument_index final {
    public:
        [[nodiscard]] const any_type& get_type() const noexcept;
        [[nodiscard]] std::size_t get_position() const noexcept;

        [[nodiscard]] std::size_t get_hash() const noexcept;
        [[nodiscard]] std::strong_ordering operator<=>(const argument_index&) const = default;
    private:
        friend detail::argument_state;
        explicit argument_index(any_type type, std::size_t position);
    private:
        any_type type_;
        std::size_t position_{};
    };

    class constructor_index final {
    public:
        [[nodiscard]] const constructor_type& get_type() const noexcept;

        [[nodiscard]] std::size_t get_hash() const noexcept;
        [[nodiscard]] std::strong_ordering operator<=>(const constructor_index&) const = default;
    private:
        friend detail::constructor_state;
        explicit constructor_index(constructor_type type);
    private:
        constructor_type type_;
    };

    class destructor_index final {
    public:
        [[nodiscard]] const destructor_type& get_type() const noexcept;

        [[nodiscard]] std::size_t get_hash() const noexcept;
        [[nodiscard]] std::strong_ordering operator<=>(const destructor_index&) const = default;
    private:
        friend detail::destructor_state;
        explicit destructor_index(destructor_type type);
    private:
        destructor_type type_;
    };

    class evalue_index final {
    public:
        [[nodiscard]] const enum_type& get_type() const noexcept;
        [[nodiscard]] const std::string& get_name() const noexcept;

        [[nodiscard]] std::size_t get_hash() const noexcept;
        [[nodiscard]] std::strong_ordering operator<=>(const evalue_index&) const = default;
    private:
        friend detail::evalue_state;
        explicit evalue_index(enum_type type, std::string name);
    private:
        enum_type type_;
        std::string name_;
    };

    class function_index final {
    public:
        [[nodiscard]] const function_type& get_type() const noexcept;
        [[nodiscard]] const std::string& get_name() const noexcept;

        [[nodiscard]] std::size_t get_hash() const noexcept;
        [[nodiscard]] std::strong_ordering operator<=>(const function_index&) const = default;
    private:
        friend detail::function_state;
        explicit function_index(function_type type, std::string name);
    private:
        function_type type_;
        std::string name_;
    };

    class member_index final {
    public:
        [[nodiscard]] const member_type& get_type() const noexcept;
        [[nodiscard]] const std::string& get_name() const noexcept;

        [[nodiscard]] std::size_t get_hash() const noexcept;
        [[nodiscard]] std::strong_ordering operator<=>(const member_index&) const = default;
    private:
        friend detail::member_state;
        explicit member_index(member_type type, std::string name);
    private:
        member_type type_;
        std::string name_;
    };

    class method_index final {
    public:
        [[nodiscard]] std::size_t get_hash() const noexcept;
        [[nodiscard]] const method_type& get_type() const noexcept;
        [[nodiscard]] const std::string& get_name() const noexcept;

        [[nodiscard]] std::strong_ordering operator<=>(const method_index&) const = default;
    private:
        friend detail::method_state;
        explicit method_index(method_type type, std::string name);
    private:
        method_type type_;
        std::string name_;
    };

    class scope_index final {
    public:
        [[nodiscard]] const std::string& get_name() const noexcept;

        [[nodiscard]] std::size_t get_hash() const noexcept;
        [[nodiscard]] std::strong_ordering operator<=>(const scope_index&) const = default;
    private:
        friend detail::scope_state;
        explicit scope_index(std::string name);
    private:
        std::string name_;
    };

    class variable_index final {
    public:
        [[nodiscard]] const pointer_type& get_type() const noexcept;
        [[nodiscard]] const std::string& get_name() const noexcept;

        [[nodiscard]] std::size_t get_hash() const noexcept;
        [[nodiscard]] std::strong_ordering operator<=>(const variable_index&) const = default;
    private:
        friend detail::variable_state;
        explicit variable_index(pointer_type type, std::string name);
    private:
        pointer_type type_;
        std::string name_;
    };
}

namespace std
{
    template < meta_hpp::detail::index_family T >
    struct hash<T> {
        size_t operator()(const T& t) const noexcept {
            return t.get_hash();
        }
    };
}
