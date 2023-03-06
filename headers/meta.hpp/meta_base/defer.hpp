/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "base.hpp"

namespace meta_hpp::detail
{
    namespace impl
    {
        template < typename F, typename... Args >
        class defer_impl {
        public:
            defer_impl() = delete;
            defer_impl(defer_impl&&) = delete;
            defer_impl(const defer_impl&) = delete;
            defer_impl& operator=(defer_impl&&) = delete;
            defer_impl& operator=(const defer_impl&) = delete;

            template < typename UF >
            explicit defer_impl(UF&& f, std::tuple<Args...>&& args)
            : f_{std::forward<UF>(f)}
            , args_{std::move(args)} {}

            void dismiss() noexcept {
                dismissed_ = true;
            }

        protected:
            ~defer_impl() noexcept {
                if ( !dismissed_ ) {
                    std::apply(std::move(f_), std::move(args_));
                }
            }

        private:
            F f_;
            std::tuple<Args...> args_;
            bool dismissed_{};
        };
    }

    template < typename F, typename... Args >
    class defer final : public impl::defer_impl<F, Args...> {
    public:
        defer() = delete;
        defer(defer&&) = delete;
        defer(const defer&) = delete;
        defer& operator=(defer&&) = delete;
        defer& operator=(const defer&) = delete;

        template < typename UF >
        explicit defer(UF&& f, std::tuple<Args...>&& args)
        : impl::defer_impl<F, Args...>{std::forward<UF>(f), std::move(args)} {}

        ~defer() noexcept = default;
    };

    template < typename F, typename... Args >
    class error_defer final : public impl::defer_impl<F, Args...> {
    public:
        error_defer() = delete;
        error_defer(error_defer&&) = delete;
        error_defer(const error_defer&) = delete;
        error_defer& operator=(error_defer&&) = delete;
        error_defer& operator=(const error_defer&) = delete;

        template < typename UF >
        explicit error_defer(UF&& f, std::tuple<Args...>&& args)
        : impl::defer_impl<F, Args...>{std::forward<UF>(f), std::move(args)}
        , exceptions_{std::uncaught_exceptions()} {}

        ~error_defer() noexcept {
            if ( exceptions_ == std::uncaught_exceptions() ) {
                this->dismiss();
            }
        }

    private:
        int exceptions_{};
    };

    template < typename F, typename... Args >
    class return_defer final : public impl::defer_impl<F, Args...> {
    public:
        return_defer() = delete;
        return_defer(return_defer&&) = delete;
        return_defer(const return_defer&) = delete;
        return_defer& operator=(return_defer&&) = delete;
        return_defer& operator=(const return_defer&) = delete;

        template < typename UF >
        explicit return_defer(UF&& f, std::tuple<Args...>&& args)
        : impl::defer_impl<F, Args...>{std::forward<UF>(f), std::move(args)}
        , exceptions_{std::uncaught_exceptions()} {}

        ~return_defer() noexcept {
            if ( exceptions_ != std::uncaught_exceptions() ) {
                this->dismiss();
            }
        }

    private:
        int exceptions_{};
    };

    template < typename F, typename... Args >
    auto make_defer(F&& f, Args&&... args) {
        using defer_t = defer<std::decay_t<F>, std::decay_t<Args>...>;
        return defer_t{std::forward<F>(f), std::make_tuple(std::forward<Args>(args)...)};
    }

    template < typename F, typename... Args >
    auto make_error_defer(F&& f, Args&&... args) {
        using defer_t = error_defer<std::decay_t<F>, std::decay_t<Args>...>;
        return defer_t{std::forward<F>(f), std::make_tuple(std::forward<Args>(args)...)};
    }

    template < typename F, typename... Args >
    auto make_return_defer(F&& f, Args&&... args) {
        using defer_t = return_defer<std::decay_t<F>, std::decay_t<Args>...>;
        return defer_t{std::forward<F>(f), std::make_tuple(std::forward<Args>(args)...)};
    }
}

#ifdef __COUNTER__
#    define META_HPP_DEFER(...) \
        auto META_HPP_PP_CAT(meta_hpp_generated_defer_, __COUNTER__) { ::meta_hpp::detail::make_defer(__VA_ARGS__) }
#    define META_HPP_ERROR_DEFER(...) \
        auto META_HPP_PP_CAT(meta_hpp_generated_error_defer_, __COUNTER__) { ::meta_hpp::detail::make_error_defer(__VA_ARGS__) }
#    define META_HPP_RETURN_DEFER(...) \
        auto META_HPP_PP_CAT(meta_hpp_generated_return_defer_, __COUNTER__) { ::meta_hpp::detail::make_return_defer(__VA_ARGS__) }
#else
#    define META_HPP_DEFER(...) \
        auto META_HPP_PP_CAT(meta_hpp_generated_defer_, __LINE__) { ::meta_hpp::detail::make_defer(__VA_ARGS__) }
#    define META_HPP_ERROR_DEFER(...) \
        auto META_HPP_PP_CAT(meta_hpp_generated_error_defer_, __LINE__) { ::meta_hpp::detail::make_error_defer(__VA_ARGS__) }
#    define META_HPP_RETURN_DEFER(...) \
        auto META_HPP_PP_CAT(meta_hpp_generated_return_defer_, __LINE__) { ::meta_hpp::detail::make_return_defer(__VA_ARGS__) }
#endif
