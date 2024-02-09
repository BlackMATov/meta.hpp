/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "../meta_base.hpp"
#include "../meta_types.hpp"

namespace meta_hpp::detail
{
    class type_registry final {
    public:
        class locker final {
        public:
            explicit locker()
            : lock_{instance().mutex_} {}

            ~locker() = default;

            locker(locker&&) = default;
            locker& operator=(locker&&) = default;

            locker(const locker&) = delete;
            locker& operator=(const locker&) = delete;

        private:
            std::unique_lock<std::recursive_mutex> lock_;
        };

        [[nodiscard]] static type_registry& instance() {
            static type_registry instance;
            return instance;
        }

    public:
        template < typename F >
        // NOLINTNEXTLINE(*-missing-std-forward)
        void for_each_type(F&& f) const {
            const locker lock;

            for ( auto&& type : types_ ) {
                std::invoke(f, type);
            }
        }

    public:
        template < typename T >
        [[nodiscard]] auto resolve_by_type() {
            return resolve_by_traits<type_to_traits_t<std::remove_cv_t<T>>>();
        }

        template < typename Traits >
        [[nodiscard]] auto resolve_by_traits() {
            using type_traits = traits_to_type_traits_t<Traits>;

            static auto type_data_instance = [this]() {
                auto new_data{std::make_unique<typename type_traits::data_type>(Traits{})};

                const locker lock;
                types_.emplace(new_data.get());

                return new_data;
            }();

            return typename type_traits::type{type_data_instance.get()};
        }

    private:
        type_registry() = default;

    private:
        std::recursive_mutex mutex_;
        std::set<any_type, std::less<>> types_;
    };
}
