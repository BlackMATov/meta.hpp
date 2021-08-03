/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_fwd.hpp"

namespace meta_hpp
{
    template < typename F, typename... As >
    class curry_t;

    namespace impl
    {
        template < typename F >
        struct is_curried_impl
        : std::false_type {};

        template < typename F, typename... As >
        struct is_curried_impl<curry_t<F, As...>>
        : std::true_type {};
    }

    template < typename F >
    struct is_curried
    : impl::is_curried_impl<std::remove_cv_t<F>> {};

    template < typename F >
    inline constexpr bool is_curried_v = is_curried<F>::value;
}

namespace meta_hpp::detail
{
    template < typename F, typename... Args >
    constexpr auto curry_or_apply(F&& f, std::tuple<Args...>&& args) {
        if constexpr ( std::is_invocable_v<std::decay_t<F>, Args...> ) {
            return std::apply(std::forward<F>(f), std::move(args));
        } else {
            return curry_t(std::forward<F>(f), std::move(args));
        }
    }
}

namespace meta_hpp
{
    template < typename F, typename... Args >
    class curry_t final {
    public:
        constexpr curry_t(F f)
        : f_(std::move(f)) {}

        constexpr curry_t(F f, std::tuple<Args...> args)
        : f_(std::move(f))
        , args_(std::move(args)) {}

        constexpr auto operator()() && {
            return detail::curry_or_apply(
                std::move(f_),
                std::move(args_));
        }

        template < typename A >
        constexpr auto operator()(A&& a) && {
            return detail::curry_or_apply(
                std::move(f_),
                std::tuple_cat(
                    std::move(args_),
                    std::make_tuple(std::forward<A>(a))));
        }

        template < typename A, typename... As >
        constexpr auto operator()(A&& a, As&&... as) && {
            return std::move(*this)(std::forward<A>(a))(std::forward<As>(as)...);
        }

        template < typename... As >
        constexpr auto operator()(As&&... as) const & {
            return std::move(curry_t(*this))(std::forward<As>(as)...);
        }
    private:
        F f_;
        std::tuple<Args...> args_;
    };
}

namespace meta_hpp
{
    template < typename F >
    constexpr auto curry(F&& f) {
        if constexpr ( is_curried_v<std::decay_t<F>> ) {
            return std::forward<F>(f);
        } else {
            return detail::curry_or_apply(std::forward<F>(f), {});
        }
    }

    template < typename F, typename A, typename... As >
    constexpr auto curry(F&& f, A&& a, As&&... as) {
        return curry(std::forward<F>(f))(std::forward<A>(a), std::forward<As>(as)...);
    }
}
