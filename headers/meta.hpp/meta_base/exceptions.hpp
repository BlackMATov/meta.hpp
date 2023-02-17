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
#    define META_HPP_RETHROW() (void)0
#endif

namespace meta_hpp::detail
{
    enum class error_code {
        no_error,

        bad_const_access,
        bad_uvalue_access,
        bad_uresult_access,

        bad_uvalue_cast,
        bad_argument_cast,
        bad_instance_cast,

        arity_mismatch,
        instance_type_mismatch,
        argument_type_mismatch,
    };

    inline const char* get_error_code_message(error_code error) noexcept {
        switch ( error ) {
        case error_code::no_error:
            return "no error";
        case error_code::bad_const_access:
            return "bad const access";
        case error_code::bad_uvalue_access:
            return "bad uvalue access";
        case error_code::bad_uresult_access:
            return "bad uresult access";
        case error_code::bad_uvalue_cast:
            return "bad uvalue cast";
        case error_code::bad_argument_cast:
            return "bad argument cast";
        case error_code::bad_instance_cast:
            return "bad instance cast";
        case error_code::arity_mismatch:
            return "arity mismatch";
        case error_code::instance_type_mismatch:
            return "instance type mismatch";
        case error_code::argument_type_mismatch:
            return "argument type mismatch";
        }

        META_HPP_ASSERT(false);
        return "unexpected error code";
    }
}

namespace meta_hpp::detail
{
    class exception final : public std::exception {
    public:
        explicit exception(error_code error)
        : error_{error} {}

        [[nodiscard]] error_code get_error() const noexcept {
            return error_;
        }

        [[nodiscard]] const char* what() const noexcept override {
            return get_error_code_message(error_);
        }

    private:
        error_code error_{};
    };

    [[noreturn]] inline void throw_exception(error_code err) {
#if !defined(META_HPP_NO_EXCEPTIONS)
        throw exception{err};
#else
        (void)err;
        std::abort();
#endif
    }
}
