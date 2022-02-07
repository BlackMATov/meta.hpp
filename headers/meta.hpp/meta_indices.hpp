/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_base.hpp"
#include "meta_types.hpp"

namespace meta_hpp
{
    class ctor_index final {
    public:
        ctor_index() = delete;
        [[nodiscard]] const ctor_type& get_type() const noexcept;
    private:
        friend detail::ctor_state;
        template < detail::class_kind Class, typename... Args >
        [[nodiscard]] static ctor_index make();
    private:
        explicit ctor_index(ctor_type type);
        friend bool operator<(const ctor_index& l, const ctor_index& r) noexcept;
        friend bool operator==(const ctor_index& l, const ctor_index& r) noexcept;
    private:
        ctor_type type_;
    };

    class dtor_index final {
    public:
        dtor_index() = delete;
        [[nodiscard]] const dtor_type& get_type() const noexcept;
    private:
        friend detail::dtor_state;
        template < detail::class_kind Class >
        [[nodiscard]] static dtor_index make();
    private:
        explicit dtor_index(dtor_type type);
        friend bool operator<(const dtor_index& l, const dtor_index& r) noexcept;
        friend bool operator==(const dtor_index& l, const dtor_index& r) noexcept;
    private:
        dtor_type type_;
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

    class parameter_index final {
    public:
        parameter_index() = delete;
        [[nodiscard]] const any_type& get_type() const noexcept;
        [[nodiscard]] const std::string& get_name() const noexcept;
    private:
        friend detail::parameter_state;
        template < typename Parameter >
        [[nodiscard]] static parameter_index make(std::string name);
    private:
        explicit parameter_index(any_type type, std::string name);
        friend bool operator<(const parameter_index& l, const parameter_index& r) noexcept;
        friend bool operator==(const parameter_index& l, const parameter_index& r) noexcept;
    private:
        any_type type_;
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
