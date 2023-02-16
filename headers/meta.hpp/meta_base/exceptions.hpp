/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "base.hpp"

#if !defined(META_HPP_NO_EXCEPTIONS)
#    define META_HPP_TRY try
#    define META_HPP_CATCH(...) catch ( __VA_ARGS__ )
#    define META_HPP_RETHROW() throw
#else
#    define META_HPP_TRY if ( true )
#    define META_HPP_CATCH(...) if ( false )
#    define META_HPP_RETHROW() std::abort()
#endif

namespace meta_hpp::detail
{
    enum class generic_error {
        no_error,

        bad_uvalue_access,
        bad_uresult_access,

        bad_uvalue_cast,
        bad_argument_cast,
        bad_instance_cast,

        arity_mismatch,
        instance_type_mismatch,
        argument_types_mismatch,
    };

    class generic_error_category final : public std::error_category {
    public:
        ~generic_error_category() override = default;

        generic_error_category(generic_error_category&&) = delete;
        generic_error_category(const generic_error_category&) = delete;

        generic_error_category& operator=(generic_error_category&&) = delete;
        generic_error_category& operator=(const generic_error_category&) = delete;

        [[nodiscard]] const char* name() const noexcept override {
            return "generic";
        }

        [[nodiscard]] std::string message(int ev) const override {
            switch ( static_cast<generic_error>(ev) ) {
            case generic_error::no_error:
                return "no error";
            case generic_error::bad_uvalue_access:
                return "bad uvalue access";
            case generic_error::bad_uresult_access:
                return "bad uresult access";
            case generic_error::bad_uvalue_cast:
                return "bad uvalue cast";
            case generic_error::bad_argument_cast:
                return "bad argument cast";
            case generic_error::bad_instance_cast:
                return "bad instance cast";
            case generic_error::arity_mismatch:
                return "arity mismatch";
            case generic_error::instance_type_mismatch:
                return "instance type mismatch";
            case generic_error::argument_types_mismatch:
                return "argument types mismatch";
            }
            return "unexpected error code";
        }

        static const std::error_category& instance() noexcept {
            static const generic_error_category instance;
            return instance;
        }

    private:
        generic_error_category() = default;
    };
}

namespace std
{
    template <>
    struct is_error_code_enum<meta_hpp::detail::generic_error> : true_type {};
}

namespace meta_hpp::detail
{
    inline std::error_code make_error_code(generic_error err) noexcept {
        static_assert(std::is_same_v<int, std::underlying_type_t<generic_error>>);
        return std::error_code{static_cast<int>(err), generic_error_category::instance()};
    }
}

namespace meta_hpp::detail
{
    class generic_exception final : public std::logic_error {
    public:
        explicit generic_exception(generic_error err)
        : generic_exception{make_error_code(err)} {}

        explicit generic_exception(std::error_code ec)
        : std::logic_error(ec.message())
        , error_code_{ec} {}

        [[nodiscard]] const std::error_code& get_code() const noexcept {
            return error_code_;
        }

    private:
        std::error_code error_code_{};
    };

    [[noreturn]] inline void throw_generic_exception(generic_error err) {
#if !defined(META_HPP_NO_EXCEPTIONS)
        throw generic_exception{err};
#else
        (void)err;
        std::abort();
#endif
    }
}
