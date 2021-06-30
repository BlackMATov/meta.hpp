/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_fwd.hpp"

#include "meta_type.hpp"

#include "meta_class.hpp"
#include "meta_field.hpp"
#include "meta_function.hpp"
#include "meta_method.hpp"
#include "meta_namespace.hpp"
#include "meta_variable.hpp"

namespace meta_hpp
{
    class registry {
    public:
        template < auto T >
        std::optional<type> resolve() const {
            const family_id fid = get_family_id<decltype(T)>();
            return detail::find_opt(family_to_types_, fid);
        }

        template < typename T >
        std::optional<type> resolve() const {
            const family_id fid = get_family_id<T>();
            return detail::find_opt(family_to_types_, fid);
        }

        template < typename T >
        std::optional<type> resolve(const T&) const {
            const family_id fid = get_family_id<T>();
            return detail::find_opt(family_to_types_, fid);
        }

        template < typename... Internals >
        registry& operator()(Internals&&...internals) {
            (add_(std::forward<Internals>(internals)), ...);
            return *this;
        }
    private:
        void add_(const class_info& info) {
            family_to_types_.emplace(info.fid(), info);

            info.each_class([this](const class_info& internal){
                add_(internal);
            });

            info.each_field([this](const field_info& internal){
                add_(internal);
            });

            info.each_function([this](const function_info& internal){
                add_(internal);
            });

            info.each_method([this](const method_info& internal){
                add_(internal);
            });

            info.each_variable([this](const variable_info& internal){
                add_(internal);
            });
        }

        void add_(const field_info& info) {
            family_to_types_.emplace(info.fid(), info);
        }

        void add_(const function_info& info) {
            family_to_types_.emplace(info.fid(), info);
        }

        void add_(const method_info& info) {
            family_to_types_.emplace(info.fid(), info);
        }

        void add_(const namespace_info& info) {
            info.each_class([this](const class_info& internal){
                add_(internal);
            });

            info.each_function([this](const function_info& internal){
                add_(internal);
            });

            info.each_namespace([this](const namespace_info& internal){
                add_(internal);
            });

            info.each_variable([this](const variable_info& internal){
                add_(internal);
            });
        }

        void add_(const variable_info& info) {
            family_to_types_.emplace(info.fid(), info);
        }
    private:
        std::map<family_id, type, std::less<>> family_to_types_;
    };
}
