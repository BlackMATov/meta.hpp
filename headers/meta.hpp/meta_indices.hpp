/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_base.hpp"
#include "meta_types.hpp"

namespace meta_hpp
{
    class argument_index final {
    public:
        argument_index() = delete;
        [[nodiscard]] const any_type& get_type() const noexcept;
        [[nodiscard]] std::size_t get_position() const noexcept;
    private:
        friend detail::argument_state;
        template < typename Argument >
        [[nodiscard]] static argument_index make(std::size_t position);
    private:
        explicit argument_index(any_type type, std::size_t position);
        friend bool operator<(const argument_index& l, const argument_index& r) noexcept;
        friend bool operator==(const argument_index& l, const argument_index& r) noexcept;
    private:
        any_type type_;
        std::size_t position_{};
    };

    class constructor_index final {
    public:
        constructor_index() = delete;
        [[nodiscard]] const constructor_type& get_type() const noexcept;
    private:
        friend detail::constructor_state;
        template < detail::class_kind Class, typename... Args >
        [[nodiscard]] static constructor_index make();
    private:
        explicit constructor_index(constructor_type type);
        friend bool operator<(const constructor_index& l, const constructor_index& r) noexcept;
        friend bool operator==(const constructor_index& l, const constructor_index& r) noexcept;
    private:
        constructor_type type_;
    };

    class destructor_index final {
    public:
        destructor_index() = delete;
        [[nodiscard]] const destructor_type& get_type() const noexcept;
    private:
        friend detail::destructor_state;
        template < detail::class_kind Class >
        [[nodiscard]] static destructor_index make();
    private:
        explicit destructor_index(destructor_type type);
        friend bool operator<(const destructor_index& l, const destructor_index& r) noexcept;
        friend bool operator==(const destructor_index& l, const destructor_index& r) noexcept;
    private:
        destructor_type type_;
    };

    class evalue_index final {
    public:
        evalue_index() = delete;
        [[nodiscard]] const enum_type& get_type() const noexcept;
        [[nodiscard]] const std::string& get_name() const noexcept;
    private:
        friend detail::evalue_state;
        template < detail::enum_kind Enum >
        [[nodiscard]] static evalue_index make(std::string name);
    private:
        explicit evalue_index(enum_type type, std::string name);
        friend bool operator<(const evalue_index& l, const evalue_index& r) noexcept;
        friend bool operator==(const evalue_index& l, const evalue_index& r) noexcept;
    private:
        enum_type type_;
        std::string name_;
    };

    class function_index final {
    public:
        function_index() = delete;
        [[nodiscard]] const function_type& get_type() const noexcept;
        [[nodiscard]] const std::string& get_name() const noexcept;
    private:
        friend detail::function_state;
        template < detail::function_kind Function >
        [[nodiscard]] static function_index make(std::string name);
    private:
        explicit function_index(function_type type, std::string name);
        friend bool operator<(const function_index& l, const function_index& r) noexcept;
        friend bool operator==(const function_index& l, const function_index& r) noexcept;
    private:
        function_type type_;
        std::string name_;
    };

    class member_index final {
    public:
        member_index() = delete;
        [[nodiscard]] const member_type& get_type() const noexcept;
        [[nodiscard]] const std::string& get_name() const noexcept;
    private:
        friend detail::member_state;
        template < detail::member_kind Member >
        [[nodiscard]] static member_index make(std::string name);
    private:
        explicit member_index(member_type type, std::string name);
        friend bool operator<(const member_index& l, const member_index& r) noexcept;
        friend bool operator==(const member_index& l, const member_index& r) noexcept;
    private:
        member_type type_;
        std::string name_;
    };

    class method_index final {
    public:
        method_index() = delete;
        [[nodiscard]] const method_type& get_type() const noexcept;
        [[nodiscard]] const std::string& get_name() const noexcept;
    private:
        friend detail::method_state;
        template < detail::method_kind Method >
        [[nodiscard]] static method_index make(std::string name);
    private:
        explicit method_index(method_type type, std::string name);
        friend bool operator<(const method_index& l, const method_index& r) noexcept;
        friend bool operator==(const method_index& l, const method_index& r) noexcept;
    private:
        method_type type_;
        std::string name_;
    };

    class scope_index final {
    public:
        scope_index() = delete;
        [[nodiscard]] const std::string& get_name() const noexcept;
    private:
        friend detail::scope_state;
        [[nodiscard]] static scope_index make(std::string name);
    private:
        explicit scope_index(std::string name);
        friend bool operator<(const scope_index& l, const scope_index& r) noexcept;
        friend bool operator==(const scope_index& l, const scope_index& r) noexcept;
    private:
        std::string name_;
    };

    class variable_index final {
    public:
        variable_index() = delete;
        [[nodiscard]] const pointer_type& get_type() const noexcept;
        [[nodiscard]] const std::string& get_name() const noexcept;
    private:
        friend detail::variable_state;
        template < detail::pointer_kind Pointer >
        [[nodiscard]] static variable_index make(std::string name);
    private:
        explicit variable_index(pointer_type type, std::string name);
        friend bool operator<(const variable_index& l, const variable_index& r) noexcept;
        friend bool operator==(const variable_index& l, const variable_index& r) noexcept;
    private:
        pointer_type type_;
        std::string name_;
    };
}
